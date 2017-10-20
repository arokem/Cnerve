function [time,stim] = makeStim_whiteNoise(thresh,sigma, delt, stimDur)
%
%
% Author: Jesse Resnick, 2017

if ~exist('delt', 'var')
    delt = 0.001;			% timestep, in ms
end
if ~exist('totDur', 'var')
    stimDur		= 300e-3;	% = 300 ms
end

% to keep in same dimension as cngrid, convert msec to seconds, and then to Hz
wait    = 100e-3; %us
Fs		= 1/ ( delt * 1e-3 );
time	= 1/Fs:1/Fs:stimDur+wait;

%% set basic parameters
pps         = 2000;          % pulses per second = 1 kHz
pulseWidth	= 25e-6;	% seconds
ipgSamples  = 8;            % interphase gap
pulseSamples= round(pulseWidth .* Fs);

numPulses = stimDur*pps;
% amp set at thresh or MCL determined via I/O curves.
    amp		= thresh + sigma*randn(numPulses); % mA
stim	= [0 wait*1e3];


for i = 1:numPulses
    if ipgSamples
        biPulse = [ amp(i)     pulseSamples; ...
                    0		ipgSamples; ...
                    -amp(i)	pulseSamples ];
    else
        biPulse = [ amp(i)     pulseSamples; ...
                    -amp(i)    pulseSamples ];
    end
    
    %% add one complete pulse cycle to stimulus
    ipiSamples	= round((Fs/pps) - sum(biPulse(:,2)));	% padding b/w biphasic pulses
    
    stim		= [stim; biPulse; 0 ipiSamples];
end
    stim(:,3) = [0; cumsum(stim(1:end-1,2))];
end % function [time, stim] = makeStim_Shannon92(type, mi, mf, delt)
