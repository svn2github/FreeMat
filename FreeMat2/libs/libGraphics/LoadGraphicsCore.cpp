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

namespace FreeMat {

  void LoadGraphicsCoreFunctions(Context* context) {
    stringVector args;
    context->addFunction("figure",FigureFunction,1,1,args);
    args.clear();
    args.push_back("handle");
    context->addFunction("close",CloseFunction,1,0,args);
    args.clear();
    context->addFunction("plot",PlotFunction,-1,0,args);
    args.clear();
    args.push_back("height");
    args.push_back("width");
    context->addFunction("sizefig",SizeFigFunction,2,0,args);
    args.clear();
    args.push_back("label");    
    context->addFunction("xlabel",XLabelFunction,1,0,args);
    context->addFunction("ylabel",YLabelFunction,1,0,args);
    context->addFunction("title",TitleFunction,1,0,args); 
    args.clear();
    args.push_back("state");        
    context->addFunction("grid",GridFunction,1,0,args);
    context->addFunction("hold",HoldFunction,1,0,args);
    args.clear();
    args.push_back("x");        
    args.push_back("zoom");        
    context->addFunction("image",ImageFunction,2,0,args);
    args.clear();
    args.push_back("window");
    args.push_back("level");
    context->addFunction("winlev",WinLevFunction,2,-1,args);
    args.clear();
    args.push_back("map");        
    context->addFunction("colormap",ColormapFunction,1,0,args);
    args.clear();
    context->addFunction("axis",AxisFunction,1,-1,args);
    args.clear();
    args.push_back("filename");    
    context->addFunction("print",PrintFunction,1,0,args);
    args.clear();
    args.push_back("x");    
    context->addFunction("zoom",ZoomFunction,1,0,args);
    args.clear();
    args.push_back("title");        
    //    context->addFunction("pickfile",PickFileFunction,1,1,args);
    args.clear();
    context->addFunction("point",PointFunction,0,1,args);
    context->addFunction("legend",LegendFunction,-1,0,args);
    context->addFunction("demo",DemoFunction,0,0,args);
    //    context->addFunction("helpwin",HelpwinFunction,0,0,args);
  }
}
