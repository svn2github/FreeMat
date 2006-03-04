/* ---------------------------------------------------------------------
 *
 * -- Automatically Tuned Linear Algebra Software (ATLAS)
 *    (C) Copyright 2000 All Rights Reserved
 *
 * -- ATLAS routine -- Version 3.2 -- December 25, 2000
 *
 * -- Suggestions,  comments,  bugs reports should be sent to the follo-
 *    wing e-mail address: atlas@cs.utk.edu
 *
 * Author         : Antoine P. Petitet
 * University of Tennessee - Innovative Computing Laboratory
 * Knoxville TN, 37996-1301, USA.
 *
 * ---------------------------------------------------------------------
 *
 * -- Copyright notice and Licensing terms:
 *
 *  Redistribution  and  use in  source and binary forms, with or without
 *  modification, are  permitted provided  that the following  conditions
 *  are met:
 *
 * 1. Redistributions  of  source  code  must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce  the above copyright
 *    notice,  this list of conditions, and the  following disclaimer in
 *    the documentation and/or other materials provided with the distri-
 *    bution.
 * 3. The name of the University,  the ATLAS group,  or the names of its
 *    contributors  may not be used to endorse or promote products deri-
 *    ved from this software without specific written permission.
 *
 * -- Disclaimer:
 *
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
 * RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
 * CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------
 */
/*
 * Include files
 */
#include "atlas_refmisc.h"
#include "atlas_reflvl2.h"
#include "atlas_reflevel2.h"

void ATL_crefgbmvH
(
   const int                  M,
   const int                  N,
   const int                  KL,
   const int                  KU,
   const float                * ALPHA,
   const float                * A,
   const int                  LDA,
   const float                * X,
   const int                  INCX,
   const float                * BETA,
   float                      * Y,
   const int                  INCY
)
{
/*
 * Purpose
 * =======
 *
 * ATL_crefgbmvH( ... ) <=> ATL_crefgbmv( AtlasConjTrans, ... )
 *
 * See ATL_crefgbmv for details.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   register float             t0_i, t0_r;
   int                        i, i0, i1, iaij, ix, j, jaj, jy, k, kx=0, ky=0;
   int                        incx2 = 2 * INCX, incy2 = 2 * INCY,
                              lda2 = ( LDA << 1 );
/* ..
 * .. Executable Statements ..
 *
 */
   for( j = 0, jaj = 0, jy = ky; j < M; j++, jaj += lda2, jy += incy2 )
   {
      Mset( ATL_sZERO, ATL_sZERO, t0_r, t0_i );
      k = KU - j; i0 = ( j - KU > 0 ? j - KU : 0 );
      i1 = ( N - 1 > j + KL ? j + KL : N - 1 );

      for( i  = i0,      iaij  = ((k+i0) << 1)+jaj, ix  = kx;
           i <= i1; i++, iaij += 2,                 ix += incx2 )
      { Mmla( A[iaij], -A[iaij+1], X[ix], X[ix+1], t0_r, t0_i ); }
      Mcelscal( BETA[0], BETA[1], Y[jy], Y[jy+1] );
      Mmla( ALPHA[0], ALPHA[1], t0_r, t0_i, Y[jy], Y[jy+1] );
      if( j >= KU ) kx += incx2;
   }
/*
 * End of ATL_crefgbmvH
 */
}
