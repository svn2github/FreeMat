/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Context.hpp"
#include "HandleAxis.hpp"

namespace FreeMat {

  void LoadGraphicsCoreFunctions(Context* context) {
    LoadHandleGraphicsFunctions(context);
    //    context->addFunction("figure",FigureFunction,1,1,"n");
    //    context->addFunction("close",CloseFunction,1,0,"handle");
    //    context->addFunction("plot",PlotFunction,-1,0);
    //    context->addFunction("sizefig",SizeFigFunction,2,0,"height","width");
    //    context->addFunction("xlabel",XLabelFunction,1,0,"label");
    //    context->addFunction("ylabel",YLabelFunction,1,0,"label");
    //    context->addFunction("title",TitleFunction,1,0,"label"); 
    //    context->addFunction("grid",GridFunction,1,0,"state");
    //    context->addFunction("hold",HoldFunction,1,0,"state");
    //    context->addFunction("ishold",IsHoldFunction,0,1);
    //    context->addFunction("image",ImageFunction,2,0,"x","zoom");
    //    context->addFunction("winlev",WinLevFunction,2,-1,"window","level");
    //    context->addFunction("colormap",ColormapFunction,1,0,"map");
    //    context->addFunction("axis",AxisFunction,1,-1,"x");
    //    context->addFunction("print",PrintFunction,1,0,"filename");
    //    context->addFunction("zoom",ZoomFunction,1,0,"x");
    //    context->addFunction("point",PointFunction,0,1);
    //    context->addFunction("legend",LegendFunction,-1,0);
    //    context->addFunction("demo",DemoFunction,-1,0);
    //    context->addFunction("copy",CopyFunction,0,0);
    //    context->addFunction("addtxt",AddTxtFunction,3,1,"x","y","label");
    //    context->addFunction("subplot",SubPlotFunction,3,0,"x","y","n");
    //    context->addFunction("colorbar",ColorbarFunction,1,0,"orientation");
    //    context->addFunction("clf",ClearFigureFunction,0,0);
  }
}
