% This is a template script to set Expt options and run the full
% configuration script SFExptConfig. There are many more parameters that
% can can be modified and with, relatively little effort, defined here for
% ease of use.

clear Expt;
Expt.ExpType = 'Multi';
% Are we initializing a new population or rerunning an existing one?
Expt.initialize = 0;
Expt.Machine = 'Hyak';

if Expt.initialize
    % Define parameters to use for parameter sweep. Here demySev mu and sigma
    % describe the mean and variance of the normal distribution that the
    % demyelination severities are drawn from to produce probabalistic
    % demyelination extent and severity. Note: currently, the same severity is
    % used to adjust both severity and extent.
    Expt.demySevmu = [0,0.5,.75,0.9,0.95];
    Expt.idx1Max = length(Expt.demySevmu);
    Expt.demySevsigma = [0.01,0.1,0.25,0.5];
    Expt.idx2Max = length(Expt.demySevsigma);
    Expt.opts.numFibers = 200;
end

% Define stimulus intensity bounds. Will be (idx1,idx2) array. Will likely
% use thresh and RS from previous trial for many experiments. 
Expt.stimMin = 0.3 * ones(Expt.idx1Max*Expt.idx2Max);    % mA
Expt.stimMax = 5 * ones(Expt.idx1Max*Expt.idx2Max);     % mA
Expt.idx3Max = 30;      % # of stimulus intensities to test

Expt.opts.monte = 1;    % # of monte carlo simulations to run
Expt.opts.fiberperjob = 200;

Expt.opts.RecordVoltage	= 0;

% DataFiles directory information will be used to save experiment
% results
[DataFiles.path, DataFiles.name, DataFiles.ext] = fileparts(mfilename('fullpath'));
if isempty(DataFiles.ext)
    DataFiles.ext = '.m';
end
DataFiles.fullPath = [fullfile(DataFiles.path, DataFiles.name) DataFiles.ext];
Expt.fnames.DataFiles  = DataFiles;

% Define stimulus type and post-stimulation wait period.
Expt.opts.stimtype = 'AF';
Expt.opts.wait = 2.5e-3;

% Define location of main simulation code repository (machine specific).
switch Expt.Machine
    case 'Hyak'
        cd /gscratch/stf/jresnick/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
    case 'Julep'
        cd /home/jesse/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
    case 'Dell'
        cd C:\Users\jsere\Documents\cnerverepo\Simulation\MATLAB\PDM_Param_JR
end
     
Expt = PopExptConfig(Expt);
