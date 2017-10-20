/*
 *  readStimFile.c
 *  cnerve
 *
 *  Created by Nikita on 5/14/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */

#include "readStimFile.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>             // malloc

#define MAX_STIMLIST	90000				///< max # of stimlist entries.  used only in pre-v3 files.  obsolete. XXX

extern int verbosity;

/**
 @defgroup readStimFile								"Read Stimulus Parameters"
 @brief This should really be modularized right...
 */
/// @{

int readStimFile(char *stimFile, stimParam *myParam, const double delt ) {
	
	FILE *stimfp	= NULL;						///< stimulus file pointer

	if( (stimfp = fopen(stimFile, "r")) == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open stimulus file %s\n", stimFile);
		return 1;
	}

	fscanf(stimfp,"%le", &(myParam->current));
	if( myParam->current < 0 ) {
		// it looks like we're dealing with stimfile where we've explicitly
		// defined # of entries.  that means we've just read the
		// -stimListLen instead of current.

		myParam->listLen = (uint32_t) ( -(myParam->current) );
		fscanf(stimfp, "%le", &(myParam->current) );
        
	} else {
		// for the old stimfile format, lets just assume that we will have
		// MAX_STIMLIST number of entries

		myParam->listLen = (uint32_t) MAX_STIMLIST;
	}
	fscanf(stimfp,"%le", &(myParam->rad));
	fscanf(stimfp,"%le", &(myParam->resmed));

	// allocate memory for stimulus amplitude and duration lists

	myParam->durs		= (uint32_t*)	malloc(myParam->listLen * sizeof(uint32_t));
	myParam->amps		= (double*)     malloc(myParam->listLen * sizeof(double));
	if( (myParam->durs == NULL ) || (myParam->amps == NULL) ) {
		fprintf(stderr, "Cannot allocate memory for stimlist: ");
		perror("malloc");
		return 1;
	}

	/* 
	 * WARNING:
	 note, that at this point although myParam.listLen is set, 
	 that number might not be reflective of a true stimlist 
	 length if we were using old stimfile format.  
	 
	 MAKE SURE YOU DONT RELY ON IT TOO MUCH UNTIL THE WHILE LOOP BELOW IS DONE!
	 */

	myParam->nstep = 0;
	{
    /* read the stimulus file.  it's layout is:
	 <amplitude>		<duration>
	 both as doubles.  duration, however, is an integer expressed as double.
	 
	 process this information as follows:
	 - myParam->amps array will consist of amplitudes
	 - calculate cumsum of durations for myParam->durs, such that myParam->durs will contain entries at which a given myParam->amps entries expire:
	 
	 original stim file:		--->		our structures:
	 <amp>	<duration>				myParam->amps:		myParam->durs:
	 0			25						0				25
	 1			5						1				30
	 0			20						0				50
	 this way, when we're iterating in our timeloop, we decide whether to move on to the next myParam->amps/myParam->durs entry
	 by checking whether current time (it) exceeds current myParam->durs entry (i.e., myParam->durs[currStimNum]).  if it does,
	 we'll move to the next entry in myParam->amps/myParam->durs by increasing currStimNum.
	 
	 we keep ourselved from overrunning myParam->amps/myParam->durs arrays by keeping track of the number of stimlist entries
	 (~= lines in *.stim file) with myParam->listLen variable.		
	 
	 */
		
		
		// maybe use for(int i = 0; !feof(stimfp); i++){}   ???
		uint32_t i = 0;
		
		while(!feof(stimfp)){
            double  tmp		= 0.0;

			const int numArgsReadAmps = fscanf(stimfp,"%le", &(myParam->amps[i]));
			const int numArgsReadDurs = fscanf(stimfp,"%le", &tmp);
			
			// was the reading of amp & dur values successful?
			if(numArgsReadAmps > 0 && numArgsReadDurs > 0){
				/* we've successfully read in ith amplitude and duration */
				
				//printf("%d %.1le mA, for %d us \n" ,i ,dpack[50]*myParam->amps[i],myParam->durs[i]);
				
				// update the number of CNERVE_dt-sized steps (typ., 1us) we will need to simulate based on this latest myParam->durs entry
				myParam->nstep += (size_t) tmp;
				
				// set duration of this myParam->amps entry
				myParam->durs[i] = myParam->nstep;
				myParam->listLen = i;
				i++;
			}
		} // while(!feof(stimfp))
	} // end of "read the stimulus file"
	
	fclose(stimfp);
	
	
	if( __builtin_expect( (verbosity), 1) ) {
		printf("[STIMULUS]\n");
		printf("\tresmed =  %lf\n", myParam->resmed);
		printf("\trad =  %lf\n", myParam->rad);
		printf("\tcurrent magnitude: \t%le mA\n",	myParam->current);
		printf("\tstimListLen: \t\t%u\n",			myParam->listLen);
		printf("\tstimulus duration: \t%6.3le ms\t\t\t\t[nstep: %u, timestep: %3.4lf ms]\n",
			   (double) ((myParam->nstep)*delt),
			   myParam->nstep,delt);
	}
	
	assert(myParam->listLen > 0);
	assert(myParam->nstep	> 0);
	
	return 0;
}
