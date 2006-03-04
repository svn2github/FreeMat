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
/* Generated on Sat Jul  5 22:11:11 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2r -compact -variables 4 -sign 1 -n 12 -name hc2r_12 -include hc2r.h */

/*
 * This function contains 38 FP additions, 10 FP multiplications,
 * (or, 34 additions, 6 multiplications, 4 fused multiply/add),
 * 25 stack variables, and 24 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "hc2r.h"

static void hc2r_12(const R *ri, const R *ii, R *O, stride ris, stride iis, stride os, int v, int ivs, int ovs)
{
     DK(KP1_732050807, +1.732050807568877293527446341505872366942805254);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     int i;
     for (i = v; i > 0; i = i - 1, ri = ri + ivs, ii = ii + ivs, O = O + ovs) {
	  E T8, Tb, Tm, TA, Tw, Tx, Tp, TB, T3, Tr, Tg, T6, Ts, Tk;
	  {
	       E T9, Ta, Tn, To;
	       T8 = ri[WS(ris, 3)];
	       T9 = ri[WS(ris, 5)];
	       Ta = ri[WS(ris, 1)];
	       Tb = T9 + Ta;
	       Tm = FMS(KP2_000000000, T8, Tb);
	       TA = KP1_732050807 * (T9 - Ta);
	       Tw = ii[WS(iis, 3)];
	       Tn = ii[WS(iis, 5)];
	       To = ii[WS(iis, 1)];
	       Tx = Tn + To;
	       Tp = KP1_732050807 * (Tn - To);
	       TB = FMA(KP2_000000000, Tw, Tx);
	  }
	  {
	       E Tf, T1, T2, Td, Te;
	       Te = ii[WS(iis, 4)];
	       Tf = KP1_732050807 * Te;
	       T1 = ri[0];
	       T2 = ri[WS(ris, 4)];
	       Td = T1 - T2;
	       T3 = FMA(KP2_000000000, T2, T1);
	       Tr = Td - Tf;
	       Tg = Td + Tf;
	  }
	  {
	       E Tj, T4, T5, Th, Ti;
	       Ti = ii[WS(iis, 2)];
	       Tj = KP1_732050807 * Ti;
	       T4 = ri[WS(ris, 6)];
	       T5 = ri[WS(ris, 2)];
	       Th = T4 - T5;
	       T6 = FMA(KP2_000000000, T5, T4);
	       Ts = Th + Tj;
	       Tk = Th - Tj;
	  }
	  {
	       E T7, Tc, Tz, TC;
	       T7 = T3 + T6;
	       Tc = KP2_000000000 * (T8 + Tb);
	       O[WS(os, 6)] = T7 - Tc;
	       O[0] = T7 + Tc;
	       {
		    E Tl, Tq, TD, TE;
		    Tl = Tg + Tk;
		    Tq = Tm - Tp;
		    O[WS(os, 2)] = Tl - Tq;
		    O[WS(os, 8)] = Tl + Tq;
		    TD = Tg - Tk;
		    TE = TB - TA;
		    O[WS(os, 5)] = TD - TE;
		    O[WS(os, 11)] = TD + TE;
	       }
	       Tz = Tr - Ts;
	       TC = TA + TB;
	       O[WS(os, 1)] = Tz - TC;
	       O[WS(os, 7)] = Tz + TC;
	       {
		    E Tv, Ty, Tt, Tu;
		    Tv = T3 - T6;
		    Ty = KP2_000000000 * (Tw - Tx);
		    O[WS(os, 9)] = Tv - Ty;
		    O[WS(os, 3)] = Tv + Ty;
		    Tt = Tr + Ts;
		    Tu = Tm + Tp;
		    O[WS(os, 10)] = Tt - Tu;
		    O[WS(os, 4)] = Tt + Tu;
	       }
	  }
     }
}

static const khc2r_desc desc = { 12, "hc2r_12", {34, 6, 4, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_hc2r_12) (planner *p) {
     X(khc2r_register) (p, hc2r_12, &desc);
}
