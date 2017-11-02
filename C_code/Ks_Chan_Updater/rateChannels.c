/*
 * rate_K.c
 *
 *  Created on: Aug 18, 2017
 *      Author: Jesse
 */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "rateChannels.h"
/** fn double rate_K(const eTypeRateK idx, const double V_m)

 \param[in] idx
 - 1: ALPHA_N
 - 2: BETA_N
 - 3: ALPHA_S
 - 4: BETA_S
 \param[in] V_m		in units of mV
 \return rate	in units of 1/ms

 \todo TODO: RETURN BOTH ALPHA AND BETA VALUES IN ONE GO, PACKED INTO 128BIT VECTOR
 */
double rate_K(const eTypeRateK idx, const double V_m, const kinParam Kins ) {

	double rate = -1;

	switch (idx) {
	case ALPHA_N: /* alpha_n */
		rate = Kins.a_n_A * (V_m - Kins.a_n_B) / (1 - exp((Kins.a_n_B - V_m) / Kins.a_n_C));
		break;

	case BETA_N: /* beta_n */
		rate = Kins.b_n_A * (Kins.b_n_B - V_m) / (1 - exp((V_m - Kins.b_n_B) / Kins.b_n_C));
		break;

	case ALPHA_S: /* alpha_s */
		rate = Kins.a_s_A * (V_m - Kins.a_s_B) / (1 - exp((Kins.a_s_B - V_m) / Kins.a_s_C));
		break;

	case BETA_S: /* beta_s */
		rate = Kins.b_s_A * (Kins.b_s_B - V_m) / (1 - exp((V_m - Kins.b_s_B) / Kins.b_s_C));
		break;

	default:
		// inform, but do not break out (too aggressive, but we DO need to catch any errant calls)
		fprintf(stderr, "Unexpected idx (%d) encountered in rate_K!\n",
				(int) idx);
		break;
	}
	return (rate);
}

/**
 \fn double rate_Na(const eTypeRateNa idx, const double V_m)

 \param[in] idx
 - 1: ALPHA_M
 - 2: BETA_M
 - 3: ALPHA_H
 - 4: BETA_H
 \param[in] V_m		in units of mV
 \return rate	in units of 1/ms

 */
double rate_Na(const eTypeRateNa idx, const double V_m, const kinParam Kins ) {

	double rate = -1;

	switch (idx) {
	case ALPHA_M: /* alpha_m */
		rate = Kins.a_m_A * (V_m - Kins.a_m_B) / (1 - exp((Kins.a_m_B - V_m) / Kins.a_m_C));
		break;

	case BETA_M: /* beta_m */
		rate = Kins.b_m_A * (Kins.b_m_B - V_m) / (1 - exp((V_m - Kins.b_m_B) / Kins.b_m_C));
		break;

	case ALPHA_H: /* ALPHA_H */
		rate = Kins.a_h_A * (Kins.a_h_B - V_m) / (1 - exp((V_m - Kins.a_h_B) / Kins.a_h_C));
		break;

	case BETA_H: /* beta_h */
		rate = Kins.b_h_A / (1 + exp((Kins.b_h_B - V_m) / Kins.b_h_C));
		break;

	case ALPHA_P: /* alpha_p */									// PERSISTENT NA
		rate = Kins.a_p_A * (V_m - Kins.a_p_B) / (1 - exp((Kins.a_p_B - V_m) / Kins.a_p_C));
		break;

	case BETA_P: /* beta_p */									// PERSISTENT NA
		rate = Kins.b_p_A * (Kins.b_p_B - V_m) / (1 - exp((V_m - Kins.b_p_B) / Kins.b_p_C));
		break;

	default:
		// inform, but do not break out (too aggressive, but we DO need to catch any errant calls)
		fprintf(stderr, "Unexpected idx (%d) encountered in rate_Na!\n",
				(int) idx);
		break;
	}

	return (rate);
}
