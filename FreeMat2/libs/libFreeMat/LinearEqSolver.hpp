/*
 * Copyright (c) 2002-2006 Samit Basu
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

#ifndef __LinearEqSolver_hpp__
#define __LinearEqSolver_hpp__

#include "Interpreter.hpp"

/**
 * Matrix-matrix divides (i.e., equation solvers).
 */

/**
 * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
 * $$m \times n$$.  All quantities are double precision.  Uses the
 * LAPACK function dgesvx.
 */
void doubleSolveLinEq(Interpreter* eval, int m, int n, double *c, double* a, double *b);
/**
 * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
 * $$m \times n$$.  All quantities are double-complex precision.  Uses the
 * LAPACK function zgesvx.
 */
void dcomplexSolveLinEq(Interpreter* eval, int m, int n, double *c, double* a, double *b);
/**
 * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
 * $$m \times n$$.  All quantities are single precision.  Uses the
 * LAPACK function sgesvx.
 */
void floatSolveLinEq(Interpreter* eval, int m, int n, float *c, float* a, float *b);
/**
 * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
 * $$m \times n$$.  All quantities are complex precision.  Uses the
 * LAPACK function cgesvx.
 */
void complexSolveLinEq(Interpreter* eval, int m, int n, float *c, float* a, float *b);

/**
 * Return the 1-norm condition number estimate for A.
 */
float floatRecipCond(Interpreter* eval, int m, int n, float *a);
double doubleRecipCond(Interpreter* eval, int m, int n, double *a);
float complexRecipCond(Interpreter* eval, int m, int n, float *a);
double dcomplexRecipCond(Interpreter* eval, int m, int n, double *a);

#endif
