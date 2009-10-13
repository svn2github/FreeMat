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
#ifndef __MPIWRAP_HPP__
#define __MPIWRAP_HPP__

#include "Array.hpp"
#include "config.h"

#ifdef USE_MPI
ArrayVector MPISend(int nargout, const ArrayVector& args);
ArrayVector MPIRecv(int nargout, const ArrayVector& args);
ArrayVector MPICommRank(int nargout, const ArrayVector& args);
ArrayVector MPICommSize(int nargout, const ArrayVector& args);
ArrayVector MPIEval(int nargout, const ArrayVector& args);
ArrayVector MPIBcast(int nargout, const ArrayVector& args);
ArrayVector MPIBarrier(int nargout, const ArrayVector& args);
ArrayVector MPIReduce(int nargout, const ArrayVector& args);
ArrayVector MPIAllReduce(int nargout, const ArrayVector& args);
ArrayVector MPIInitialized(int nargout, const ArrayVector& args);
ArrayVector MPIInit(int nargout, const ArrayVector& args);
void LoadMPIFunctions(Context*ctxt);
void InitializeMPIWrap();
#endif

#endif
