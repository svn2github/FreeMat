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

#include "ScalarImage.hpp"
#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"

namespace FreeMat {

#define MAX_IMAGES 50

  ScalarImage* images[MAX_IMAGES];
  int currentImage;

  void CloseImageHelper(int fig) {
    if (fig == -1) return;
    if (images[fig] == NULL) return;
    images[fig]->Close();
    images[fig] = NULL;
    if (currentImage == fig)
      currentImage = -1;
  }
    
  void InitializeImageSubsystem() {
    currentImage = -1;
    for (int i=0;i<MAX_IMAGES;i++) 
      images[i] = NULL;
  }
  
  void NewImage() {
    // First search for an unused image number
    int imageNum = 0;
    bool imageFree = false;
    while ((imageNum < MAX_IMAGES) && !imageFree) {
      imageFree = (images[imageNum] == NULL);
      if (!imageFree) imageNum++;
    }
    if (!imageFree) {
      throw Exception("No more image handles available!  Close some images...");
    }
    images[imageNum] = new ScalarImage(imageNum);
    images[imageNum]->Show();
    images[imageNum]->Raise();
    currentImage = imageNum;
  }

  void SelectImage(int imagenum) {
    if (images[imagenum] == NULL) {
      images[imagenum] = new ScalarImage(imagenum);
    }
    images[imagenum]->Show();
    images[imagenum]->Raise();
    currentImage = imagenum;
  } 

  ScalarImage* GetCurrentImage() {
    if (currentImage == -1)
      NewImage();
    images[currentImage]->Raise();
    return images[currentImage];
  }

  void NotifyImageClose(int figNum) {
    images[figNum] = NULL;
    if (currentImage == figNum)
      currentImage = -1;
  }

  ArrayVector NewImageFunction(int nargout,const ArrayVector& arg) {  
    NewImage();
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(currentImage+1));
    return retval;
  }

  ArrayVector UseImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("useimage function takes a single, integer argument");
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_GFX))
      throw Exception("image number is out of range - it must be between 1 and 50");
    SelectImage(fignum-1);
    return ArrayVector();
  }

  ArrayVector CloseImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("closeimage takes at most one argument - either the string 'all' to close all images, or a scalar integer indicating which image is to be closed.");
    int action;
    if (arg.size() == 0) 
      action = 0;
    else {
      Array t(arg[0]);
      if (t.isString()) {
	char *allflag = t.getContentsAsCString();
	if (strcmp(allflag,"all") == 0) 
	  action = -1;
	else
	  throw Exception("string argument to closeimage function must be 'all'");
      } else {
	int handle = t.getContentsAsIntegerScalar();
	if (handle < 1)
	  throw Exception("Invalid image number argument to closeimage function");
	action = handle;
      }
    }
    if (action == 0) {
      if (currentImage != -1) 
	CloseImageHelper(currentImage);
    } else if (action == -1) {
      for (int i=0;i<MAX_IMAGES;i++)
	CloseImageHelper(i);
    } else {
      if ((action < MAX_IMAGES) && (action >= 1))
	CloseImageHelper(action-1);
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
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetColormap(t);
    return ArrayVector();
  }

  ArrayVector SizeImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("sizeimage function takes two arguments: height and width");
    Array w(arg[1]);
    Array h(arg[0]);
    int width;
    int height;
    width = w.getContentsAsIntegerScalar();
    height = h.getContentsAsIntegerScalar();
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetSize(width,height);
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
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetImageArray(img);
    return ArrayVector();
  }

  ArrayVector PrintImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("printimage function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("printimage function takes a single, string argument");
    Array t(arg[0]);
    ScalarImage* f = GetCurrentImage();
    f->PrintMe(t.getContentsAsCString());
    return ArrayVector();
  }

  ArrayVector ZoomFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Zoom function takes a single, real argument.");
    Array fact(arg[0]);
    fact.promoteType(FM_FLOAT);
    ScalarImage *f;
    f = GetCurrentImage();
    f->Zoom(fact.getContentsAsDoubleScalar());
    return ArrayVector();
  }
  
  ArrayVector PointFunction(int nargout, const ArrayVector& arg) {
    ScalarImage *f;
    f = GetCurrentImage();
    ArrayVector retval;
    retval.push_back(f->GetPoint());
    return retval;
  }
}
