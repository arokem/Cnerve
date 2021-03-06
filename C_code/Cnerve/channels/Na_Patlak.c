/*
 *  Na_Patlak.c
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

/**
	@brief	Gillespie's algorithm for N_Na from Patlak (5 state model)
	@see	Marom1994, "Modeling State-Dependent Inactivation of Membrane Currents" (paper copy only), and
			Patlak1991, "Molecular Kineticsof Voltage-Dependent Na+ Channels"



	\param[in] N_Na 	# of channels
	\param[in] V_m		voltage current at the node where the # of open chans is being calculated (mV)
	\param[in] si		time step increment, "delt" in our jargon
	\param[in] it		iteration # (time step #)
	\param[in] node		node number for which we're performing the calculation

	\return n_Na		number of Na channels open ?

*/


#ifdef DEBUG
double Patlak_Na(const bool doInit, const uint32_t N_Na, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) ) {
#else
double Patlak_Na(const bool doInit, const uint32_t N_Na, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins) {
#endif
/*		STATIC VARIABLES	*/
	static double t_occ_Na[MAX_RAN_NODES];
	static int  state1[MAX_RAN_NODES],
				state2[MAX_RAN_NODES],
				state3[MAX_RAN_NODES],
				state4[MAX_RAN_NODES],
				state5[MAX_RAN_NODES];

/* 		OTHERS 				*/
	int		state = -1;
  double    gamma_1,
			gamma_2,
			gamma_3,
			gamma_4,
			gamma_5;
  double    kcoeff,
			k1now,
			k2now,
			k3now;
  double    a_m,
			b_m,
			a_h,
			den;
  double    lambda;
  double    u,
			t,
			t_life;
  double    P[11],
			eta[11];
  
	 kcoeff = -2.1219e-20*pow(V_m,10.0) + -1.8917e-18*pow(V_m,9.0) 
			+ 6.7559e-16*pow(V_m,8.0) + 4.5252e-14*pow(V_m,7.0)
			+ -8.4925e-12*pow(V_m,6.0) + -3.6037e-10*pow(V_m,5.0)
			+ 5.6059e-08*pow(V_m,4.0) + 4.5839e-7*pow(V_m,3.0)
			+ -.00017648*pow(V_m,2.0) + 0.0059815*pow(V_m,1.0) + 0.92214;
			
			 k1now = kcoeff*Kins.k1;
			 k2now = kcoeff*Kins.k2;
			 k3now = kcoeff*Kins.k3;
	
			 
/* initialize(0) or generation(1) */
  if( unlikely( doInit )){

  /*                */
  /* initialization */
  /*                */

    /* Rate Constants for V_m=0.0 */
      a_m = rate_Na(ALPHA_M,V_m,Kins);
      b_m = rate_Na(BETA_M,V_m,Kins);
      a_h = rate_Na(ALPHA_H,V_m,Kins);
     	  
     	  
    /* Initialize the number of channels in each state */
      
    /* den = -(3.0*k1*k3*pow(a_m,2.0) - 3.0*k1*a_h*pow(a_m,2.0) + 9.0*k1*b_m*k2*a_m + 3.0*k1*k3*k2*a_m 
	  + 18*a_m*pow(b_m,2.0)*k1 - 3.0*k1*a_m*a_h* k3 + 6.0*k1*a_m*k3*b_m - 9.0*k1*a_h*a_m*b_m 
	  - 6.0*a_h*pow(a_m,3.0) + 6.0*pow(a_m, 3.0) * k3 - 18.0*a_h*pow(a_m,2.0)*b_m + 18*pow(a_m,2.0)*b_m*k2 
	  - 6.0* a_h*pow(a_m,2.0)*k3 + 6.0*pow(a_m,2.0)*k2*k3 - 18.0*a_h*a_m*pow(b_m,2.0) - 6.0*a_h*a_m*k3*b_m 
	  - 2.0*a_h*pow(b_m,2.0)*k3 - 6.0*a_h*pow(b_m, 3.0));*/

		den = (b_m + 3*a_m)*(2*b_m)*(3*b_m+k3now)*(a_h) 
			+ a_m*(k1now+2*a_m)*(3*a_m)*(a_h-3*b_m)
			+ 3*a_m*(3*b_m + k3now)*((k2now + a_m + 2*b_m)*(k1now+2*a_m)-4*a_m*b_m);
    
		state1[node] = (int)(N_Na*a_h*(3*b_m+k3now)*(k1now+2*a_m)*(3*a_m)/den);
		state2[node] = (int)(N_Na*2*b_m*a_h*(3*b_m+k3now)*(3*a_m)/den);
		state3[node] = (int)(N_Na*2*b_m*a_h*(3*b_m+k3now)*(b_m)/den);
		state4[node] = (int)(N_Na*a_h*(k1now+2*a_m)*(a_m)*(3*a_m)/den);
		state5[node] = (int)(N_Na - (state1[node]+state2[node]+state3[node]+state4[node]));
      
    /* gamma */
      gamma_1 = 3.0*a_m + 0.0*b_m;
      gamma_2 = 2.0*a_m + 1.0*b_m + k1now;
      gamma_3 = 1.0*a_m + 2.0*b_m + k2now;
      gamma_4 = 0.0*a_m + 3.0*b_m + k3now;
      gamma_5 = a_h;
            
    /* lambda */
      lambda = (double)state1[node]*gamma_1 + (double)state2[node]*gamma_2 
             + (double)state3[node]*gamma_3 + (double)state4[node]*gamma_4 
             + (double)state5[node]*gamma_5;
    
    /* Lifetime */
    /* ``+si'' is important if it=[1,nd], remove it if it=[0,nd-1] */
      t_occ_Na[node]=-log(genrand_real2())/lambda;

  } else {

  /*            */
  /* generation */
  /*            */


    /* determine if t_occ is within [t,t+si) */
      t=si*(double)it;

      while ( (t<=t_occ_Na[node]) && (t_occ_Na[node]<=(t+si)) ) {

      /* rate for Na channels at V_m */
        a_m = rate_Na(ALPHA_M,V_m,Kins); b_m = rate_Na(BETA_M,V_m,Kins);
        a_h = rate_Na(ALPHA_H,V_m,Kins);
       
        
      /* gamma */
        gamma_1 = 3.0*a_m + 0.0*b_m;
        gamma_2 = 2.0*a_m + 1.0*b_m + k1now;
        gamma_3 = 1.0*a_m + 2.0*b_m + k2now;
        gamma_4 = 0.0*a_m + 3.0*b_m + k3now;
        gamma_5 = a_h;


      /* lambda */
        lambda = (double)state1[node]*gamma_1 + (double)state2[node]*gamma_2 
             + (double)state3[node]*gamma_3 + (double)state4[node]*gamma_4 
             + (double)state5[node]*gamma_5;

      /* eta */
        eta[0]= 0.0;
        eta[1]= 3.0 * a_m * (double)state1[node];  
        eta[2]= 1.0 * b_m * (double)state2[node];
        eta[3]= 2.0 * a_m * (double)state2[node];
        eta[4]= 1.0 * k1now * (double)state2[node];
        eta[5]= 2.0 * b_m * (double)state3[node];
        eta[6]= 1.0 * a_m * (double)state3[node];
        eta[7]= 1.0 * k2now * (double)state3[node];
        eta[8]= 3.0 * b_m * (double)state4[node];
        eta[9]= 1.0 * k3now * (double)state4[node];
        eta[10]= 1.0 * a_h * (double)state5[node];
   

      /* cummulative state transition prob */
        double tmp=0.0;
        for(int i=0;i<=10;i++){
          tmp+=eta[i];
          P[i]=tmp;
        }

      /* determine which one of those states */ 
        u=genrand_real2()*lambda;
        for(int i=1;i<=10;i++){
          if ((P[i-1]<=u)&&(u<P[i])) {
            state=i;
     	  }
        }

      /* update the number of channels */
	switch(state) {
	    case 1:  state1[node]--; state2[node]++; break; 
        case 2:  state2[node]--; state1[node]++; break;
        case 3:  state2[node]--; state3[node]++; break;
        case 4:  state2[node]--; state5[node]++; break;
        case 5:  state3[node]--; state2[node]++; break;
        case 6:  state3[node]--; state4[node]++; break;
        case 7:  state3[node]--; state5[node]++; break;
        case 8:  state4[node]--; state3[node]++; break;
        case 9:  state4[node]--; state5[node]++; break;
        case 10: state5[node]--; state3[node]++; break;
		default:	
			// no need to break out here, genrand_real2()*lambda was not high enough
			// to warrant a state transition, so we just sail on
			break;
        }

      /* lifetime & update occurrence time */
        t_life=-log(genrand_real2())/lambda;
        t_occ_Na[node]+=t_life;

      }
    
  }
/*
      printf("   m0h0,m1h0,m2h0,m3h0 : %4d %4d %4d %4d\n",
                 m0h0[node],m1h0[node],m2h0[node],m3h0[node]);
      printf("   m0h1,m1h1,m2h1,m3h1 : %4d %4d %4d %4d\n",
                 m0h1[node],m1h1[node],m2h1[node],m3h1[node]);
*/
	int n_Na=state4[node];
	return(n_Na);
}
