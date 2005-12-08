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
#include "Context.hpp"
#include "HandleAxis.hpp"

namespace FreeMat {

  void LoadGraphicsCoreFunctions(Context* context) {
    LoadHandleGraphicsFunctions(context);
    //    context->addFunction("figure",FigureFunction,1,1,"n");
    context->addFunction("close",CloseFunction,1,0,"handle");
    //    context->addFunction("plot",PlotFunction,-1,0);
    context->addFunction("sizefig",SizeFigFunction,2,0,"height","width");
    //    context->addFunction("xlabel",XLabelFunction,1,0,"label");
    //    context->addFunction("ylabel",YLabelFunction,1,0,"label");
    //    context->addFunction("title",TitleFunction,1,0,"label"); 
    //    context->addFunction("grid",GridFunction,1,0,"state");
    //    context->addFunction("hold",HoldFunction,1,0,"state");
    context->addFunction("ishold",IsHoldFunction,0,1);
    //    context->addFunction("image",ImageFunction,2,0,"x","zoom");
    context->addFunction("winlev",WinLevFunction,2,-1,"window","level");
    context->addFunction("colormap",ColormapFunction,1,0,"map");
    context->addFunction("axis",AxisFunction,1,-1,"x");
    context->addFunction("print",PrintFunction,1,0,"filename");
    context->addFunction("zoom",ZoomFunction,1,0,"x");
    context->addFunction("point",PointFunction,0,1);
    context->addFunction("legend",LegendFunction,-1,0);
    context->addFunction("demo",DemoFunction,-1,0);
    context->addFunction("copy",CopyFunction,0,0);
    //    context->addFunction("addtxt",AddTxtFunction,3,1,"x","y","label");
    //    context->addFunction("subplot",SubPlotFunction,3,0,"x","y","n");
    //    context->addFunction("colorbar",ColorbarFunction,1,0,"orientation");
    //    context->addFunction("clf",ClearFigureFunction,0,0);
  }
}
