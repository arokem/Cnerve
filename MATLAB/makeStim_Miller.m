function [time, stim] = makeStim_Miller(delt, wait)
% Delivers a cathodic, rectangular pulse according to Charlie Miller's
% feline physiology work. For replication of Nikita's 2011 modeling paper.
% delt:		(optional) timestep of the simulation program, in [ms] (usu. = 0.001 ms == 1e-6 sec)
% totDur:	(optional) duration of the generated stumulus, in [sec]
%				default: 10e-3.
%
%
%   Author: Elle O'Brien
%


if ~exist('delt', 'var')
	delt = 0.001;			% timestep, in ms
end
if ~exist('totDur', 'var')
	wait		= 10e-3;	% = 10 ms
end




% to keep in same dimension as cnerve, convert msec to seconds, and then to Hz
Fs		= 1/ ( delt * 1e-3 );	
time	= 1/Fs:1/Fs:wait;

delay = 1000; % wait a ms delay from onset (in us)
pw = 39; %in us
buffer = round(wait*1e6); % How long to wait after pulse, in us

stim = [0 delay; -1 pw; 0 buffer];


	
end % function [time, stim] = makeStim_Miller
