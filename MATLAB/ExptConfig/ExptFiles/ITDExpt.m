function [Expt,OldExpt] = ITDExpt(ExptName,LastFileName,StimAmpsFile,Machine,Ini)
%   Usage: Expt = multiReRun(ExptName,LastFileName,MinMaxFile,Machine,Ini);
%
% This function sets Expt options and rusn the full
% configuration script PopExptConfig. There are many more parameters that
% can can be modified and with, relatively little effort, defined here for
% ease of use.

Expt.ExpType = 'ITDPulseTrains';
% Are we initializing a new population or rerunning an existing one?
Expt.initialize = Ini;
Expt.Machine = Machine;

if ~Expt.initialize
    OldExpt = load(LastFileName,'Expt');
    OldExpt = OldExpt.Expt;
    load(StimAmpsFile);
    Expt.fnames.lastDir = OldExpt.fnames.DataFiles.path;
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
else
    Expt.demySevmu = OldExpt.demySevmu;
    Expt.idx1Max = OldExpt.idx1Max;
    Expt.demySevsigma = OldExpt.demySevsigma;
    Expt.idx2Max = OldExpt.idx2Max;
    Expt.opts.numFibers = OldExpt.opts.numFibers;
end
% Other experimental options.
Expt.opts.monte = 10;    % # of monte carlo simulations to run
Expt.opts.fiberperjob = 4;
Expt.opts.RecordVoltage	= 0;

% Define amplitudes of pulseTrain. Will be (idx1,idx2,# amps) array. Will
% likely use thresh or half-maximal firing rates from single pulse expts.
if exist('ITDamps','var')
    Expt.stimList = ITDamps;
    % Make sure we have stim intensities for each fiber.
    assert(size(Expt.stimList,1)==Expt.idx1Max);
    assert(size(Expt.stimList,2)==Expt.idx2Max);
    Expt.idx3Max = size(ITDamps,3);
else
    error('Need to specify amplitude of pulse trains.')
end
% Define stimulus type and post-stimulation wait period.
Expt.opts.pps = 100;
Expt.opts.wait = 1; % Wait before stimulus (ms)
Expt.opts.stimDur = 100e-3; % Stimulus duration (s)

% CodeFiles directory information cane be used to refer back to code
[CodeFiles.path, CodeFiles.name, CodeFiles.ext] = fileparts(mfilename('fullpath'));
CodeFiles.fullPath = [fullfile(CodeFiles.path, CodeFiles.name) CodeFiles.ext];
Expt.fnames.CodeFiles  = CodeFiles;
Expt.fnames.DataFiles.name = ExptName;
cd(CodeFiles.path); cd('..');

Expt = ITDExptConfig(Expt,OldExpt);
end
