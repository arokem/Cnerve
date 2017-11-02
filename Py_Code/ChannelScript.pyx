# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 15:11:43 2017

@author: Jesse
"""

from libc.stdint cimport uint32_t
from libcpp cimport bool
ctypedef struct kinParam:
    double a_s_A, a_s_B, a_s_C, b_s_A, b_s_B, b_s_C

cdef extern from "../C_Code/Ks_Chan_Updater/Ks_Gillespie.h":
    double Gillespie_slowK(const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const kinParam Kins)

#kinParams = {"a_s_A": 0.3,	"a_s_B": -12.5,	"a_s_C": 23.6,
#        "b_s_A":(1.7**3*0.000739),"b_s_B":-80.1,"b_s_C": 21.8}
#
#E_rest = -168
#V_m = 0
#doInit = 1
#N_slowK = 150
#it = 0.001
#si = 0 
#s = [0,0]
#s0 = []
#s1 = []
#s = Ks_Gillespie(N_slowK,s,V_m+E_rest,si,it,kinParams)
#s0 = np.append(s0,s[0]/N_slowK)
#s1 = np.append(s1,s[1]/N_slowK)
#stim = np.linspace(0,40-E_rest,150)
#for V_m in stim:
#    s = Ks_Gillespie(N_slowK,s,V_m+E_rest,si,it,kinParams)
#    s0 = np.append(s0,s[0]/N_slowK)
#    s1 = np.append(s1,s[1]/N_slowK)
#    
#plt.plot(s0,'bo',s1,'r^')