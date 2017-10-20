function Expt = GenStimFile_ITD(Expt,stimFileName,idx1,idx2,idx3)
%GenStimFile: This function accepts an Expt structure, filename, and trial
%indices from ExptConfig then produces a .stim file containing stimulation
%parameters to pass to cnerve.
% Usage: Expt = GenStimFile(Expt,stimFileName,idx1,idx2,idx3)

%% --------------------------------------------------------------------------------
% DEFINE STIMULUS
%--------------------------------------------------------------------------------
if ~isfield(Expt.opts,'wait')
    Expt.opts.wait = 1e-3; % Sets interpulse interval between monte runs. Increase for 'Volt' ExpType
end

% stim.current	= 0.595;	% in mA, @ 50% FE for 1.5um fiber
% stim.current	= 0.795;	% in mA, get good response in 2.0um fib w/this
stim.current = Expt.stimList(idx1,idx2,idx3);
delt = Expt.opts.delt;
wait = Expt.opts.wait;
pps  = Expt.opts.pps;
stimDur = Expt.opts.stimDur;

[stim.time, stim.pulselist] = makeStim_ITDpulsetrain(wait,delt,pps,stimDur);

%% --------------------------------------------------------------------------------
% ELECTRODE PARAMETERS
%--------------------------------------------------------------------------------

stim.rad 		= 0.001;  	% radius of disc electrode (mm)
stim.resmed		= 25000;	% resistivity of medium (bone) (units??)

%%  For version 2.2 and above of cnerve stimlist, we can explicitly
%   specify the # of entries in the pulselist.  For compatibility's sake,
%   we code the # as negative, so as to recognise between current that
%   comes in pre-2.2 versions first, and neg. pulselist that comes in
%   post-2.2 cnerve versions first.
%
%	In upcoming V3 of code, we will specify the number of list entries w/o
%	negating it.

stim.listLen		= -length(stim.pulselist);

FileName = [Expt.fnames.DataFiles.path stimFileName];

save(FileName,'listLen','current','rad','resmed', 'pulselist',...
    '-struct', 'stim', '-ascii')

Expt.fibers{idx1,idx2}.stim{idx3} = stim;
end