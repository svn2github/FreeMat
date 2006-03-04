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
/* Generated on Sat Jul  5 22:11:26 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2hc -compact -variables 4 -sign 1 -n 7 -dif -name hb_7 -include hb.h */

/*
 * This function contains 72 FP additions, 60 FP multiplications,
 * (or, 36 additions, 24 multiplications, 36 fused multiply/add),
 * 36 stack variables, and 28 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "hb.h"

static const R *hb_7(R *rio, R *iio, const R *W, stride ios, int m, int dist)
{
     DK(KP222520933, +0.222520933956314404288902564496794759466355569);
     DK(KP900968867, +0.900968867902419126236102319507445051165919162);
     DK(KP623489801, +0.623489801858733530525004884004239810632274731);
     DK(KP781831482, +0.781831482468029808708444526674057750232334519);
     DK(KP974927912, +0.974927912181823607018131682993931217232785801);
     DK(KP433883739, +0.433883739117558120475768332848358754609990728);
     int i;
     for (i = m - 2; i > 0; i = i - 2, rio = rio + dist, iio = iio - dist, W = W + 12) {
	  E T1, Ta, T4, T7, Tq, TI, TR, TU, TE, Tt, Tb, Tk, Te, Th, Tx;
	  E TF, TV, TQ, TJ, Tm;
	  {
	       E To, Tn, Tp, T8, T9;
	       T1 = rio[0];
	       T8 = rio[WS(ios, 3)];
	       T9 = iio[-WS(ios, 4)];
	       Ta = T8 + T9;
	       To = T8 - T9;
	       {
		    E T2, T3, T5, T6;
		    T2 = rio[WS(ios, 1)];
		    T3 = iio[-WS(ios, 6)];
		    T4 = T2 + T3;
		    Tn = T2 - T3;
		    T5 = rio[WS(ios, 2)];
		    T6 = iio[-WS(ios, 5)];
		    T7 = T5 + T6;
		    Tp = T5 - T6;
	       }
	       Tq = FMA(KP433883739, Tn, KP974927912 * To) - (KP781831482 * Tp);
	       TI = FMA(KP781831482, Tn, KP974927912 * Tp) + (KP433883739 * To);
	       TR = FNMS(KP781831482, To, KP974927912 * Tn) - (KP433883739 * Tp);
	       TU = FMA(KP623489801, Ta, T1) + FNMA(KP900968867, T7, KP222520933 * T4);
	       TE = FMA(KP623489801, T4, T1) + FNMA(KP900968867, Ta, KP222520933 * T7);
	       Tt = FMA(KP623489801, T7, T1) + FNMA(KP222520933, Ta, KP900968867 * T4);
	  }
	  {
	       E Tv, Tw, Tu, Ti, Tj;
	       Tb = iio[0];
	       Ti = rio[WS(ios, 4)];
	       Tj = iio[-WS(ios, 3)];
	       Tk = Ti - Tj;
	       Tv = Ti + Tj;
	       {
		    E Tc, Td, Tf, Tg;
		    Tc = iio[-WS(ios, 1)];
		    Td = rio[WS(ios, 6)];
		    Te = Tc - Td;
		    Tw = Tc + Td;
		    Tf = rio[WS(ios, 5)];
		    Tg = iio[-WS(ios, 2)];
		    Th = Tf - Tg;
		    Tu = Tf + Tg;
	       }
	       Tx = FNMS(KP974927912, Tv, KP781831482 * Tu) - (KP433883739 * Tw);
	       TF = FMA(KP781831482, Tw, KP974927912 * Tu) + (KP433883739 * Tv);
	       TV = FMA(KP433883739, Tu, KP781831482 * Tv) - (KP974927912 * Tw);
	       TQ = FMA(KP900968867, Th, Tb) + FNMA(KP623489801, Tk, KP222520933 * Te);
	       TJ = FMA(KP623489801, Te, Tb) + FMA(KP222520933, Th, KP900968867 * Tk);
	       Tm = FMA(KP222520933, Tk, Tb) + FNMA(KP623489801, Th, KP900968867 * Te);
	  }
	  rio[0] = T1 + T4 + T7 + Ta;
	  iio[-WS(ios, 6)] = Tb + Te - (Th + Tk);
	  {
	       E TM, TO, TL, TN;
	       TM = TJ - TI;
	       TO = TE + TF;
	       TL = W[10];
	       TN = W[11];
	       iio[0] = FMA(TL, TM, TN * TO);
	       rio[WS(ios, 6)] = FNMS(TN, TM, TL * TO);
	  }
	  {
	       E TY, T10, TX, TZ;
	       TY = TU + TV;
	       T10 = TR + TQ;
	       TX = W[2];
	       TZ = W[3];
	       rio[WS(ios, 2)] = FNMS(TZ, T10, TX * TY);
	       iio[-WS(ios, 4)] = FMA(TX, T10, TZ * TY);
	  }
	  {
	       E Tr, Ty, Tl, Ts;
	       Tr = Tm - Tq;
	       Ty = Tt - Tx;
	       Tl = W[6];
	       Ts = W[7];
	       iio[-WS(ios, 2)] = FMA(Tl, Tr, Ts * Ty);
	       rio[WS(ios, 4)] = FNMS(Ts, Tr, Tl * Ty);
	  }
	  {
	       E TA, TC, Tz, TB;
	       TA = Tt + Tx;
	       TC = Tq + Tm;
	       Tz = W[4];
	       TB = W[5];
	       rio[WS(ios, 3)] = FNMS(TB, TC, Tz * TA);
	       iio[-WS(ios, 3)] = FMA(Tz, TC, TB * TA);
	  }
	  {
	       E TS, TW, TP, TT;
	       TS = TQ - TR;
	       TW = TU - TV;
	       TP = W[8];
	       TT = W[9];
	       iio[-WS(ios, 1)] = FMA(TP, TS, TT * TW);
	       rio[WS(ios, 5)] = FNMS(TT, TS, TP * TW);
	  }
	  {
	       E TG, TK, TD, TH;
	       TG = TE - TF;
	       TK = TI + TJ;
	       TD = W[0];
	       TH = W[1];
	       rio[WS(ios, 1)] = FNMS(TH, TK, TD * TG);
	       iio[-WS(ios, 5)] = FMA(TD, TK, TH * TG);
	  }
     }
     return W;
}

static const tw_instr twinstr[] = {
     {TW_FULL, 0, 7},
     {TW_NEXT, 1, 0}
};

static const hc2hc_desc desc = { 7, "hb_7", twinstr, {36, 24, 36, 0}, &GENUS, 0, 0, 0 };

void X(codelet_hb_7) (planner *p) {
     X(khc2hc_dif_register) (p, hb_7, &desc);
}
