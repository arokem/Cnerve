#ifndef _CHANNELS_H_INCLUDED_
#define _CHANNELS_H_INCLUDED_
/*
 *  channels.h
 *  cnerve
 *
 *  Created by Nikita on 2/15/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include <math.h>			// for misc. log/exp operations
#include <stdbool.h>			// for bool defenition
#include <stdint.h>			// for the uint32_t type
#include <stdio.h>			// for FILE definition
#include <assert.h>

typedef struct kineticParam {
	double k1, k2, k3;
	double a_m_A, a_m_B, a_m_C;
	double b_m_A, b_m_B, b_m_C;
	double a_h_A, a_h_B, a_h_C;
	double b_h_A, b_h_B, b_h_C;
	double a_n_A, a_n_B, a_n_C;
	double b_n_A, b_n_B, b_n_C;
	double a_s_A, a_s_B, a_s_C;
	double b_s_A, b_s_B, b_s_C;
	double a_p_A, a_p_B, a_p_C;
	double b_p_A, b_p_B, b_p_C;
} kinParam;

double Gillespie_slowK(const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const kinParam Kins);

#endif //_CHANNELS_H_INCLUDED_
