/*
 *  Nap_deter.c
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
/* Deterministic algorithm for persistent Na   */
/*****************************************/
#ifdef DEBUG
double Deterministic_pNa_McIntyre(const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) ) {
#else
double Deterministic_pNa_McIntyre(const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins) {
#endif

	static double p_det[MAX_RAN_NODES];
 
	double a_p = rate_Na(ALPHA_P,V_m,Kins);
	double b_p = rate_Na(BETA_P,V_m,Kins);

	double p_tau = 1/(a_p+b_p);
	double p_inf = a_p*p_tau;

	/* initialize(0) or generation(1) */
	if( unlikely( doInit )) {
		
			/* Initialize the number of channels in each state */
			p_det[node] = p_inf;

	} else {

			p_det[node] += si/p_tau*(p_inf-p_det[node]);
	
	}

	int open_pNa = (int) ( N_pNa*pow(p_det[node],3.0) );
	// printf("a_p=%lf\t b_p = %lf\t p_det[%d] = %lf\n",a_p, b_p, node, p_det[node]);

	return(open_pNa);
}
