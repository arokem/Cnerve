# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 16:50:00 2017

@author: Jesse
"""

class ChannelType:
    def __init__(self,name):
        self.name = name`

def ALPHA_N(V_m,kinParams):
    return Kins.a_n_A * (V_m - Kins.a_n_B) / (1 - exp((Kins.a_n_B - V_m) / Kins.a_n_C));

def BETA_N(V_m,kinParams):
    return Kins.b_n_A * (V_m - Kins.b_n_B) / (1 - exp((Kins.b_n_B - V_m) / Kins.b_n_C));

def ALPHA_S(V_m,kinParams):
    return Kins.a_s_A * (V_m - Kins.a_s_B) / (1 - exp((Kins.a_s_B - V_m) / Kins.a_s_C));

def BETA_S(V_m,kinParams):
    return Kins.b_s_A * (Kins.b_s_B - V_m) / (1 - exp((V_m - Kins.b_s_B) / Kins.b_s_C));




def rate_K(eTypeRateK,V_m,kinParams)

	rate = -1;

	switch (idx) {
	case ALPHA_N: /* alpha_n */
		rate = Kins.a_n_A * (V_m - Kins.a_n_B) / (1 - exp((Kins.a_n_B - V_m) / Kins.a_n_C));
		break;

	case BETA_N: /* beta_n */
		rate = Kins.b_n_A * (Kins.b_n_B - V_m) / (1 - exp((V_m - Kins.b_n_B) / Kins.b_n_C));
		break;

	case ALPHA_S: /* alpha_s */
		rate = Kins.a_s_A * (V_m - Kins.a_s_B) / (1 - exp((Kins.a_s_B - V_m) / Kins.a_s_C));
		break;

	case BETA_S: /* beta_s */
		rate = Kins.b_s_A * (Kins.b_s_B - V_m) / (1 - exp((V_m - Kins.b_s_B) / Kins.b_s_C));
		break;

	default:
		// inform, but do not break out (too aggressive, but we DO need to catch any errant calls)
		fprintf(stderr, "Unexpected idx (%d) encountered in rate_K!\n",
				(int) idx);
		break;
	return (rate);