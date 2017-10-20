/*
 * readModlFile.h
 *
 *  Created on: Aug 15, 2017
 *      Author: jesse
 */

#ifndef _READMODLFILE_H_
#define _READMODLFILE_H_
#include <stdint.h>		// defines uint32_t needed here
#include "readOptsFile.h"

#define MAX_FILENAME	255				///< max # of characters in a filename
#define MAX_RAN_NODES   250				///< max # of nodes of Ranvier in a fiber.

typedef struct kineticParam {
	double k1, k2, k3;
	double a_m_A, a_m_B, a_m_C;
	double b_m_A, b_m_B, b_m_C;
	double a_h_A, a_h_B, a_h_C;
	double b_h_A, b_h_B, b_h_C;
	double a_n_A, a_n_B, a_n_C;
	double b_n_A, b_n_B, b_n_C;
	double a_s_A, a_s_B, a_s_C;
	double b_s_A, b_s_B, b_s_C;
	double a_p_A, a_p_B, a_p_C;
	double b_p_A, b_p_B, b_p_C;
} kinParam;

typedef struct chanParam {
	/* set parameters */
	double	eRev;						///< reversal (Nernst) potential for that particular channel, E_<channel name>		(mV)
	double	density;					///< density of channels at node of ranvier	(chans / mm^2)
	double	gMax;						///< channel conductance per channel

	/* ptr to a func to calc # of open chans */
#ifdef DEBUG
	double (*calcNumOpenChan)(const bool,		// bool doInit
							  const uint32_t,	// num of channels
							  const double,		// membrane voltage at the node
							  const double,		// step increment, =delt
							  const uint32_t,	// iteration of time
							  const uint32_t,	// node number for which we're doing these calculations
							  const kinParam, // structure holding kinetic parameters
							  FILE*);		// fp where to write additional info on gating
#else
	double (*calcNumOpenChan)(const bool,		// bool doInit
							  const uint32_t,	//
							  const double,		//
							  const double,		//
							  const uint32_t,	//
							  const uint32_t,	//
							  const kinParam);
#endif

	/* derived/calculated parameters */
	uint32_t		numChan;			///< number of channels per node of ranvier
	double			numOpen;			///< number of channels in open state. always <= numChan

} chanParam;

typedef struct ModlStructure {
	uint32_t globTotFibs;	///< total fiber count (not just this cnerve invocation)
	uint32_t globMaxNodes;	///< maximum number of nodes to expect
	uint32_t insegs;
	double E_rest;
	chanParam NafParam;
	chanParam NapParam;
	chanParam KfParam;
	chanParam KsParam;

	/* GEOMETRIC PROPERTIES: Morphological constants and relationships */
	double 	dtoD,		///< ratio of inner/outer diameters for normal segments
			LtoD,		///< ratio of internode's length to its diameter
			nlen,		///< length of a node (mm)
			constrict;	///< anatomic constriction factor

	/* Resistances and capacitances */
	double 	rmnod,		///< membrane resistance at node (Ohm-mm^2)
			cmnod,		///< membrane capacitance at node (nF/mm^2)
			ranod;		///< axoplasmic resistance at node (Ohm-mm)
	double 	rmmy,	///< membrane resistance per length at internode (Ohm-mm)
			cmmy,	///< membrane capacitance per length at internode (nF/mm)
			ramy;	///< axoplasmic resistance per length at internode (Ohm-mm)

	/* FiberLists */
	double * diameters; 	///<diameter of axon plasma membrane
	uint32_t * nodeNums;	///<number of nodes in fiber
	double * zs;			///<radial distance from electrode
	uint32_t * cutoffs;		///<most central demyelinated fiber
	double * demydtoD;	///<ratio of inner/outer diameters of demy. segments

	/*Stim and recording locations*/
	double stimLoc;
	double recordLoc;
	double ECAPaway;	///< unused in main()
	double ECAPheight;	///< unused in main()
} modlParam;

int readModlFile(char *modlFile, modlParam *Modl, kinParam *Kins, const optsParam Opts,
		const int verbosity);

#endif /* _READMODLFILE_H_ */
