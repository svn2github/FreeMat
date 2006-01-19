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

#include "Plot2D.hpp"
#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "Core.hpp"
#include "Figure.hpp"
#include "Label.hpp"
#include "Util.hpp"
#include <QWidget>

namespace FreeMat {
  Plot2D* GetCurrentPlot() {
    Figure* fig = GetCurrentFig();
    if (fig->getType() == figplot) {
      QWidget *w = (QWidget *) fig->GetChildWidget();
      const QObjectList children = w->children();
      for (int i = 0; i < children.size(); ++i) {
	Plot2D* p = dynamic_cast<Plot2D*>(children.at(i));
	if (p)
	  return p;
      }
    }
    QWidget *w = new QWidget(fig);
    QGridLayout *l = new QGridLayout(w);
    Plot2D* p = new Plot2D(w);
    l->addWidget(p,1,1);
    l->setColumnStretch(1,1);
    l->setRowStretch(1,1);
    fig->SetFigureChild(w,figplot);
    return p;
  }
  
  static char *colors = "rgbkcmy";
  static int colornumber;
  static char *styles = "-:;|";
  static char *symbols = ".ox+*sdv^<>ph";

  void ResetLineColors() {
    colornumber = 0;
  }

  char StringMatch(char* options, char* choices) {
    char *cp, *rp;
    if (!options)
      return 0;
    cp = options;
    while (*cp != 0) {
      rp = strchr(choices,*cp);
      if (rp) return *rp;
      cp++;
    }
    return 0;
  }

  char* GetLineStyle(char* arg) {
    static char outStyle[4];
    char color = 0;
    char style = 0;
    char symbol = 0;
    color = StringMatch(arg,colors);
    style = StringMatch(arg,styles);
    symbol = StringMatch(arg,symbols);

    if ((style == 0) && (symbol == 0))
      style = '-';
    if (color == 0) {
      color = colors[colornumber];
      colornumber++;
      colornumber %= strlen(colors);
    }
    if (symbol == 0)
      symbol = ' ';
    if (style == 0)
      style = ' ';

    outStyle[0] = color;
    outStyle[1] = symbol;
    outStyle[2] = style;
    outStyle[3] = 0;
    return outStyle;
  }


