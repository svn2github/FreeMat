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
  ArrayVector NewImageFunction(int nargout,const ArrayVector& arg) {  
    SendGUICommand(new Command(CMD_ImageNew));
    Command *reply = GetGUIResponse();
    ArrayVector retval;
    retval.push_back(reply->data);
    delete reply;
    return retval;
  }

  ArrayVector UseImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("useimage function takes a single, integer argument");
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_GFX))
      throw Exception("image number is out of range - it must be between 1 and 50");
    SendGUICommand(new 
		   Command(CMD_ImageUse,Array::int32Constructor(fignum-1)));
    return ArrayVector();
  }

  ArrayVector CloseImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("closeimage takes at most one argument - either the string 'all' to close all images, or a scalar integer indicating which image is to be closed.");
    if (arg.size() == 0) {
      SendGUICommand(new
		     Command(CMD_ImageClose,Array::int32Constructor(0)));
      return ArrayVector();
    } else {
      Array t(arg[0]);
      if (t.isString()) {
	char *allflag = t.getContentsAsCString();
	if (strcmp(allflag,"all") == 0) {
	  SendGUICommand(new 
			 Command(CMD_ImageClose,Array::int32Constructor(-1)));
	  return ArrayVector();
	} else
	  throw Exception("string argument to closeimage function must be 'all'");
      }
      int handle = t.getContentsAsIntegerScalar();
      if (handle < 1)
	throw Exception("Invalid image number argument to closeimage function");
      SendGUICommand(new
		     Command(CMD_ImageClose,Array::int32Constructor(handle)));
    }
    return ArrayVector();
  }

  ArrayVector ColormapFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("colormap takes a single argument");
    Array t(arg[0]);
    t.promoteType(FM_DOUBLE);
    if (t.getLength() != 256*3)
      throw Exception("Colormap argument must have exactly 768 elements");
    SendGUICommand(new Command(CMD_ImageColormap,t));
    return ArrayVector();
  }

  ArrayVector ImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("image function takes a single argument");
    Array img;
    img = arg[0];
    if (!img.is2D())
      throw Exception("argument to image function must be 2D");
    if (img.isReferenceType())
      throw Exception("argument to image function must be a data type");
    if (img.isComplex())
      throw Exception("argument to image function must be real");
    img.promoteType(FM_DOUBLE);
    SendGUICommand(new Command(CMD_ImageSet,img));
    return ArrayVector();
  }

  ArrayVector PrintImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("printimage function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("printimage function takes a single, string argument");
    SendGUICommand(new Command(CMD_ImagePrint,
			       arg[0]));
    return ArrayVector();
  }

  ArrayVector ZoomFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Zoom function takes a single, real argument.");
    Array fact(arg[0]);
    fact.promoteType(FM_FLOAT);
    SendGUICommand(new Command(CMD_ImageZoom,fact));
    return ArrayVector();
  }
  
  ArrayVector PointFunction(int nargout, const ArrayVector& arg) {
    SendGUICommand(new Command(CMD_Point));
    Command *reply = GetGUIResponse();
    ArrayVector retval;
    retval.push_back(reply->data);
    delete reply;
    return retval;
  }
}
