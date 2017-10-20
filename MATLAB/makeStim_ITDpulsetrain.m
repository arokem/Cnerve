function [time,stim] = makeStim_ITDpulsetrain(wait,delt,pps,stimDur)
% This function makes a pulse train for use in ITD studies like that used
% by in: Todd, A.E., Goupell, M.J., Litovsky, R.Y., 2016. Ear Hear. 1–14.
%
% USAGE: [time,stim] = makeStim_ITDpulsetrain(amp, stimDur, delt)
%
% Default amp = 0.5 mA, delt = 0.001 us, and stimDur = 300 ms. Time is a
% vector containing simulation time values while stim is a vector of form:
%       [ I1 dur1;
%         I2 dur2 
%        ... ...];
%
% Author: Jesse Resnick, 2017

% Check that 
if ~exist('delt', 'var');   delt    = 0.001; end	% timestep, in ms
if ~exist('stimDur', 'var'); stimDur = 100e-3; end	% = 300 ms
if ~exist('pps', 'var');    pps     = 100; end      % pulses per second
if ~exist('wait', 'var');   wait	= 1000e-3; end  % pre-pulse wait (ms)

% to keep in same dimension as cngrid, convert msec to seconds, and then to Hz
Fs		= 1/ ( delt * 1e-3 );
time	= 1/Fs:1/Fs:stimDur+wait;

%% set basic parameters
pulseWidth	= 25e-6;	% seconds
ipgSamples  = 8;            % interphase gap
pulseSamples= round(pulseWidth .* Fs);

% amp set at thresh or MCL determined via I/O curves.
if ~exist('amp', 'var')
    amp		= 0.5; % mA
end
stim	= [0 wait*1e3];
numPulses = stimDur*pps;

for i = 1:numPulses
    if ipgSamples
        biPulse = [ 1     pulseSamples; ...
                    0		ipgSamples; ...
                    -1	pulseSamples ];
    else
        biPulse = [ 1     pulseSamples; ...
                    -1    pulseSamples ];
    end
    
    %% add one complete pulse cycle to stimulus
    ipiSamples	= round((Fs/pps) - sum(biPulse(:,2)));	% padding b/w biphasic pulses
    
    stim		= [stim; biPulse; 0 ipiSamples];
end
    % stim(:,3) = [0; cumsum(stim(1:end-1,2))];
end % function [time, stim] = makeStim_Shannon92(type, mi, mf, delt)
