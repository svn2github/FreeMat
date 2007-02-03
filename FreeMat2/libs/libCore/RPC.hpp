#ifndef __RPC_hpp__
#define __RPC_hpp__

#include "Array.hpp"

ArrayVector TCPServerFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPAcceptFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPConnectFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPSendFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPRecvFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPCloseFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPServerCloseFunction(int nargout, const ArrayVector& arg);
#endif
