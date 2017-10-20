/*
 *  Kf_deter.c
 *  cnerve
 *
 *  Created by Nikita on 2/15/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include "../channels.h"

/*****************************************/
/* Deterministic algorithm for nodal K   */
/*****************************************/
/** int it argument: this is a dummy argument, supplied only such that the
 * prototype of the function is the same as in stochastic cases.
 */
#ifdef DEBUG
double Deterministic_K(const bool doInit, const uint32_t N_K __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) ) {
#else
double Deterministic_K(const bool doInit, const uint32_t N_K __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins) {
#endif
	
	
	static double n_det[MAX_RAN_NODES];
	
	const double	a_n = rate_K( ALPHA_N,	V_m, Kins);
	const double	b_n = rate_K( BETA_N,	V_m, Kins);
	
	const double	n_tau = 1/(a_n+b_n);
	const double	n_inf = a_n*n_tau;
	
	/* initialize(0) or generation(1) */
	if( unlikely( doInit )) {
		
		/* Initialize the number of channels in each state */
		n_det[node] = n_inf;
		
	} else {

		n_det[node] += si/n_tau*(n_inf-n_det[node]);
	}
	
	return (pow(n_det[node],4.0) );
}

