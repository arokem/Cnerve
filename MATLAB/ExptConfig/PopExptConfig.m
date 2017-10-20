function Expt = PopExptConfig(Expt)
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

%% ------------------------------------------------------------------------
% FILE NAMING CONVENTIONS
%--------------------------------------------------------------------------
% ConfigFiles directory information will be used to add the necessary scripts
% and functions to the path.
[ConfigFiles.path, ConfigFiles.name, ConfigFiles.ext] = fileparts(mfilename('fullpath'));
ConfigFiles.fullPath = [fullfile(ConfigFiles.path, ConfigFiles.name) ConfigFiles.ext];
Expt.fnames.ConfigFiles = ConfigFiles;

addpath(genpath('..'));

[~,Expt.git_hash_string] = system('git rev-parse HEAD');

% PathDefining accepts Expt and returns the necessary data directory
% information based on the Machine being used and Expt.DataFiles location.
% It also copies the relevant executable and this configuration function
% into Expt.dataDir.
Expt = PathDefining(Expt);
% Store DataFiles directory information in easy to access variable.
DataFiles = Expt.fnames.DataFiles;

modlSuffix	= '.modl';
stimSuffix	= '.stim';
optsSuffix	= '.opts';

optsFileName	= strcat([ Expt.name	optsSuffix]);
GenOptsFile(Expt,optsFileName);

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
%% --------------------------------------------------------------------------------
% MAIN-- GENERATE EACH TRIAL INSIDE THE 3 IDX LOOPS
%--------------------------------------------------------------------------------
for idx1=1:Expt.idx1Max % demy mean severity index
    for idx2=1:Expt.idx2Max % demy severity variance
        
        %% Produce model file:
        % Note: produce model files only for new populations.
        itersuffix		= strcat(['_' num2str(idx1) '_' num2str(idx2)]);
        modlFileName	= strcat([Expt.name	itersuffix modlSuffix]);
        GenModlFile(modlFileName,Expt,Expt.fibers{idx1,idx2},Expt.univParams,Expt.opts);
        for idx3=1:Expt.idx3Max % stim current intensities)
            % Define stim filenames for each trial. Leading 0 in stim idx
            % allows proper sorting of output files.
            itersuffix		= strcat(['_' num2str(idx1) '_' num2str(idx2) '_' num2str(idx3,'%02d')]);
            stimFileName	= strcat([ Expt.name	itersuffix stimSuffix]);
            
            %% Produce stim file
            Expt = GenStimFile(Expt,stimFileName,idx1,idx2,idx3);
            fibersplitter = Expt.opts.fibersplitter;
            for idxFiberSplitter = 1:fibersplitter
                
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.modlFilename     = modlFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.stimFilename     = stimFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.optsFilename     = optsFileName;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.outputfilename	= [Expt.name itersuffix '_' num2str(idxFiberSplitter)];
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.seed 			= randi(intmax('uint32'));
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.mcrper			= Expt.opts.monte;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.numfibs         = Expt.univParams.numFibers/fibersplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.firstfib         = idxFiberSplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.fibskip         = fibersplitter;
                Expt.jobs{idx1,idx2,idx3,idxFiberSplitter}.fiblist         = idxFiberSplitter:fibersplitter:Expt.univParams.numFibers;
                
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
