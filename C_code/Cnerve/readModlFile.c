/*
 * readModlFile.c
 *
 *  Created on: Aug 15, 2017
 *      Author: Jesse
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>					// for the uint32_t type
#include "readModlFile.h"
#include "channels.h"

int readModlFile(char *modlFile, modlParam *Modl, kinParam *Kins, const optsParam Opts,
		const int verbosity) {

	FILE *mfp	= NULL;
	if ((mfp = fopen(modlFile, "r")) == NULL) {
		fprintf(stderr, "Cannot open model file %s\n", modlFile);
		exit(EXIT_FAILURE);
	}
	double tmpdata;

	fscanf(mfp, "%le", &tmpdata); Modl->globTotFibs = (uint32_t) tmpdata;
	uint32_t globTotFibs = Modl->globTotFibs;

	fscanf(mfp, "%le", &tmpdata); Modl->globMaxNodes = (uint32_t) tmpdata;

	if (Modl->globMaxNodes > MAX_RAN_NODES) {
		fprintf(stderr,
				"Too many nodes used in simulation. Please recompile or enable malloc for the nodes as well. (%u > max of %d)\n",
				Modl->globMaxNodes, MAX_RAN_NODES);
		exit(EXIT_FAILURE);
	}

	fscanf(mfp, "%le", &tmpdata); Modl->insegs = (uint32_t) tmpdata;
	assert(Modl->insegs > 0);

	fscanf(mfp, "%le", &(Modl->E_rest));
	fscanf(mfp, "%le", &(Modl->NafParam.eRev));		// Naf channels
	fscanf(mfp, "%le", &(Modl->NafParam.density));
	fscanf(mfp, "%le", &(Modl->NafParam.gMax));
	fscanf(mfp, "%le", &(Modl->NapParam.eRev));		// Nap channels
	fscanf(mfp, "%le", &(Modl->NapParam.density));
	fscanf(mfp, "%le", &(Modl->NapParam.gMax));
	fscanf(mfp, "%le", &(Modl->KfParam.eRev));		// Kf channels
	fscanf(mfp, "%le", &(Modl->KfParam.density));
	fscanf(mfp, "%le", &(Modl->KfParam.gMax));
	fscanf(mfp, "%le", &(Modl->KsParam.eRev));		// Ks channels
	fscanf(mfp, "%le", &(Modl->KsParam.density));
	fscanf(mfp, "%le", &(Modl->KsParam.gMax));

	// Naf channel kinetics			XXX: To do- package these together
	fscanf(mfp, "%le", &(Kins->a_m_A));		// Activation gate kinetic parameters
	fscanf(mfp, "%le", &(Kins->a_m_B));
	fscanf(mfp, "%le", &(Kins->a_m_C));
	fscanf(mfp, "%le", &(Kins->b_m_A));
	fscanf(mfp, "%le", &(Kins->b_m_B));
	fscanf(mfp, "%le", &(Kins->b_m_C));

	fscanf(mfp, "%le", &(Kins->a_h_A));		// Inactivation gate kinetic parameters
	fscanf(mfp, "%le", &(Kins->a_h_B));
	fscanf(mfp, "%le", &(Kins->a_h_C));
	fscanf(mfp, "%le", &(Kins->b_h_A));
	fscanf(mfp, "%le", &(Kins->b_h_B));
	fscanf(mfp, "%le", &(Kins->b_h_C));

	fscanf(mfp, "%le", &(Kins->a_n_A));		// Kf channel kinetic parameters
	fscanf(mfp, "%le", &(Kins->a_n_B));
	fscanf(mfp, "%le", &(Kins->a_n_C));
	fscanf(mfp, "%le", &(Kins->b_n_A));
	fscanf(mfp, "%le", &(Kins->b_n_B));
	fscanf(mfp, "%le", &(Kins->b_n_C));

	fscanf(mfp, "%le", &(Kins->a_s_A));		// Ks channel kinetic parameters
	fscanf(mfp, "%le", &(Kins->a_s_B));
	fscanf(mfp, "%le", &(Kins->a_s_C));
	fscanf(mfp, "%le", &(Kins->b_s_A));
	fscanf(mfp, "%le", &(Kins->b_s_B));
	fscanf(mfp, "%le", &(Kins->b_s_C));

	fscanf(mfp, "%le", &(Kins->a_p_A));		// Nap channel kinetic parameters
	fscanf(mfp, "%le", &(Kins->a_p_B));
	fscanf(mfp, "%le", &(Kins->a_p_C));
	fscanf(mfp, "%le", &(Kins->b_p_A));
	fscanf(mfp, "%le", &(Kins->b_p_B));
	fscanf(mfp, "%le", &(Kins->b_p_C));

	fscanf(mfp, "%le", &(Kins->k1));			// Patlak-related variables
	fscanf(mfp, "%le", &(Kins->k2));
	fscanf(mfp, "%le", &(Kins->k3));

	// Anatomical parameters
	fscanf(mfp, "%le", &(Modl->rmnod));
	fscanf(mfp, "%le", &(Modl->cmnod));
	fscanf(mfp, "%le", &(Modl->ranod));
	fscanf(mfp, "%le", &(Modl->rmmy));
	fscanf(mfp, "%le", &(Modl->cmmy));
	fscanf(mfp, "%le", &(Modl->ramy));
	fscanf(mfp, "%le", &(Modl->dtoD));
	fscanf(mfp, "%le", &(Modl->LtoD));
	fscanf(mfp, "%le", &(Modl->nlen));
	fscanf(mfp, "%le", &(Modl->constrict));
	fscanf(mfp, "%le", &(Modl->ECAPheight));
	fscanf(mfp, "%le", &(Modl->ECAPaway));
	fscanf(mfp, "%le", &(Modl->stimLoc));
	fscanf(mfp, "%le", &(Modl->recordLoc));

	// Dynamically allocate memory for fiberlist properties
	Modl->nodeNums = (uint32_t*) malloc(globTotFibs * sizeof(uint32_t));
	Modl->diameters = (double*) malloc(globTotFibs * sizeof(double)); ///< internal diameter of axon in mm (i.e. w/o myelin sheath) (mm)
	Modl->zs = (double*) malloc(globTotFibs * sizeof(double));	///< radial distance from electrode to fiber (mm)
	Modl->cutoffs = (uint32_t*) malloc(globTotFibs * sizeof(uint32_t));///< Indicates the node where demyelination ends.
	Modl->demydtoD = (double*) malloc(globTotFibs * sizeof(double)); ///dtoD of demyelinated internode
	if ((Modl->nodeNums == NULL) || (Modl->diameters == NULL) || (Modl->zs == NULL)
			|| (Modl->cutoffs == NULL)|| (Modl->demydtoD == NULL)) {
		fprintf(stderr, "Cannot allocate for NodNums, diameters, zs, "
				"cutoffs, or node_aboves: ");
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// Reading 'fiberlist' properties.
	for (uint32_t i = 0; i < globTotFibs; i++) {
		fscanf(mfp, "%le", &tmpdata); Modl->nodeNums[i] = (uint32_t) tmpdata;
		fscanf(mfp, "%le", &(Modl->diameters[i]));
		fscanf(mfp, "%le", &(Modl->zs[i]));

		/* a note about cutoffs and node_aboves:
		 cutoffs and node_aboves are defined as NODE # IN C LEXICON, i.e., a cutoffs=9 specifies
		 that stimulus should be over 10th node if the 1st node is node #0 in C program..
		 In other words, MATLAB passes on the variable to C program,
		 and hence counts from 0, as C does (not like it would usually, starting from 1). */

		fscanf(mfp, "%le", &(Modl->demydtoD[i]));
		fscanf(mfp, "%le", &tmpdata); Modl->cutoffs[i] = (uint32_t) tmpdata;
		if (unlikely(verbosity == 11)){
			printf("\tnodeNum:%u\t diameters:%.5lf\t zs:%.1lf\t demydtoD:%.3lf\t cutoffs:%u\n",
					Modl->nodeNums[i],Modl->diameters[i],Modl->zs[i],Modl->demydtoD[i],Modl->cutoffs[i]);}
	}

	fclose(mfp);

	// print out all of the variables we just read in. XXX: Move to readFile functions...
	if (likely(verbosity)) {

		printf("[MODEL PARAMETERS:]\n");
		printf("\tE_rest:\t%.1lf\n", Modl->E_rest);

		printf("\tfibers: %u\tglobMaxNodes: %u\tinsegs: %u\n", Modl->globTotFibs,
				Modl->globMaxNodes, Modl->insegs);

		printf(
				"\ta_m_A:% 10.5lf\ta_m_B:% 10.5lf\ta_m_C:% 10.5lf\tb_m_A:% 10.5lf\tb_m_B:% 10.5lf\tb_m_C:% 10.5lf\n",
				Kins->a_m_A, Kins->a_m_B, Kins->a_m_C, Kins->b_m_A, Kins->b_m_B, Kins->b_m_C);
		printf(
				"\ta_h_A:% 10.5lf\ta_h_B:% 10.5lf\ta_h_C:% 10.5lf\tb_h_A:% 10.5lf\tb_h_B:% 10.5lf\tb_h_C:% 10.5lf\n",
				Kins->a_h_A, Kins->a_h_B, Kins->a_h_C, Kins->b_h_A, Kins->b_h_B, Kins->b_h_C);
		printf(
				"\ta_n_A:% 10.5lf\ta_n_B:% 10.5lf\ta_n_C:% 10.5lf\tb_n_A:% 10.5lf\tb_n_B:% 10.5lf\tb_n_C:% 10.5lf\n",
				Kins->a_n_A, Kins->a_n_B, Kins->a_n_C, Kins->b_n_A, Kins->b_n_B, Kins->b_n_C);
		printf(
				"\ta_s_A:% 10.5lf\ta_s_B:% 10.5lf\ta_s_C:% 10.5lf\tb_s_A:% 10.5lf\tb_s_B:% 10.5lf\tb_s_C:% 10.5lf\n",
				Kins->a_s_A, Kins->a_s_B, Kins->a_s_C, Kins->b_s_A, Kins->b_s_B, Kins->b_s_C);
		printf(
				"\ta_p_A:% 10.5lf\ta_p_B:% 10.5lf\ta_p_C:% 10.5lf\tb_p_A:% 10.5lf\tb_p_B:% 10.5lf\tb_p_C:% 10.5lf\n",
				Kins->a_p_A, Kins->a_p_B, Kins->a_p_C, Kins->b_p_A, Kins->b_p_B, Kins->b_p_C);

		printf("\tk1:% 10.5lf\tk2:% 10.5lf\tk3:% 10.5lf\n", Kins->k1, Kins->k2, Kins->k3);
		printf("\trmnod:% 10.5lf\tcmnod:% 10.5lf\tranod:% 10.5lf\n", Modl->rmnod,
				Modl->cmnod, Modl->ranod);
		printf("\trmmy:% 10.5lf\tcmmy:% 10.10lf\t\tramy:% 10.5lf\n", Modl->rmmy, Modl->cmmy,
				Modl->ramy);

		printf("\tdtoD:% 10.5lf\tLtoD:% 10.5lf\tnlen:% 10.5lf\n", Modl->dtoD, Modl->LtoD,
				Modl->nlen);

		printf("\tconstrict =  %lf\n", Modl->constrict);

//		if (unlikely(Opts.doRecordECAP)) {
//			printf("\tECAPheight =  %lf\n", Modl->ECAPheight);
//			printf("\tECAPaway =	%lf\n", Modl->ECAPaway);
//		}
	}

	/* Set a function pointer which will be used to calculate the
	 # of open channels depending on the options specified for this simulation.
	 */
	switch (Opts.typeNaf) {
	case nafPatlak: /* PATLAK	*/
		Modl->NafParam.calcNumOpenChan = &Patlak_Na;
		break;
	default: /* DEFAULT	*/
	case nafDefault: /* Gillespie */
		Modl->NafParam.calcNumOpenChan = &Gillespie_Na;
		break;
	}

	/*	decide which function to use for Nap */
	switch (Opts.typeNap) {
	default: /* DEFAULT */
	case napMcIntyre: /* McIntyre */
		Modl->NapParam.calcNumOpenChan = &McIntyre_pNa;
		break;
	case napNotUsed: /* dbl check, should crash program */
		Modl->NapParam.calcNumOpenChan = NULL;
		break;
	}

	/*	decide which function to use for Kf */
	switch (Opts.typeKf) {
	default: /* DEFAULT*/
	case kfDefault: /* Gillespie */
		Modl->KfParam.calcNumOpenChan = &Gillespie_K;
		break;
	case kfNotUsed: /* dbl check, should crash program */
		Modl->KfParam.calcNumOpenChan = NULL;
		break;
	}

	/*	decide which function to use for Ks */
	switch (Opts.typeKs) {
	default: /* DEFAULT*/
	case ksDefault: /* Gillespie */
		Modl->KsParam.calcNumOpenChan = &Gillespie_slowK;
		break;
	case ksNotUsed: /* dbl check, should crash program */
		Modl->KsParam.calcNumOpenChan = NULL;
		break;
	}

	return 0;
}
