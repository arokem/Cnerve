/*
 * rate_K.h
 *
 *  Created on: Aug 18, 2017
 *      Author: Jesse
 */

#ifndef CODE_V2_6_2DEMYELINPERI_RATE_K_H_
#define CODE_V2_6_2DEMYELINPERI_RATE_K_H_

# include "Ks_Gillespie.h"

/* eTypeRate enums are used in rateNa() and rateK() functions as an idx of
 * what values to return */
typedef enum { ALPHA_M, BETA_M, ALPHA_H, BETA_H, ALPHA_P, BETA_P }	eTypeRateNa;
typedef enum { ALPHA_N, BETA_N, ALPHA_S, BETA_S}					eTypeRateK;

double rate_Na(const eTypeRateNa idx, const double V_m, const kinParam Kins );
double rate_K(const eTypeRateK idx, const double V_m, const kinParam Kins );

#endif /* CODE_V2_6_2DEMYELINPERI_RATE_K_H_ */
