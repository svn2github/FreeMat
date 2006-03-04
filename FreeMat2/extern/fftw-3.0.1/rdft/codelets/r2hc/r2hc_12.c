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
/* Generated on Sat Jul  5 21:56:41 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_r2hc -compact -variables 4 -n 12 -name r2hc_12 -include r2hc.h */

/*
 * This function contains 38 FP additions, 8 FP multiplications,
 * (or, 34 additions, 4 multiplications, 4 fused multiply/add),
 * 21 stack variables, and 24 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "r2hc.h"

static void r2hc_12(const R *I, R *ro, R *io, stride is, stride ros, stride ios, int v, int ivs, int ovs)
{
     DK(KP866025403, +0.866025403784438646763723170752936183471402627);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     int i;
     for (i = v; i > 0; i = i - 1, I = I + ivs, ro = ro + ovs, io = io + ovs) {
	  E T5, Tp, Tb, Tn, Ty, Tt, Ta, Tq, Tc, Ti, Tz, Tu, Td, To;
	  {
	       E T1, T2, T3, T4;
	       T1 = I[0];
	       T2 = I[WS(is, 4)];
	       T3 = I[WS(is, 8)];
	       T4 = T2 + T3;
	       T5 = T1 + T4;
	       Tp = FNMS(KP500000000, T4, T1);
	       Tb = T3 - T2;
	  }
	  {
	       E Tj, Tk, Tl, Tm;
	       Tj = I[WS(is, 3)];
	       Tk = I[WS(is, 7)];
	       Tl = I[WS(is, 11)];
	       Tm = Tk + Tl;
	       Tn = FNMS(KP500000000, Tm, Tj);
	       Ty = Tl - Tk;
	       Tt = Tj + Tm;
	  }
	  {
	       E T6, T7, T8, T9;
	       T6 = I[WS(is, 6)];
	       T7 = I[WS(is, 10)];
	       T8 = I[WS(is, 2)];
	       T9 = T7 + T8;
	       Ta = T6 + T9;
	       Tq = FNMS(KP500000000, T9, T6);
	       Tc = T8 - T7;
	  }
	  {
	       E Te, Tf, Tg, Th;
	       Te = I[WS(is, 9)];
	       Tf = I[WS(is, 1)];
	       Tg = I[WS(is, 5)];
	       Th = Tf + Tg;
	       Ti = FNMS(KP500000000, Th, Te);
	       Tz = Tg - Tf;
	       Tu = Te + Th;
	  }
	  ro[WS(ros, 3)] = T5 - Ta;
	  io[WS(ios, 3)] = Tt - Tu;
	  Td = KP866025403 * (Tb - Tc);
	  To = Ti - Tn;
	  io[WS(ios, 1)] = Td + To;
	  io[WS(ios, 5)] = To - Td;
	  {
	       E Tx, TA, Tv, Tw;
	       Tx = Tp - Tq;
	       TA = KP866025403 * (Ty - Tz);
	       ro[WS(ros, 5)] = Tx - TA;
	       ro[WS(ros, 1)] = Tx + TA;
	       Tv = T5 + Ta;
	       Tw = Tt + Tu;
	       ro[WS(ros, 6)] = Tv - Tw;
	       ro[0] = Tv + Tw;
	  }
	  {
	       E Tr, Ts, TB, TC;
	       Tr = Tp + Tq;
	       Ts = Tn + Ti;
	       ro[WS(ros, 2)] = Tr - Ts;
	       ro[WS(ros, 4)] = Tr + Ts;
	       TB = Ty + Tz;
	       TC = Tb + Tc;
	       io[WS(ios, 2)] = KP866025403 * (TB - TC);
	       io[WS(ios, 4)] = KP866025403 * (TC + TB);
	  }
     }
}

static const kr2hc_desc desc = { 12, "r2hc_12", {34, 4, 4, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_r2hc_12) (planner *p) {
     X(kr2hc_register) (p, r2hc_12, &desc);
}