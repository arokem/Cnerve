#ifndef _CNERVE_H_
#define	_CNERVE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>		// for uint32_t


#include "mt19937ar.h"		// for the RNG
#include "readStimFile.h"	// parsing the .stim file
#include "readOptsFile.h"	// parsing the .opts file
#include "readModlFile.h"	// parsing the .modl file
#include "channels.h"
#include "crankNich.h"

#define DEFAULT_MODL_NAME	"default_1_1_1.modl"
#define DEFAULT_STIM_NAME	"default_1_1_1.stim"
#define DEFAULT_OPTS_NAME	"default_1_1_1.opts"
#define DEFAULT_PREFIX		"debugout"
#define DEFAULT_SEED		-176

/* This loses on a few early 2.96 snapshots, but hopefully nobody uses them anymore. */
#if __GNUC__ > 2 || (__GNUC__ == 2 && _GNUC_MINOR__ == 96)
	#define likely(x) __builtin_expect(!!(x), 1)
	#define unlikely(x) __builtin_expect((x), 0)
#else
	#define likely(x) (x)
	#define unlikely(x) (x)
#endif

#ifdef __ICC
/*	ICC doesn't define M_PI in its super-optimised math header, unlike GCC 	*/
#define M_PI  3.14159265358979323846
#include <mathimf.h>
#else
#define M_PI  3.14159265358979323846
#include <math.h>
#endif

//	INITIALIZATION (generation 0), or GENERATIONS (generations > 0)
#define INIT_CHANNEL    true
#define GEN_CHANNEL     false

/*
	Function Declarations
 */
double randm();
void usage(void);

#endif	//_CNERVE_H_
