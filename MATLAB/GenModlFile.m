function GenModlFile(modlFileName,Expt,fiberPop,univParams,Opts)
%GenModlFile: This function accepts an Expt structure, filename, and trial
%indices from ExptConfig then produces a .modl file containing model
%parameters to pass to cnerve.
%
% Usage: Expt = GenModlFile(Expt,modlFileName,idx1,idx2,idx3,Initialize)
% idx1-demyelination severity mean
% idx2-demyelination severity sigma
% idx3-current intensity
% Initialize is binary flag indicating either a new experiment (1) or a rerun
% (0).

%% Read fiber-dependent Information from passed fiberPop struct
    zs =        fiberPop.zs;
    maxNodes =  fiberPop.maxNodes;
    diameters = fiberPop.diameters;
    numnodes =  fiberPop.numNodes;
    demydtoD =  fiberPop.dtoD;
    cutoff =    fiberPop.cutoff;

fiberlist 	= [numnodes', diameters', zs', demydtoD', cutoff'];

%% Read fiber-independent parameters from univParam struct. 
numFibers = univParams.numFibers;
normdtoD = univParams.normdtoD; 
insegs = univParams.insegs;
LtoD = univParams.LtoD;
nlen = univParams.nlen;
constrict = univParams.constrict;

% Electricophysiological properties
E_rest = univParams.E_rest;
% Naf
Naf_eRev    = univParams.Naf_eRev;
Naf_density = univParams.Naf_density;
Naf_gMax    = univParams.Naf_gMax;
% Nap
Nap_eRev    = univParams.Nap_eRev;
Nap_density = univParams.Nap_density;
Nap_gMax    = univParams.Nap_gMax;
% Ks
Ks_eRev = univParams.Ks_eRev;
Ks_density = univParams.Ks_density;
Ks_gMax = univParams.Ks_gMax;
% Kf
Kf_eRev = univParams.Kf_eRev;
Kf_density = univParams.Kf_density;
Kf_gMax = univParams.Kf_gMax;
% Node electric
rmnod = univParams.rmnod;
cmnod = univParams.cmnod;
ranod = univParams.ranod;
% Myelin electric
ramy = univParams.ramy;
rmmy = univParams.rmmy;
cmmy = univParams.cmmy;

% Channel gating properties
% Naf
a_m_A = univParams.a_m_A; a_m_B = univParams.a_m_B; a_m_C = univParams.a_m_C;
b_m_A = univParams.b_m_A; b_m_B = univParams.b_m_B; b_m_C = univParams.b_m_C;
a_h_A = univParams.a_h_A; a_h_B = univParams.a_h_B; a_h_C = univParams.a_h_C;
b_h_A = univParams.b_h_A; b_h_B = univParams.b_h_B; b_h_C = univParams.b_h_C;
% Nap
a_p_A = univParams.a_p_A; a_p_B = univParams.a_p_B; a_p_C = univParams.a_p_C;
b_p_A = univParams.b_p_A; b_p_B = univParams.b_p_B; b_p_C = univParams.b_p_C;
% Kf
a_n_A = univParams.a_n_A; a_n_B = univParams.a_n_B; a_n_C = univParams.a_n_C;
b_n_A = univParams.b_n_A; b_n_B = univParams.b_n_B; b_n_C = univParams.b_n_C;
% Ks
a_s_A = univParams.a_s_A; a_s_B = univParams.a_s_B; a_s_C = univParams.a_s_C;
b_s_A = univParams.b_s_A; b_s_B = univParams.b_s_B; b_s_C = univParams.b_s_C;
% PATLAK PARAMS
k1 = univParams.k1; k2 = univParams.k2; k3 = univParams.k3;
%% Recording and stimulation options.
posAbove = Opts.posAbove;
posRecord = Opts.posRecord;
ECAPheight	= Opts.ECAPheight;	%#ok<*NASGU> % UNUSED
ECAPaway	= Opts.ECAPaway;    % UNUSED

%% ------------------------------------------------------------------------------
% ACTIVITY OPTIONS
%--------------------------------------------------------------------------------
FileName = [Expt.fnames.DataFiles.path modlFileName];
save( FileName ,'numFibers','maxNodes','insegs','E_rest',...
    'Naf_eRev', 'Naf_density', 'Naf_gMax', ...
    'Nap_eRev', 'Nap_density', 'Nap_gMax', ...
    'Kf_eRev', 'Kf_density', 'Kf_gMax', ...
    'Ks_eRev', 'Ks_density', 'Ks_gMax', ...
    'a_m_A','a_m_B','a_m_C','b_m_A','b_m_B','b_m_C',...
    'a_h_A','a_h_B','a_h_C','b_h_A','b_h_B','b_h_C',...
    'a_n_A','a_n_B','a_n_C','b_n_A','b_n_B','b_n_C',...
    'a_s_A','a_s_B','a_s_C','b_s_A','b_s_B','b_s_C',...
    'a_p_A','a_p_B','a_p_C','b_p_A','b_p_B','b_p_C',...
    'k1','k2','k3',...
    'rmnod','cmnod','ranod','rmmy','cmmy','ramy',...
    'normdtoD','LtoD','nlen','constrict',...
    'ECAPheight','ECAPaway','posAbove','posRecord','fiberlist','-ascii');

end

