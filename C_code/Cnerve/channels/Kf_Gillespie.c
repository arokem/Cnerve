/*
 *  Kf_stoch_Gillespie.c
 *  cnerve
 *
 *  Created by Nikita on 2/15/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include <stdbool.h>				// for bool defenition
#include <stdint.h>					// for the uint32_t type

#include "../rateChannels.h"
#include "../channels.h"
#include "../mt19937ar.h"

/**
	Gillespie's algorithm for N_K
*/
#ifdef DEBUG
double Gillespie_K(const bool doInit, const uint32_t N_K, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins, FILE *fp) {
#else
double Gillespie_K(const bool doInit, const uint32_t N_K, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins) {
#endif
	
/*		STATIC VARIABLES	*/
	static double t_occ_K[MAX_RAN_NODES];
	static int  n0[MAX_RAN_NODES], n1[MAX_RAN_NODES], n2[MAX_RAN_NODES], 
				n3[MAX_RAN_NODES], n4[MAX_RAN_NODES];
	
/* 		OTHERS 				*/
	
	int		state = -1;
	double	gamma_0=0.0,
			gamma_1=0.0,
			gamma_2=0.0,
			gamma_3=0.0,
			gamma_4=0.0;
	double 	a_n=0.0,
			b_n=0.0,
			d_n=0.0;
	double	lambda=0.0;
	double	u,t,t_life;
	double	P[9],
			eta[9];
	
/* initialize(0) or generation(1) */
	if( unlikely( doInit ) ) {
		
		/*                */
		/* initialization */
		/*                */
		
		/* Rate Constants for V_m=0.0 */
		a_n = rate_K(ALPHA_N,V_m,Kins); b_n = rate_K(BETA_N,V_m,Kins);
		
		/* Initialize the number of channels in each state */
		d_n=a_n/(a_n+b_n);
		n4[node]=(int)(N_K*pow(d_n,4.0));
		n3[node]=(int)(N_K*4.0*pow(d_n,3.0)*(1.0-d_n));
		n2[node]=(int)(N_K*6.0*pow(d_n,2.0)*pow((1.0-d_n),2.0));
		n1[node]=(int)(N_K*4.0*pow((1.0-d_n),3.0)*d_n);
		n0[node]=(int)(N_K-(n4[node]+n3[node]+n2[node]+n1[node]));

		
		/* gamma */
		gamma_0 = 4.0*a_n+0.0*b_n; gamma_1 = 3.0*a_n+1.0*b_n; 
		gamma_2 = 2.0*a_n+2.0*b_n; gamma_3 = 1.0*a_n+3.0*b_n; 
		gamma_4 = 0.0*a_n+4.0*b_n;
		
		/* lambda */
		lambda = (double)n0[node]*gamma_0 + (double)n1[node]*gamma_1 
		+ (double)n2[node]*gamma_2 + (double)n3[node]*gamma_3 
		+ (double)n4[node]*gamma_4 ;
		
		/* Lifetime */
		/* ``+si'' is important if it=[1,nd], remove it if it=[0,nd-1] */
		t_occ_K[node]=-log(genrand_real2())/lambda;
		
	} else {

		/*            */
		/* generation */
		/*            */
		
		
		/* determine if t_occ is within [t,t+si) */
		t=si*(double)it;
		while ( (t<=t_occ_K[node]) && (t_occ_K[node]<=(t+si)) ) {
			
			/* rate for K channels at V_m */
			a_n = rate_K(ALPHA_N,V_m,Kins); b_n = rate_K(BETA_N,V_m,Kins);
			
			/* gamma */
			gamma_0 = 4.0*a_n+0.0*b_n; gamma_1 = 3.0*a_n+1.0*b_n; 
			gamma_2 = 2.0*a_n+2.0*b_n; gamma_3 = 1.0*a_n+3.0*b_n; 
			gamma_4 = 0.0*a_n+4.0*b_n; 
			
			/* lambda */
			lambda = (double)n0[node]*gamma_0 + (double)n1[node]*gamma_1 
			+ (double)n2[node]*gamma_2 + (double)n3[node]*gamma_3 
			+ (double)n4[node]*gamma_4 ;
			
			/* eta */
			eta[0]= 0.0;
			eta[1]= 4.0 * a_n * (double)n0[node];  			// n0	->	n1
			eta[2]= 1.0 * b_n * (double)n1[node];  			// n1	->	n0
			eta[3]= 3.0 * a_n * (double)n1[node];  			// n1	->	n2
			eta[4]= 2.0 * b_n * (double)n2[node];  			// n2	->	n1
			eta[5]= 2.0 * a_n * (double)n2[node];  			// n2	->	n3
			eta[6]= 3.0 * b_n * (double)n3[node];  			// n3	-> 	n2
			eta[7]= 1.0 * a_n * (double)n3[node];  			// n3	->	n4
			eta[8]= 4.0 * b_n * (double)n4[node];  			// n4	-> 	n3
			
			/* cummulative state transition prob */
			double tmp=0.0;
			for(int i=0;i<=8;i++){
				tmp+=eta[i];
				P[i]=tmp;
			}
			
			/* determine which one of those states */ 
			u=genrand_real2()*lambda;
			for(int i=1;i<=8;i++){
				if ((P[i-1]<=u)&&(u<P[i])) {
					state=i;
				}
			}
			
			/* update the number of channels */
			switch(state){
				case 1:  if(n0[node]>0){n0[node]--; n1[node]++;} break; 
				case 2:  if(n1[node]>0){n1[node]--; n0[node]++;} break; 
				case 3:  if(n1[node]>0){n1[node]--; n2[node]++;} break; 
				case 4:  if(n2[node]>0){n2[node]--; n1[node]++;} break; 
				case 5:  if(n2[node]>0){n2[node]--; n3[node]++;} break; 
				case 6:  if(n3[node]>0){n3[node]--; n2[node]++;} break; 
				case 7:  if(n3[node]>0){n3[node]--; n4[node]++;} break; 
				case 8:  if(n4[node]>0){n4[node]--; n3[node]++;} break; 
				default:
					// no need to break out here, genrand_real2()*lambda was not high enough
					// to warrant a state transition, so we just sail on
					break;
			}
			
			/* lifetime & update occurrence time */
			t_life=-log(genrand_real2())/lambda;
			t_occ_K[node]+=t_life;
			
		}
		
	}
	
	// check if anything's gone awry:
    assert(n0[node] >= 0);
    assert(n1[node] >= 0);
    assert(n2[node] >= 0);
    assert(n3[node] >= 0);
    assert(n4[node] >= 0);
    

	const int n_K=n4[node];				///< # of channels in open state (n4)
	
	
#ifdef DEBUG
	fwrite(&n_K,	sizeof(int),	1,	fp);
#endif
	
	
	return(n_K);
}


