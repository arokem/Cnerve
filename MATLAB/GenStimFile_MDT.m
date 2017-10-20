function Expt = GenStimFile_MDT(Expt,stimFileName,idx1,idx2,idx3)
%GenStimFile: This function accepts an Expt structure, filename, and trial
%indices from ExptConfig then produces a .stim file containing stimulation
%parameters to pass to cnerve.
% Usage: Expt = GenStimFile(Expt,stimFileName,idx1,idx2,idx3)

%% --------------------------------------------------------------------------------
% DEFINE STIMULUS
%--------------------------------------------------------------------------------
try
    stimMin = Expt.stim.stimMin;
    stimMax = Expt.stim.stimMax;
    CDlist	= linspace(stimMin,stimMax,Expt.idx3Max);
    stim.current	= CDlist(idx2);
catch
    error(['Not enough information to generate stimulus list. ',...
        'Provide stimMin and stimMax to Expt.'])
end

mi = Expt.miList(idx3);
mf = Expt.miList(idx1);

type = Expt.stim.stimtype;
delt = Expt.opts.delt;
IPI  = Expt.stim.IPI;
pw   = Expt.stim.pw;
totalDur = Expt.stim.totalDur;

[stim.time,~,stim.pulselist] = makeStim_Shannon92(type,mi,mf,delt,IPI,pw,totalDur);

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

Expt.stim.trial{idx1,idx2,idx3} = stim;
end