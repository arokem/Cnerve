function Expt = MultiReRun(ExptName,LastFileName,StimsFile,Machine,Ini)
%   Usage: Expt = multiReRun(ExptName,LastFileName,MinMaxFile,Machine,Ini);
%
% This function sets Expt options and rusn the full
% configuration script PopExptConfig. There are many more parameters that
% can can be modified and with, relatively little effort, defined here for
% ease of use.

clear Expt;
Expt.ExpType = 'Multi';
% Are we initializing a new population or rerunning an existing one?
Expt.initialize = Ini;
Expt.Machine = Machine;

if ~Expt.initialize
    load(LastFileName);
    load(StimsFile);
    Expt.fnames.lastDir = Expt.fnames.DataFiles.path;
end

if Expt.initialize
    % Define parameters to use for parameter sweep. Here demySev mu and sigma
    % describe the mean and variance of the normal distribution that the
    % demyelination severities are drawn from to produce probabalistic
    % demyelination extent and severity. Note: currently, the same severity is
    % used to adjust both severity and extent.
    Expt.demySevmu = [0, 0;
                      0.5,0.5;
                      0.75,0.5; 0.75,0.75;
                      0.9,0.5;  0.9,0.9;
                      0.95,0.5; 0.95,0.95];
    Expt.idx1Max = length(Expt.demySevmu);
    Expt.demySevsigma = [0.01,0.01;
                         0.1,0.1];
    Expt.idx2Max = length(Expt.demySevsigma);
    Expt.opts.numFibers = 200;
end

% Define stimulus intensity bounds. Will be (idx1,idx2) array. Will likely
% use thresh and RS from previous trial for many experiments. 

if exist('stimList','var')
    Expt.stimList = stimList;
    Expt.idx3Max = size(stimList,3);
else
    Expt.stimMin = 0.15*ones(Expt.idx1Max,Expt.idx2Max);    % mA from MinMaxFile
    Expt.stimMax = 8*ones(Expt.idx1Max,Expt.idx2Max);     % mA
    Expt.idx3Max = 30;      % # of stimulus intensities to test
end

Expt.opts.monte = 10;    % # of monte carlo simulations to run
Expt.opts.fiberperjob = 50;

Expt.opts.RecordVoltage	= 0;

% CodeFiles directory information cane be used to refer back to code
[CodeFiles.path, CodeFiles.name, CodeFiles.ext] = fileparts(mfilename('fullpath'));
CodeFiles.fullPath = [fullfile(CodeFiles.path, CodeFiles.name) CodeFiles.ext];
Expt.fnames.CodeFiles  = CodeFiles;
Expt.fnames.DataFiles.name = ExptName;
cd(CodeFiles.path); cd('..');

% Define stimulus type and post-stimulation wait period.
Expt.opts.stimtype = 'AF';
Expt.opts.wait = 2.5e-3;
Expt.opts.IPI = 8; % Stimulus IPI.
Expt.opts.pw = 25; % Stimulus pw.

Expt = PopExptConfig(Expt);
end
