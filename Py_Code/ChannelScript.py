# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 15:11:43 2017

@author: Jesse
"""
from Ks_Gillespie import Ks_Gillespie
import numpy as np
import matplotlib.pyplot as plt

kinParams = {"a_s_A": 0.3,	"a_s_B": -12.5,	"a_s_C": 23.6,
        "b_s_A":(1.7**3*0.000739),"b_s_B":-80.1,"b_s_C": 21.8}

E_rest = -168
V_m = 0
doInit = 1
N_slowK = 150
it = 0.001
si = 0 
chan_states = []
s0 = []
s1 = []
chan_states = Ks_Gillespie(N_slowK,chan_states,V_m+E_rest,si,it,kinParams)
s0 = np.append(s0,chan_states[0]/N_slowK)
s1 = np.append(s1,chan_states[1]/N_slowK)
stim = np.linspace(0,40-E_rest,150)
for V_m in stim:
    s = Ks_Gillespie(N_slowK,chan_states,V_m+E_rest,si,it,kinParams)
    s0 = np.append(s0,chan_states[0]/N_slowK)
    s1 = np.append(s1,chan_states[1]/N_slowK)
    
plt.plot(s0,'bo',s1,'r^')