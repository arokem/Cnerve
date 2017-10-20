/*
 * crankNich.h
 *
 *  Created on: Aug 18, 2017
 *      Author: jesse
 */

#ifndef CRANKNICH_H_
#define CRANKNICH_H_

#include <stdint.h>

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

void tridag(const double a[], const double b[], const double c[], const double r[], double u[], const uint32_t n);



#endif /* CRANKNICH_H_ */
