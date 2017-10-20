/*
 * crankNich.c
 *
 *  Created on: Aug 18, 2017
 *      Author: jesse
 */

#include <stdint.h>				// for the uint32_t type
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "crankNich.h"

/** @fn void tridag(double a[], double b[], double c[], double r[], double u[], uint32_t n)
 @brief	Solves linear tridiagonal systems by Gauss elimination without pivoting.
 Returns u which solves A u = r, where A is an n X n matrix. So a, b, c, u, r
 are all n x 1 row vects.

 Note a(1) and c(n) are irrelevant.  They do not correspond to matrix elements of A.

 @see http://www.physics.drexel.edu/students/courses/physics-307/crank-nic/


 @param[in]	a		sub diagonal of A.  n x 1 -- row vector
 @param[in]	b		diagonal of A.  n x 1 -- row vector
 @param[in]	c		super diagonal of A.  n x 1 -- row vector
 @param[in]	r		rhs of equation, what A u equals to.

 @param[out]	u		solution to a matrix equation A u = r
 */

void tridag(const double * restrict a, const double * restrict b, const double * restrict c,
		const double * restrict r, double * restrict u, const uint32_t n) {
	/* The restrict flag lets the function know that the given function pointer is
	 * the only one that points to a block of memory. This enables optimization
	 * during compilation. */

	double bet, gam[n];

	if (unlikely(b[0] == 0.0)) {
		puts("Error 1 in tridag");

		// NSI: abort?!?
	}

	u[0] = r[0] / (bet = b[0]);

	/* decomposition and forward substitution phase */
	for (uint32_t j = 1; j < n; j++) {
		gam[j] = c[j - 1] / bet;
		bet = b[j] - a[j] * gam[j];

		if (unlikely(bet == 0.0)) {
			// algorithm has failed, see text
			puts("Error 2 in tridag");

			// NSI: abort?!?
		}

		u[j] = (r[j] - a[j] * u[j - 1]) / bet;
	}

	/* backward substitution phase? */
	//for (uint32_t j=(n-1);j>=1;j--){
	assert((n - 2) >= 0);
	for (int j = (n - 2); j >= 0; j--) {
		u[j] -= gam[j + 1] * u[j + 1];
	}
}