  ArrayVector LegendFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 5)
      throw Exception("legend requires at least 5 arguments to be useful");
    Array x(arg[0]);
    Array y(arg[1]);
    Array legendLS(arg[2]);
    if (!legendLS.isString())
      throw Exception("linestyle for legend box is invalid");
    char *cp = legendLS.getContentsAsCString();
    char *boxstyle;
    if (strlen(cp) == 0) 
      boxstyle = "   ";
    else 
      boxstyle = GetLineStyle(legendLS.getContentsAsCString());
    legendLS = Array::stringConstructor(boxstyle);
    Plot2D* t;
    t = GetCurrentPlot();
    ArrayVector legendData;
    if (arg.size()%2 == 0)
      throw Exception("legend requires matched pairs of line styles and labels");
    int i;
    for (i=3;i<arg.size();i+=2) {
      Array argLS(arg[i]);
      char *linestyle = GetLineStyle(argLS.getContentsAsCString());
      legendData.push_back(Array::stringConstructor(linestyle));
      if (!arg[i+1].isString())
	throw Exception("label arguments to legend function must be of type string");
      legendData.push_back(arg[i+1]);
    }
    double xcorner;
    double ycorner;
    xcorner = x.getContentsAsDoubleScalar();
    ycorner = y.getContentsAsDoubleScalar();
    t->SetLegend(xcorner, ycorner, legendLS, legendData);
    t->repaint();
    return ArrayVector();
  }

  ArrayVector GridFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Grid function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("Grid function takes only a single, string argument");
    char *txt;
    txt = arg[0].getContentsAsCString();
    Plot2D* f = GetCurrentPlot();
    if (strcmp(txt,"on") == 0)
      f->SetGrid(true);
    else if (strcmp(txt,"off") == 0)
      f->SetGrid(false);
    else
      throw Exception("Grid function argument needs to be 'on/off'");
    f->repaint();
    return ArrayVector();
  }

  ArrayVector HoldFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Hold function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("Hold function takes only a single, string argument");
    char *txt;
    txt = arg[0].getContentsAsCString();
    Plot2D* f = GetCurrentPlot();
    if (strcmp(txt,"on") == 0)
      f->SetHoldFlag(true);
    else if (strcmp(txt,"off") == 0)
      f->SetHoldFlag(false);
    else
      throw Exception("Hold function argument needs to be 'on/off'");
    return ArrayVector();  
  }

  ArrayVector IsHoldFunction(int nargout, const ArrayVector& arg) {
    Plot2D* f = GetCurrentPlot();
    return singleArrayVector(Array::logicalConstructor(f->GetHoldFlag()));
  }

  Array GetRealPart(Array& z) {
    ArrayVector tmp;
    tmp.push_back(z);
    ArrayVector realPart(RealFunction(1,tmp));
    return realPart[0];
  }

  Array GetImagPart(Array& z) {
    ArrayVector tmp;
    tmp.push_back(z);
    ArrayVector imagPart(ImagFunction(1,tmp));
    return imagPart[0];
  } 

  Array ComposeDataLineStyle(Array& x, Array& y, char* linestyle) {
    ArrayVector srow;
    srow.push_back(x);
    srow.push_back(y);
    srow.push_back(Array::stringConstructor(linestyle));
    ArrayMatrix smat;
    smat.push_back(srow);
    return(Array::cellConstructor(smat));
  }


  // The plot command - 
  //   A plot command consists of a sequence of plot triplets.
  //   A plot triplet may be of the form:
  //     Z - a complex matrix, in which case the real part is plotted against
  //         the complex part - one plot for each column of Z
  //     x,Y - a vector-matrix pair in which case each column of Y
  //           is plotted against the same vector x
  //   combined with an optional linestyle string.  The plot function
  //   must parse out the arguments to detemine which of these triplets
  //   are being used in each case.
  //
  //   The decision tree for the plot command is fairly simple:
  //    First arg is complex? - case 2
  //    Else - case 1/3 (combined because they are the same)
  //    Then, the next token is checked for a string.  If it is
  //    a string, then a linestyle is extracted.  Otherwise,
  //    a default linestyle is generated.
  ArrayVector PlotFunction(int nargout,const ArrayVector& arg) {
    char *linestyle;
    char *linestyle_arg;
    // Check for zero arguments
    if (arg.size() == 0)
      throw Exception("Not enough inputs to plot function");
    // Make sure all arguments are 2D and numeric types
    bool all2D = true;
    for (int i=0;i<arg.size();i++)
      all2D &= arg[i].is2D();
    if (!all2D)
      throw Exception("Plot does not handle N-ary arrays (one or more arguments supplied were not 1-D or 2-D).");
    int argptr = 0;
    // Start the plot 
    Array x, y, sliceY, sliceX, Z;
    // Reset the color generator
    ResetLineColors();
    Plot2D* f = GetCurrentPlot();
    f->StartSequence();
    try {
      while (argptr < arg.size()) {
	if (arg[argptr].isComplex()) {
	  Z = arg[argptr];
	  // Get the real part into x;
 	  x = GetRealPart(Z);
	  x.promoteType(FM_DOUBLE);
 	  // Get the imaginary part into y;
 	  y = GetImagPart(Z);
	  y.promoteType(FM_DOUBLE);
	  argptr++;
	  // Check for a linestyle
	  if (argptr < arg.size()) {
	    if (arg[argptr].isString()) {
	      linestyle_arg = arg[argptr].getContentsAsCString();
	      argptr++;
	    }
	  } else
	    linestyle_arg = NULL;
	  // Slice up x and y into columns...
	  int columns = y.getDimensionLength(1);
	  int rows = y.getDimensionLength(0);
	  if ((columns != 1) && (rows != 1)) {
	    for (int i=0;i<columns;i++) {
	      Array ndx;
	      ndx = Array::int32RangeConstructor(1+i*rows,1,rows+i*rows,true);
	      sliceY = y.getVectorSubset(ndx);
	      sliceX = x.getVectorSubset(ndx);
	      linestyle = GetLineStyle(linestyle_arg);
	      f->AddPlot(DataSet2D(sliceX,sliceY,linestyle[0],
				   linestyle[1],linestyle[2]));
	    }
	  } else {
	    linestyle = GetLineStyle(linestyle_arg);
	    f->AddPlot(DataSet2D(x,y,linestyle[0],linestyle[1],linestyle[2])); 
	  }
	} else {
	  x = arg[argptr];
	  argptr++;
	  // There are two tests for an unpaired-vector.  The first is that we are the last
	  // element in the argument array.  The second is that the next argument is a line 
	  // style
	  if ((argptr >= arg.size()) || (arg[argptr].isString())) {
	    // Unpaired vector - x is really y...
	    y = x;
	    // We have to synthesize x.  If y is a vector, we just set x to 
	    // be a "linear" array 
	    if (y.isVector())
	      x = Array::int32RangeConstructor(1,1,y.getLength(),true);
	    else
	      x = Array::int32RangeConstructor(1,1,y.getDimensionLength(0),true);
	  } else {
	    y = arg[argptr];
	    argptr++;
	  }
	  // Make sure x and y are real and numeric
	  if (!(x.isReal() && y.isReal()))
	    throw Exception("Unrecognized combination of complex and real arguments to 'plot' command.");
	  // Make sure x is a vector
	  if (!(x.isVector()))
	    throw Exception("Plotting argument pair x,Y requires x to be a vector.");
	  // Make sure y is a matrix
	  if (!(y.is2D()))
	    throw Exception("Plotting argument pair x,Y requires Y to be a matrix.");
	  // Make sure the length x = y.rows
	  if (x.getLength() != y.getDimensionLength(0) && !y.isVector())
	    throw Exception("Plotting argument pair x,Y requires Y to have as many rows as x has elements.");
	  x.promoteType(FM_DOUBLE);
	  y.promoteType(FM_DOUBLE);
	  // Loop over the number of columns in y (unless its a vector, in 
	  // which case there is only one plot
	  linestyle_arg = NULL;
	  if (argptr < arg.size())
	    if (arg[argptr].isString()) {
	      linestyle_arg = arg[argptr].getContentsAsCString();
	      argptr++;
	    } 
	  if (y.isEmpty())
	    throw Exception("Plotting of empty vectors is not defined.");
	  if (y.isVector()) {
	    if (y.getLength() != x.getLength())
	      throw Exception("Plotting argument pair x,y requires y to be the same length as x.");
	    linestyle = GetLineStyle(linestyle_arg);
	    // Send off the command
	    f->AddPlot(DataSet2D(x,y,linestyle[0],linestyle[1],linestyle[2]));
	  } else {
	    int columns = y.getDimensionLength(1);
	    int rows = y.getDimensionLength(0);
	    for (int i=0;i<columns;i++) {
	      Array ndx;
	      ndx = Array::int32RangeConstructor(1+i*rows,1,rows+i*rows,true);
	      sliceY = y.getVectorSubset(ndx);
	      linestyle = GetLineStyle(linestyle_arg);
	      f->AddPlot(DataSet2D(x,sliceY,linestyle[0],linestyle[1],linestyle[2]));
	    }
	  }
	}
      }
      f->StopSequence();
    } catch(Exception &e) {
      f->StopSequence();
      throw e;
    }
    f->repaint();
    // Stop the plot 
    return ArrayVector();
  }
  
  ArrayVector AddTxtFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("addtxt function takes three arguments, x, y and text label");
    Plot2D* f = GetCurrentPlot();
    Array xval(arg[0]);
    Array yval(arg[1]);
    Array label(arg[2]);
    f->AddText(xval.getContentsAsDoubleScalar(),
	       yval.getContentsAsDoubleScalar(),
	       label.getContentsAsCString());
    f->repaint();
    return ArrayVector();
  }

  //@Module AXIS Plot Axis Set/Get Function
  //@@Section PLOT
  //@@Usage
  //Changes the axis configuration for the currently active plot,
  //or returns the currently active limits for the axis,  The
  //general syntax for its use is either
  //@[
  //   [x1,x2,y1,y2] = axis
  //@]
  //to get the current axis limits, or
  //@[
  //   axis([x1,x2,y1,y2])
  //@]
  //to set the axis limits.  There are also two additional @|axis|
  //commands:
  //@[
  //   axis('tight')
  //@]
  //which sets the axis boundaries to be tight as possible, and
  //@[
  //   axis('auto')
  //@]
  //which uses a heuristic algorithm to choose a ``reasonable'' set of
  //axis limits.
  //@@Function Internals
  //The @|axis| command is used to change the ranges of the @|x|
  //and @|y| axes on the current plot.  This permits ``zooming'' of
  //plots.  By default, when a @|plot| command is issued, a heuristic
  //algorithm adjusts the ranges on the @|x| and @|y| axes so that
  //the increments on the axes are ``reasonable'' values.  It also adjusts
  //the start and stop values on each axis (by enlarging the range and
  //domain of the plot).  You can reset a plot to this state using the 
  //@|'auto'| argument to the axis.  
  //
  //Another option is to choose the axes so that they tightly fit the
  //domain and range of the current datasets.  This is accomplished
  //using the @|'tight'| argument to the axis command.  It will
  //set the axes to @|[x_min,x_max,y_min,y_max]|, where
  //@|x_min| is the minimum @|x| value over all datasets in the current
  //plot series, etc.
  //
  //You can also retrieve the current ranges of the plot axes by issuing
  //an @|axis| command with no arguments.
  //@@Example
  //We start by plotting a sinusoid of amplitude @|sqrt(2)| over the 
  //range @|-pi,pi|, which is not a ``nice'' range, and thus the
  //auto axis heuristic shrinks the plot to make the range nicer.
  //@<
  //x = linspace(-pi,pi);
  //y = sqrt(2)*sin(3*x);
  //plot(x,y,'r-');
  //grid on
  //mprint axis1
  //@>
  //@figure axis1
  //
  //Suppose we now want to make the axis fit the plot exactly.  We can issue an
  //@|axis('tight')| command, which results in the following plot.
  //@<
  //plot(x,y,'r-');
  //grid on
  //axis tight
  //mprint axis2
  //@>
  //@figure axis2
  //
  //We can now use the @|axis| command to retrieve the current axis values.  By
  //modifying only the first two entries (@|x_min| and @|x_max|), we can
  //zoom in on one period of the sinusoid.
  //@<
  //plot(x,y,'r-');
  //grid on;
  //a = axis
  //a(1) = -pi/3; a(2) = pi/3;
  //axis(a);
  //mprint axis3
  //@>
  //@figure axis3
  //
  //Finally, we can restore the original plot by issuing an @|axis('auto')|
  //command.
  //@<
  //plot(x,y,'r-');
  //axis auto
  //mprint axis4
  //@>
  //@figure axis4
  ArrayVector AxisFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("Axis function takes at most one argument: the strings 'tight', 'auto' or a 4-tuple of coordinates.");
    Plot2D* f = GetCurrentPlot();
    if (arg.size() == 1) {
      if (arg[0].isString()) {
	char *txt = arg[0].getContentsAsCString();
	if (strcmp(txt,"tight")==0)
	  f->SetAxesTight();
	else if (strcmp(txt,"auto")==0)
	  f->SetAxesAuto();
	else
	  throw Exception("Valid string arguments to 'axis' function are 'tight' and 'auto'");
      } else {
	Array t(arg[0]);
	t.promoteType(FM_DOUBLE);
	if ((!t.isVector()) || (t.getLength() != 4))
	  throw Exception("numeric argument to 'axis' function must be a vector of length 4");
	double *dp;
	dp = (double*) t.getDataPointer();
	f->SetAxes(dp[0],dp[1],dp[2],dp[3]);
      }
      f->repaint();
      ArrayVector retval;
      return retval;
    } else {
      Array retArr;
      retArr = Array::doubleVectorConstructor(4);
      double *dp;
      dp = (double*) retArr.getReadWriteDataPointer();
      f = GetCurrentPlot();
      f->GetAxes(dp[0],dp[1],dp[2],dp[3]);
      f->repaint();
      ArrayVector retval;
      retval.push_back(retArr);
      return retval;
    }
  }
}
