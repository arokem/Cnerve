# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 11:36:21 2017

@author: Jesse
"""
from math import exp, log
from random import random
import numpy as np

def ALPHA_S(V_m,kinParams):
    return kinParams["a_s_A"] * (V_m - kinParams["a_s_B"]) / (1 - exp((kinParams["a_s_B"] - V_m) / kinParams["a_s_C"]))

def BETA_S(V_m,kinParams):
    return kinParams["b_s_A"] * (kinParams["b_s_B"] - V_m) / (1 - exp((V_m - kinParams["b_s_B"]) / kinParams["b_s_C"]))

def Ks_Gillespie(doInit,N_slowK,s,V_m,si,it,kinParams):
    "/*		STATIC VARIABLES	*/"
    t_occ_slowK = 0
    state = -1
    "Rate Constants for V_m=V_rest"
    a_s = ALPHA_S(V_m,kinParams)
    b_s = BETA_S(V_m,kinParams)
    gamma_0 = a_s
    gamma_1 = b_s 

    "Initialize(0) or generation(1)"
    if doInit:

        "Initialize the number of channels in each state"
        d_s=a_s/(a_s+b_s)
        s[1]=int(N_slowK*d_s)
        s[0]=int(N_slowK-s[1])

        Lambda = s[0]*gamma_0 + s[1]*gamma_1

        "Lifetime"
        " ''+si'' is important if it=[1,nd], remove it if it=[0,nd-1]"
        t_occ_slowK=-log(random()/Lambda);

    else:
        "generation"
        "determine if t_occ is within [t,t+si)"
        t=si*it
        while (t<=t_occ_slowK) and (t_occ_slowK<=(t+si)):
        
            Lambda = s[0]*gamma_0 + s[1]*gamma_1 
                  
            eta= [0.0, a_s *s[0], b_s * s[1]]
            
            "cummulative state transition prob"
            tmp = 0.0
            P = []
            for i in range(3):
                tmp+=eta[i]
                P = np.append(P,tmp)
    
            "determine which one of those states" 
            u=random()*Lambda
            for i in range(3):
                if (P[i-1]<=u) and (u<P[i]):
                    state=i
    
            "Update the number of channels"
            if state == 1:
                if s[0] > 0:
                    s[0] -= 1; s[1] += 1
            elif state == 2:
                if s[1]>0: s[1] -= 1; s[0] += 1
    
            "lifetime & update occurrence time"
            t_life=-log(random()/Lambda)
            t_occ_slowK+=t_life

    assert s[1] >= 0, "Illegal number of channels in open state."
    return(s)


