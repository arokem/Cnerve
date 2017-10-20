function MDT_Modular(ExptName,LastFileName,StimsFile,Machine,Ini)

% This is a template script to initialize a model, set Expt options and run
% the full configuration script.
fclose('all');
initialize = Ini;
elecParamType = 'orig';
gateParamType = 'SMhybrid';
if initialize
    % Define parameters to use for parameter sweep. Here demySev mu and sigma
    % describe the mean and variance of the normal distribution that the
    % demyelination severities are drawn from to produce probabalistic
    % demyelination extent and severity. Note: currently, the same severity is
    % used to adjust both severity and extent.
    demySevmu = [0, 0;
                0.5,0.5;
                0.75,0.5; 0.75,0.75;
                0.9,0.5;  0.9,0.9;
                0.95,0.5; 0.95,0.95];
    fibIdx1Max = length(demySevmu);
    demySevsigma = [0.01,0.01;
        0.1,0.1];
    fibIdx2Max = length(demySevsigma);
    univParams.numFibers = 200;
    univParams = setUnivParams(univParams.numFibers,elecParamType,gateParamType);
    anamProps = [univParams.diamMean, univParams.diamStdev];
    for i = 1:fibIdx1Max
        for j = 1:fibIdx2Max
            demySevParam = [demySevmu(i,:); demySevsigma(j,:)];
            Fibers{i,j} = initializePopulation(univParams.numFibers,demySevParam,anamProps); %#ok<SAGROW>
        end
    end
    save('Fibers.mat','Fibers','univParams');
else
    load(LastFileName); %Load in cell array of fibers created prev.
    univParams = setUnivParams(length(Fibers{1,1}.diameters),elecParamType,gateParamType);
    fibIdx1Max = size(Fibers,1);
    fibIdx2Max = size(Fibers,2);
end % Fiber initialization

maindir = pwd;
for i = 1:fibIdx1Max
    for j = 1:fibIdx2Max
        clear Expt;
	cd(maindir);
        Expt.name = sprintf([ExptName '%.2d_%.2d'],i,j);
        Expt.Machine = Machine;
        Expt.fibers = Fibers{i,j};
        Expt.univParams = univParams;
        Expt.opts = setExptOpts([],Expt.univParams);
        Expt.ExpType = 'Multi';

        % Define stimulus intensity bounds. Will be (idx1,idx2) array. Will likely
        % use thresh and RS from previous trial for many experiments.
        load(StimsFile);
        Expt.stim.stimMin = stimMins(i,j);    % mA
        Expt.stim.stimMax = stimMaxs(i,j);     % mA

        %MDT parameter space definitions
        Expt.mfList  =  100;    % Modulation frequencies to try
        Expt.idx1Max =  length(Expt.mfList); % idx1: mf
        Expt.idx2Max =  10;		% idx2: stim.currents
        Expt.idx3Max =  15;     % idx3: # mi values to test
        % Modulation intensities to test
        Expt.miList	 = [0 logspace(log10(.001),log10(.1), Expt.idx3Max-1 )];
        

        % Define stimulus type and post-stimulation wait period.
        Expt.stim.stimtype = 'AM';
        Expt.stim.wait = 2.5e-3;
        Expt.stim.IPI = 8;
        Expt.stim.pw = 25;
        Expt.stim.totalDur = 100e-3; %ms 

        % Define location of main simulation code repository (machine specific).
        switch Expt.Machine
            case 'Hyak'
                cd /gscratch/stf/jresnick/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
            case 'Julep'
                cd /home/jesse/CnerveRepo/Simulation/MATLAB/PDM_Param_JR
            case 'Dell'
                cd C:\Users\jsere\Documents\cnerverepo\Simulation\MATLAB\PDM_Param_JR
        end
        Expt = MDTExptConfig(Expt);
    end
end
