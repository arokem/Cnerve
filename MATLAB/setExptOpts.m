function Opts = setExptOpts(Opts,univParams)
%setExptOpts: This provides a user interface for setting experimental
%options.

%% ------------------------------------------------------------------------
%DEFINE SIMULATION OPTIONS (Need to move some outside function into config script??)
%--------------------------------------------------------------------------
Opts.V_th      = 50.0;		% Spike threshold above E_rest
Opts.TypeNa		= 0;		% 0) use default Na chan model (Gillespie),
% 1) use Patlak (probably don't want)
% 2) use Gillespie
% 3) use Jeff's model (DON'T USE!)
Opts.UsepNa		= 0;
Opts.UseK			= 1;
Opts.UseSlowK		= 1;
if ~isfield(Opts,'RecordVoltage'); Opts.RecordVoltage	= 0; end
Opts.RecordECAP	= 0;
Opts.VoltageSample = 0.01;  % [ms] Defines the voltage sampling timestep.
Opts.delt      	= .001;  % time step of simulation
Opts.MaxVoltage    = 1500; % Voltage at which cnerve throws an error. Typically 1500 mV for a normal fiber
Opts.MaxSpikes		= 100;      % Needs to be modified based on type of stimulus

Opts.is_active     = 1;
Opts.is_stochastic = 1;

if ~isfield(Opts,'monte'); Opts.monte = 10; end


% decrease the number of jobs we need to spawn by assignying more than one
% MC simulation and/or more than one fiber per cnerve program.
numFibers = univParams.numFibers;
Opts.fiberperjob = 10;
Opts.fibersplitter	= numFibers/Opts.fiberperjob;
if rem(numFibers, numFibers/Opts.fibersplitter )
    error('Non-integer number of fibers assigned to each job: %.2f', fibersplitter);
end

% Pull necessary quantities from univParams struct for calculations.
LtoD = univParams.LtoD;
diamMean = univParams.diamMean;
normdtoD = univParams.normdtoD;
nlen = univParams.nlen;

% Define electrode, recording, and eCAP recording lcoations.
Opts.meanNodeAbove = 10;
Opts.posAbove = -1*(LtoD*diamMean/normdtoD+nlen)*Opts.meanNodeAbove;
Opts.meanRecordNode = 32;
Opts.posRecord = Opts.meanRecordNode/36; %36 is mean fiber length
Opts.nodeint        = Opts.posRecord;
Opts.ECAPheight	= 6;				%#ok<*NASGU> % UNUSED
Opts.ECAPaway	= (10)*0.231;		% UNUSED
end

