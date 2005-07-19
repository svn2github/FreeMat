// Copyright (c) 2005 Samit Basu
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

#ifndef __QTDraw_hpp__
#define __QTDraw_hpp__

#include "XPWidget.hpp"
#include "FunctionDef.hpp"
#include "WalkTree.hpp"

namespace FreeMat {
  // This class allows for user-defined drawing functions.
  // It works through a callback.
  class QTDraw : public XPWidget {
    FunctionDef* m_fdef;
    WalkTree* m_tree;
  public:
    void SetWalkTree(WalkTree* tree);
    void SetCallback(FunctionDef* fdef);
    void OnDraw(GraphicsContext &gc);
  };
  
  ArrayVector GetCanvasSizeFunction(int nargout, const ArrayVector& arg);
  ArrayVector GetTextExtentFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawTextStringAlignedFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawTextStringFunction(int nargout, const ArrayVector& arg);
  ArrayVector SetFontFunction(int nargout, const ArrayVector& arg);
  ArrayVector SetForeGroundColorFunction(int nargout, const ArrayVector& arg);
  ArrayVector SetLineStyleFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawLineFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawPointFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawCircleFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawRectangleFunction(int nargout, const ArrayVector& arg);
  ArrayVector FillRectangleFunction(int nargout, const ArrayVector& arg);
  ArrayVector FillQuadFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawQuadFunction(int nargout, const ArrayVector& arg);
  ArrayVector DrawLinesFunction(int nargout, const ArrayVector& arg);
  ArrayVector SetClipFunction(int nargout, const ArrayVector& arg);
  ArrayVector BlitImageFunction(int nargout, const ArrayVector& arg);
}

#endif
