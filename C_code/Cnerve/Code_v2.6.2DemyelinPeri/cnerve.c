/**
 @file	cnerve_dmp.c
 @brief	Performs simulation of E-field and current propagation along neuron. 
 Simulates partial demyelination of axon. 

 This program attempts to do it all.  I will refactor it into OO framework just
 as soon as I have a spare moment.

 @author		Jesse Resnick,				2016-		modified peripheral demyelination to fix limit issues.
 @author		Elle O'Brien, 				2016-2016	   added peripheral demyelination
 @author		Nikita Imennov <imennov@uw.edu>		2007-2014
 @author		Jeff Longnion				2005-2006
 @author		Hiroyuki Mino, 				2000-    , modified from exactax.c Ver0.1 May.12,00 by hiro_mino
 @author		Jay Rubinstein, 			    -1994, modified from exactax.f written by jay
 
 @version	$LastChangedRevision: 378 $
 
 $LastChangedDate: 2016-11-08 14:20:57 -0700 (Wed, 03 Nov 2010) $
 */

//#include <gsl/gsl_math.h>
/*
 - Function: double gsl_pow_int (double x, int n)

 This routine computes the power x^n for integer n. The power is computed efficiently—for example, x^8 is computed as ((x^2)^2)^2, requiring only 3 multiplications. A version of this function which also computes the numerical error in the result is available as gsl_sf_pow_int_e. 

 - Function: double gsl_pow_2 (const double x)
 - Function: double gsl_pow_3 (const double x)
 - Function: double gsl_pow_4 (const double x)
 - Function: double gsl_pow_5 (const double x)
 - Function: double gsl_pow_6 (const double x)
 - Function: double gsl_pow_7 (const double x)
 - Function: double gsl_pow_8 (const double x)
 - Function: double gsl_pow_9 (const double x)

 These functions can be used to compute small integer powers x^2, x^3, etc. efficiently. The functions will be inlined when HAVE_INLINE is defined, so that use of these functions should be as efficient as explicitly writing the corresponding product expression. 

 #include <gsl/gsl_math.h>
 double y = gsl_pow_4 (3.141)  // compute 3.141**4 
 */
#include <string.h>
#include <stdlib.h>

#include "../cnerve.h"

	const int verbosity = 2;				///< how much information to print out
	const unsigned int endianHdr = 0xDEADBEEF;///< header we use to determine if BE or LE processor is writing binary files

/**	@brief The main function

 it will do some stuff...fix this description, it's incomplete...


 \param	argc		automatically set to # of args + 1
 \param	argv[]	cmd-line arguments:
 - \c	modlFile: ____.modl
 - \c	stimFile:	____.stim
 - \c	optsFile: ____.opts
 - \c	vltgFile		_-_-.vltg spksFile		_-_-.spks ecapFile		_-_-.ecap
 - \c	#, seed
 - \c	#, myTotMontes
 - \c	#, myTotFibs
 - \c	#, fibFirst
 - \c	#, fibStride


 @return 	0 if all is well, 1 if not
 @warning 	It is a little confusing to read
 */
