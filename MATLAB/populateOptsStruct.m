function Opts = setExptOpts(Opts)
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

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
Expt.opts.MaxSpikes		= 10;      % Needs to be modified based on type of stimulus

Expt.opts.is_active     = 1;
Expt.opts.is_stochastic = 1;

% Check for user defined number of monte carlos and then set based on
% experiment type.
if ~isfield(Expt.opts,'monte')
    Expt.opts.monte      = 10;
end

end

