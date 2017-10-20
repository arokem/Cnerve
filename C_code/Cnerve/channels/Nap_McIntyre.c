/*
 *  pNa_stoch_McIntyre.c
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

#include "../cnerve.h"
#include "../mt19937ar.h"				// for random number generator
#include "../channels.h"

/************************************/
/* Gillespie's algorithm for N_Na   */
/************************************/
#ifdef DEBUG
double McIntyre_pNa(const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) ) {
#else
double McIntyre_pNa(const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins) {
#endif


/*		STATIC VARIABLES	*/
	static int  mcint0[MAX_RAN_NODES], 
				mcint1[MAX_RAN_NODES], 
				mcint2[MAX_RAN_NODES], 
				mcint3[MAX_RAN_NODES];
	static double t_occ_pNa[MAX_RAN_NODES];

/* 		OTHERS 				*/

	int		state = -1;
	int		n_pNa;
  double    gamma_0,
			gamma_1,
			gamma_2,
			gamma_3;
  double    a_p,
			b_p,
			d_p;
  double    lambda;
  double    u,
			t,
			t_life;
  double    P[21],					// TODO: shouldn't this be "7" instead of 21?
			eta[21];

/* initialize(0) or generation(1) */
  if( unlikely( doInit )){

	  /*                */
	  /* initialization */
	  /*                */

    /* Rate Constants for V_m=0.0 */
      a_p = rate_Na(ALPHA_P,V_m,Kins); b_p = rate_Na(BETA_P,V_m,Kins);
     
    /* Initialize the number of channels in each state */
      d_p=1.0/(a_p+b_p)/(a_p+b_p)/(a_p+b_p);
      mcint3[node]=(int)(N_pNa*pow(a_p,3.0)*d_p);
      mcint2[node]=(int)(N_pNa*3.0*pow(a_p,2.0)*b_p*d_p);
      mcint1[node]=(int)(N_pNa*3.0*a_p*pow(b_p,2.0)*d_p);
      mcint0[node]=(int)(N_pNa-mcint3[node]-mcint2[node]-mcint1[node]);


    /* gamma */
      gamma_0 = 3.0*a_p+0.0*b_p; gamma_1 = 2.0*a_p+1.0*b_p;
      gamma_2 = 1.0*a_p+2.0*b_p; gamma_3 = 0.0*a_p+3.0*b_p;
     

    /* lambda */
      lambda = (double)mcint0[node]*gamma_0 + (double)mcint1[node]*gamma_1 
             + (double)mcint2[node]*gamma_2 + (double)mcint3[node]*gamma_3;

    /* Lifetime */
    /* ``+si'' is important if it=[1,nd], remove it if it=[0,nd-1] */
      t_occ_pNa[node]=-log(genrand_real2())/lambda;

  } else {
  /*            */
  /* generation */
  /*            */


    /* determine if t_occ is within [t,t+si) */
      t=si*(double)it;

      while ( (t<=t_occ_pNa[node]) && (t_occ_pNa[node]<=(t+si)) ) {

     /* Rate Constants for V_m=0.0 */
      a_p = rate_Na(ALPHA_P,V_m,Kins); b_p = rate_Na(BETA_P,V_m,Kins);
     

    /* gamma */
      gamma_0 = 3.0*a_p+0.0*b_p; gamma_1 = 2.0*a_p+1.0*b_p;
      gamma_2 = 1.0*a_p+2.0*b_p; gamma_3 = 0.0*a_p+3.0*b_p;
     

    /* lambda */
      lambda = (double)mcint0[node]*gamma_0 + (double)mcint1[node]*gamma_1 
             + (double)mcint2[node]*gamma_2 + (double)mcint3[node]*gamma_3;

      /* eta */
        eta[0]= 0.0;
        eta[1]= 3.0 * a_p * (double)mcint0[node];  
        eta[2]= 1.0 * b_p * (double)mcint1[node];
        eta[3]= 2.0 * a_p * (double)mcint1[node];
        eta[4]= 2.0 * b_p * (double)mcint2[node];
        eta[5]= 1.0 * a_p * (double)mcint2[node];
        eta[6]= 3.0 * b_p * (double)mcint3[node];
       

			/* cumulative state transition prob */
        double tmp=0.0;
        for(int i=0;i<=6;i++){
          tmp+=eta[i];
          P[i]=tmp;
        }

      /* determine which one of those states */ 
        u=genrand_real2()*lambda;
        for(int i=1;i<=6;i++){
          if ((P[i-1]<=u)&&(u<P[i])) {
            state=i;
     	  }
        }

      /* update the number of channels */
        switch(state){
		  case 1:  mcint0[node]--; mcint1[node]++; break; 
          case 2:  mcint1[node]--; mcint0[node]++; break;
          case 3:  mcint1[node]--; mcint2[node]++; break;
          case 4:  mcint2[node]--; mcint1[node]++; break;
          case 5:  mcint2[node]--; mcint3[node]++; break;
          case 6:  mcint3[node]--; mcint2[node]++; break;
        }

      /* lifetime & update occurrence time */
        t_life=-log(genrand_real2())/lambda;
        t_occ_pNa[node]+=t_life;

      }
  }
/*
      printf("   m0h0,m1h0,m2h0,m3h0 : %4d %4d %4d %4d\n",
                 m0h0[node],m1h0[node],m2h0[node],m3h0[node]);
      printf("   m0h1,m1h1,m2h1,m3h1 : %4d %4d %4d %4d\n",
                 m0h1[node],m1h1[node],m2h1[node],m3h1[node]);
*/
  n_pNa=mcint3[node];

  return(n_pNa);
}
