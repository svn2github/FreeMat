/* zffti.f -- translated by f2c (version 20030211).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int zffti_(integer *n, doublereal *wsave)
{
    static integer iw1, iw2;
    extern /* Subroutine */ int zffti1_(integer *, doublereal *, doublereal *)
	    ;

    /* Parameter adjustments */
    --wsave;

    /* Function Body */
    if (*n == 1) {
	return 0;
    }
    iw1 = *n + *n + 1;
    iw2 = iw1 + *n + *n;
    zffti1_(n, &wsave[iw1], &wsave[iw2]);
    return 0;
} /* zffti_ */

