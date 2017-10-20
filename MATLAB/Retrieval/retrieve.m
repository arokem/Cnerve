function retrieve(FileName)

% This is a script for submitting job retrieval on the cluster.

% Load Expt data structure.
load(FileName);
[thisFiles.path, thisFiles.name, thisFiles.ext] = fileparts(FileName);
dataDir = strcat(thisFiles.path,'/',thisFiles.name,'/');
cd(dataDir);
tmpDataFile = strcat(thisFiles.path,'/',thisFiles.name,'tmp','.mat');

% Add necessary .m files to path
addpath(genpath('/gscratch/stf/jresnick/CnerveRepo/Simulation/MATLAB/'));
addpath(genpath('/gscratch/stf/jresnick/CnerveRepo/Analysis/'));

% Call retrievedFinished to extract spiking data from spks binaries and
% return it in the Expt stucture.

switch Expt.ExpType
    case {'Volt','Normal'}
        [Expt,SpikesData,SpikeNodeData,VoltageData] = retrieveFinished(Expt); %#ok<*ASGLU>
        save(tmpDataFile,'Expt','SpikesData','SpikeNodeData','VoltageData','-v7.3');
    otherwise
        [Expt,SpikesData,SpikeNodeData] = retrieveFinished(Expt); %#ok<*ASGLU>
        save(tmpDataFile,'Expt','SpikesData','SpikeNodeData','-v7.3');
end
fprintf('Saved.\n');

% Calculate first order parameters from experimental data and save them to
% a new parameter file.
switch Expt.ExpType
    case {'Full-Scan','Scan-Fix'}
        [latency, jitter] = LatJitComputeParam(Expt,SpikeNodeData);
        thresh = RoughScanThresh(Expt,latency); %#ok<*NASGU>
        ParamFileName = strcat(thisFiles.path,'/',thisFiles.name,'param','.mat');
        save(ParamFileName,'thresh','latency','jitter');
    case 'Multi'
        stim = popstims(Expt); 
        [fiberFE, popFE] = popfe(SpikeNodeData,Expt);
        [popThresh,popRS,fiberThresh,fiberRS] = popCDFfit(Expt,popFE,fiberFE,stim);
        [latency, jitter] = popTiming(Expt,SpikeNodeData);
        ParamFileName = strcat(thisFiles.path,'/',thisFiles.name,'param','.mat');
        save(ParamFileName,'stim','latency','jitter',...
            'fiberFE','popFE','popThresh','popRS','fiberThresh','fiberRS');
    otherwise
        [FE, stim, thresh] = FEcomputePDMparam(Expt,SpikeNodeData);
        [latency, jitter] = LatJitComputeParam(Expt,SpikeNodeData);
        RS = Relative_Spread_JR(stim,FE);
        ParamFileName = strcat(thisFiles.path,'/',thisFiles.name,'param','.mat');
        save(ParamFileName,'FE','stim','dtoDs','extents','thresh','latency','jitter','RS');
end

end