int main(int argc, char *argv[]) {
	// Initialize I/O variables
	char modlFile[MAX_FILENAME];	// input file names
	char stimFile[MAX_FILENAME];
	char optsFile[MAX_FILENAME];

	char vltgFile[MAX_FILENAME]; 	// output file name
	char spksFile[MAX_FILENAME];
	char ecapFile[MAX_FILENAME];

	FILE *vfp = NULL;				///< voltage output file pointer
	FILE *ofp = NULL;				///< spike timing output file pointer
	FILE *efp = NULL;				///< ECAP output file pointer

	// Initialize Expt global variables read from ARGS
	uint32_t myTotMontes,	///< total # of MC simulations
			myTotFibs;///< total # of fibers (for this job, not for whole expt)

	uint32_t *globNodNums = NULL;
	uint32_t *myFibsToGlob = NULL;///< map of THIS INSTANCE fiber numbers to GLOBAL fiber numbers for the whole experiment
	uint32_t fibFirst;///< passed as an argument, first fiber to process, as global fiber ID
	uint32_t fibStride;	///< passed as an argument, stride in assigning fibers to THIS INSTANCE

	// Report and calculate time only if *really* interested
	long int gts1 = 0, gts2 = 0;		///< declared for keeping time
	if (__builtin_expect(verbosity > 1, 0)) {
		/** start time */
		long int tt = time(NULL);
		gts1 = mktime(gmtime(&tt));
	}

	/******* 	Parse Inputs	******/

	if (argc < 10) {
#ifdef DEBUG
		strncpy(modlFile, DEFAULT_MODL_NAME, sizeof(modlFile));
		strncpy(stimFile, DEFAULT_STIM_NAME, sizeof(stimFile));
		strncpy(optsFile, DEFAULT_OPTS_NAME, sizeof(optsFile));
		strncpy(vltgFile, DEFAULT_PREFIX, sizeof(vltgFile));
		strncpy(spksFile, DEFAULT_PREFIX, sizeof(spksFile));
		strncpy(ecapFile, DEFAULT_PREFIX, sizeof(ecapFile));
		myTotMontes = 10;
		myTotFibs = 2;
		fibFirst = 1;
		fibStride = 1;

		// initialize with given seed #
		init_genrand( (unsigned long) DEFAULT_SEED );
#else
		printf("MAX_FILENAME: %d\n", MAX_FILENAME);
		printf("MAX_RAN_NODES: %d\n", MAX_RAN_NODES);
		usage();
		exit(EXIT_FAILURE);
#endif	
	} else {
		strncpy(modlFile, argv[1], sizeof(modlFile));
		strncpy(stimFile, argv[2], sizeof(stimFile));
		strncpy(optsFile, argv[3], sizeof(optsFile));
		strncpy(vltgFile, argv[4], sizeof(vltgFile));
		strncpy(spksFile, argv[4], sizeof(spksFile));
		strncpy(ecapFile, argv[4], sizeof(ecapFile));
		init_genrand(strtoul(argv[5], NULL, 10));	//< initialise mt-rng with seed
		myTotMontes = strtoul(argv[6], NULL, 10);
		myTotFibs = strtoul(argv[7], NULL, 10);
		fibFirst = strtoul(argv[8], NULL, 10);
		fibStride = strtoul(argv[9], NULL, 10);
	}

	// finish construction of the filename:
	strncat(vltgFile, ".vltg", sizeof(vltgFile));
	strncat(spksFile, ".spks", sizeof(spksFile));
	strncat(ecapFile, ".ecap", sizeof(ecapFile));

	assert(myTotMontes > 0);
	assert(myTotFibs > 0);

	// allocate fibers
	myFibsToGlob = (uint32_t *) malloc(myTotFibs * sizeof(uint32_t));
	if (myFibsToGlob == NULL) {
		fprintf(stderr, "Cannot allocate memory for myFibsToGlob: ");
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// figure out a list of fibers i'm going to process
	for (uint32_t i = 0; i < myTotFibs; i++) {
		myFibsToGlob[i] = fibFirst + (i) * fibStride;
	}

	if (__builtin_expect(verbosity, 1)) {
		printf("[COMMAND-LINE SETTINGS:]\n");
		printf("  (in)\tmodlfile\t= %s\n", modlFile);
		printf("  (in)\tstimFile\t= %s\n", stimFile);
		printf("  (in)\toptsFile\t= %s\n", optsFile);
		printf("  (out)\tvltgFile\t= %s\n", vltgFile);
		printf("  (out)\tspksFile\t= %s\n", spksFile);
		printf("  (out)\tecapFile\t= %s\n", ecapFile);

		if (unlikely(argc < 10)) {
			printf("  (seed)\t\t\t %ld\n", (long) DEFAULT_SEED);
		} else {
			printf("  (seed)\t\t\t %s\n", argv[5]);
		}

		printf("\tMonte Carlo simulations: %u\t\tmyTotFibs: %u\n",
				myTotMontes, myTotFibs);

		printf("\tFiber numbers to process: ");
		for (uint32_t i = 0; i < myTotFibs; i++) {
			printf("\t %u", myFibsToGlob[i]);
		}
		printf(".\n");
	}

	/*****			Read Simulation Options File		*****/
	optsParam Opts; ///< simulation options structure, @see readOptsFile.h
	if (readOptsFile(optsFile, &Opts)) {
		exit(EXIT_FAILURE);
	}
	double delt = Opts.delt; ///< Simulation time step. Used repeatedly in calculations.

	/*********************/
	/* Read Model Parameter File  */
	/*********************/
	modlParam Modl; ///< model parameters structure, @see readModlFile.h
	kinParam Kins; 	///< channel kinetics parameter structure, @see readModlFile.h
	if (readModlFile(modlFile, &Modl, &Kins, Opts, verbosity)) {
		exit(EXIT_FAILURE);
	}

	/* Error check Modl input. XXX: To Do-flush this out */
	assert(myTotFibs <= Modl.globTotFibs);// check that we don't process more fibers than there are specified in the model
	assert(myFibsToGlob[myTotFibs - 1] <= Modl.globTotFibs);	// make sure the last fiber we process is not greater than overall population of fibers

	/*******	SCAN IN STIMULUS PARAMETERS	******/
	stimParam stim;	///< stimulation parameters structure, @see readStimFile.h
	if (readStimFile(stimFile, &stim, delt)) {
		exit(EXIT_FAILURE);
	}

	// Unpack constants from structures.
	const uint32_t insegs = Modl.insegs;
	const double nlen = Modl.nlen;
	const double rmnod = Modl.rmnod;
	const double cmnod = Modl.cmnod;
	const double ranod = Modl.ranod;
	const double rmmy = Modl.rmmy;
	const double cmmy = Modl.cmmy;
	const double ramy = Modl.ramy;
	const double E_rest = Modl.E_rest;
	const double resmed = stim.resmed;
	const double rad = stim.rad;
	uint32_t globTotFibs = Modl.globTotFibs;
	uint32_t globMaxNodes = Modl.globMaxNodes;

	// Unpack chanParam structures from Modl.
	chanParam Naf = Modl.NafParam;
	chanParam Nap = Modl.NapParam;
	chanParam Kf = Modl.KfParam;
	chanParam Ks = Modl.KsParam;

	/* Initialize derived quantities that change each fiber loop iteration */
	uint32_t MAX_SEGMENTS = Modl.globMaxNodes*10;
	double *rm; rm = (double *) malloc(MAX_SEGMENTS * sizeof(double));
	double *cm; cm = (double *) malloc(MAX_SEGMENTS * sizeof(double));
	double *ra; ra = (double *) malloc(MAX_SEGMENTS * sizeof(double));

	if ((rm == NULL) || (cm == NULL) || (ra  == NULL)) {
		fprintf(stderr, "\n rm, cm, or ra memory did not allocate!\n");
		perror("malloc");
		exit(EXIT_FAILURE); // throw error and exit; this is a catastrophic event
	}
	uint32_t * nodeToSegmnt = NULL, * spike_count = NULL;
	nodeToSegmnt = (uint32_t *) malloc(Modl.globMaxNodes * sizeof(uint32_t)); //< maps nodes of Ranvier to segment #.
	spike_count = (uint32_t *) malloc(Modl.globMaxNodes * sizeof(uint32_t)); //< number of spikes at a given node of Ranvier
	if ((nodeToSegmnt == NULL) || (spike_count == NULL)) {
		fprintf(stderr, "\n nodeToSegment or spike_count memory did not allocate!\n");
		perror("malloc");
		exit(EXIT_FAILURE); // throw error and exit; this is a catastrophic event
	}

	/*******	Initialize Output Variable		******/
	uint32_t *allSpikeTimes = NULL;	///< memory space for spike times for all fibers, nodes, and montes
	if ((allSpikeTimes = (uint32_t *) calloc(
			myTotFibs * globMaxNodes * myTotMontes * Opts.maxSpikes,
			sizeof(uint32_t))) == NULL) {
		fprintf(stderr, "\n allSpikeTimes memory did not allocate!\n");
		perror("calloc(allSpikeTimes)");
		exit(EXIT_FAILURE); // throw error and exit; this is a catastrophic event
	}

	double *ecap = NULL; ///< memory space for ECAP reading at each time step
	if (unlikely(Opts.doRecordECAP)) {
		ecap = (double *) calloc(stim.nstep, sizeof(double));
		if (ecap == NULL) {
			fprintf(stderr, "\n ecap memory did not allocate!\n");
			perror("calloc");
			exit(EXIT_FAILURE); // throw error and exit; this is a catastrophic event
		}
	}

	/* Write voltage header, if using. Append voltage info as it's produced.
	 * Voltages used in simulation even if not recorded so initialized elsewhere */
	if (unlikely(Opts.doRecordVltg)) {
		if ((vfp = fopen(vltgFile, "w")) == NULL) {
			fprintf(stderr, "Cannot open vfile %s\n", vltgFile);
			exit(EXIT_FAILURE); // throw error and exit; this is a catastrophic event
		}

		fwrite(&endianHdr, sizeof(unsigned int), 1, vfp); // endian header
		fwrite(&myTotFibs, sizeof(uint32_t), 1, vfp);
		fwrite(myFibsToGlob, sizeof(uint32_t), myTotFibs, vfp);

		// write how many nodes my fibers have
		for (uint32_t i = 0; i < myTotFibs; i++) {
			fwrite(&(Modl.nodeNums[myFibsToGlob[i] - 1]), sizeof(uint32_t), 1,
					vfp);
		}

		fwrite(&stim.nstep, sizeof(uint32_t), 1, vfp);
		fwrite(&E_rest, sizeof(double), 1, vfp);
		fwrite(&Opts.delt, sizeof(double), 1, vfp);
	}
#ifdef DEBUG
	int initializeDebugFiles(const int argc,const char *argv,const uint32_t myTotFibs,
			const uint32_t myTotMontes, const stimParam *stim, const uint32_t *globNodNums,
			const uint32_t *myFibsToGlob);
#endif

	/********************************************************************************
	 *
	 * 				START FIBER LOOPS
	 *
	 ********************************************************************************/
	for (uint32_t idxMyFib = 0; idxMyFib < myTotFibs; idxMyFib++) {

		const uint32_t idxGlobFib = myFibsToGlob[idxMyFib] - 1;	///< map OUR fiber number to GLOBAL fib #

		printf("Processing fiber No.  %u:\t\t%u of %u fibers assigned to me (from a pop of %u)\n",
				idxGlobFib + 1, idxMyFib + 1, myTotFibs, globTotFibs);

		// Pull fiber specific parameters from fiberlist arrays.
		const double d = Modl.diameters[idxGlobFib];		///< internal diameter of axon (mm)
		const uint32_t nodnum = Modl.nodeNums[idxGlobFib]; 	///< number of active nodes
		assert(nodnum > 0);
		const double zs = Modl.zs[idxGlobFib]; 				///< radial distance to electrode (mm)
		assert(zs > 0);
		const uint32_t cutoff = Modl.cutoffs[idxGlobFib];	///< node demarcating
		assert(cutoff >= 0 && cutoff <= nodnum);
		const double demydtoD = Modl.demydtoD[idxGlobFib];	///< dtoD of demyelinated segments
		assert(demydtoD <= 1 && demydtoD >= 0.6);
		if (unlikely(verbosity >= 11)){
					printf("\td:%.5lf\t nodeNums:%u\t zs:%.1lf\t cutoff:%u\t demydtoD:%.3lf\n",
							d,nodnum,zs,cutoff,demydtoD);}
		const double D = d / Modl.dtoD; ///< fiber diameter (axon+myelin sheath) (mm)
		const double demyD = d / demydtoD; ///<demyelinated fiber diameter
		double invdemycmmy = log(demyD/d)/log(D/d)*(1/cmmy-1/(cmnod*M_PI*d))+1/(cmnod*M_PI*d);
		const double h2 = D * Modl.LtoD / insegs;///< length of an internode (mm)
		const double area_mm = M_PI * Modl.constrict * nlen * d; ///< surface area at node of ranvier (mm^2)
		assert(area_mm > 0.0);
		if (unlikely(verbosity >= 11)){
					printf("\tD:%.5lf\t h2:%.5lf\t area_mm:%.10lf\n",
							D,h2,area_mm);}

		// Calculate number of channels per active node
		Naf.numChan = (uint32_t) (area_mm * Naf.density);
		Nap.numChan = (uint32_t) (area_mm * Nap.density);
		Kf.numChan = (uint32_t) (area_mm * Kf.density);
		Ks.numChan = (uint32_t) (area_mm * Ks.density);

		assert(Naf.numChan > 0);
		assert(Nap.numChan > 0);
		assert(Kf.numChan > 0);
		assert(Ks.numChan > 0);

		if (unlikely(verbosity >= 11)) {
			printf("\tChannels:     Naf = %u\t\tKf = %u\t\tKs = %u\t\tNap = ",
					Naf.numChan, Kf.numChan, Ks.numChan);
			if (Opts.typeNap == napNotUsed) {
				printf("n/a\n");}
			else {
				printf("%u\n", Nap.numChan);}
		}

		/*  SET DERIVED GEOMETRIC AND ELECTRIC PROPERTIES  */

		const uint32_t ndx = (insegs + 1) * nodnum;	///< total number of fiber segments
		assert(ndx > 1);
		const double xb = Modl.stimLoc;/// axial distance from end of axon in mm  - negative means overlapping axon
		double xset[ndx];///< width of a segment i.  depends on how many segments there are, what type of segment
		for (uint32_t currNode = 0; currNode < nodnum; currNode++) {

			/** set spatial property of the fiber -- its length */
			xset[currNode * (insegs + 1)] = nlen;

			/** set electric properties of fiber nodes */
			rm[currNode * (insegs + 1)] = (double) rmnod / area_mm;///< membrane resistance at node
			cm[currNode * (insegs + 1)] = (double) cmnod * area_mm;///< membrane capacitance at node
			ra[currNode * (insegs + 1)] = (double) ranod * nlen
					/ (M_PI * pow(d / 2.0, 2.0));///< axoplasmic resistance at node
			nodeToSegmnt[currNode] = (uint32_t) currNode * (insegs + 1);
			/** set internode distances and electrical properties */
			for (uint32_t k = 0; k < insegs; k++) {
				xset[currNode * (insegs + 1) + k + 1] = h2;

				/** set electrical properties for demyelinated segment */
				// XXX: This section is wrong now that d varies between fibers...
				if (currNode < cutoff + 1) {
					/* NOTE: this is based on an assumption of
					 lack of compensatory channel redistribution */
					rm[currNode * (insegs + 1) + k + 1] = rmmy / h2;
					// XXX: Check cm calculation is correct. Look at notebook.

					cm[currNode * (insegs + 1) + k + 1] = 1/invdemycmmy * h2;

					ra[currNode * (insegs + 1) + k + 1] = ramy * h2
							/ (M_PI * pow(d / 2.0, 2.0)); ///< axoplasmic resistance. May want to alter.
				}
				/** set electrical properties for the rest of the axon */
				else {
					rm[currNode * (insegs + 1) + k + 1] = rmmy / h2;
					cm[currNode * (insegs + 1) + k + 1] = cmmy * h2;
					ra[currNode * (insegs + 1) + k + 1] = ramy * h2
							/ (M_PI * pow(d / 2.0, 2.0));
				}
			}
		}

		/* set x values along fiber */
		double x[ndx + 1];///< stores spatial location (x-coord) at which the segment i begins. width of this segment is xset[i]
		x[0] = 0.0;
		double tmp = 0.0;

		assert(ndx + 1 > nodnum);      // this assert needs to be verified!

		for (uint32_t i = 1; i < ndx + 1; i++) {
			// XXX: why not use x[i] = x[i-1] + xset[i-1]; ???
			tmp += xset[i - 1];
			x[i] = tmp;
		}

		if (unlikely(verbosity >= 11)) {
			for (uint32_t i = 0; i < ndx; i++) {
				printf("rm:%.0lf\t cm:%.12lf\t ra:%.0lf\t x:%.3f\n ",
						rm[i],cm[i],ra[i],x[i]);
			}
		}

		/*******************************************************************************************
		 CALCULATE EXTRACELLULAR RESISTIVITY between electrodes and each segment

		 TODO: put formula in here, w/notations. Make sure this section is correct.

		 ********************************************************************************************/
		double *field = NULL;
		field = (double *) malloc(ndx * sizeof(double));
		for (uint32_t i = 0; i < ndx + 1; i++) {
			field[i] = resmed / (4.0 * M_PI * rad) * asin(
						2 * rad/ (sqrt(pow(zs, 2.0)
						+ pow(x[i] + xb- rad,2.0)) + sqrt(pow(zs,2.0)
						+ pow(x[i] + xb + rad,2.0))));
		}

		/**
		 SETUP ECAP resistivity
		 */
		double *ecapRes = NULL;
		if (Opts.doRecordECAP) {
			ecapRes = (double *) malloc(ndx * sizeof(double));
			for (uint32_t i = 0; i < ndx; i++) {
				/// resistance from each point source of axon
				ecapRes[i] = resmed / (4.0 * M_PI)/ (sqrt(pow(Modl.ECAPheight, 2.0)
						+ pow(Modl.ECAPaway- (x[i] + x[i + 1])/ 2.0, 2.0)));
			}
		}
		/******************************************************************************************
		 INITIALISE NON-STOCHASTIC VARIABLES FOR CRANK-NICHOLSON CALCULATIONS
		 allocate just enough space to hold ndx entries

		 *******************************************************************************************/

		double *Vapp = NULL;///< extracellular potential, calc from the current & amp. of stimulus

		double *xina = NULL; /**< amount of current entering each fib segment.
		 at nodes of Ranvier, this is prop. to # of channels open & their conductivity
		 elsewhere (myelenated segments, e.g.), it is 0. */

		/* variables for tridiag solver */
		double *a = (double*)			///< sub diagonal of ndx by ndx matrix A
				calloc(ndx, sizeof(double));
		double *b = (double*)				///< diagonal of ndx by ndx matrix A
				calloc(ndx, sizeof(double));
		double *c = (double*)		///< super diagonal of ndx by ndx matrix A
				calloc(ndx, sizeof(double));

		double *r = (double*)	///< r, what "A u" equals, for tridiag solution
				calloc(ndx, sizeof(double));

		double *dx1 = (double*) ///< Constant used in RHS calculation (A*u = r)
				calloc(ndx, sizeof(double));
		double *dx2 = (double*) ///< Constant used in RHS calculation (A*u = r)
				calloc(ndx, sizeof(double));
		double *ux = (double*) ///< Constant used in RHS calculation (A*u = r)
				calloc(ndx, sizeof(double));

		/** init w/zeros to set initial and boundary conditions */
		if (unlikely(
				(Vapp = (double*) calloc( ndx+1, sizeof(double) )) == NULL)) {
			perror("calloc(Vapp)");
			exit(EXIT_FAILURE);
		}

		if (unlikely(
				(xina = (double*) calloc( ndx, sizeof(double) )) == NULL)) {
			perror("calloc(xina)");
			exit(EXIT_FAILURE);
		}

		/** set nonchanging matrix entries */
		a[0] = 0.0;
		b[0] = -(1.0 + ra[0] / rm[0] + 2.0 * ra[0] * cm[0] / delt);
		c[0] = 1.0;

		for (uint32_t j = 1; j < ndx; j++) {
			a[j] = ra[j] / ra[j - 1];
			b[j] = -(1.0 + ra[j] / ra[j - 1] + ra[j] / rm[j]
					+ 2.0 * ra[j] * cm[j] / delt);
			c[j] = 1.0;
			dx1[j] = a[j];
			dx2[j] = 1 + dx1[j];
			ux[j] = dx2[j] + ra[j] / rm[j] - 2 * ra[j] * cm[j] / delt;
		}
		c[ndx - 1] = 0.0;

		/// Begin Monte Carlo loops
		for (uint32_t idxMyMonte = 0; idxMyMonte < myTotMontes; idxMyMonte++) {
			printf("Starting Monte Carlo #: %u/%u\n", idxMyMonte + 1,
					myTotMontes);

			/** declared for voltage waveform*/
			double *Vm = NULL; /*< transmembrane voltage for current time iteration, defined
			 w/outside as a ground (= 0).  (mV) */
			double *Vm_prev = NULL;	///< transmembrane voltage for time t-1 (i.e. the preceeding time step)

			/** init w/zeros to set initial conditions for each Monte */
			if (unlikely(
					(Vm = (double*) calloc( ndx+1, sizeof(double) )) == NULL)) {
				perror("calloc(Vm)"); exit(EXIT_FAILURE);
			}

			if (unlikely(
					(Vm_prev = (double*) calloc( ndx+1, sizeof(double) )) == NULL)) {
				perror("calloc(Vm)"); exit(EXIT_FAILURE);
			}
			/***************************************************************** 

			 Initialize channel states @ each node of Ranvier
			 
			 *****************************************************************/

			for (uint32_t currNode = 0; currNode < nodnum; currNode++) {
				const uint32_t currNodeAsSegmnt = nodeToSegmnt[currNode];
#ifdef DEBUG
				Naf.numOpen= Naf.calcNumOpenChan(INIT_CHANNEL, Naf.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,0,currNode, Kins,debugNaf_fp);
#else
				Naf.numOpen = Naf.calcNumOpenChan(INIT_CHANNEL, Naf.numChan,
						Vm[currNodeAsSegmnt] + E_rest, delt, 0, currNode, Kins);
#endif				

				if (Opts.typeNap != napNotUsed) {
#ifdef DEBUG
					Nap.numOpen= Nap.calcNumOpenChan(INIT_CHANNEL, Nap.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,0,currNode, Kins, debugNap_fp);
#else
					Nap.numOpen = Nap.calcNumOpenChan(INIT_CHANNEL, Nap.numChan,
							Vm[currNodeAsSegmnt] + E_rest, delt, 0, currNode, Kins);
#endif
				}

				if (likely(Opts.typeKf != kfNotUsed)) {
#ifdef DEBUG
					Kf.numOpen= Kf.calcNumOpenChan(INIT_CHANNEL, Kf.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,0,currNode, Kins,debugKf_fp);
#else
					Kf.numOpen = Kf.calcNumOpenChan(INIT_CHANNEL, Kf.numChan,
							Vm[currNodeAsSegmnt] + E_rest, delt, 0, currNode, Kins);
#endif
				}

				if (likely(Opts.typeKs != ksNotUsed)) {
#ifdef DEBUG
					Ks.numOpen= Ks.calcNumOpenChan(INIT_CHANNEL, Ks.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,0,currNode, Kins,debugKs_fp);
#else
					Ks.numOpen = Ks.calcNumOpenChan(INIT_CHANNEL, Ks.numChan,
							Vm[currNodeAsSegmnt] + E_rest, delt, 0, currNode, Kins);
#endif
				}

				spike_count[currNode] = 0;
//				printf("n_Na = %lf\t, Kf.numOpen = %lf\t, Ks.numOpen = %lf\t",Naf.numOpen, Kf.numOpen, Ks.numOpen);
			} // Initialize channel states @ each node of Ranvier

			/****************************************************************************
			 * START TIME LOOP
			 *	We're about to start the clock (time loop), so when the time comes,
			 *	we will want to start with the first stimulus we've received.  Set 
			 *	stimulus counter (currStimNum) to the first item.
			 ****************************************************************************/
			for (uint32_t it = 0, currStimNum = 0; it < stim.nstep; it++) { ///< it: time loop iterator.  always < nstep
																			///< currStimNum: current entry in stimAmps/stimDurs array
				/** Check whether we're supposed to move on to the next stimlist entry */
				while (stim.durs[currStimNum] < it && currStimNum < stim.listLen) {
					/*   since stimDurs is a cumsum list of stimuli durations, check that current time step has NOT exceeded
					 the number of steps we're supposed to hold stimAmps[currStimNum].
					 if we've exceeded duration assigned to amplitude stimAmps[currStimNum], and we're not done yet 
					 (currStimNum < stimListLen), then move on to the next stimulus with "currStimNum++"
					 */
					currStimNum++;
					/*printf("currStimNum = %d\n",currStimNum);*/
				}

				/** set applied voltage based on current, amplitude of stimulus, and e-field */
				for (uint32_t ix = 0; ix < ndx + 1; ix++) {
					Vapp[ix] = field[ix] * stim.current
							* stim.amps[currStimNum];
				}

				/** status updater */
				if (unlikely(verbosity > 11)) {
					if (it % 5000 == 40) {
						const long int tt = time(NULL);
						gts2 = mktime(gmtime(&tt));

						const double tmpProgress =
								(double) (idxMyFib * myTotMontes * stim.nstep
										+ idxMyMonte * stim.nstep + it)
										/ (double) (myTotFibs * myTotMontes
												* stim.nstep);

						printf("\t%0.4lf percent done, % 4.2lf minutes left\n",
								100.0 * tmpProgress,
								(double) (gts2 - gts1) / 60.0
										* (1.0 / (tmpProgress + 0.00000001) - 1));
					}
				}

				/** compute ion currents at each node
				 * XXX: Why isn't the case handling done in calcNumOpenchannels? */
				for (uint32_t currNode = 0; currNode < nodnum; currNode++) {
					const uint32_t currNodeAsSegmnt = currNode*(insegs+1);
					// const uint32_t currNodeAsSegmnt = nodeToSegmnt[currNode]; //< Same issue...

#ifdef DEBUG
					Naf.numOpen = Naf.calcNumOpenChan(GEN_CHANNEL, Naf.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,it,currNode, Kins, debugNaf_fp);
#else
					Naf.numOpen = Naf.calcNumOpenChan(GEN_CHANNEL, Naf.numChan,
							Vm[currNodeAsSegmnt] + E_rest, delt, it, currNode, Kins);
#endif				
					/** 
					 *	driving force 	:= (Vm + Erest - Naf.eRev)
					 * 	current in		:= g_Naf * (driving force)
					 */
					xina[currNodeAsSegmnt] = Naf.gMax * Naf.numOpen
							* ((Vm[currNodeAsSegmnt] + E_rest) - Naf.eRev);

					if (unlikely(Opts.typeNap != napNotUsed)) {
#ifdef DEBUG
						Nap.numOpen = Nap.calcNumOpenChan(GEN_CHANNEL, Nap.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,it,currNode, Kins, debugNap_fp);
#else
						Nap.numOpen = Nap.calcNumOpenChan(GEN_CHANNEL,
								Nap.numChan, Vm[currNodeAsSegmnt] + E_rest,
								delt, it, currNode, Kins);
#endif
						xina[currNodeAsSegmnt] += Nap.gMax * Nap.numOpen
								* ((Vm[currNodeAsSegmnt] + E_rest) - Nap.eRev);
					}

					if (likely(Opts.typeKf != kfNotUsed)) {
#ifdef DEBUG
						Kf.numOpen = Kf.calcNumOpenChan(GEN_CHANNEL, Kf.numChan,Vm[currNodeAsSegmnt]+E_rest,delt,it,currNode, Kins, debugKf_fp);
#else
						Kf.numOpen = Kf.calcNumOpenChan(GEN_CHANNEL, Kf.numChan,
								Vm[currNodeAsSegmnt] + E_rest, delt, it,
								currNode, Kins);
#endif
						xina[currNodeAsSegmnt] += Kf.gMax * Kf.numOpen
								* ((Vm[currNodeAsSegmnt] + E_rest) - Kf.eRev);
					}

					if (likely(Opts.typeKs != ksNotUsed)) {
#ifdef DEBUG
						Ks.numOpen = Ks.calcNumOpenChan(GEN_CHANNEL, Ks.numChan, Vm[currNodeAsSegmnt]+E_rest, delt, it, currNode, Kins, debugKs_fp);
#else
						Ks.numOpen = Ks.calcNumOpenChan(GEN_CHANNEL, Ks.numChan,
								Vm[currNodeAsSegmnt] + E_rest, delt, it,
								currNode, Kins);
#endif
						xina[currNodeAsSegmnt] += Ks.gMax * Ks.numOpen
								* ((Vm[currNodeAsSegmnt] + E_rest) - Ks.eRev);
					}

				}      // for (uint32_t currNode=0;currNode < nodnum;currNode++)

					   // --- by jay
				// Vm[0] = 0.0; ///< Membrane potential at end of fiber. TODO: Confirm this is best approach.

				double dist = 2.0 * (Vapp[1] - Vapp[0]);///< Calculate axonal potential flux through first node segment. 

				r[0] = (1.0 + ra[0] / rm[0] - 2.0 * ra[0] * cm[0] / delt)
						* Vm[0] - Vm[1] - dist + ra[0] * xina[0]; ///< Calculate RHS for first node segment.

				for (uint32_t i = 1; i < ndx; i++) {
					dist = dx1[i] * Vapp[i - 1] - dx2[i] * Vapp[i]
							+ Vapp[i + 1]; ///< Axonal potential flux through segment i. 
					r[i] = -dx1[i] * Vm[i - 1] + ux[i] * Vm[i] - Vm[i + 1]
							- 2 * dist + 2.0 * ra[i] * xina[i]; ///< RHS for segment i. xina is 0 for internodes.
				}

				/**
				 *
				 SOLVE TRIDIAGONAL MATRIX EQUATION
				 (i.e., calculate new Vm value)

				 */

				// save voltage levels from this timestep for later b/c we'll
				// use it to check if a spike has occurred:
				double *ptrHolder = Vm;
				Vm = Vm_prev;
				Vm_prev = ptrHolder;

				// solve the eqn, overwriting Vm with values that'll be used the next timestep
				tridag(a, b, c, r, Vm, ndx);

				/* 
				 * Check whether the voltage we've calculated makes sense 
				 */
				int badflag = 0;

				for (uint32_t j = 0; j < ndx; j++) {

					if (unlikely(fabs(Vm[j]) >= Opts.MaxVoltage)) {
						badflag = 1;
						fprintf(stderr,
								"\tTime is: %05u, Vm[%03u] is out of range: % 10.3f\n",
								it, j, Vm[j]);
					}
				}

				/* It is possible that somehow we've exceeded a realistic voltage at a given x-segment. */
				if (unlikely(badflag)) {
					fprintf(stderr,
							"\tIT BLEW UP!!!!!!!!!!!!!!!!!!!!!!!!! ...");
					/*
					 * Write out an invalid spiketime file to signal that we've hit a problem.  exit afterwards
					 */
					if ((ofp = fopen(spksFile, "w")) == NULL) {
						perror("fopen");
						fprintf(stderr, "Cannot open spike file %s\n",
								spksFile);
						exit(EXIT_FAILURE);
					}

					// first, write endian header
					fwrite(&endianHdr, sizeof(unsigned int), 1, ofp);

					// write mcr and an invalid # of fibers to signal error/request resubmit
					fwrite(&myTotMontes, sizeof(uint32_t), 1, ofp);
					int i = -1;
					fwrite(&i, sizeof(int), 1, ofp);
					fclose(ofp);

					exit(EXIT_FAILURE);

				}

				/**
				 Write out voltages */
				if (unlikely(Opts.doRecordVltg)) {
					if (it % (long) (Opts.VoltageSample / delt + 0.5)
							== 0) {
						for (uint32_t currNode = 0; currNode < nodnum;
								currNode++) {
							const uint32_t currNodeAsSegmnt =
									nodeToSegmnt[currNode];
							fwrite(&(Vm[currNodeAsSegmnt]), sizeof(double), 1,
									vfp);
						}
					}
				}

				/* Update ECAP with this fiber's contribution */
				if (!idxMyMonte && Opts.doRecordECAP) {
					for (uint32_t i = 1; i < ndx - 1; i++) {
						//The resistance from each point source of axon
						//ecap[it] += 1.0*ecapRes[i]*(-xina[i]-Vm[i]/rm[i]-cm[i]*(Vm[i]-Vm_prev[i])/delt);
						ecap[it] += 1.0 * ecapRes[i]
								* (((Vm[i - 1] - Vm[i]) / ra[i - 1])
										+ (Vm[i] - Vm[i + 1]) / ra[i]);
					}
				}

				/*
				 * Check whether newly-calculated voltage meets conditions for a spike.  
				 * Do this for every node of Ranvier.
				 */
				for (uint32_t currNode = 0; currNode < nodnum; currNode++) {
					const uint32_t currNodeAsSegmnt = nodeToSegmnt[currNode];

					/* TODO: CHANGE FROM if(spike) to if(! spike), continue, and then deblock the rest */

					/* Conditions for a spike */
					if ((Vm_prev[currNodeAsSegmnt] < Opts.Vth)
							&& (Vm[currNodeAsSegmnt] >= Opts.Vth) && (it > 0)) {

						allSpikeTimes[idxMyMonte * myTotFibs * globMaxNodes
								* Opts.maxSpikes
								+ idxMyFib * globMaxNodes
										* Opts.maxSpikes
								+ currNode * Opts.maxSpikes
								+ spike_count[currNode]] = (unsigned int) it;

						spike_count[currNode]++;

#ifdef DEBUG
						printf("SPIKE! fiber: %3u monte: %3u, currNode: %3u, Spike number: %3u, Spike time: %4u, Vm: % 5.2f\n",
								idxGlobFib+1, idxMyMonte+1, currNode, spike_count[currNode], it, Vm[currNodeAsSegmnt]);
#endif			 

						if (spike_count[currNode]
								>= Opts.maxSpikes - 1) {
							fprintf(stderr,
									"[ATTN:]\t MaxSpikes (%u) exceeded at node %u/%u, RESETTING spike count (was: %u)!\n",
									Opts.maxSpikes, currNode+1,nodnum,spike_count[currNode]);
							spike_count[currNode] = 0;
						}
					}
				} // end for (currNode=0;currNode<nodnum;currNode++) loop

			}	 // end it loop
			free(Vm); free(Vm_prev);
		}	// end idxMyMonte loop

		free(Vapp); free(xina);
		free(a); free(b); free(c); free(r); free(dx1); free(dx2); free(ux);

	} // end for(uint32_t idxMyFib=0; idxMyFib < myTotFibs; idxMyFib++)

	free(rm); free(cm); free(ra); free(nodeToSegmnt); free(spike_count);

	// these are not needed anymore either:
	free(Modl.diameters); free(Modl.zs); free(Modl.cutoffs); free(stim.durs); free(stim.amps);

	if (Opts.doRecordVltg) {
		if (likely(verbosity > 1)) {
			printf("Writing Voltage\n");
		}
		fclose(vfp);
		/// @warning quick hack to get around size limitation of xgrid: bzip all of the files
		/*	snprintf( bzipCmd, MAX_FILENAME*2, "bzip2 --fast --force %s", vltgFile );
		 system( bzipCmd );
		 */
	}

	if (Opts.doRecordECAP) {
		printf("Writing ECAP\n");

		if ((efp = fopen(ecapFile, "w")) == NULL) {
			perror("fopen");
			fprintf(stderr, "Cannot open ecapFile %s\n", ecapFile);
		} else {

			// first, write endian header
			fwrite(&endianHdr, sizeof(unsigned int), 1, efp);

			fwrite(&stim.nstep, sizeof(uint32_t), 1, efp);
			fwrite(&myTotFibs, sizeof(uint32_t), 1, efp);
			fwrite(myFibsToGlob, sizeof(uint32_t), myTotFibs, efp);
			fwrite(ecap, sizeof(double), stim.nstep, efp);

			fclose(efp);
		}

		free(ecap);

		/// @warning quick hack to get around size limitation of xgrid: bzip all of the files
		/*	snprintf( bzipCmd, MAX_FILENAME*2, "bzip2 --fast --force %s", ecapFile );
		 system( bzipCmd );
		 */
	}

	/*
	 * open and write out spiketime file.  we always output spiketimes
	 */
	if ((ofp = fopen(spksFile, "w")) == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open spksFile %s\n", spksFile);
		exit(EXIT_FAILURE);
	}

	// first, write endian header
	fwrite(&endianHdr, sizeof(unsigned int), 1, ofp);

	fwrite(&myTotMontes, sizeof(uint32_t), 1, ofp);
	fwrite(&myTotFibs, sizeof(uint32_t), 1, ofp);
	fwrite(&globMaxNodes, sizeof(uint32_t), 1, ofp);
	fwrite(myFibsToGlob, sizeof(uint32_t), myTotFibs, ofp);

	for (uint32_t i = 0; i < myTotFibs; i++) {
		fwrite(&(Modl.nodeNums[myFibsToGlob[i] - 1]), sizeof(uint32_t), 1, ofp);
	}

	fwrite(&Opts.maxSpikes, sizeof(uint32_t), 1, ofp);
	fwrite(&stim.nstep, sizeof(uint32_t), 1, ofp);
	fwrite(&delt, sizeof(double), 1, ofp);
	fwrite(allSpikeTimes, sizeof(uint32_t),
			globMaxNodes * myTotFibs * myTotMontes * Opts.maxSpikes,
			ofp);
	fclose(ofp);
	free(allSpikeTimes);

	/*
	 /// @warning quick hack to get around size limitation of xgrid: bzip all of the files
	 snprintf( bzipCmd, MAX_FILENAME*2, "bzip2 --fast --force %s", spksFile );
	 system( bzipCmd );
	 */

	/*
	 * CLEANUP
	 */

	free(globNodNums);
	free(myFibsToGlob);

#ifdef DEBUG
	fclose(debugNaf_fp);
	fclose(debugNap_fp);
	fclose(debugKf_fp);
	fclose(debugKs_fp);
#endif	

	exit(EXIT_SUCCESS);
} //end of main

void usage(void) {
	printf(
			"$HeadURL: svn+ssh://nikita@master.oto.washington.edu/Volumes/DataHD/Users/nikita/Documents/repos/cnerve/trunk/CodeV2.6.1/cnerve.c $\n");
	printf(
			"$LastChangedDate: 2010-11-03 14:20:57 -0700 (Wed, 03 Nov 2010) $\n");
	printf("Compiled on: %s %s\n", __DATE__, __TIME__);

	printf("\n\tcmd-line arguments:\n");
	printf("\t\t1) modlFile		usually:		____.modl\n");
	printf("\t\t2) stimFile					____.stim\n");
	printf("\t\t3) optsFile					____.opts\n");
	printf("\t\t4) outputPrefix		prefix for	voltage	____.vltg\n");
	printf("\t\t						spikes	____.spks\n");
	printf("\t\t						ECAP	____.ecap\n");
	printf("\t\t5) #, seed for RNG\n");
	printf("\t\t6) #, myTotMontes\n");
	printf("\t\t7) #, myTotFibs\n");
	printf("\t\t8) #, the first fiber to process, as global fiber ID\n");
	printf("\t\t9) #, fibStride\n");

	return;
}
