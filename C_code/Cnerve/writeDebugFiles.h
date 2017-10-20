/*
 * writeDebugFiles.h
 *
 *  Created on: Aug 18, 2017
 *      Author: jesse
 */

#ifndef WRITEDEBUGFILES_H_
#define WRITEDEBUGFILES_H_

#include <stdlib.h>
#include "cnerve.h"

int initializeDebugFiles(const int argNum,const char *argList,const uint32_t myTotFibs,
		const uint32_t myTotMontes, const stimParam *stim, const uint32_t *globNodNums,
		const uint32_t *myFibsToGlob, const unsigned int endianHdr);

#endif /* WRITEDEBUGFILES_H_ */
