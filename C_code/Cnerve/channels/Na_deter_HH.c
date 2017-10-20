/*
 *  Naf_deter.c
 *  cnerve
 *
 *  Created by Nikita on 2/15/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include <math.h>					// for misc. log/exp operations

#include <stdbool.h>				// for bool defenition
#include <stdint.h>					// for uint32_t defenition
#include <stdio.h>					// for FILE defenition

#include "../rateChannels.h"
#include "../mt19937ar.h"				// for random number generator

/*****************************************/
/* Deterministic algorithm for fast Na   */
/*****************************************/
/** int it argument: this is a dummy argument, supplied only such that the
 * prototype of the function is the same as in stochastic cases.
 */
#ifdef DEBUG
double Deterministic_Na(const bool doInit, const uint32_t N_Na __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) ) {
#else
double Deterministic_Na(const bool doInit, const uint32_t N_Na __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins) {
#endif

	static double m_det[MAX_RAN_NODES],
				  h_det[MAX_RAN_NODES];
	
	double a_m = rate_Na(ALPHA_M,V_m,Kins);		double b_m = rate_Na(BETA_M,V_m,Kins);
	double a_h = rate_Na(ALPHA_H,V_m,Kins);		double b_h = rate_Na(BETA_H,V_m,Kins);

	double m_tau = 1/(a_m+b_m);
	double m_inf = a_m*m_tau;
	double h_tau = 1/(a_h+b_h);
	double h_inf = a_h*h_tau;

	/* initialize(0) or generation(1) */
	if( unlikely( doInit )) {
		
		/* Initialize the number of channels in each state */
		m_det[node] = m_inf;
		h_det[node] = h_inf;

	} else {

		m_det[node] += si/m_tau*(m_inf-m_det[node]);
		h_det[node] += si/h_tau*(h_inf-h_det[node]); 
	}

	return (  (pow(m_det[node],3.0)*h_det[node]));		// return the number of open channels

}
