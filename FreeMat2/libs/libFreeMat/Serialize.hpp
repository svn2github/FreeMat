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

#ifndef __Serialize_hpp__
#define __Serialize_hpp__

#include "Stream.hpp"
#include "Array.hpp"

// This is the cross-platform serialization object...

namespace FreeMat {
  class Serialize {
    Stream *s;
    bool endianSwap;
  public:
    Serialize(Stream*);
    ~Serialize();
    // Handshake between two serialize objects
    void handshakeServer();
    void handshakeClient();
    // Send a signature
    void sendSignature(char sig, int count);
    // Check a signature
    void checkSignature(char sig, int count);
    // Send a stream of base objects
    void putBytes(const char* ptr, int count);
    void putShorts(const short* ptr, int count);
    void putInts(const int* ptr, int count);
    void putFloats(const float* ptr, int count);
    void putDoubles(const double* ptr, int count);
    // Send a scalar object
    void putString(const char* ptr);
    void putByte(char t);
    void putShort(short t);
    void putInt(int t);
    void putFloat(float t);
    void putDouble(double t);
    void putBool(bool t);
    void putStringVector(stringVector t);
    // Receive a stream of base objects
    void getBytes(char* ptr, int count);
    void getShorts(short* ptr, int count);
    void getInts(int* ptr, int count);
    void getFloats(float* ptr, int count);
    void getDoubles(double* ptr, int count);
    // Get a scalar object
    char*  getString();
    char   getByte();
    short  getShort();
    int    getInt();
    float  getFloat();
    double getDouble();
    bool   getBool();
    stringVector getStringVector();
    // Put an array
    void putDataClass(Class cls, bool sparseflag);
    void putArray(const Array& dat);
    void putDimensions(const Dimensions& dim);
    // Get an array
    Class getDataClass(bool& sparseflag);
    void getArray(Array& dat);
    Dimensions getDimensions();
  };
}

#endif
