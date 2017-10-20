/*  readStimFile.h
 *  cnerve
 *
 *  Created by Nikita on 5/14/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */


#include <stdint.h>					// for the uint32_t type

typedef struct stimOpts {

// values read in:
	uint32_t	listLen;	///< total # of entries stimAmps/stimDurs arrays.  ~# of lines in *.stim file
	double		current;	///< current magnitude (mA)
	double 		*amps;
	uint32_t 	*durs;		///< how long, in us, to hold respective stimAmps entries
	double 		rad;        ///< radius of electrode
	double		resmed; 	///< resistivity of medium
// calculated values:	
	uint32_t	nstep;		///< # of time steps we'll simulate.  derived from *.stim file
} stimParam;

int readStimFile(char *stimFile, stimParam *myParam, const double delt );
