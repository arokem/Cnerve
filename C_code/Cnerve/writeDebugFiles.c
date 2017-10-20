/*
 * writeDebugFiles.c
 *
 *  Created on: Aug 18, 2017
 *      Author: jesse
 */

#include "writeDebugFiles.h"

	/** the following files are used for debugging, they contain
	 output of gating parameters of each indiv. channel:	*/

int initializeDebugFiles(const int argNum,const char *argList,const uint32_t myTotFibs,
		const uint32_t myTotMontes, const stimParam *stim, const uint32_t *globNodNums,
		const uint32_t *myFibsToGlob, const unsigned int endianHdr) {

	FILE *debugNaf_fp = NULL;
	FILE *debugNap_fp = NULL;
	FILE *debugKf_fp = NULL;
	FILE *debugKs_fp = NULL;

	char debugNaf_file[MAX_FILENAME];
	char debugNap_file[MAX_FILENAME];
	char debugKf_file[MAX_FILENAME];
	char debugKs_file[MAX_FILENAME];

	if( unlikely(argNum < 10) ) {
		strncpy(debugNaf_file, DEFAULT_PREFIX, sizeof(debugNaf_file));
		strncpy(debugNap_file, DEFAULT_PREFIX, sizeof(debugNap_file));
		strncpy(debugKf_file, DEFAULT_PREFIX, sizeof(debugKf_file));
		strncpy(debugKs_file, DEFAULT_PREFIX, sizeof(debugKs_file));}
	else {
		strncpy(debugNaf_file, &argList[4], sizeof(debugNaf_file));
		strncpy(debugNap_file, &argList[4], sizeof(debugNap_file));
		strncpy(debugKf_file, &argList[4], sizeof(debugKf_file));
		strncpy(debugKs_file, &argList[4], sizeof(debugKs_file));}

	// add file extensions:
	strncat(debugNaf_file, ".Naf", sizeof(debugNaf_file));
	strncat(debugNap_file, ".Nap", sizeof(debugNap_file));
	strncat(debugKf_file, ".Kf", sizeof(debugKf_file));
	strncat(debugKs_file, ".Ks", sizeof(debugKs_file));

	/*
	 we're debuggin and want to record low-level channel dynamics.
	 - now:		open files
	 - later:	record # open (int32), then relevant coeff's.
	 */
	if( (debugNaf_fp = fopen(debugNaf_file, "w")) == NULL) {
		perror("fopen"); fprintf(stderr, "Cannot open Naf's debug file for writing %s\n", debugNaf_file);
		exit(EXIT_FAILURE);
	}
	if( (debugNap_fp = fopen(debugNap_file, "w")) == NULL) {
		perror("fopen"); fprintf(stderr, "Cannot open Nap's debug file for writing %s\n", debugNap_file);
		exit(EXIT_FAILURE);
	}
	if( (debugKf_fp = fopen(debugKf_file, "w")) == NULL) {
		perror("fopen"); fprintf(stderr, "Cannot open Kf's debug file for writing %s\n", debugKf_file);
		exit(EXIT_FAILURE);
	}
	if( (debugKs_fp = fopen(debugKs_file, "w")) == NULL) {
		perror("fopen"); fprintf(stderr, "Cannot open Ks's debug file for writing %s\n", debugKs_file);
		exit(EXIT_FAILURE);
	}

	/* To properly read the debug files, we need to know the following
	 - endianess, so we know how to read the binary file
	 - # of fibers, # of monte, # of nsteps, # of nodes, since the debug
	 functions that write # of open channels are executed deep w/in nested
	 for loops: (for # of fibs (for # of monte (for # of nsteps (for # of
	 nodes (... ) ) ) ) )
	 */

	// first, write endian header to all the channel files
	fwrite(&endianHdr, sizeof(unsigned int), 1, debugNaf_fp);
	fwrite(&endianHdr, sizeof(unsigned int), 1, debugNap_fp);
	fwrite(&endianHdr, sizeof(unsigned int), 1, debugKf_fp);
	fwrite(&endianHdr, sizeof(unsigned int), 1, debugKs_fp);

	fwrite(&myTotFibs, sizeof(uint32_t), 1, debugNaf_fp);
	fwrite(&myTotFibs, sizeof(uint32_t), 1, debugNap_fp);
	fwrite(&myTotFibs, sizeof(uint32_t), 1, debugKf_fp);
	fwrite(&myTotFibs, sizeof(uint32_t), 1, debugKs_fp);

	fwrite(&myTotMontes, sizeof(uint32_t), 1, debugNaf_fp);
	fwrite(&myTotMontes, sizeof(uint32_t), 1, debugNap_fp);
	fwrite(&myTotMontes, sizeof(uint32_t), 1, debugKf_fp);
	fwrite(&myTotMontes, sizeof(uint32_t), 1, debugKs_fp);

	// number of steps, i.e. the number of times we'll be writing to the file
	fwrite(&(stim->nstep), sizeof(uint32_t), 1, debugNaf_fp);
	fwrite(&(stim->nstep), sizeof(uint32_t), 1, debugNap_fp);
	fwrite(&(stim->nstep), sizeof(uint32_t), 1, debugKf_fp);
	fwrite(&(stim->nstep), sizeof(uint32_t), 1, debugKs_fp);

	// write how many nodes my fibers have
	for(uint32_t i=0; i<myTotFibs; i++) {
		fwrite(&(globNodNums[myFibsToGlob[i]-1]), sizeof(uint32_t), 1, debugNaf_fp);
		fwrite(&(globNodNums[myFibsToGlob[i]-1]), sizeof(uint32_t), 1, debugNap_fp);
		fwrite(&(globNodNums[myFibsToGlob[i]-1]), sizeof(uint32_t), 1, debugKf_fp);
		fwrite(&(globNodNums[myFibsToGlob[i]-1]), sizeof(uint32_t), 1, debugKs_fp);}

	return 0;
}
