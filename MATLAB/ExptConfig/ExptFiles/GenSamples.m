% This is a template script to set Expt options and run the full
% configuration script SFExptConfig. There are many more parameters that
% can can be modified and with, relatively little effort, defined here for
% ease of use.

Expt.ExpType = 'Normal';
Expt.Machine = 'Dell';

% Define estimates to use
% load('/gscratch/stf/jresnick/Combined.mat');
load('/home/jesse/Documents/Rubinstein/2017SimPaper/2017_06_14/Data/Combined.mat');
Expt.threshEstimates = Trial{3}.thresh; % Required for all experiment types
Expt.RSEstimates = Trial{3}.RS; % Required for Full, Fix, and volt ExpTypes

% Define parameters to use for 'Fix' and 'Scan-Fix' ExpTypesy
Expt.cutoffs = [];
Expt.dtoDs = [];

% DataFiles directory information will be used to save experiment
% results
[DataFiles.path, DataFiles.name, DataFiles.ext] = fileparts(mfilename('fullpath'));
if isempty(DataFiles.ext)
    DataFiles.ext = '.m';
end
DataFiles.fullPath = [fullfile(DataFiles.path, DataFiles.name) DataFiles.ext];
Expt.fnames.DataFiles  = DataFiles;

% Define stimulus type.
Expt.opts.stimtype = 'CF';

% Define location of main simulation code repository (machine specific).
switch Expt.Machine
    case 'Hyak'
        cd /gscratch/stf/jresnick/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
    case 'Julep'
        cd /media/datadrive/Rub_Code/Simulation/MATLAB/PDM_Param_JR
    case 'Dell'
        cd /home/jesse/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
end
        
Expt = SFExptConfig(Expt);
