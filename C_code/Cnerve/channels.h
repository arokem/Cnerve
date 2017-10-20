#ifndef _CHANNELS_H_INCLUDED_
#define _CHANNELS_H_INCLUDED_
/*
 *  channels.h
 *  cnerve
 *
 *  Created by Nikita on 2/15/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include <math.h>			// for misc. log/exp operations
#include <stdbool.h>			// for bool defenition
#include <stdint.h>			// for the uint32_t type
#include <stdio.h>			// for FILE definition
#include <assert.h>

#include "readModlFile.h"
#include "readOptsFile.h"
#include "rateChannels.h"


#ifdef DEBUG
	double Gillespie_Na(		const bool doInit, const uint32_t N_Na,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);
	double Patlak_Na(			const bool doInit, const uint32_t N_Na,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);
	double McIntyre_pNa(		const bool doInit, const uint32_t N_pNa,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp  __attribute__ ((unused)) );

	double Gillespie_K(			const bool doInit, const uint32_t N_K,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);
	double Gillespie_slowK(		const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);

	double Deterministic_Na(	const bool doInit, const uint32_t N_Na __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) );
	double Deterministic_pNa_McIntyre(const bool doInit, const uint32_t N_pNa,	const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins, FILE *fp  __attribute__ ((unused)) );
	double Deterministic_K(		const bool doInit, const uint32_t N_K,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);
	double Deterministic_slowK(	const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins,	FILE *fp);

#else
	double Gillespie_Na(		const bool doInit, const uint32_t N_Na,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);
	double Patlak_Na(			const bool doInit, const uint32_t N_Na,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);
	double McIntyre_pNa(		const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);

	double Gillespie_K(			const bool doInit, const uint32_t N_K,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);
	double Gillespie_slowK(		const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);

	double Deterministic_Na(	const bool doInit, const uint32_t N_Na __attribute__ ((unused)), const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins);
	double Deterministic_pNa_McIntyre(const bool doInit, const uint32_t N_pNa, const double V_m, const double si, const uint32_t it  __attribute__ ((unused)), const uint32_t node, const kinParam Kins);
	double Deterministic_K(		const bool doInit, const uint32_t N_K,	const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);
	double Deterministic_slowK(	const bool doInit, const uint32_t N_slowK,const double V_m, const double si, const uint32_t it, const uint32_t node, const kinParam Kins);

#endif

#endif //_CHANNELS_H_INCLUDED_
