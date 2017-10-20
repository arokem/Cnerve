/*
 *  Ks_deter.c
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

#include "../channels.h"
#include "../mt19937ar.h"				// for random number generator



/*****************************************/
/* Deterministic algorithm for slow K    */
/*****************************************/
#ifdef DEBUG
double Deterministic_slowK(const bool doInit, const uint32_t N_slowK, const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp __attribute__ ((unused)) ) {
#else
double Deterministic_slowK(const bool doInit, const uint32_t N_slowK, const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins) {
#endif
	
	
	static double s_det[MAX_RAN_NODES];
	
	double	a_s = rate_K( ALPHA_S,	V_m, Kins);
	double	b_s = rate_K( BETA_S,	V_m, Kins);
	
	double s_tau = 1/(a_s+b_s);
	double s_inf = a_s*s_tau;
	
	/* initialize(0) or generation(1) */
	if ( unlikely(doInit) ){
		
		s_det[node] = s_inf;

	} else {
		
		s_det[node] += si/s_tau*(s_inf-s_det[node]);
		
	}
	
	/*
	 * THIS DOESNT SEEM RIGHT -- DETERM. MODEL SHOULDNT DEPEND ON N_slowK!
	 *
	 * see Kf_deter_HH for possible way of handling this....
	 */
	return ( N_slowK*s_det[node] );
}

