function [time,cumTime, stim] = makeStim_Shannon92(type, mi, mf, delt,IPI,pw,totDur)
%
% type:		{'AM', 'amplitude', 
%			 'PW', 'pulsewidth'}
%
%			Specifies what type of modulation to perform on the signal.
%			Shannon used pulse width modulation to SIMULATE amplitude
%			modulation, as a way to overcome technical limitations of the
%			hardware.
%				Alternatively, one could use a true, amplitude modulated
%			signal, as was the original intent of the experiment
%
% mf:		Modulation frequency desired	[Hz]
% mi:		Attenuation depth [0-1]
% delt:		(optional) timestep of the simulation program, in [ms] (usu. = 0.001 ms == 1e-6 sec)
% totDur:	(optional) duration of the generated stumulus, in [sec]
%				default: 400e-3.
%
%
%   Author: Nikita Imennov		<imennov@uw.edu>
%


if ~exist('delt', 'var')
	delt = 0.001;			% timestep, in ms
end
if ~exist('totDur', 'var')
	totDur		= 400e-3;	% = 400 ms
end

PRF			= 2000;		% pps
pulseWidth	= pw*1e-6;	% us converted to seconds
ipgSamples  = IPI; % interphase gap:

% to keep in same dimension as cngrid, convert msec to seconds, and then to Hz
Fs		= 1/ ( delt * 1e-3 );	
time	= 1/Fs:1/Fs:totDur;
cumTime = cumsum(time);


switch lower(type)
	case {'am', 'amplitude'}
		% use amplitude modulation to produce stimuli
		stim = modulateAM(  mi, ...		% modulation index
							mf, ...		% modulation frequency
							Fs);		% sampling freq, in Hz

	case {'pw', 'pulsewidth'}
		% use Shannon's workaround to achieve amplitude modulation-like
		% effects by performing pulse width modulation

		% get shannon's stimulus
		% pass on modFreq and depth
		stim = modulatePW(	mi, ...		% mi
							mf, ...		% mf
							Fs);		% sampling freq, in Hz
	otherwise
		error('Unknown stimulus type requested for Shannon''s 1992 experiment');
end


	function stim = modulateAM(mi, mf, Fs)
	%
	% amplitude modulation (1+ mi*sin(2*pi*mf))
	%
	% mf: modulation frequency
	% mi: modulation index
	%
	% GENERATES:
	% 1000 pps pulse train, totDur ms in duration, 100 us biphasic pulses

	%% set basic parameters
	pulseSamples= round(pulseWidth .* Fs);

	
	t = 1/PRF:1/PRF:totDur;	% prepare data for modulation


	% this is what we're modulating, the amplitude (about 1)
	amp		= 1 + mi*sin(2*pi*mf*t);
	stim	= [];

	for i = 1:size(amp, 2)
		if ipgSamples
			biPulse = [ amp(i)	pulseSamples; ...
						0		ipgSamples; ...
						-amp(i)	pulseSamples ];
		else
			biPulse = [ amp(i)	pulseSamples; ...
						-amp(i)	pulseSamples ];
		end

		%% add one complete pulse cycle to stimulus
		ipiSamples	= round((Fs/PRF) - sum(biPulse(:,2)));	% padding b/w biphasic pulses

		stim		= [stim; biPulse; 0 ipiSamples];
	end






	end % function stim = modulateAM(mi, mf, Fs)

	function stim = modulatePW(mi, mf, Fs)
	%
	% modulate duration of the pulse
	%
	% mf: modulation frequency
	% mi: modulation index
	%
	% GENERATES:
	% 1000 pps pulse train, totDur ms in duration, 100 us biphasic pulses

	%% set basic parameters
	
	t = 1/PRF:1/PRF:totDur;	% prepare data for modulation

	% this is what we're modulating, pulse duration (about 100 us)
	pulseDur	= pulseWidth * (1 + mi*sin(2*pi*mf * t) );
	pulseSamples= round(pulseDur .* Fs);
	
	%Fs			= lcm(PRF, round(1/(min(pulseDur))));	% sampling rate for the signal

	stim = [];
	for i = 1:size(pulseDur, 2)
		
		if ipgSamples
			biPulse = [	1	pulseSamples(i); ...
						0	ipgSamples; ...
						-1	pulseSamples(i) ];
		else	% if no pulse gap, don't write it
			biPulse = [	1	pulseSamples(i); ...
						-1	pulseSamples(i) ];
		end
		
		%% add one complete pulse cycle to stimulus
		ipiSamples  = round((Fs/PRF) - sum(biPulse(:,2)));	% padding between biphasic pulses
		
		stim		= [stim; biPulse; 0 ipiSamples];
	end
	end % function [time, stim] = modulatePW(mi, mf, Fs)
end % function [time, stim] = makeStim_Shannon92(type, mi, mf, delt)
