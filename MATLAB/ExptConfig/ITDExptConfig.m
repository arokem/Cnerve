function Expt = ITDExptConfig(Expt,OldExpt)
%
% Description:
% This code sets up the fiber, stimulation, and optional parameters for
% simulating ANF electrical stimulation and then produces a .txt file of
% commands to run the code. Expt is a structure that gets passed between
% the various functions. Some of the highest level parameters need to be
% user defined as in ExptTemplate.m.
%
% Usage: Expt = SFExptConfig(Expt)
%
%   Author: Jesse Resnick 2017_02-altered paths to consolidate code, added
%           code for exploring demyelination degree and extent effects. Use
%           thresh and RS from previous runs and RS to estimate stim space.
%           These are now saved in cngExp structure. Defined alternative
%           experiment types with appropriate switches for parameters and
%           stimuli. Changed to more flexible, modular setup.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Expt.created = datestr(now);
%Shuffle the rng using the current time and save the state for future use.
Expt.rngState = rng('shuffle');

%% Unpack input structure.
% Set machine to run on. Used as switch for defining paths and submitting
% jobs. Can take values: 'Julep', 'Hyak', 'Comet'
if ~isfield(Expt,'Machine')
    Expt.Machine = 'Hyak';
end


% Set number of fibers to run per trial.
if ~isfield(Expt.opts,'numFibers')
    numFibers = 50;
    Expt.opts.numFibers = numFibers;
end
% Set electrical parameters switch. See GenModlfiles for details of switch
% function. Can be 'orig' or 'nikita-v1'
if ~isfield(Expt,'opts.elecParams'); Expt.opts.elecParams = 'orig'; end

% Set channel gating parameters switch. See GenModlfiles for details of switch
% function. Can be 'schwarz','mcintyre2002',or 'SMhybrid'.
if ~isfield(Expt,'opts.gateParams'); Expt.opts.gateParams = 'SMhybrid'; end

% decrease the number of jobs we need to spawn by assigning more than one
% MC simulation and/or more than one fiber per cnerve program.
fibersplitter	= Expt.opts.numFibers/Expt.opts.fiberperjob;
Expt.opts.fibersplitter = fibersplitter;

% check that we assign each job an integer number of fibers.
if rem( Expt.opts.numFibers, Expt.opts.numFibers/fibersplitter )
    error('Non-integer number of fibers assigned to each job: %.2f', fibersplitter);
end

%% ------------------------------------------------------------------------
% FILE NAMING CONVENTIONS % (A little messy. Will need to fix, someday.
%--------------------------------------------------------------------------
% ConfigFiles directory information will be used to add the necessary scripts
% and functions to the path.
[ConfigFiles.path, ConfigFiles.name, ConfigFiles.ext] = fileparts(mfilename('fullpath'));
ConfigFiles.fullPath = [fullfile(ConfigFiles.path, ConfigFiles.name) ConfigFiles.ext];
Expt.fnames.ConfigFiles = ConfigFiles;

addpath(genpath('..'));

[~,Expt.git_hash_string] = system('git rev-parse HEAD');

% PathDefining accepts Expt and ConfigFiles and returns
% the necessary data directory information based on the Machine being
% used and Expt.DataFiles location. It also copies the relevant executable
% and this configuration function into Expt.dataDir.
Expt = PathDefining(Expt);

% Store DataFiles directory information in easy to access variable. Defined
% in experiment setup script.
DataFiles = Expt.fnames.DataFiles;

modlSuffix	= '.modl';
stimSuffix	= '.stim';
optsSuffix	= '.opts';

%% ------------------------------------------------------------------------
%DEFINE SIMULATION OPTIONS (Need to move some outside function into config script??)
%--------------------------------------------------------------------------
Expt.opts.V_th			= 50.0;		% Spike threshold above E_rest
Expt.opts.TypeNa		= 0;		% 0) use default Na chan model (Gillespie),
% 1) use Patlak (probably don't want)
% 2) use Gillespie
% 3) use Jeff's model (DON'T USE!)
Expt.opts.UsepNa		= 0;
Expt.opts.UseK			= 1;
Expt.opts.UseSlowK		= 1;
if ~isfield(Expt.opts,'RecordVoltage'); Expt.opts.RecordVoltage	= 0; end
Expt.opts.RecordECAP	= 0;
Expt.opts.VoltageSample = 0.01;  % [ms] Defines the voltage sampling timestep.
Expt.opts.delt      	= .001;  % time step of simulation
Expt.opts.MaxVoltage    = 1500; % Voltage at which cnerve throws an error. Typically 1500 mV for a normal fiber
Expt.opts.MaxSpikes		= 100;      % Needs to be modified based on type of stimulus

