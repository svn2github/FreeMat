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

#ifndef __CLIBuffer_h__
#define __CLIBuffer_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class CLIBuffer
{
 public:
  /**
   * Allocate a buffer with a maximum length.
   */
  CLIBuffer(int maxLength);
  
  /** 
   * Destructor.
   */
  ~CLIBuffer();

  /**
   * Insert a character into our buffer
   */
  void InsertChar(long position, wxChar value);
  /**
   * Append a character to our buffer
   */
  void AppendChar(wxChar value);
  /**
   * Reset the buffer to be empty.
   */
  void Reset();
  /**
   * Delete a designated character from the buffer.
   */
  void DeleteChar(long ndx);
  /**
   * Get the contents of the buffer as a wxString.
   */
  wxString GetContents();
  /**
   * Set the contents of the buffer from a wxString.
   */
  void SetContents(wxString val);
  /**
   * Get the current buffer length
   */
  const int Length();

 private:
  char *buffer;
  int bufferLength;
};

#endif
