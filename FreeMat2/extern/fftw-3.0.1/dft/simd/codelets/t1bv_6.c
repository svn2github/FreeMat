/*
 * Copyright (c) 2003 Matteo Frigo
 * Copyright (c) 2003 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sat Jul  5 21:43:42 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_twiddle_c -simd -compact -variables 4 -n 6 -name t1bv_6 -include t1b.h -sign 1 */

/*
 * This function contains 23 FP additions, 14 FP multiplications,
 * (or, 21 additions, 12 multiplications, 2 fused multiply/add),
 * 19 stack variables, and 12 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "t1b.h"

static const R *t1bv_6(R *ri, R *ii, const R *W, stride ios, int m, int dist)
{
     DVK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DVK(KP866025403, +0.866025403784438646763723170752936183471402627);
     int i;
     R *x;
     x = ii;
     BEGIN_SIMD();
     for (i = m; i > 0; i = i - VL, x = x + (VL * dist), W = W + (TWVL * 10)) {
	  V Tf, Ti, Ta, Tk, T5, Tj, Tc, Te, Td;
	  Tc = LD(&(x[0]), dist, &(x[0]));
	  Td = LD(&(x[WS(ios, 3)]), dist, &(x[WS(ios, 1)]));
	  Te = BYTW(&(W[TWVL * 4]), Td);
	  Tf = VSUB(Tc, Te);
	  Ti = VADD(Tc, Te);
	  {
	       V T7, T9, T6, T8;
	       T6 = LD(&(x[WS(ios, 4)]), dist, &(x[0]));
	       T7 = BYTW(&(W[TWVL * 6]), T6);
	       T8 = LD(&(x[WS(ios, 1)]), dist, &(x[WS(ios, 1)]));
	       T9 = BYTW(&(W[0]), T8);
	       Ta = VSUB(T7, T9);
	       Tk = VADD(T7, T9);
	  }
	  {
	       V T2, T4, T1, T3;
	       T1 = LD(&(x[WS(ios, 2)]), dist, &(x[0]));
	       T2 = BYTW(&(W[TWVL * 2]), T1);
	       T3 = LD(&(x[WS(ios, 5)]), dist, &(x[WS(ios, 1)]));
	       T4 = BYTW(&(W[TWVL * 8]), T3);
	       T5 = VSUB(T2, T4);
	       Tj = VADD(T2, T4);
	  }
	  {
	       V Tb, Tg, Th, Tn, Tl, Tm;
	       Tb = VBYI(VMUL(LDK(KP866025403), VSUB(T5, Ta)));
	       Tg = VADD(T5, Ta);
	       Th = VFNMS(LDK(KP500000000), Tg, Tf);
	       ST(&(x[WS(ios, 1)]), VADD(Tb, Th), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 3)]), VADD(Tf, Tg), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 5)]), VSUB(Th, Tb), dist, &(x[WS(ios, 1)]));
	       Tn = VBYI(VMUL(LDK(KP866025403), VSUB(Tj, Tk)));
	       Tl = VADD(Tj, Tk);
	       Tm = VFNMS(LDK(KP500000000), Tl, Ti);
	       ST(&(x[WS(ios, 2)]), VSUB(Tm, Tn), dist, &(x[0]));
	       ST(&(x[0]), VADD(Ti, Tl), dist, &(x[0]));
	       ST(&(x[WS(ios, 4)]), VADD(Tn, Tm), dist, &(x[0]));
	  }
     }
     END_SIMD();
     return W;
}

static const tw_instr twinstr[] = {
     VTW(1),
     VTW(2),
     VTW(3),
     VTW(4),
     VTW(5),
     {TW_NEXT, VL, 0}
};

static const ct_desc desc = { 6, "t1bv_6", twinstr, {21, 12, 2, 0}, &GENUS, 0, 0, 0 };

void X(codelet_t1bv_6) (planner *p) {
     X(kdft_dit_register) (p, t1bv_6, &desc);
}
