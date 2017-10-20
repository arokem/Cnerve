function univParams = setUnivParams(numFibers,elecParamType,gateParamType)
% This function sets the parameters that apply to all fiber populations
% withing a set of experiments. If a parameter will change dynamically
% between fibers it should be moved to the makePop function. This structure
% is then passed into the Expt structure for each experiment and printed
% into Modl files for use by the C SGN simulation.
%
%   Usage: univParams = setUnivParams(numFibers,elecParamType,gateParamType)
%%  Set Constant Anatomic Parameters
univParams.numFibers = numFibers;
% diamMean	= 1.5e-3;	% use 1.5um for single-fib fitting studies
univParams.diamMean	= 2.0e-3;	% use 2um for population studies
univParams.diamStdev	= 0.5e-3;
univParams.normdtoD    = 0.6;      %dtoD of healthy neurons
univParams.insegs	= 9;			% the number of segments per internode
univParams.LtoD	= 92;			% the ratio of internode's length to its diameter
univParams.nlen	= .001;			% the length of a node  (mm)
univParams.constrict       = 0.50;		% anatomic constriction factor

%% Physiologic Parameters
switch elecParamType
    case 'orig'						% closest to schwarz' parameters?
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        E_rest		=-84.0;			% Resting Potential
        
        Naf_eRev	= 40.0;			% Na reversal potential
        Naf_density = 618e6;
        Naf_gMax	= 20e-12;		% Na single channel conductance
        
        Nap_eRev	= Naf_eRev;				% UNUSED
        Nap_density	= 0.01*Naf_density;		% UNUSED
        Nap_gMax	= Naf_gMax;				% UNUSED
        
        Ks_eRev		=-84.0;
        Ks_density	= 41168079;
        Ks_gMax		= 10e-12;
        
        Kf_eRev		= Ks_eRev;		% K reversal potential
        Kf_density	= 20371833;
        Kf_gMax		= Ks_gMax;		% K channel conductance per channel
        
        % Node of Ravier
        rmnod 	= 10 	* 831;		% Ohm-mm^2
        cmnod 	= 1/2 	* 0.041e-3;	% F/mm^2
        ranod 	= 0.69	* 1063; 	% Ohm-mm
        
        % Myelin Segments
        ramy	= 0.69	* 1063;		% Ohm-mm
        rmmy	= 6		* 209e6;	% Ohm-mm
        cmmy	= 1/11	* 1.6e-9;	% F/mm
        
    case 'nikita-v1'		% based on latest reported parameters
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        E_rest		=-84.0;		% Resting Potential
        
        Naf_eRev	= 50.0;			% Schwarz1995
        Naf_density = 1000e6;	% chan/mm^2, Waxman1993 (=1000 chan/um^2)
        Naf_gMax	= 15.2e-12;		% Scholz1993 (=15pS)
        
        Nap_eRev	= Naf_eRev;
        Nap_density	= 0.01*Naf_density;
        Nap_gMax	= Naf_gMax;
        
        Ks_eRev		=-75.0;			% Schwarz2006
        Ks_density	= 110e6;		% chan/mm^2, Safronov1993 (=110 chan/um^2)
        Ks_gMax		= 19.48e-12;	% Safronov1993's 10pS @ 22*C adjusted w/Q10 (1.56 in Lee1990) to 37*C
        
        Kf_eRev		= Ks_eRev;
        Kf_density	= Ks_density/15;	% (calculated, Imennov)
        Kf_gMax		= 74e-12;		% (calculated based on Schwarz2006's weighted K_I and K_F conductivity, Imennov)
        
        % Node of Ravier
        rmnod 	= 831;				% Ohm-mm^2
        cmnod 	= 2e-5;				% nF/mm^2
        ranod 	= 1063;				% Ohm-mm
        
        % Myelin Segments
        ramy	= 1063;							% Ohm-mm
        rmmy	= 1e6/(pi*diameters/dtoD);		% Ohm-mm
        cmmy	= 1e-6*(pi*diameters/dtoD);		% nF/mm
end% switch elecParams
%% Save electric parameters to univParams structure.
univParams.E_rest       = E_rest;
% Naf
univParams.Naf_eRev     = Naf_eRev;
univParams.Naf_density  = Naf_density;
univParams.Naf_gMax     = Naf_gMax;
% Nap
univParams.Nap_eRev     = Nap_eRev;
univParams.Nap_density	= Nap_density;
univParams.Nap_gMax     = Nap_gMax;
% Ks
univParams.Ks_eRev		=Ks_eRev;
univParams.Ks_density	= Ks_density;
univParams.Ks_gMax		= Ks_gMax;
% Kf
univParams.Kf_eRev		= Kf_eRev;
univParams.Kf_density	= Kf_density;
univParams.Kf_gMax		= Kf_gMax;
% Node electric
univParams.rmnod    = rmnod;
univParams.cmnod    = cmnod;
univParams.ranod    = ranod;
% Myelin electric
univParams.ramy     = ramy;
univParams.rmmy     = rmmy;
univParams.cmmy     = cmmy;
%% DEFINE GATING PARAMS

switch gateParamType
    case 'schwarz'
        a_m_A = 1.7^2.2*1.86; a_m_B =-18.4; a_m_C = 10.3;
        b_m_A = 1.7^2.2*.086; b_m_B =-22.7; b_m_C = 9.16;
        a_h_A = 1.7^2.9*.0336; a_h_B =-111; a_h_C = 11.0;
        b_h_A = 1.7^2.9*2.3;	b_h_B =-28.8;				b_h_C = 13.4;
        % Nap,												McIntyre0000
        a_p_A = 0.0353;				a_p_B =-27;					a_p_C = 10.2;
        b_p_A = 0.000883;			b_p_B =-34;					b_p_C = 10;
        % Kf: activation n									McIntyre0000
        a_n_A = 1.7^3*.00798;				a_n_B =-93.2;				a_n_C = 1.10;
        b_n_A = 1.7^3*.0142;				b_n_B =-76.0;				b_n_C = 10.5;
        % note, these are not mcintyre values (a_s_*, b_s_*)
        a_s_A = 1.7^3*.00122;				a_s_B =-12.5;				a_s_C = 23.6;
        b_s_A = 1.7^3*.000739;		b_s_B =-80.1;				b_s_C = 21.8;
        
        % PATLAK PARAMS
        k1 = 0;		% UNUSED
        k2 = 0;		% UNUSED
        k3 = 0;		% UNUSED
        
        
    case 'mcintyre2002'
        % Naf
        a_m_A = 6.57;				a_m_B =-20.4;				a_m_C = 10.3;
        b_m_A = 0.304;				b_m_B =-25.7;				b_m_C = 9.16;
        a_h_A = 0.34;				a_h_B =-114;				a_h_C = 11.0;
        b_h_A = 12.6;				b_h_B =-31.8;				b_h_C = 13.4;
        % Nap,			McIntyre0000
        a_p_A = 0.0353;				a_p_B =-27;					a_p_C = 10.2;
        b_p_A = 0.000883;			b_p_B =-34;					b_p_C = 10;
        % Kf			McIntyre0000
        a_n_A = 0.0462;				a_n_B =-93.2;				a_n_C = 1.10;
        b_n_A = 0.0824;				b_n_B =-76.0;				b_n_C = 10.5;
        % note, these are not mcintyre values (a_s_*, b_s_*)
        a_s_A = 0.3;				a_s_B =-12.5;				a_s_C = 23.6;
        b_s_A = 1.7^3*0.000739;		b_s_B =-80.1;				b_s_C = 21.8;
        
        
        % PATLAK PARAMS
        k1 = 0;		% UNUSED
        k2 = 0;		% UNUSED
        k3 = 0;		% UNUSED
        
        
    case 'SMhybrid'
        % Naf
        a_m_A = 6.57;				a_m_B =-27.4;				a_m_C = 10.3;
        b_m_A = 0.304;				b_m_B =-25.7;				b_m_C = 9.16;
        a_h_A = 0.34;				a_h_B =-114;				a_h_C = 11.0;
        b_h_A = 12.6;				b_h_B =-31.8;				b_h_C = 13.4;
        % Nap,			UNUSED
        a_p_A = 0;					a_p_B = 0;					a_p_C = 0;
        b_p_A = 0;					b_p_B = 0;					b_p_C = 0;
        % Kf			McIntyre0000
        a_n_A = 0.0462;				a_n_B =-93.2;				a_n_C = 1.10;
        b_n_A = 0.0824;				b_n_B =-76.0;				b_n_C = 10.5;
        % Ks,			note, these are not mcintyre values (a_s_*, b_s_*)
        a_s_A = 0.3;				a_s_B =-12.5;				a_s_C = 23.6;
        b_s_A = 1.7^3*0.000739;		b_s_B =-80.1;				b_s_C = 21.8;
        
        
        % PATLAK PARAMS
        k1 = 0;		% UNUSED
        k2 = 0;		% UNUSED
        k3 = 0;		% UNUSED
        
end% switch gateParams

%% Save gatingParams to univParams structure.
% Naf
univParams.a_m_A = a_m_A; univParams.a_m_B = a_m_B; univParams.a_m_C = a_m_C;
univParams.b_m_A = b_m_A; univParams.b_m_B = b_m_B; univParams.b_m_C = b_m_C;
univParams.a_h_A = a_h_A; univParams.a_h_B = a_h_B; univParams.a_h_C = a_h_C;
univParams.b_h_A = b_h_A; univParams.b_h_B = b_h_B; univParams.b_h_C = b_h_C;
% Nap
univParams.a_p_A = a_p_A; univParams.a_p_B = a_p_B; univParams.a_p_C = a_p_C;
univParams.b_p_A = b_p_A; univParams.b_p_B = b_p_B; univParams.b_p_C = b_p_C;
% Kf
univParams.a_n_A = a_n_A; univParams.a_n_B = a_n_B; univParams.a_n_C = a_n_C;
univParams.b_n_A = b_n_A; univParams.b_n_B = b_n_B; univParams.b_n_C = b_n_C;
% Ks
univParams.a_s_A = a_s_A; univParams.a_s_B = a_s_B; univParams.a_s_C = a_s_C;
univParams.b_s_A = b_s_A; univParams.b_s_B = b_s_B; univParams.b_s_C = b_s_C;
% PATLAK PARAMS
univParams.k1 = k1; univParams.k2 = k2; univParams.k3 = k3;
end
