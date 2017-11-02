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

#include "rateChannels.h"
#include "mt19937ar.h"			// for random number generator

/**
	Gillespie's algorithm for N_slowK
*/

double Gillespie_slowK(const bool doInit, const uint32_t N_slowK, const double V_m, const double si, const uint32_t it, const kinParam Kins) {

/*		STATIC VARIABLES	*/
	static double t_occ_slowK;
	static int  s0, s1;

/* 		OTHERS 				*/

	
  int		state = -1;
  double    gamma_0,
  			gamma_1;
  double    a_s,b_s,d_s;
  double    lambda;
  double    u,t,t_life;
  double    P[3],eta[3];

/* initialize(0) or generation(1) */
  if( doInit ){

  /*                */
  /* initialization */
  /*                */

    /* Rate Constants for V_m=0.0 */
      a_s = rate_K(ALPHA_S,V_m,Kins); b_s = rate_K(BETA_S,V_m,Kins);

    /* Initialize the number of channels in each state */
      d_s=a_s/(a_s+b_s);
      s1=(int)(N_slowK*d_s);
      s0=(int)(N_slowK-s1);

    /* gamma */
      gamma_0 = a_s; gamma_1 = b_s; 
     

    /* lambda */
      lambda = (double)s0*gamma_0 + (double)s1*gamma_1;

    /* Lifetime */
    /* ``+si'' is important if it=[1,nd], remove it if it=[0,nd-1] */
      t_occ_slowK=-log(genrand_real2())/lambda;

  } else {
  /*            */
  /* generation */
  /*            */

    
    /* determine if t_occ is within [t,t+si) */
      t=si*(double)it;
      while ( (t<=t_occ_slowK) && (t_occ_slowK<=(t+si)) ) {

      /* rate for Na channels at V_m */
        a_s = rate_K(ALPHA_S,V_m,Kins); b_s = rate_K(BETA_S,V_m,Kins);

      /* gamma */
        gamma_0 = a_s; gamma_1 = b_s; 

      /* lambda */
        lambda = (double)s0*gamma_0 + (double)s1*gamma_1;
              
      /* eta */
        eta[0]= 0.0;
        eta[1]= a_s * (double)s0;
        eta[2]= b_s * (double)s1;
        

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
        case 1:  if(s0>0){s0--; s1++;} break;
        case 2:  if(s1>0){s1--; s0++;} break;
			default:
			// no need to break out here, genrand_real2()*lambda was not high enough
			// to warrant a state transition, so we just sail on
				break;
        }

      /* lifetime & update occurrence time */
        t_life=-log(genrand_real2())/lambda;
        t_occ_slowK+=t_life;

      }
		
	}

	const int n_slowK=s1;
	assert( N_slowK >= 0 );
	return(n_slowK);
}