Expt.opts.is_active     = 1;
Expt.opts.is_stochastic = 1;

% Check for user defined number of monte carlos and then set based on
% experiment type.
if ~isfield(Expt.opts,'monte')
    Expt.opts.monte      = 10;
end

ExptOpts = Expt.opts; %#ok<NASGU>

optsFileName	= strcat([ DataFiles.name	optsSuffix]);
FileName = [DataFiles.path optsFileName];
save(FileName,'TypeNa','UsepNa', 'UseK', 'UseSlowK','V_th', ...
    'RecordECAP','RecordVoltage','MaxSpikes','delt', ...
    'VoltageSample','MaxVoltage','is_active','is_stochastic',...
    '-struct','ExptOpts','-ascii');

%% --------------------------------------------------------------------------------
% MAIN-- GENERATE EACH TRIAL INSIDE THE 3 IDX LOOPS
%--------------------------------------------------------------------------------
% Updated in GenModlFile during each iteration.
if Expt.initialize; Expt.maxNodes = 0; end

%% Generate and open command file for writing.
% Specify machine specific command format
cmdfile = [DataFiles.path '/commands'];
fid = fopen(cmdfile,'a');
switch Expt.Machine
    case 'Julep'
        format = './Cnerve %s %s %s %s %d %d %d %d %d\n';
    case 'Hyak'
        format = './Cnerve %s %s %s %s %d %d %d %d %d\n';
    case 'Dell'
        format = './Cnerve.exe %s %s %s %s %d %d %d %d %d\n';
end

%% Begin fiber loops
for idx1=1:Expt.idx1Max % demy mean severity index
    for idx2=1:Expt.idx2Max % demy severity variance
        
        %% Produce model file:
        % Note: produce model files only for new populations.
        itersuffix		= strcat(['_' num2str(idx1) '_' num2str(idx2)]);
        modlFileName	= strcat([DataFiles.name	itersuffix modlSuffix]);
        if ~Expt.initialize
            Expt = GenModlFile(Expt,modlFileName,idx1,idx2,Expt.initialize,OldExpt);
        else
            Expt = GenModlFile(Expt,modlFileName,idx1,idx2,Expt.initialize);
        end
        for idx3=1:Expt.idx3Max % stim current intensities)
            % Define stim filenames for each trial. Leading 0 in stim idx
            % allows proper sorting of output files.
            itersuffix		= strcat(['_' num2str(idx1) '_' num2str(idx2) '_' num2str(idx3,'%02d')]);
            stimFileName	= strcat([ DataFiles.name	itersuffix stimSuffix]);
            
            %% Produce stim file
            Expt = GenStimFile_ITD(Expt,stimFileName,idx1,idx2,idx3);
            
            for idxFiberSplitter = 1:fibersplitter
                
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.modlFilename     = modlFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.stimFilename     = stimFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.optsFilename     = optsFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.outputfilename	= [DataFiles.name itersuffix '_' num2str(idxFiberSplitter)];
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.seed 			= randi(intmax('uint32'));
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.mcrper			= Expt.opts.monte;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.numfibs         = Expt.opts.numFibers/fibersplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.firstfib         = idxFiberSplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.fibskip         = fibersplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.fiblist         = idxFiberSplitter:fibersplitter:Expt.opts.numFibers;
                
                ajob = Expt.jobs{idx1,idx2,idx3,idxFiberSplitter};
                
                fprintf(fid, format, ajob.modlFilename, ajob.stimFilename, ajob.optsFilename, ajob.outputfilename, ...
                    ajob.seed, ajob.mcrper, ajob.numfibs, ajob.firstfib, ajob.fibskip);
                
            end %for idxFiberSplitter = 1:fibersplitter
        end %for idx3=1:Exp.idx3Max
    end %for idx2=1:Exp.idx2Max
end %for idx1=1:Exp.idx1Max
fclose(fid); % close command file
%% ------------------------------------------------------------------------
% Finish up
%--------------------------------------------------------------------------
cd(ConfigFiles.path)% all other paths are relative to thisFiles.path, so go back to origin
save(Expt.fnames.DataFiles.ExptFile, 'Expt', '-v7');
Expt = SubmitJob(Expt);
Expt = orderfields(Expt);

end % function SFExptConfig
