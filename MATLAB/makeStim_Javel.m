function [time, stim] = makeStim_Javel(type,delt,wait,IPI,pw)
% Creates a stimulus list for either monophasic or biphasic square pulses.
% Biphasic pulses are like those used in Shepherd and Javel Correlating
% Cochlear Status paper. He used pulse trains, we use a single impulse.
% type: Switch to define phases and order of pulses.
% delt:		(optional) timestep of the simulation program, in [ms]
%       defualt: 0.001 ms == 1e-6 sec.
% totDur:	(optional) duration of the generated stumulus, in [sec]
%		default: 10e-3.
%
%
%   Author: Elle O'Brien
%

if ~exist('delt', 'var')
    delt = 0.001;			% timestep, in ms
end
if ~exist('wait', 'var')
    wait		= 2e-3;	% = 2 ms, no need to go longer...for normal fibers.
end
if ~exist('pw','var')
    pw = 100; %in us
end
if ~exist('IPI','var')
    IPI = 0;
end

% to keep in same dimension as cnerve, convert msec to seconds, and then to Hz
Fs		= 1/ (delt*1e-3);

delay = 100; % wait a ms delay from onset (in us)

time	= 1/Fs:1/Fs:wait+delay/Fs+2*pw/Fs+IPI/Fs;

buffer = round(wait); % How long to wait after pulse, in us
if IPI
    switch lower(type)
        case 'cf'
            stim = [0 delay; -1 pw; 0 IPI; 1 pw; 0 buffer];
        case 'af'
            stim = [0 delay; 1 pw; 0 IPI; -1 pw; 0 buffer];
        case 'cathodic'
            error('No IPI for monophasic pulses.')
        case 'anodic'
            error('No IPI for monophasic pulses.')
        otherwise
            error('Uknown stimulus type.')
    end
else
    switch lower(type)
        case 'cf'
            stim = [0 delay; -1 pw; 1 pw; 0 buffer];
        case 'af'
            stim = [0 delay; 1 pw; -1 pw; 0 buffer];
        case 'cathodic'
            stim = [0 delay; -1 pw; 0 buffer];
        case 'anodic'
            stim = [0 delay; 1 pw; 0 buffer];
        otherwise
            error('Uknown stimulus type.')
    end
end
end % function [time, stim] = makeStim_Javel
