function fiberPop = initializePopulation(numFibers,demySevParam,anamProps)

%%  Set Constant Anatomic Parameters
diamMean	= anamProps(1);	% use 2e-3 mm for population studies
diamStdev	= anamProps(2);   % use 0.5e-3 mm
normdtoD    = 0.6;      %dtoD of healthy neurons

diameters = normpdftoparams('fibnum',numFibers, 'avg', diamMean,...
    'std', diamStdev,'min',0.5e-3,'max',3.5e-3);

zs = ones(1,numFibers)*3; % Electrode distance from fiber.
numnodes = ceil(36./(diameters./diamMean));  % the number of active nodes in the nerve
maxNodes = max(numnodes);

%% Set demyelination state for each fiber
% Uses demySev mean and sigma to construct normpdf to pull random
% values from to assign the dtoD and cutoff for each fiber. Column 1
% contains mu and sigma values for dtoD and column 2 for cutoffs.
dtoDMu    = demySevParam(1,1);
dtoDSigma = demySevParam(2,1);

if dtoDSigma == 0
    dtoDSev = dtoDMu*ones(1,numFibers);
else
    dtoDSev = normpdftoparams('fibnum',numFibers, 'avg',...
        dtoDMu,'std',dtoDSigma,'min',0,'max',1);
end
demydtoD = normdtoD + 0.4*dtoDSev;

extentMu    = demySevParam(1,2);
extentSigma = demySevParam(2,2);
if extentSigma == 0
    demySev = extentMu*ones(1,numFibers);
else
    demySev = normpdftoparams('fibnum',numFibers, 'avg',...
        extentMu,'std',extentSigma,'min',0,'max',1);
end
cutoff = round(numnodes.*demySev);

%% -----------------------------------------------------------------------
% SAVE EXPERIMENT CONFIGURATION INFORMATION to Struct
%-------------------------------------------------------------------------
fiberPop.dtoDMu = dtoDMu;
fiberPop.dtoDSigma = dtoDSigma;
fiberPop.extentMu = extentMu;
fiberPop.extentSigma = extentSigma;
fiberPop.zs                   = zs;
fiberPop.maxNodes             = maxNodes;
fiberPop.diameters			= diameters;
fiberPop.numNodes             = numnodes;
fiberPop.dtoD                 = demydtoD;
fiberPop.cutoff               = cutoff;
end