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


namespace FreeMat {

#define MAX_PLOTS 50

  Plot2D* plots[MAX_PLOTS];
  int currentPlot;
  
  void InitializePlotSubsystem() {
    currentPlot = -1;
    for (int i=0;i<MAX_PLOTS;i++) 
      plots[i] = NULL;
  }

  void NewPlot() {
    // First search for an unused plot number
    int plotNum = 0;
    bool plotFree = false;
    while ((plotNum < MAX_PLOTS) && !plotFree) {
      plotFree = (plots[plotNum] == NULL);
      if (!plotFree) plotNum++;
    }
    if (!plotFree) {
      throw Exception("No more plot handles available!  Close some plots...");
    }
    plots[plotNum] = new Plot2D(plotNum);
    plots[plotNum]->Show();
    plots[plotNum]->Raise();
    plots[plotNum]->SetHoldFlag(false);
    currentPlot = plotNum;
  }

  void SelectPlot(int plotnum) {
    if (plots[plotnum] == NULL) {
      plots[plotnum] = new Plot2D(plotnum);
    }
    plots[plotnum]->Show();
    plots[plotnum]->Raise();
    currentPlot = plotnum;
  } 
  
  Plot2D* GetCurrentPlot() {
    if (currentPlot == -1)
      NewPlot();
    plots[currentPlot]->Raise();
    return plots[currentPlot];
  }

  void ClosePlotHelper(int fig) {
    if (fig == -1) return;
    if (plots[fig] == NULL) return;
    plots[fig]->Close();
    plots[fig] = NULL;
    if (currentPlot == fig)
      currentPlot = -1;
  }
  
  void NotifyPlotClose(int fig) {
    plots[fig] = NULL;
    if (currentPlot == fig)
      currentPlot = -1;
  }

  static char *colors = "rgbkcmy";
  static int colornumber;
  static char *styles = "-:;|";
  static char *symbols = ".ox+*sdv^<>ph";

  void ResetLineColors() {
    colornumber = 0;
  }

  char SearchString(char*options, char val) {
    if (val == 0)
      return 0;
    char tstr[2];
    tstr[0] = val;
    tstr[1] = 0;
    char *dp;
    if (options == NULL)
      return 0;
    dp = strstr(options,tstr);
    if (dp == NULL)
      return 0;
    else
      return *dp;
  }

  char* GetLineStyle(char* arg) {
    static char outStyle[4];
    char color = 0;
    char style = 0;
    char symbol = 0;
    char match;

    while ((arg != NULL) && (arg[0] != 0)) {
      match = SearchString(colors,*arg);
      if (match != 0) {
	if (color != 0)
	  throw Exception("Too many color arguments in linestyle <" + std::string(arg) + ">");
	else
	  color = match;
	arg++;
      }
      match = SearchString(styles,*arg);
      if (match != 0) {
	if (style != 0)
	  throw Exception("Too many style arguments in linestyle <" + std::string(arg) + ">");
	else
	  style = match;
	arg++;
      }
      match = SearchString(symbols,*arg);
      if (match != 0) {
	if (symbol != 0)
	  throw Exception("Too many symbol-type arguments in linestyle <" + std::string(arg) + ">");
	else
	  symbol = match;
	arg++;
      }
    }
    if (style == 0) 
      if ((color != 0) || (symbol != 0))
	style = ' ';
      else
	style = '-';
    if (color == 0) {
      color = colors[colornumber];
      colornumber++;
      colornumber %= strlen(colors);
    }
    if (symbol == 0)
      symbol = ' ';
    outStyle[0] = color;
    outStyle[1] = symbol;
    outStyle[2] = style;
    outStyle[3] = 0;
    return outStyle;
  }

  ArrayVector NewPlotFunction(int nargout,const ArrayVector& arg) {  
    NewPlot();
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(currentPlot+1));
    return retval;
  }

  ArrayVector UsePlotFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("useplot function takes a single, integer argument");
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_GFX))
      throw Exception("figure number is out of range - it must be between 1 and 50");
    SelectPlot(fignum-1);
    return ArrayVector();
  }

  ArrayVector ClosePlotFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("closeplot takes at most one argument - either the string 'all' to close all plots, or a scalar integer indicating which plot is to be closed.");
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
	  throw Exception("string argument to closeplot function must be 'all'");
      } else {
	int handle = t.getContentsAsIntegerScalar();
	if (handle < 1)
	  throw Exception("Invalid plot number argument to closeplot function");
	action = handle;
      }
    }
    if (action == 0) {
      if (currentPlot != -1) 
	ClosePlotHelper(currentPlot);
    } else if (action == -1) {
      for (int i=0;i<MAX_PLOTS;i++)
	ClosePlotHelper(i);
    } else {
      if ((action < MAX_PLOTS) && (action >= 1))
	ClosePlotHelper(action-1);
    }
    Run();
    return ArrayVector();
  }

  ArrayVector XLabelFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("xlabel function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("xlabel function takes only a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
    f->SetXLabel(t.getContentsAsCString());
    return ArrayVector();
  }

  ArrayVector YLabelFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("ylabel function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("ylabel function takes only a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
    f->SetYLabel(t.getContentsAsCString());
    return ArrayVector();
  }

  ArrayVector TitleFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("title function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("title function takes only a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
    f->SetTitleText(t.getContentsAsCString());
    return ArrayVector();
  }

  ArrayVector PrintPlotFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("printplot function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("printplot function takes a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
    f->Print(t.getContentsAsCString());
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
    int plotLength;
    double *xvals;
    double *yvals;
    double *dp;
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
    Array x, y, sliceY, Z;
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
 	  // Get the imaginary part into y;
 	  y = GetImagPart(Z);
	  argptr++;
	  // Check for a linestyle
	  if (argptr < arg.size()) {
	    if (arg[argptr].isString()) {
	      linestyle = GetLineStyle(arg[argptr].getContentsAsCString());
	      argptr++;
	    }
	  } else
	    linestyle = GetLineStyle(NULL);
	  f->AddPlot(DataSet2D(x,y,linestyle[0],linestyle[1],linestyle[2]));
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
      throw e;
    }
    // Stop the plot 
    return ArrayVector();
  }

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
      ArrayVector retval;
      return retval;
    } else {
      Array retArr;
      retArr = Array::doubleVectorConstructor(4);
      double *dp;
      dp = (double*) retArr.getReadWriteDataPointer();
      f = GetCurrentPlot();
      f->GetAxes(dp[0],dp[1],dp[2],dp[3]);
      ArrayVector retval;
      retval.push_back(retArr);
      return retval;
    }
  }
}
