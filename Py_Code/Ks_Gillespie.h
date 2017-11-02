/*
 *  Ks_stoch_Gillespie.c
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
#include <assert.h>

#include "../rateChannels.h"
#include "../mt19937ar.h"			// for random number generator

/**
	Gillespie's algorithm for N_slowK
*/
#ifdef DEBUG
double Gillespie_slowK(const bool doInit, const uint32_t N_slowK, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins, FILE *fp) {
#else
double Gillespie_slowK(const bool doInit, const uint32_t N_slowK, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins) {
#endif

/*		STATIC VARIABLES	*/
	static double t_occ_slowK[MAX_RAN_NODES];
	static int  s0[MAX_RAN_NODES], s1[MAX_RAN_NODES];

/* 		OTHERS 				*/

	
  int		state = -1;
  double    gamma_0,
  			gamma_1;
  double    a_s,b_s,d_s;
  double    lambda;
  double    u,t,t_life;
  double    P[3],eta[3];

/* initialize(0) or generation(1) */
  if( unlikely( doInit )){

  /*                */
  /* initialization */
  /*                */

    /* Rate Constants for V_m=0.0 */
      a_s = rate_K(ALPHA_S,V_m,Kins); b_s = rate_K(BETA_S,V_m,Kins);

    /* Initialize the number of channels in each state */
      d_s=a_s/(a_s+b_s);
      s1[node]=(int)(N_slowK*d_s);
      s0[node]=(int)(N_slowK-s1[node]);

    /* gamma */
      gamma_0 = a_s; gamma_1 = b_s; 
     

    /* lambda */
      lambda = (double)s0[node]*gamma_0 + (double)s1[node]*gamma_1;

    /* Lifetime */
    /* ``+si'' is important if it=[1,nd], remove it if it=[0,nd-1] */
      t_occ_slowK[node]=-log(genrand_real2())/lambda;

  } else {
  /*            */
  /* generation */
  /*            */

    
    /* determine if t_occ is within [t,t+si) */
      t=si*(double)it;
      while ( (t<=t_occ_slowK[node]) && (t_occ_slowK[node]<=(t+si)) ) {

      /* rate for Na channels at V_m */
        a_s = rate_K(ALPHA_S,V_m,Kins); b_s = rate_K(BETA_S,V_m,Kins);

      /* gamma */
        gamma_0 = a_s; gamma_1 = b_s; 

      /* lambda */
        lambda = (double)s0[node]*gamma_0 + (double)s1[node]*gamma_1; 
              
      /* eta */
        eta[0]= 0.0;
        eta[1]= a_s * (double)s0[node];  
        eta[2]= b_s * (double)s1[node];  
        

      /* cummulative state transition prob */
        double tmp=0.0;
        for(int i=0;i<=2;i++){
          tmp+=eta[i];
          P[i]=tmp;
        }

      /* determine which one of those states */ 
        u=genrand_real2()*lambda;
        for(int i=1;i<=2;i++){
          if ((P[i-1]<=u)&&(u<P[i])) {
            state=i;
     	  }
        }

      /* update the number of channels */
        switch(state){

	/* WARNING:						BUG			BUG			BUG
	 *
	 * 		We should not be referencing n0 here, this is a typo!
	  case 1:  if(s0[node]>0){n0[node]--; s1[node]++;} break; 
	 *
	 * WARNING:						BUG			BUG			BUG
	 */
	  case 1:  if(s0[node]>0){s0[node]--; s1[node]++;} break; 
	  case 2:  if(s1[node]>0){s1[node]--; s0[node]++;} break; 
			default:
			// no need to break out here, genrand_real2()*lambda was not high enough
			// to warrant a state transition, so we just sail on
				break;
        }

      /* lifetime & update occurrence time */
        t_life=-log(genrand_real2())/lambda;
        t_occ_slowK[node]+=t_life;

      }
		
	}

	const int n_slowK=s1[node];
	assert( N_slowK >= 0 );
	
#ifdef DEBUG
	fwrite(&n_slowK,sizeof(int),	1,	fp);
#endif
	
	return(n_slowK);
}


