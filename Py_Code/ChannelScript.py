# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 15:11:43 2017

@author: Jesse
"""
from Ks_Gillespie import Ks_Gillespie
import numpy

kinParams = {"a_s_A": 0.3,	"a_s_B": -12.5,	"a_s_C": 23.6,
        "b_s_A":(1.7**3*0.000739),"b_s_B":-80.1,"b_s_C": 21.8}

V_m = -84
doInit = 1
N_slowK = 150
it = 0.001
si = 0 
[s0,s1] = Ks_Gillespie(doInit,N_slowK,V_m,si,it,kinParams)
print(s0,s1)
stim = numpy.linspace(-84,20,150)
for V_m in stim:
    print(Ks_Gillespie(0,N_slowK,V_m,si,it,kinParams))