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

#include "CLIBuffer.hpp"

CLIBuffer::CLIBuffer(int maxLength) {
  buffer = new char[maxLength];
}

CLIBuffer::~CLIBuffer() {
  delete(buffer);
}

void CLIBuffer::DeleteChar(long pos) {
  int i;
  for (i=pos+1;i<bufferLength;i++)
    buffer[i-1] = buffer[i];
  bufferLength--;
}

void CLIBuffer::AppendChar(wxChar val) {
  buffer[bufferLength] = val;
  bufferLength++;
}

void CLIBuffer::InsertChar(long pos, wxChar val) {
  int i;
  for (i=bufferLength-1;i>=pos;i--)
    buffer[i+1] = buffer[i];
  buffer[pos] = val;
  bufferLength++;
}

void CLIBuffer::Reset() {
  bufferLength = 0;
}

wxString CLIBuffer::GetContents() {
  return wxString(buffer,bufferLength);
}

void CLIBuffer::SetContents(wxString val) {
  strcpy(buffer,val.c_str());
  bufferLength = val.Length();
}

const int CLIBuffer::Length() {
  return bufferLength;
}
