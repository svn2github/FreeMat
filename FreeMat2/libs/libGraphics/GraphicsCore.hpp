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

#ifndef __GraphicsCore_hpp__
#define __GraphicsCore_hpp__

#include "Array.hpp"
#include "Context.hpp"

namespace FreeMat {

#define MAX_GFX 50
  ArrayVector FigureFunction(int, const ArrayVector& arg);
  ArrayVector CloseFunction(int, const ArrayVector& arg);
  ArrayVector PrintFunction(int,const ArrayVector& arg);
  ArrayVector PlotFunction(int,const ArrayVector& arg);
  ArrayVector ImageFunction(int,const ArrayVector& arg);
  ArrayVector SizeFigFunction(int,const ArrayVector& arg);
  ArrayVector VolumeFunction(int,const ArrayVector& arg);
  ArrayVector XLabelFunction(int,const ArrayVector& arg);
  ArrayVector YLabelFunction(int,const ArrayVector& arg);
  ArrayVector TitleFunction(int,const ArrayVector& arg);
  ArrayVector GridFunction(int,const ArrayVector& arg);
  ArrayVector HoldFunction(int,const ArrayVector& arg);
  ArrayVector TicFunction(int,const ArrayVector& arg);
  ArrayVector TocFunction(int,const ArrayVector& arg);
  ArrayVector AxisFunction(int,const ArrayVector& arg);
  ArrayVector ColormapFunction(int,const ArrayVector& arg);
  ArrayVector ZoomFunction(int,const ArrayVector& arg);
  ArrayVector PickFileFunction(int,const ArrayVector& arg);
  ArrayVector PointFunction(int,const ArrayVector& arg);
  ArrayVector HelpwinFunction(int,const ArrayVector& arg);
  ArrayVector WinLevFunction(int,const ArrayVector& arg);
  ArrayVector LegendFunction(int, const ArrayVector& arg);
  ArrayVector DemoFunction(int, const ArrayVector& arg);
  void LoadGraphicsCoreFunctions(Context* context);
  void InitializeFigureSubsystem();
}

#endif
