/*
 *  readOptsFile.c
 *  cnerve
 *
 *  Created by Nikita on 5/14/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "readOptsFile.h"

extern int verbosity;

/**
 @defgroup readOptsFile								"Read Simulation Options"
 @brief Read in simulation options parameters
 
 */
/// @{
int readOptsFile(char *simFile, optsParam *myParam ) {
	
	FILE *simfp	= NULL;						///< simulation options file pointer
	double  tmp = 0.0;

	
	if ( (simfp = fopen(simFile, "r")) == NULL) {
		fprintf(stderr, "Cannot open simulation file %s\n", simFile);
		return 1;
	}

	fscanf(simfp,"%le", &tmp); myParam->typeNaf= (eTypeNaf)tmp;
	fscanf(simfp,"%le", &tmp); myParam->typeNap= (eTypeNap)tmp;
	fscanf(simfp,"%le", &tmp); myParam->typeKf = (eTypeKf)tmp;
	fscanf(simfp,"%le", &tmp); myParam->typeKs = (eTypeKs)tmp;
	fscanf(simfp,"%le", &tmp); myParam->Vth = tmp;
	fscanf(simfp,"%le", &tmp); myParam->doRecordECAP = (bool)tmp;
	fscanf(simfp,"%le", &tmp); myParam->doRecordVltg = (bool)tmp;
	fscanf(simfp,"%le", &tmp); myParam->maxSpikes = (uint32_t)tmp;
	fscanf(simfp,"%le", &tmp); myParam->delt = tmp;
	fscanf(simfp,"%le", &tmp); myParam->VoltageSample = tmp;
	fscanf(simfp,"%le", &tmp); myParam->MaxVoltage = tmp;
//	fscanf(simfp,"%le", &tmp); myParam->is_active = tmp;
//	fscanf(simfp,"%le", &tmp); myParam->is_stochastic = tmp;
	fclose(simfp);
	
	if( __builtin_expect( (verbosity), 1) ) {
		printf("[SIMULATION OPTIONS]\n");
		printf("\tTypeNa: %d\tUsepNa: %d\tUseK: %d\tUseSlowK: %d\n",	myParam->typeNaf, myParam->typeNap, 
																		myParam->typeKf, myParam->typeKs );
		printf("\tRecordECAP: %d ",		myParam->doRecordECAP );
		printf("\tRecordVoltage: %d \n",	myParam->doRecordVltg );
		printf("\tThreshhold: %lf \n",		myParam->Vth);
		printf("\tMaxVoltage: %lf \n",		myParam->MaxVoltage);
		printf("\tMaxSpikes: %u \n",		myParam->maxSpikes );
	}

	assert(myParam->maxSpikes > 0);
	
	return 0;
}
/// @}



