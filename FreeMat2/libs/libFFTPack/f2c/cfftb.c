/* cfftb.f -- translated by f2c (version 20030211).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int cfftb_(integer *n, real *c__, real *wsave)
{
    static integer iw1, iw2;
    extern /* Subroutine */ int cfftb1_(integer *, real *, real *, real *, 
	    real *);

    /* Parameter adjustments */
    --wsave;
    --c__;

    /* Function Body */
    if (*n == 1) {
	return 0;
    }
    iw1 = *n + *n + 1;
    iw2 = iw1 + *n + *n;
    cfftb1_(n, &c__[1], &wsave[1], &wsave[iw1], &wsave[iw2]);
    return 0;
} /* cfftb_ */

