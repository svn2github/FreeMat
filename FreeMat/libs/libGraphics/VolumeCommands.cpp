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

#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "CLIThread.hpp"

#include <wx/timer.h>
#include <wx/image.h>

namespace FreeMat {
  ArrayVector NewVolumeFunction(int nargout,const ArrayVector& arg) {  
    SendGUICommand(new Command(CMD_VolumeNew));
    Command *reply = GetGUIResponse();
    ArrayVector retval;
    retval.push_back(reply->data);
    delete reply;
    return retval;
  }

  ArrayVector UseVolumeFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("usevolume function takes a single, integer argument");
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_GFX))
      throw Exception("volume number is out of range - it must be between 1 and 50");
    SendGUICommand(new 
		   Command(CMD_VolumeUse,Array::int32Constructor(fignum-1)));
    return ArrayVector();
  }

  ArrayVector CloseVolumeFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("closevolume takes at most one argument - either the string 'all' to close all volumes, or a scalar integer indicating which volume is to be closed.");
    if (arg.size() == 0) {
      SendGUICommand(new
		     Command(CMD_VolumeClose,Array::int32Constructor(0)));
      return ArrayVector();
    } else {
      Array t(arg[0]);
      if (t.isString()) {
	char *allflag = t.getContentsAsCString();
	if (strcmp(allflag,"all") == 0) {
	  SendGUICommand(new 
			 Command(CMD_VolumeClose,Array::int32Constructor(-1)));
	  return ArrayVector();
	} else
	  throw Exception("string argument to closevolume function must be 'all'");
      }
      int handle = t.getContentsAsIntegerScalar();
      if (handle < 1)
	throw Exception("Invalid volume number argument to closevolume function");
      SendGUICommand(new
		     Command(CMD_VolumeClose,Array::int32Constructor(handle)));
    }
    return ArrayVector();
  }

  ArrayVector VolumeFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("volume function takes a single argument");
    Array img(arg[0]);
    Dimensions dim(img.getDimensions());
    dim.simplify();
    if (dim.getLength() != 3)
      throw Exception("argument to volume function must be 3D");
    if (img.isReferenceType())
      throw Exception("argument to volume function must be a data type");
    if (img.isComplex())
      throw Exception("argument to volume function must be real");
    img.promoteType(FM_UINT8);
    SendGUICommand(new Command(CMD_VolumeSet,img));
    return ArrayVector();
  }
}
