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
/* Generated on Sat Jul  5 21:44:08 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_twiddle_c -simd -compact -variables 4 -n 64 -name t1bv_64 -include t1b.h -sign 1 */

/*
 * This function contains 519 FP additions, 250 FP multiplications,
 * (or, 467 additions, 198 multiplications, 52 fused multiply/add),
 * 107 stack variables, and 128 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "t1b.h"

static const R *t1bv_64(R *ri, R *ii, const R *W, stride ios, int m, int dist)
{
     DVK(KP290284677, +0.290284677254462367636192375817395274691476278);
     DVK(KP956940335, +0.956940335732208864935797886980269969482849206);
     DVK(KP471396736, +0.471396736825997648556387625905254377657460319);
     DVK(KP881921264, +0.881921264348355029712756863660388349508442621);
     DVK(KP634393284, +0.634393284163645498215171613225493370675687095);
     DVK(KP773010453, +0.773010453362736960810906609758469800971041293);
     DVK(KP098017140, +0.098017140329560601994195563888641845861136673);
     DVK(KP995184726, +0.995184726672196886244836953109479921575474869);
     DVK(KP195090322, +0.195090322016128267848284868477022240927691618);
     DVK(KP980785280, +0.980785280403230449126182236134239036973933731);
     DVK(KP555570233, +0.555570233019602224742830813948532874374937191);
     DVK(KP831469612, +0.831469612302545237078788377617905756738560812);
     DVK(KP382683432, +0.382683432365089771728459984030398866761344562);
     DVK(KP923879532, +0.923879532511286756128183189396788286822416626);
     DVK(KP707106781, +0.707106781186547524400844362104849039284835938);
     int i;
     R *x;
     x = ii;
     BEGIN_SIMD();
     for (i = m; i > 0; i = i - VL, x = x + (VL * dist), W = W + (TWVL * 126)) {
	  V Tg, T4B, T6v, T7G, T3r, T4w, T5q, T7F, T5Y, T62, T28, T4d, T2g, T4a, T7g;
	  V T7Y, T6f, T6j, T2Z, T4k, T37, T4h, T7n, T81, T7w, T7x, T7y, T5M, T6q, T1k;
	  V T4s, T1r, T4t, T7t, T7u, T7v, T5F, T6p, TV, T4p, T12, T4q, T7A, T7B, TD;
	  V T4x, T3k, T4C, T5x, T6s, T1R, T4b, T7j, T7Z, T2j, T4e, T5V, T63, T2I, T4i;
	  V T7q, T82, T3a, T4l, T6c, T6k;
	  {
	       V T1, T3, T3p, T3n, Tb, Td, Te, T6, T8, T9, T2, T3o, T3m;
	       T1 = LD(&(x[0]), dist, &(x[0]));
	       T2 = LD(&(x[WS(ios, 32)]), dist, &(x[0]));
	       T3 = BYTW(&(W[TWVL * 62]), T2);
	       T3o = LD(&(x[WS(ios, 48)]), dist, &(x[0]));
	       T3p = BYTW(&(W[TWVL * 94]), T3o);
	       T3m = LD(&(x[WS(ios, 16)]), dist, &(x[0]));
	       T3n = BYTW(&(W[TWVL * 30]), T3m);
	       {
		    V Ta, Tc, T5, T7;
		    Ta = LD(&(x[WS(ios, 56)]), dist, &(x[0]));
		    Tb = BYTW(&(W[TWVL * 110]), Ta);
		    Tc = LD(&(x[WS(ios, 24)]), dist, &(x[0]));
		    Td = BYTW(&(W[TWVL * 46]), Tc);
		    Te = VSUB(Tb, Td);
		    T5 = LD(&(x[WS(ios, 8)]), dist, &(x[0]));
		    T6 = BYTW(&(W[TWVL * 14]), T5);
		    T7 = LD(&(x[WS(ios, 40)]), dist, &(x[0]));
		    T8 = BYTW(&(W[TWVL * 78]), T7);
		    T9 = VSUB(T6, T8);
	       }
	       {
		    V T4, Tf, T6t, T6u;
		    T4 = VSUB(T1, T3);
		    Tf = VMUL(LDK(KP707106781), VADD(T9, Te));
		    Tg = VSUB(T4, Tf);
		    T4B = VADD(T4, Tf);
		    T6t = VADD(T6, T8);
		    T6u = VADD(Tb, Td);
		    T6v = VSUB(T6t, T6u);
		    T7G = VADD(T6t, T6u);
	       }
	       {
		    V T3l, T3q, T5o, T5p;
		    T3l = VMUL(LDK(KP707106781), VSUB(T9, Te));
		    T3q = VSUB(T3n, T3p);
		    T3r = VSUB(T3l, T3q);
		    T4w = VADD(T3q, T3l);
		    T5o = VADD(T1, T3);
		    T5p = VADD(T3n, T3p);
		    T5q = VSUB(T5o, T5p);
		    T7F = VADD(T5o, T5p);
	       }
	  }
	  {
	       V T24, T26, T61, T2b, T2d, T60, T1W, T5W, T21, T5X, T22, T27;
	       {
		    V T23, T25, T2a, T2c;
		    T23 = LD(&(x[WS(ios, 17)]), dist, &(x[WS(ios, 1)]));
		    T24 = BYTW(&(W[TWVL * 32]), T23);
		    T25 = LD(&(x[WS(ios, 49)]), dist, &(x[WS(ios, 1)]));
		    T26 = BYTW(&(W[TWVL * 96]), T25);
		    T61 = VADD(T24, T26);
		    T2a = LD(&(x[WS(ios, 1)]), dist, &(x[WS(ios, 1)]));
		    T2b = BYTW(&(W[0]), T2a);
		    T2c = LD(&(x[WS(ios, 33)]), dist, &(x[WS(ios, 1)]));
		    T2d = BYTW(&(W[TWVL * 64]), T2c);
		    T60 = VADD(T2b, T2d);
	       }
	       {
		    V T1T, T1V, T1S, T1U;
		    T1S = LD(&(x[WS(ios, 9)]), dist, &(x[WS(ios, 1)]));
		    T1T = BYTW(&(W[TWVL * 16]), T1S);
		    T1U = LD(&(x[WS(ios, 41)]), dist, &(x[WS(ios, 1)]));
		    T1V = BYTW(&(W[TWVL * 80]), T1U);
		    T1W = VSUB(T1T, T1V);
		    T5W = VADD(T1T, T1V);
	       }
	       {
		    V T1Y, T20, T1X, T1Z;
		    T1X = LD(&(x[WS(ios, 57)]), dist, &(x[WS(ios, 1)]));
		    T1Y = BYTW(&(W[TWVL * 112]), T1X);
		    T1Z = LD(&(x[WS(ios, 25)]), dist, &(x[WS(ios, 1)]));
		    T20 = BYTW(&(W[TWVL * 48]), T1Z);
		    T21 = VSUB(T1Y, T20);
		    T5X = VADD(T1Y, T20);
	       }
	       T5Y = VSUB(T5W, T5X);
	       T62 = VSUB(T60, T61);
	       T22 = VMUL(LDK(KP707106781), VSUB(T1W, T21));
	       T27 = VSUB(T24, T26);
	       T28 = VSUB(T22, T27);
	       T4d = VADD(T27, T22);
	       {
		    V T2e, T2f, T7e, T7f;
		    T2e = VSUB(T2b, T2d);
		    T2f = VMUL(LDK(KP707106781), VADD(T1W, T21));
		    T2g = VSUB(T2e, T2f);
		    T4a = VADD(T2e, T2f);
		    T7e = VADD(T60, T61);
		    T7f = VADD(T5W, T5X);
		    T7g = VSUB(T7e, T7f);
		    T7Y = VADD(T7e, T7f);
	       }
	  }
	  {
	       V T2V, T2X, T6i, T32, T34, T6h, T2N, T6d, T2S, T6e, T2T, T2Y;
	       {
		    V T2U, T2W, T31, T33;
		    T2U = LD(&(x[WS(ios, 15)]), dist, &(x[WS(ios, 1)]));
		    T2V = BYTW(&(W[TWVL * 28]), T2U);
		    T2W = LD(&(x[WS(ios, 47)]), dist, &(x[WS(ios, 1)]));
		    T2X = BYTW(&(W[TWVL * 92]), T2W);
		    T6i = VADD(T2V, T2X);
		    T31 = LD(&(x[WS(ios, 63)]), dist, &(x[WS(ios, 1)]));
		    T32 = BYTW(&(W[TWVL * 124]), T31);
		    T33 = LD(&(x[WS(ios, 31)]), dist, &(x[WS(ios, 1)]));
		    T34 = BYTW(&(W[TWVL * 60]), T33);
		    T6h = VADD(T32, T34);
	       }
	       {
		    V T2K, T2M, T2J, T2L;
		    T2J = LD(&(x[WS(ios, 7)]), dist, &(x[WS(ios, 1)]));
		    T2K = BYTW(&(W[TWVL * 12]), T2J);
		    T2L = LD(&(x[WS(ios, 39)]), dist, &(x[WS(ios, 1)]));
		    T2M = BYTW(&(W[TWVL * 76]), T2L);
		    T2N = VSUB(T2K, T2M);
		    T6d = VADD(T2K, T2M);
	       }
	       {
		    V T2P, T2R, T2O, T2Q;
		    T2O = LD(&(x[WS(ios, 55)]), dist, &(x[WS(ios, 1)]));
		    T2P = BYTW(&(W[TWVL * 108]), T2O);
		    T2Q = LD(&(x[WS(ios, 23)]), dist, &(x[WS(ios, 1)]));
		    T2R = BYTW(&(W[TWVL * 44]), T2Q);
		    T2S = VSUB(T2P, T2R);
		    T6e = VADD(T2P, T2R);
	       }
	       T6f = VSUB(T6d, T6e);
	       T6j = VSUB(T6h, T6i);
	       T2T = VMUL(LDK(KP707106781), VSUB(T2N, T2S));
	       T2Y = VSUB(T2V, T2X);
	       T2Z = VSUB(T2T, T2Y);
	       T4k = VADD(T2Y, T2T);
	       {
		    V T35, T36, T7l, T7m;
		    T35 = VSUB(T32, T34);
		    T36 = VMUL(LDK(KP707106781), VADD(T2N, T2S));
		    T37 = VSUB(T35, T36);
		    T4h = VADD(T35, T36);
		    T7l = VADD(T6h, T6i);
		    T7m = VADD(T6d, T6e);
		    T7n = VSUB(T7l, T7m);
		    T81 = VADD(T7l, T7m);
	       }
	  }
	  {
	       V T1g, T1i, T5K, T1m, T1o, T5J, T18, T5G, T1d, T5H, T5I, T5L;
	       {
		    V T1f, T1h, T1l, T1n;
		    T1f = LD(&(x[WS(ios, 14)]), dist, &(x[0]));
		    T1g = BYTW(&(W[TWVL * 26]), T1f);
		    T1h = LD(&(x[WS(ios, 46)]), dist, &(x[0]));
		    T1i = BYTW(&(W[TWVL * 90]), T1h);
		    T5K = VADD(T1g, T1i);
		    T1l = LD(&(x[WS(ios, 62)]), dist, &(x[0]));
		    T1m = BYTW(&(W[TWVL * 122]), T1l);
		    T1n = LD(&(x[WS(ios, 30)]), dist, &(x[0]));
		    T1o = BYTW(&(W[TWVL * 58]), T1n);
		    T5J = VADD(T1m, T1o);
	       }
	       {
		    V T15, T17, T14, T16;
		    T14 = LD(&(x[WS(ios, 6)]), dist, &(x[0]));
		    T15 = BYTW(&(W[TWVL * 10]), T14);
		    T16 = LD(&(x[WS(ios, 38)]), dist, &(x[0]));
		    T17 = BYTW(&(W[TWVL * 74]), T16);
		    T18 = VSUB(T15, T17);
		    T5G = VADD(T15, T17);
	       }
	       {
		    V T1a, T1c, T19, T1b;
		    T19 = LD(&(x[WS(ios, 54)]), dist, &(x[0]));
		    T1a = BYTW(&(W[TWVL * 106]), T19);
		    T1b = LD(&(x[WS(ios, 22)]), dist, &(x[0]));
		    T1c = BYTW(&(W[TWVL * 42]), T1b);
		    T1d = VSUB(T1a, T1c);
		    T5H = VADD(T1a, T1c);
	       }
	       T7w = VADD(T5J, T5K);
	       T7x = VADD(T5G, T5H);
	       T7y = VSUB(T7w, T7x);
	       T5I = VSUB(T5G, T5H);
	       T5L = VSUB(T5J, T5K);
	       T5M = VFNMS(LDK(KP382683432), T5L, VMUL(LDK(KP923879532), T5I));
	       T6q = VFMA(LDK(KP923879532), T5L, VMUL(LDK(KP382683432), T5I));
	       {
		    V T1e, T1j, T1p, T1q;
		    T1e = VMUL(LDK(KP707106781), VSUB(T18, T1d));
		    T1j = VSUB(T1g, T1i);
		    T1k = VSUB(T1e, T1j);
		    T4s = VADD(T1j, T1e);
		    T1p = VSUB(T1m, T1o);
		    T1q = VMUL(LDK(KP707106781), VADD(T18, T1d));
		    T1r = VSUB(T1p, T1q);
		    T4t = VADD(T1p, T1q);
	       }
	  }
	  {
	       V TR, TT, T5A, TX, TZ, T5z, TJ, T5C, TO, T5D, T5B, T5E;
	       {
		    V TQ, TS, TW, TY;
		    TQ = LD(&(x[WS(ios, 18)]), dist, &(x[0]));
		    TR = BYTW(&(W[TWVL * 34]), TQ);
		    TS = LD(&(x[WS(ios, 50)]), dist, &(x[0]));
		    TT = BYTW(&(W[TWVL * 98]), TS);
		    T5A = VADD(TR, TT);
		    TW = LD(&(x[WS(ios, 2)]), dist, &(x[0]));
		    TX = BYTW(&(W[TWVL * 2]), TW);
		    TY = LD(&(x[WS(ios, 34)]), dist, &(x[0]));
		    TZ = BYTW(&(W[TWVL * 66]), TY);
		    T5z = VADD(TX, TZ);
	       }
	       {
		    V TG, TI, TF, TH;
		    TF = LD(&(x[WS(ios, 10)]), dist, &(x[0]));
		    TG = BYTW(&(W[TWVL * 18]), TF);
		    TH = LD(&(x[WS(ios, 42)]), dist, &(x[0]));
		    TI = BYTW(&(W[TWVL * 82]), TH);
		    TJ = VSUB(TG, TI);
		    T5C = VADD(TG, TI);
	       }
	       {
		    V TL, TN, TK, TM;
		    TK = LD(&(x[WS(ios, 58)]), dist, &(x[0]));
		    TL = BYTW(&(W[TWVL * 114]), TK);
		    TM = LD(&(x[WS(ios, 26)]), dist, &(x[0]));
		    TN = BYTW(&(W[TWVL * 50]), TM);
		    TO = VSUB(TL, TN);
		    T5D = VADD(TL, TN);
	       }
	       T7t = VADD(T5z, T5A);
	       T7u = VADD(T5C, T5D);
	       T7v = VSUB(T7t, T7u);
	       T5B = VSUB(T5z, T5A);
	       T5E = VSUB(T5C, T5D);
	       T5F = VFMA(LDK(KP382683432), T5B, VMUL(LDK(KP923879532), T5E));
	       T6p = VFNMS(LDK(KP382683432), T5E, VMUL(LDK(KP923879532), T5B));
	       {
		    V TP, TU, T10, T11;
		    TP = VMUL(LDK(KP707106781), VSUB(TJ, TO));
		    TU = VSUB(TR, TT);
		    TV = VSUB(TP, TU);
		    T4p = VADD(TU, TP);
		    T10 = VSUB(TX, TZ);
		    T11 = VMUL(LDK(KP707106781), VADD(TJ, TO));
		    T12 = VSUB(T10, T11);
		    T4q = VADD(T10, T11);
	       }
	  }
	  {
	       V Tl, T5r, TB, T5u, Tq, T5s, Tw, T5v, Tr, TC;
	       {
		    V Ti, Tk, Th, Tj;
		    Th = LD(&(x[WS(ios, 4)]), dist, &(x[0]));
		    Ti = BYTW(&(W[TWVL * 6]), Th);
		    Tj = LD(&(x[WS(ios, 36)]), dist, &(x[0]));
		    Tk = BYTW(&(W[TWVL * 70]), Tj);
		    Tl = VSUB(Ti, Tk);
		    T5r = VADD(Ti, Tk);
	       }
	       {
		    V Ty, TA, Tx, Tz;
		    Tx = LD(&(x[WS(ios, 60)]), dist, &(x[0]));
		    Ty = BYTW(&(W[TWVL * 118]), Tx);
		    Tz = LD(&(x[WS(ios, 28)]), dist, &(x[0]));
		    TA = BYTW(&(W[TWVL * 54]), Tz);
		    TB = VSUB(Ty, TA);
		    T5u = VADD(Ty, TA);
	       }
	       {
		    V Tn, Tp, Tm, To;
		    Tm = LD(&(x[WS(ios, 20)]), dist, &(x[0]));
		    Tn = BYTW(&(W[TWVL * 38]), Tm);
		    To = LD(&(x[WS(ios, 52)]), dist, &(x[0]));
		    Tp = BYTW(&(W[TWVL * 102]), To);
		    Tq = VSUB(Tn, Tp);
		    T5s = VADD(Tn, Tp);
	       }
	       {
		    V Tt, Tv, Ts, Tu;
		    Ts = LD(&(x[WS(ios, 12)]), dist, &(x[0]));
		    Tt = BYTW(&(W[TWVL * 22]), Ts);
		    Tu = LD(&(x[WS(ios, 44)]), dist, &(x[0]));
		    Tv = BYTW(&(W[TWVL * 86]), Tu);
		    Tw = VSUB(Tt, Tv);
		    T5v = VADD(Tt, Tv);
	       }
	       T7A = VADD(T5r, T5s);
	       T7B = VADD(T5u, T5v);
	       Tr = VFMA(LDK(KP382683432), Tl, VMUL(LDK(KP923879532), Tq));
	       TC = VFNMS(LDK(KP382683432), TB, VMUL(LDK(KP923879532), Tw));
	       TD = VSUB(Tr, TC);
	       T4x = VADD(Tr, TC);
	       {
		    V T3i, T3j, T5t, T5w;
		    T3i = VFNMS(LDK(KP382683432), Tq, VMUL(LDK(KP923879532), Tl));
		    T3j = VFMA(LDK(KP923879532), TB, VMUL(LDK(KP382683432), Tw));
		    T3k = VSUB(T3i, T3j);
		    T4C = VADD(T3i, T3j);
		    T5t = VSUB(T5r, T5s);
		    T5w = VSUB(T5u, T5v);
		    T5x = VMUL(LDK(KP707106781), VADD(T5t, T5w));
		    T6s = VMUL(LDK(KP707106781), VSUB(T5t, T5w));
	       }
	  }
	  {
	       V T1z, T5P, T1P, T5T, T1E, T5Q, T1K, T5S;
	       {
		    V T1w, T1y, T1v, T1x;
		    T1v = LD(&(x[WS(ios, 5)]), dist, &(x[WS(ios, 1)]));
		    T1w = BYTW(&(W[TWVL * 8]), T1v);
		    T1x = LD(&(x[WS(ios, 37)]), dist, &(x[WS(ios, 1)]));
		    T1y = BYTW(&(W[TWVL * 72]), T1x);
		    T1z = VSUB(T1w, T1y);
		    T5P = VADD(T1w, T1y);
	       }
	       {
		    V T1M, T1O, T1L, T1N;
		    T1L = LD(&(x[WS(ios, 13)]), dist, &(x[WS(ios, 1)]));
		    T1M = BYTW(&(W[TWVL * 24]), T1L);
		    T1N = LD(&(x[WS(ios, 45)]), dist, &(x[WS(ios, 1)]));
		    T1O = BYTW(&(W[TWVL * 88]), T1N);
		    T1P = VSUB(T1M, T1O);
		    T5T = VADD(T1M, T1O);
	       }
	       {
		    V T1B, T1D, T1A, T1C;
		    T1A = LD(&(x[WS(ios, 21)]), dist, &(x[WS(ios, 1)]));
		    T1B = BYTW(&(W[TWVL * 40]), T1A);
		    T1C = LD(&(x[WS(ios, 53)]), dist, &(x[WS(ios, 1)]));
		    T1D = BYTW(&(W[TWVL * 104]), T1C);
		    T1E = VSUB(T1B, T1D);
		    T5Q = VADD(T1B, T1D);
	       }
	       {
		    V T1H, T1J, T1G, T1I;
		    T1G = LD(&(x[WS(ios, 61)]), dist, &(x[WS(ios, 1)]));
		    T1H = BYTW(&(W[TWVL * 120]), T1G);
		    T1I = LD(&(x[WS(ios, 29)]), dist, &(x[WS(ios, 1)]));
		    T1J = BYTW(&(W[TWVL * 56]), T1I);
		    T1K = VSUB(T1H, T1J);
		    T5S = VADD(T1H, T1J);
	       }
	       {
		    V T1F, T1Q, T7h, T7i;
		    T1F = VFNMS(LDK(KP382683432), T1E, VMUL(LDK(KP923879532), T1z));
		    T1Q = VFMA(LDK(KP923879532), T1K, VMUL(LDK(KP382683432), T1P));
		    T1R = VSUB(T1F, T1Q);
		    T4b = VADD(T1F, T1Q);
		    T7h = VADD(T5P, T5Q);
		    T7i = VADD(T5S, T5T);
		    T7j = VSUB(T7h, T7i);
		    T7Z = VADD(T7h, T7i);
	       }
	       {
		    V T2h, T2i, T5R, T5U;
		    T2h = VFMA(LDK(KP382683432), T1z, VMUL(LDK(KP923879532), T1E));
		    T2i = VFNMS(LDK(KP382683432), T1K, VMUL(LDK(KP923879532), T1P));
		    T2j = VSUB(T2h, T2i);
		    T4e = VADD(T2h, T2i);
		    T5R = VSUB(T5P, T5Q);
		    T5U = VSUB(T5S, T5T);
		    T5V = VMUL(LDK(KP707106781), VSUB(T5R, T5U));
		    T63 = VMUL(LDK(KP707106781), VADD(T5R, T5U));
	       }
	  }
	  {
	       V T2q, T66, T2G, T6a, T2v, T67, T2B, T69;
	       {
		    V T2n, T2p, T2m, T2o;
		    T2m = LD(&(x[WS(ios, 3)]), dist, &(x[WS(ios, 1)]));
		    T2n = BYTW(&(W[TWVL * 4]), T2m);
		    T2o = LD(&(x[WS(ios, 35)]), dist, &(x[WS(ios, 1)]));
		    T2p = BYTW(&(W[TWVL * 68]), T2o);
		    T2q = VSUB(T2n, T2p);
		    T66 = VADD(T2n, T2p);
	       }
	       {
		    V T2D, T2F, T2C, T2E;
		    T2C = LD(&(x[WS(ios, 11)]), dist, &(x[WS(ios, 1)]));
		    T2D = BYTW(&(W[TWVL * 20]), T2C);
		    T2E = LD(&(x[WS(ios, 43)]), dist, &(x[WS(ios, 1)]));
		    T2F = BYTW(&(W[TWVL * 84]), T2E);
		    T2G = VSUB(T2D, T2F);
		    T6a = VADD(T2D, T2F);
	       }
	       {
		    V T2s, T2u, T2r, T2t;
		    T2r = LD(&(x[WS(ios, 19)]), dist, &(x[WS(ios, 1)]));
		    T2s = BYTW(&(W[TWVL * 36]), T2r);
		    T2t = LD(&(x[WS(ios, 51)]), dist, &(x[WS(ios, 1)]));
		    T2u = BYTW(&(W[TWVL * 100]), T2t);
		    T2v = VSUB(T2s, T2u);
		    T67 = VADD(T2s, T2u);
	       }
	       {
		    V T2y, T2A, T2x, T2z;
		    T2x = LD(&(x[WS(ios, 59)]), dist, &(x[WS(ios, 1)]));
		    T2y = BYTW(&(W[TWVL * 116]), T2x);
		    T2z = LD(&(x[WS(ios, 27)]), dist, &(x[WS(ios, 1)]));
		    T2A = BYTW(&(W[TWVL * 52]), T2z);
		    T2B = VSUB(T2y, T2A);
		    T69 = VADD(T2y, T2A);
	       }
	       {
		    V T2w, T2H, T7o, T7p;
		    T2w = VFNMS(LDK(KP382683432), T2v, VMUL(LDK(KP923879532), T2q));
		    T2H = VFMA(LDK(KP923879532), T2B, VMUL(LDK(KP382683432), T2G));
		    T2I = VSUB(T2w, T2H);
		    T4i = VADD(T2w, T2H);
		    T7o = VADD(T66, T67);
		    T7p = VADD(T69, T6a);
		    T7q = VSUB(T7o, T7p);
		    T82 = VADD(T7o, T7p);
	       }
	       {
		    V T38, T39, T68, T6b;
		    T38 = VFMA(LDK(KP382683432), T2q, VMUL(LDK(KP923879532), T2v));
		    T39 = VFNMS(LDK(KP382683432), T2B, VMUL(LDK(KP923879532), T2G));
		    T3a = VSUB(T38, T39);
		    T4l = VADD(T38, T39);
		    T68 = VSUB(T66, T67);
		    T6b = VSUB(T69, T6a);
		    T6c = VMUL(LDK(KP707106781), VSUB(T68, T6b));
		    T6k = VMUL(LDK(KP707106781), VADD(T68, T6b));
	       }
	  }
	  {
	       V T7s, T7R, T7M, T7U, T7D, T7T, T7J, T7Q;
	       {
		    V T7k, T7r, T7K, T7L;
		    T7k = VFNMS(LDK(KP382683432), T7j, VMUL(LDK(KP923879532), T7g));
		    T7r = VFMA(LDK(KP923879532), T7n, VMUL(LDK(KP382683432), T7q));
		    T7s = VSUB(T7k, T7r);
		    T7R = VADD(T7k, T7r);
		    T7K = VFMA(LDK(KP382683432), T7g, VMUL(LDK(KP923879532), T7j));
		    T7L = VFNMS(LDK(KP382683432), T7n, VMUL(LDK(KP923879532), T7q));
		    T7M = VSUB(T7K, T7L);
		    T7U = VADD(T7K, T7L);
	       }
	       {
		    V T7z, T7C, T7H, T7I;
		    T7z = VMUL(LDK(KP707106781), VSUB(T7v, T7y));
		    T7C = VSUB(T7A, T7B);
		    T7D = VSUB(T7z, T7C);
		    T7T = VADD(T7C, T7z);
		    T7H = VSUB(T7F, T7G);
		    T7I = VMUL(LDK(KP707106781), VADD(T7v, T7y));
		    T7J = VSUB(T7H, T7I);
		    T7Q = VADD(T7H, T7I);
	       }
	       {
		    V T7E, T7N, T7W, T7X;
		    T7E = VBYI(VSUB(T7s, T7D));
		    T7N = VSUB(T7J, T7M);
		    ST(&(x[WS(ios, 20)]), VADD(T7E, T7N), dist, &(x[0]));
		    ST(&(x[WS(ios, 44)]), VSUB(T7N, T7E), dist, &(x[0]));
		    T7W = VSUB(T7Q, T7R);
		    T7X = VBYI(VSUB(T7U, T7T));
		    ST(&(x[WS(ios, 36)]), VSUB(T7W, T7X), dist, &(x[0]));
		    ST(&(x[WS(ios, 28)]), VADD(T7W, T7X), dist, &(x[0]));
	       }
	       {
		    V T7O, T7P, T7S, T7V;
		    T7O = VBYI(VADD(T7D, T7s));
		    T7P = VADD(T7J, T7M);
		    ST(&(x[WS(ios, 12)]), VADD(T7O, T7P), dist, &(x[0]));
		    ST(&(x[WS(ios, 52)]), VSUB(T7P, T7O), dist, &(x[0]));
		    T7S = VADD(T7Q, T7R);
		    T7V = VBYI(VADD(T7T, T7U));
		    ST(&(x[WS(ios, 60)]), VSUB(T7S, T7V), dist, &(x[0]));
		    ST(&(x[WS(ios, 4)]), VADD(T7S, T7V), dist, &(x[0]));
	       }
	  }
	  {
	       V T84, T8c, T8l, T8n, T87, T8h, T8b, T8g, T8i, T8m;
	       {
		    V T80, T83, T8j, T8k;
		    T80 = VSUB(T7Y, T7Z);
		    T83 = VSUB(T81, T82);
		    T84 = VMUL(LDK(KP707106781), VSUB(T80, T83));
		    T8c = VMUL(LDK(KP707106781), VADD(T80, T83));
		    T8j = VADD(T7Y, T7Z);
		    T8k = VADD(T81, T82);
		    T8l = VBYI(VSUB(T8j, T8k));
		    T8n = VADD(T8j, T8k);
	       }
	       {
		    V T85, T86, T89, T8a;
		    T85 = VADD(T7t, T7u);
		    T86 = VADD(T7w, T7x);
		    T87 = VSUB(T85, T86);
		    T8h = VADD(T85, T86);
		    T89 = VADD(T7F, T7G);
		    T8a = VADD(T7A, T7B);
		    T8b = VSUB(T89, T8a);
		    T8g = VADD(T89, T8a);
	       }
	       T8i = VSUB(T8g, T8h);
	       ST(&(x[WS(ios, 48)]), VSUB(T8i, T8l), dist, &(x[0]));
	       ST(&(x[WS(ios, 16)]), VADD(T8i, T8l), dist, &(x[0]));
	       T8m = VADD(T8g, T8h);
	       ST(&(x[WS(ios, 32)]), VSUB(T8m, T8n), dist, &(x[0]));
	       ST(&(x[0]), VADD(T8m, T8n), dist, &(x[0]));
	       {
		    V T88, T8d, T8e, T8f;
		    T88 = VBYI(VSUB(T84, T87));
		    T8d = VSUB(T8b, T8c);
		    ST(&(x[WS(ios, 24)]), VADD(T88, T8d), dist, &(x[0]));
		    ST(&(x[WS(ios, 40)]), VSUB(T8d, T88), dist, &(x[0]));
		    T8e = VBYI(VADD(T87, T84));
		    T8f = VADD(T8b, T8c);
		    ST(&(x[WS(ios, 8)]), VADD(T8e, T8f), dist, &(x[0]));
		    ST(&(x[WS(ios, 56)]), VSUB(T8f, T8e), dist, &(x[0]));
	       }
	  }
	  {
	       V T5O, T6H, T6x, T6F, T6n, T6I, T6A, T6E;
	       {
		    V T5y, T5N, T6r, T6w;
		    T5y = VSUB(T5q, T5x);
		    T5N = VSUB(T5F, T5M);
		    T5O = VSUB(T5y, T5N);
		    T6H = VADD(T5y, T5N);
		    T6r = VSUB(T6p, T6q);
		    T6w = VSUB(T6s, T6v);
		    T6x = VSUB(T6r, T6w);
		    T6F = VADD(T6w, T6r);
		    {
			 V T65, T6y, T6m, T6z;
			 {
			      V T5Z, T64, T6g, T6l;
			      T5Z = VSUB(T5V, T5Y);
			      T64 = VSUB(T62, T63);
			      T65 = VFMA(LDK(KP831469612), T5Z, VMUL(LDK(KP555570233), T64));
			      T6y = VFNMS(LDK(KP555570233), T5Z, VMUL(LDK(KP831469612), T64));
			      T6g = VSUB(T6c, T6f);
			      T6l = VSUB(T6j, T6k);
			      T6m = VFNMS(LDK(KP555570233), T6l, VMUL(LDK(KP831469612), T6g));
			      T6z = VFMA(LDK(KP555570233), T6g, VMUL(LDK(KP831469612), T6l));
			 }
			 T6n = VSUB(T65, T6m);
			 T6I = VADD(T6y, T6z);
			 T6A = VSUB(T6y, T6z);
			 T6E = VADD(T65, T6m);
		    }
	       }
	       {
		    V T6o, T6B, T6K, T6L;
		    T6o = VADD(T5O, T6n);
		    T6B = VBYI(VADD(T6x, T6A));
		    ST(&(x[WS(ios, 54)]), VSUB(T6o, T6B), dist, &(x[0]));
		    ST(&(x[WS(ios, 10)]), VADD(T6o, T6B), dist, &(x[0]));
		    T6K = VBYI(VADD(T6F, T6E));
		    T6L = VADD(T6H, T6I);
		    ST(&(x[WS(ios, 6)]), VADD(T6K, T6L), dist, &(x[0]));
		    ST(&(x[WS(ios, 58)]), VSUB(T6L, T6K), dist, &(x[0]));
	       }
	       {
		    V T6C, T6D, T6G, T6J;
		    T6C = VSUB(T5O, T6n);
		    T6D = VBYI(VSUB(T6A, T6x));
		    ST(&(x[WS(ios, 42)]), VSUB(T6C, T6D), dist, &(x[0]));
		    ST(&(x[WS(ios, 22)]), VADD(T6C, T6D), dist, &(x[0]));
		    T6G = VBYI(VSUB(T6E, T6F));
		    T6J = VSUB(T6H, T6I);
		    ST(&(x[WS(ios, 26)]), VADD(T6G, T6J), dist, &(x[0]));
		    ST(&(x[WS(ios, 38)]), VSUB(T6J, T6G), dist, &(x[0]));
	       }
	  }
	  {
	       V T6O, T79, T6Z, T77, T6V, T7a, T72, T76;
	       {
		    V T6M, T6N, T6X, T6Y;
		    T6M = VADD(T5q, T5x);
		    T6N = VADD(T6p, T6q);
		    T6O = VSUB(T6M, T6N);
		    T79 = VADD(T6M, T6N);
		    T6X = VADD(T5F, T5M);
		    T6Y = VADD(T6v, T6s);
		    T6Z = VSUB(T6X, T6Y);
		    T77 = VADD(T6Y, T6X);
		    {
			 V T6R, T70, T6U, T71;
			 {
			      V T6P, T6Q, T6S, T6T;
			      T6P = VADD(T5Y, T5V);
			      T6Q = VADD(T62, T63);
			      T6R = VFMA(LDK(KP980785280), T6P, VMUL(LDK(KP195090322), T6Q));
			      T70 = VFNMS(LDK(KP195090322), T6P, VMUL(LDK(KP980785280), T6Q));
			      T6S = VADD(T6f, T6c);
			      T6T = VADD(T6j, T6k);
			      T6U = VFNMS(LDK(KP195090322), T6T, VMUL(LDK(KP980785280), T6S));
			      T71 = VFMA(LDK(KP195090322), T6S, VMUL(LDK(KP980785280), T6T));
			 }
			 T6V = VSUB(T6R, T6U);
			 T7a = VADD(T70, T71);
			 T72 = VSUB(T70, T71);
			 T76 = VADD(T6R, T6U);
		    }
	       }
	       {
		    V T6W, T73, T7c, T7d;
		    T6W = VADD(T6O, T6V);
		    T73 = VBYI(VADD(T6Z, T72));
		    ST(&(x[WS(ios, 50)]), VSUB(T6W, T73), dist, &(x[0]));
		    ST(&(x[WS(ios, 14)]), VADD(T6W, T73), dist, &(x[0]));
		    T7c = VBYI(VADD(T77, T76));
		    T7d = VADD(T79, T7a);
		    ST(&(x[WS(ios, 2)]), VADD(T7c, T7d), dist, &(x[0]));
		    ST(&(x[WS(ios, 62)]), VSUB(T7d, T7c), dist, &(x[0]));
	       }
	       {
		    V T74, T75, T78, T7b;
		    T74 = VSUB(T6O, T6V);
		    T75 = VBYI(VSUB(T72, T6Z));
		    ST(&(x[WS(ios, 46)]), VSUB(T74, T75), dist, &(x[0]));
		    ST(&(x[WS(ios, 18)]), VADD(T74, T75), dist, &(x[0]));
		    T78 = VBYI(VSUB(T76, T77));
		    T7b = VSUB(T79, T7a);
		    ST(&(x[WS(ios, 30)]), VADD(T78, T7b), dist, &(x[0]));
		    ST(&(x[WS(ios, 34)]), VSUB(T7b, T78), dist, &(x[0]));
	       }
	  }
	  {
	       V T4z, T5g, T4R, T59, T4H, T5j, T4O, T55, T4o, T4S, T4K, T4P, T52, T5k, T5c;
	       V T5h;
	       {
		    V T4y, T57, T4v, T58, T4r, T4u;
		    T4y = VADD(T4w, T4x);
		    T57 = VSUB(T4B, T4C);
		    T4r = VFMA(LDK(KP980785280), T4p, VMUL(LDK(KP195090322), T4q));
		    T4u = VFNMS(LDK(KP195090322), T4t, VMUL(LDK(KP980785280), T4s));
		    T4v = VADD(T4r, T4u);
		    T58 = VSUB(T4r, T4u);
		    T4z = VSUB(T4v, T4y);
		    T5g = VADD(T57, T58);
		    T4R = VADD(T4y, T4v);
		    T59 = VSUB(T57, T58);
	       }
	       {
		    V T4D, T54, T4G, T53, T4E, T4F;
		    T4D = VADD(T4B, T4C);
		    T54 = VSUB(T4x, T4w);
		    T4E = VFNMS(LDK(KP195090322), T4p, VMUL(LDK(KP980785280), T4q));
		    T4F = VFMA(LDK(KP195090322), T4s, VMUL(LDK(KP980785280), T4t));
		    T4G = VADD(T4E, T4F);
		    T53 = VSUB(T4E, T4F);
		    T4H = VSUB(T4D, T4G);
		    T5j = VADD(T54, T53);
		    T4O = VADD(T4D, T4G);
		    T55 = VSUB(T53, T54);
	       }
	       {
		    V T4g, T4I, T4n, T4J;
		    {
			 V T4c, T4f, T4j, T4m;
			 T4c = VADD(T4a, T4b);
			 T4f = VADD(T4d, T4e);
			 T4g = VFNMS(LDK(KP098017140), T4f, VMUL(LDK(KP995184726), T4c));
			 T4I = VFMA(LDK(KP098017140), T4c, VMUL(LDK(KP995184726), T4f));
			 T4j = VADD(T4h, T4i);
			 T4m = VADD(T4k, T4l);
			 T4n = VFMA(LDK(KP995184726), T4j, VMUL(LDK(KP098017140), T4m));
			 T4J = VFNMS(LDK(KP098017140), T4j, VMUL(LDK(KP995184726), T4m));
		    }
		    T4o = VSUB(T4g, T4n);
		    T4S = VADD(T4I, T4J);
		    T4K = VSUB(T4I, T4J);
		    T4P = VADD(T4g, T4n);
	       }
	       {
		    V T4Y, T5a, T51, T5b;
		    {
			 V T4W, T4X, T4Z, T50;
			 T4W = VSUB(T4a, T4b);
			 T4X = VSUB(T4e, T4d);
			 T4Y = VFNMS(LDK(KP634393284), T4X, VMUL(LDK(KP773010453), T4W));
			 T5a = VFMA(LDK(KP634393284), T4W, VMUL(LDK(KP773010453), T4X));
			 T4Z = VSUB(T4h, T4i);
			 T50 = VSUB(T4l, T4k);
			 T51 = VFMA(LDK(KP773010453), T4Z, VMUL(LDK(KP634393284), T50));
			 T5b = VFNMS(LDK(KP634393284), T4Z, VMUL(LDK(KP773010453), T50));
		    }
		    T52 = VSUB(T4Y, T51);
		    T5k = VADD(T5a, T5b);
		    T5c = VSUB(T5a, T5b);
		    T5h = VADD(T4Y, T51);
	       }
	       {
		    V T4A, T4L, T5i, T5l;
		    T4A = VBYI(VSUB(T4o, T4z));
		    T4L = VSUB(T4H, T4K);
		    ST(&(x[WS(ios, 17)]), VADD(T4A, T4L), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 47)]), VSUB(T4L, T4A), dist, &(x[WS(ios, 1)]));
		    T5i = VADD(T5g, T5h);
		    T5l = VBYI(VADD(T5j, T5k));
		    ST(&(x[WS(ios, 57)]), VSUB(T5i, T5l), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 7)]), VADD(T5i, T5l), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T5m, T5n, T4M, T4N;
		    T5m = VSUB(T5g, T5h);
		    T5n = VBYI(VSUB(T5k, T5j));
		    ST(&(x[WS(ios, 39)]), VSUB(T5m, T5n), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 25)]), VADD(T5m, T5n), dist, &(x[WS(ios, 1)]));
		    T4M = VBYI(VADD(T4z, T4o));
		    T4N = VADD(T4H, T4K);
		    ST(&(x[WS(ios, 15)]), VADD(T4M, T4N), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 49)]), VSUB(T4N, T4M), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T4Q, T4T, T56, T5d;
		    T4Q = VADD(T4O, T4P);
		    T4T = VBYI(VADD(T4R, T4S));
		    ST(&(x[WS(ios, 63)]), VSUB(T4Q, T4T), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 1)]), VADD(T4Q, T4T), dist, &(x[WS(ios, 1)]));
		    T56 = VBYI(VSUB(T52, T55));
		    T5d = VSUB(T59, T5c);
		    ST(&(x[WS(ios, 23)]), VADD(T56, T5d), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 41)]), VSUB(T5d, T56), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T5e, T5f, T4U, T4V;
		    T5e = VBYI(VADD(T55, T52));
		    T5f = VADD(T59, T5c);
		    ST(&(x[WS(ios, 9)]), VADD(T5e, T5f), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 55)]), VSUB(T5f, T5e), dist, &(x[WS(ios, 1)]));
		    T4U = VSUB(T4O, T4P);
		    T4V = VBYI(VSUB(T4S, T4R));
		    ST(&(x[WS(ios, 33)]), VSUB(T4U, T4V), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 31)]), VADD(T4U, T4V), dist, &(x[WS(ios, 1)]));
	       }
	  }
	  {
	       V T1u, T43, T3D, T3V, T3t, T45, T3B, T3K, T3d, T3E, T3w, T3A, T3R, T46, T3Y;
	       V T42;
	       {
		    V TE, T3U, T1t, T3T, T13, T1s;
		    TE = VSUB(Tg, TD);
		    T3U = VADD(T3r, T3k);
		    T13 = VFMA(LDK(KP831469612), TV, VMUL(LDK(KP555570233), T12));
		    T1s = VFNMS(LDK(KP555570233), T1r, VMUL(LDK(KP831469612), T1k));
		    T1t = VSUB(T13, T1s);
		    T3T = VADD(T13, T1s);
		    T1u = VSUB(TE, T1t);
		    T43 = VADD(T3U, T3T);
		    T3D = VADD(TE, T1t);
		    T3V = VSUB(T3T, T3U);
	       }
	       {
		    V T3s, T3I, T3h, T3J, T3f, T3g;
		    T3s = VSUB(T3k, T3r);
		    T3I = VADD(Tg, TD);
		    T3f = VFNMS(LDK(KP555570233), TV, VMUL(LDK(KP831469612), T12));
		    T3g = VFMA(LDK(KP555570233), T1k, VMUL(LDK(KP831469612), T1r));
		    T3h = VSUB(T3f, T3g);
		    T3J = VADD(T3f, T3g);
		    T3t = VSUB(T3h, T3s);
		    T45 = VADD(T3I, T3J);
		    T3B = VADD(T3s, T3h);
		    T3K = VSUB(T3I, T3J);
	       }
	       {
		    V T2l, T3u, T3c, T3v;
		    {
			 V T29, T2k, T30, T3b;
			 T29 = VSUB(T1R, T28);
			 T2k = VSUB(T2g, T2j);
			 T2l = VFMA(LDK(KP881921264), T29, VMUL(LDK(KP471396736), T2k));
			 T3u = VFNMS(LDK(KP471396736), T29, VMUL(LDK(KP881921264), T2k));
			 T30 = VSUB(T2I, T2Z);
			 T3b = VSUB(T37, T3a);
			 T3c = VFNMS(LDK(KP471396736), T3b, VMUL(LDK(KP881921264), T30));
			 T3v = VFMA(LDK(KP471396736), T30, VMUL(LDK(KP881921264), T3b));
		    }
		    T3d = VSUB(T2l, T3c);
		    T3E = VADD(T3u, T3v);
		    T3w = VSUB(T3u, T3v);
		    T3A = VADD(T2l, T3c);
	       }
	       {
		    V T3N, T3W, T3Q, T3X;
		    {
			 V T3L, T3M, T3O, T3P;
			 T3L = VADD(T28, T1R);
			 T3M = VADD(T2g, T2j);
			 T3N = VFMA(LDK(KP956940335), T3L, VMUL(LDK(KP290284677), T3M));
			 T3W = VFNMS(LDK(KP290284677), T3L, VMUL(LDK(KP956940335), T3M));
			 T3O = VADD(T2Z, T2I);
			 T3P = VADD(T37, T3a);
			 T3Q = VFNMS(LDK(KP290284677), T3P, VMUL(LDK(KP956940335), T3O));
			 T3X = VFMA(LDK(KP290284677), T3O, VMUL(LDK(KP956940335), T3P));
		    }
		    T3R = VSUB(T3N, T3Q);
		    T46 = VADD(T3W, T3X);
		    T3Y = VSUB(T3W, T3X);
		    T42 = VADD(T3N, T3Q);
	       }
	       {
		    V T3e, T3x, T44, T47;
		    T3e = VADD(T1u, T3d);
		    T3x = VBYI(VADD(T3t, T3w));
		    ST(&(x[WS(ios, 53)]), VSUB(T3e, T3x), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 11)]), VADD(T3e, T3x), dist, &(x[WS(ios, 1)]));
		    T44 = VBYI(VSUB(T42, T43));
		    T47 = VSUB(T45, T46);
		    ST(&(x[WS(ios, 29)]), VADD(T44, T47), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 35)]), VSUB(T47, T44), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T48, T49, T3y, T3z;
		    T48 = VBYI(VADD(T43, T42));
		    T49 = VADD(T45, T46);
		    ST(&(x[WS(ios, 3)]), VADD(T48, T49), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 61)]), VSUB(T49, T48), dist, &(x[WS(ios, 1)]));
		    T3y = VSUB(T1u, T3d);
		    T3z = VBYI(VSUB(T3w, T3t));
		    ST(&(x[WS(ios, 43)]), VSUB(T3y, T3z), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 21)]), VADD(T3y, T3z), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T3C, T3F, T3S, T3Z;
		    T3C = VBYI(VSUB(T3A, T3B));
		    T3F = VSUB(T3D, T3E);
		    ST(&(x[WS(ios, 27)]), VADD(T3C, T3F), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 37)]), VSUB(T3F, T3C), dist, &(x[WS(ios, 1)]));
		    T3S = VADD(T3K, T3R);
		    T3Z = VBYI(VADD(T3V, T3Y));
		    ST(&(x[WS(ios, 51)]), VSUB(T3S, T3Z), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 13)]), VADD(T3S, T3Z), dist, &(x[WS(ios, 1)]));
	       }
	       {
		    V T40, T41, T3G, T3H;
		    T40 = VSUB(T3K, T3R);
		    T41 = VBYI(VSUB(T3Y, T3V));
		    ST(&(x[WS(ios, 45)]), VSUB(T40, T41), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 19)]), VADD(T40, T41), dist, &(x[WS(ios, 1)]));
		    T3G = VBYI(VADD(T3B, T3A));
		    T3H = VADD(T3D, T3E);
		    ST(&(x[WS(ios, 5)]), VADD(T3G, T3H), dist, &(x[WS(ios, 1)]));
		    ST(&(x[WS(ios, 59)]), VSUB(T3H, T3G), dist, &(x[WS(ios, 1)]));
	       }
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
     VTW(6),
     VTW(7),
     VTW(8),
     VTW(9),
     VTW(10),
     VTW(11),
     VTW(12),
     VTW(13),
     VTW(14),
     VTW(15),
     VTW(16),
     VTW(17),
     VTW(18),
     VTW(19),
     VTW(20),
     VTW(21),
     VTW(22),
     VTW(23),
     VTW(24),
     VTW(25),
     VTW(26),
     VTW(27),
     VTW(28),
     VTW(29),
     VTW(30),
     VTW(31),
     VTW(32),
     VTW(33),
     VTW(34),
     VTW(35),
     VTW(36),
     VTW(37),
     VTW(38),
     VTW(39),
     VTW(40),
     VTW(41),
     VTW(42),
     VTW(43),
     VTW(44),
     VTW(45),
     VTW(46),
     VTW(47),
     VTW(48),
     VTW(49),
     VTW(50),
     VTW(51),
     VTW(52),
     VTW(53),
     VTW(54),
     VTW(55),
     VTW(56),
     VTW(57),
     VTW(58),
     VTW(59),
     VTW(60),
     VTW(61),
     VTW(62),
     VTW(63),
     {TW_NEXT, VL, 0}
};

static const ct_desc desc = { 64, "t1bv_64", twinstr, {467, 198, 52, 0}, &GENUS, 0, 0, 0 };

void X(codelet_t1bv_64) (planner *p) {
     X(kdft_dit_register) (p, t1bv_64, &desc);
}
