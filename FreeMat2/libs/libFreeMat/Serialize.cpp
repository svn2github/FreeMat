// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Serialize.hpp"
#include "Exception.hpp"
#include "Malloc.hpp"
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace FreeMat {
  Serialize::Serialize(Stream *sck) {
    endianSwap = false;
    s = sck;
  }

  Serialize::~Serialize() {
  }

  void Serialize::handshakeServer() {
    try {
      char hand = 'A';
      s->writeBytes(&hand,sizeof(char));
      hand = 'Y';
      s->writeBytes(&hand,sizeof(char));
      hand = 'B';
      s->writeBytes(&hand,sizeof(char));
      hand = 'S';
      s->writeBytes(&hand,sizeof(char));
      unsigned short t;
//       s->readBytes(&t,sizeof(short));
//       if (t == 1)
// 	endianSwap = false;
//       else if (t == 256)
// 	endianSwap = true;
//       else
// 	throw Exception("Handshaking error! Unable to resolve byte ordering between server/client");
      t = 1;
      s->writeBytes(&t,sizeof(short));
    } catch (Exception& e) {
      throw;
    }
  }

  void Serialize::handshakeClient() {
    try {
      char hand;
      s->readBytes(&hand,sizeof(char));
      if (hand != 'A')
	throw Exception("Handshaking error! Unable to establish serialization");
      s->readBytes(&hand,sizeof(char));
      if (hand != 'Y')
	throw Exception("Handshaking error! Unable to establish serialization");
      s->readBytes(&hand,sizeof(char));
      if (hand != 'B')
	throw Exception("Handshaking error! Unable to establish serialization");
      s->readBytes(&hand,sizeof(char));
      if (hand != 'S')
	throw Exception("Handshaking error! Unable to establish serialization");
      unsigned short t = 1;
      s->readBytes(&t,sizeof(short));
      if (t == 1)
	endianSwap = false;
      else if (t == 256)
	endianSwap = true;
      else
	throw Exception("Handshaking error! Unable to resolve byte ordering between server/client");    
    } catch (Exception &e) {
      throw ;
    }
  }

  void Serialize::sendSignature(char type, int count) {
    s->writeBytes(&type,1);
    long netcount;
    netcount = htonl(count);
    s->writeBytes(&netcount,sizeof(long));
  }

  void Serialize::checkSignature(char type, int count) {
    char rtype;
    long rcount;
    s->readBytes(&rtype,1);
    s->readBytes(&rcount,sizeof(long));
    rcount = ntohl(rcount);
    if (!((type == rtype) && (count == rcount))) {
      char buffer[1000];
      sprintf(buffer,"Serialization Mismatch: expected <%c,%d>, got <%c,%d>",
	      type,count,rtype,rcount);
      throw Exception(buffer);
    }
  }

  // Send a stream of base objects
  void Serialize::putBytes(const char *ptr, int count) {
    sendSignature('c',count);
    s->writeBytes(ptr,count*sizeof(char));
  }

  void Serialize::putShorts(const short *ptr, int count) {
    sendSignature('s',count);
    s->writeBytes(ptr,count*sizeof(short));
  }

  void Serialize::putInts(const int *ptr, int count) {
    sendSignature('i',count);
    s->writeBytes(ptr,count*sizeof(int));
  }

  void Serialize::putFloats(const float *ptr, int count) {
    sendSignature('f',count);
    s->writeBytes(ptr,count*sizeof(float));
  }

  void Serialize::putDoubles(const double *ptr, int count) {
    sendSignature('d',count);
    s->writeBytes(ptr,count*sizeof(double));
  }

  void Serialize::putString(const char *ptr) {
    unsigned int len;
    sendSignature('x',0);
    len = strlen(ptr);
    putInts((int*)&len,1);
    putBytes(ptr,len);
  }

  void Serialize::putByte(char t) {
    putBytes(&t,1);
  }

  void Serialize::putShort(short t) {
    putShorts(&t,1);
  }

  void Serialize::putInt(int t) {
    putInts(&t,1);
  }

  void Serialize::putFloat(float t) {
    putFloats(&t,1);
  }

  void Serialize::putDouble(double t) {
    putDoubles(&t,1);
  }

  void Serialize::getBytes(char *ptr, int count) {
    checkSignature('c',count);
    s->readBytes(ptr,count*sizeof(char));
  }

#define SWAP(a,b) {tmp = a; a = b; b = tmp;}
  void Serialize::getShorts(short *ptr, int count) {
    checkSignature('s',count);
    s->readBytes(ptr,count*sizeof(short));
    if (endianSwap) {
      char *cptr = (char *) ptr;
      char tmp;
      for (int i=0;i<2*count;i+=2)
	SWAP(cptr[i],cptr[i+1]);
    }
  }

  void Serialize::getInts(int *ptr, int count) {
    checkSignature('i',count);
    s->readBytes(ptr,count*sizeof(int));
    if (endianSwap) {
      char *cptr = (char *) ptr;
      char tmp;
      for (int i=0;i<4*count;i+=4) {
	SWAP(cptr[i],cptr[i+3]);
	SWAP(cptr[i+1],cptr[i+2]);
      }
    }
  }

  void Serialize::getFloats(float *ptr, int count) {
    checkSignature('f',count);
    s->readBytes(ptr,count*sizeof(float));
    if (endianSwap) {
      char *cptr = (char *) ptr;
      char tmp;
      for (int i=0;i<4*count;i+=4) {
	SWAP(cptr[i],cptr[i+3]);
	SWAP(cptr[i+1],cptr[i+2]);
      }
    }
  }

  void Serialize::getDoubles(double *ptr, int count) {
    checkSignature('d',count);
    s->readBytes(ptr,count*sizeof(double));
    if (endianSwap) {
      char *cptr = (char *) ptr;
      char tmp;
      for (int i=0;i<8*count;i+=8) {
	SWAP(cptr[i],cptr[i+7]);
	SWAP(cptr[i+1],cptr[i+6]);
	SWAP(cptr[i+2],cptr[i+5]);
	SWAP(cptr[i+3],cptr[i+4]);
      }
    }
  }

  char* Serialize::getString() {
    checkSignature('x',0);
    unsigned int len;
    getInts((int*) &len,1);
    char *cp = (char*) malloc((len+1)*sizeof(char));
    getBytes(cp,len);
    cp[len] = 0;
    return cp;
  }

  char Serialize::getByte() {
    char t;
    getBytes(&t,1);
    return t;
  }

  short Serialize::getShort() {
    short t;
    getShorts(&t,1);
    return t;
  }

  int Serialize::getInt() {
    int t;
    getInts(&t,1);
    return t;
  }

  float Serialize::getFloat() {
    float t;
    getFloats(&t,1);
    return t;
  }

  double Serialize::getDouble() {
    double t;
    getDoubles(&t,1);
    return t;
  }

  Class Serialize::getDataClass() {
    checkSignature('a',1);
    char a = getByte();
    switch (a) {
    case 1:
      return FM_CELL_ARRAY;
    case 2:
      return FM_STRUCT_ARRAY;
    case 3:
      return FM_LOGICAL;
    case 4:
      return FM_UINT8;
    case 5:
      return FM_INT8;
    case 6:
      return FM_UINT16;
    case 7:
      return FM_INT16;
    case 8:
      return FM_UINT32;
    case 9:
      return FM_INT32;
    case 10:
      return FM_FLOAT;
    case 11:
      return FM_DOUBLE;
    case 12:
      return FM_COMPLEX;
    case 13:
      return FM_DCOMPLEX;
    case 14:
      return FM_STRING;
    default:
      throw Exception("Unrecognized array type received!");
    }
  }

  void Serialize::putDataClass(Class cls) {
    sendSignature('a',1);
    switch (cls) {
    case FM_CELL_ARRAY:
      putByte(1);
      return;
    case FM_STRUCT_ARRAY:
      putByte(2);
      return;
    case FM_LOGICAL:
      putByte(3);
      return;
    case FM_UINT8:
      putByte(4);
      return;
    case FM_INT8:
      putByte(5);
      return;
    case FM_UINT16:
      putByte(6);
      return;
    case FM_INT16:
      putByte(7);
      return;
    case FM_UINT32:
      putByte(8);
      return;
    case FM_INT32:
      putByte(9);
      return;
    case FM_FLOAT:
      putByte(10);
      return;
    case FM_DOUBLE:
      putByte(11);
      return;
    case FM_COMPLEX:
      putByte(12);
      return;
    case FM_DCOMPLEX:
      putByte(13);
      return;
    case FM_STRING:
      putByte(14);
      return;
    }
  }

  void Serialize::putDimensions(const Dimensions& dim) {
    sendSignature('D',1);
    putInt(dim.getLength());
    for (int i=0;i<dim.getLength();i++)
      putInt(dim.getDimensionLength(i));
  }

  Dimensions Serialize::getDimensions() {
    checkSignature('D',1);
    int len;
    len = getInt();
    Dimensions dim(len);
    for (int i=0;i<len;i++)
      dim.setDimensionLength(i,getInt());
    return dim;
  }

  void Serialize::putArray(const Array& dat) {
    sendSignature('A',1);
    Class dclass(dat.getDataClass());
    putDataClass(dclass);
    putDimensions(dat.getDimensions());
    int elCount(dat.getLength());
    if (dat.isEmpty()) return;
    switch(dclass) {
    case FM_CELL_ARRAY: {
      const Array *dp=((const Array *) dat.getDataPointer());
      for (int i=0;i<elCount;i++)
	putArray(dp[i]);
      return;
    }
    case FM_STRUCT_ARRAY: {
      stringVector fnames(dat.getFieldNames());
      int ncount(fnames.size());
      putInt(ncount);
	  int i;
      for (i=0;i<ncount;i++)
	putString(fnames[i].c_str());
      const Array *dp=((const Array *) dat.getDataPointer());
      for (i=0;i<elCount*ncount;i++)
	putArray(dp[i]);
      return;
    }
    case FM_LOGICAL: {
      const logical *dp=((const logical *)dat.getDataPointer());
      putBytes((const char*) dp,elCount);
      return;
    }
    case FM_STRING:
    case FM_UINT8: {
      const uint8 *dp=((const uint8 *)dat.getDataPointer());
      putBytes((const char*) dp,elCount);
      return;
    }
    case FM_UINT16: {
      const uint16 *dp=((const uint16 *)dat.getDataPointer());
      putShorts((const short*) dp,elCount);
      return;
    }
    case FM_UINT32: {
      const uint32 *dp=((const uint32 *)dat.getDataPointer());
      putInts((const int*) dp,elCount);
      return;
    }
    case FM_INT8: {
      const int8 *dp=((const int8 *)dat.getDataPointer());
      putBytes((const char*) dp,elCount);
      return;
    }
    case FM_INT16: {
      const int16 *dp=((const int16 *)dat.getDataPointer());
      putShorts((const short*) dp,elCount);
      return;
    }
    case FM_INT32: {
      const int32 *dp=((const int32 *)dat.getDataPointer());
      putInts((const int*) dp,elCount);
      return;
    }
    case FM_FLOAT: {
      const float *dp=((const float *)dat.getDataPointer());
      putFloats(dp,elCount);
      return;
    }
    case FM_DOUBLE: {
      const double *dp=((const double *)dat.getDataPointer());
      putDoubles(dp,elCount);
      return;
    }
    case FM_COMPLEX: {
      const float *dp=((const float *)dat.getDataPointer());
      putFloats(dp,elCount*2);
      return;
    }
    case FM_DCOMPLEX: {
      const double *dp=((const double *)dat.getDataPointer());
      putDoubles(dp,elCount*2);
      return;
    }
    }
  }

  void Serialize::getArray(Array& dat) {
    checkSignature('A',1);
    Class dclass(getDataClass());
    Dimensions dims(getDimensions());
    int elCount(dims.getElementCount());
    if (elCount == 0) {
      dat = Array(dclass,dims,NULL);
      return;
    }
    switch(dclass) {
    case FM_CELL_ARRAY: {
      Array *dp = new Array[elCount];
      for (int i=0;i<elCount;i++)
	getArray(dp[i]);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_STRUCT_ARRAY: {
      stringVector fnames;
      int ncount(getInt());
	  int i;
      for (i=0;i<ncount;i++) {
	char *dp = getString();
	fnames.push_back(dp);
	free(dp);
      }
      Array *dp = new Array[elCount*ncount];
      for (i=0;i<elCount*ncount;i++)
	getArray(dp[i]);
      dat = Array(dclass,dims,dp,fnames);
      return;
    }
    case FM_LOGICAL: {
      logical *dp = (logical*) Malloc(sizeof(logical)*elCount);
      getBytes((char*)dp, elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_STRING:
    case FM_UINT8: {
      uint8 *dp = (uint8*) Malloc(sizeof(uint8)*elCount);
      getBytes((char*)dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_INT8: {
      int8 *dp =  (int8*) Malloc(sizeof(int8)*elCount);
      getBytes((char*) dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_UINT16: {
      uint16 *dp = (uint16*) Malloc(sizeof(uint16)*elCount);
      getShorts((short*) dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_INT16: {
      int16 *dp = (int16*) Malloc(sizeof(int16)*elCount);
      getShorts((short*) dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_UINT32: {
      uint32 *dp = (uint32*) Malloc(sizeof(uint32)*elCount);
      getInts((int*) dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_INT32: {
      int32 *dp = (int32*) Malloc(sizeof(int32)*elCount);
      getInts((int*) dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_FLOAT: {
      float *dp =  (float*) Malloc(sizeof(float)*elCount);
      getFloats(dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_DOUBLE: {
      double *dp = (double*) Malloc(sizeof(double)*elCount);
      getDoubles(dp,elCount);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_COMPLEX: {
      float *dp = (float*) Malloc(sizeof(float)*elCount*2);
      getFloats(dp,elCount*2);
      dat = Array(dclass,dims,dp);
      return;
    }
    case FM_DCOMPLEX: {
      double *dp = (double*) Malloc(sizeof(double)*elCount*2);
      getDoubles(dp,elCount*2);
      dat = Array(dclass,dims,dp);
      return;
    }
    }
  }
}

			      
			      
