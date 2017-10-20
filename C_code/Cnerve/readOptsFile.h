/*
 *  readOptsFile.h\
 *  cnerve
 *
 *  Created by Nikita on 5/14/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */
#ifndef READOPTSFILE_H_
#define READOPTSFILE_H_
#include <stdbool.h>
#include <stdint.h>		// defines uint32_t needed here

typedef enum { nafDefault = 0, nafPatlak = 1, nafJeff = 2 }	eTypeNaf;
typedef enum { napNotUsed = 0, napMcIntyre = 1 }			eTypeNap;
typedef enum { kfNotUsed = 0, kfDefault = 1 }				eTypeKf;
typedef enum { ksNotUsed = 0, ksDefault = 1 }				eTypeKs;

typedef struct OptsStructure {
	eTypeNaf typeNaf;
	eTypeNap typeNap;
	eTypeKf  typeKf;
	eTypeKs  typeKs;
	
	double  Vth;						///< Threshold voltage for spike
	bool	doRecordECAP;				///< whether or not to record ECAP for output
	bool	doRecordVltg;				///< whether or not to output voltage
	uint32_t maxSpikes;					///< maximum number of spikes we expect to see. needed to properly reserve space for spiketime output file
	double	delt;						///< time step of simulation
	double	VoltageSample;				///< sampling period for voltage recordings
	double	MaxVoltage;					///< Maximum voltage before error thrown.
//	bool	is_active;
//	bool	is_stochastic;

} optsParam;

int readOptsFile(char *simFile, optsParam *myParam );

#endif /* READMODLFILE_H_ */
