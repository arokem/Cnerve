# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 11:36:21 2017

@author: Jesse
"""
from math import exp, log
from random import random

def ALPHA_S(V_m,kinParams):
    return kinParams["a_s_A"] * (V_m - kinParams["a_s_B"]) / (1 - exp((kinParams["a_s_B"] - V_m) / kinParams["a_s_C"]))

def BETA_S(V_m,kinParams):
    return kinParams["b_s_A"] * (kinParams["b_s_B"] - V_m) / (1 - exp((V_m - kinParams["b_s_B"]) / kinParams["b_s_C"]))

def Cy_Ks_Gillespie(int N_slowK,int[:] chan_states,double V_m,double si,double it,kinParams):
    "/*		STATIC VARIABLES	*/"
    cdef double t_occ_slowK = 0
    cdef int state = -1
    "Rate Constants for V_m=V_rest"
    cdef double a_s = ALPHA_S(V_m,kinParams)
    cdef double b_s = BETA_S(V_m,kinParams)
    cdef double gamma_0 = a_s
    cdef double gamma_1 = b_s
    cdef double d_s
    cdef double Lambda

    "Initialize(0) or generation(1)"
    if not chan_states :

        "Initialize the number of channels in each state"
        d_s = a_s/(a_s+b_s)
        chan_states.append(int(N_slowK*d_s))
        chan_states.insert(0,int(N_slowK-chan_states[0]))

        Lambda = chan_states[0]*gamma_0 + chan_states[1]*gamma_1

        "Lifetime"
        " ''+si'' is important if it=[1,nd], remove it if it=[0,nd-1]"
        t_occ_slowK=-log(random()/Lambda);

    else:
        "generation"
        "determine if t_occ is within [t,t+si)"
        cdef double t=si*it
        while (t<=t_occ_slowK) and (t_occ_slowK<=(t+si)):
        
            Lambda = chan_states[0]*gamma_0 + chan_states[1]*gamma_1 
                  
            cdef double eta= [0.0, a_s *chan_states[0], b_s * chan_states[1]]
            
            "cummulative state transition prob"
            cdef double tmp = 0.0
            P = []
            for i in range(3):
                tmp+=eta[i]
                P.append(tmp)
    
            "determine which one of those chan_states" 
            u=random()*Lambda
            for i in range(3):
                if (P[i-1]<=u) and (u<P[i]):
                    state=i
    
            "Update the number of channels"
            if state == 1:
                if chan_states[0] > 0:
                    chan_states[0] -= 1; chan_states[1] += 1
            elif state == 2:
                if chan_states[1]>0: chan_states[1] -= 1; chan_states[0] += 1
    
            "lifetime & update occurrence time"
            t_life=-log(random()/Lambda)
            t_occ_slowK+=t_life

    assert chan_states[1] >= 0, "Illegal number of channels in open state."
    return chan_states


