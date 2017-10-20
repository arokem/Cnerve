function GenOptsFile(Expt,optsFileName) 
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here

FileName = [Expt.fnames.DataFiles.path optsFileName];
Opts = Expt.opts; %#ok<NASGU>
save(FileName,'TypeNa','UsepNa', 'UseK', 'UseSlowK','V_th', ...
    'RecordECAP','RecordVoltage','MaxSpikes','delt', ...
    'VoltageSample','MaxVoltage','is_active','is_stochastic',...
    '-struct','Opts','-ascii');
end

