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

  
  //!
  //@Module SIZEPLOT Set Size of an Plot Window
  //@@Usage
  //The @|sizeplot| function changes the size of the currently
  //selected plot window.  The general syntax for its use is
  //@[
  //   sizeplot(width,height)
  //@]
  //where @|width| and @|height| are the dimensions of the plot
  //window.
  //!
  ArrayVector SizePlotFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("sizeplot function takes two arguments: height and width");
    Array w(arg[1]);
    Array h(arg[0]);
    int width;
    int height;
    width = w.getContentsAsIntegerScalar();
    height = h.getContentsAsIntegerScalar();
    Plot2D *f;
    f = GetCurrentPlot();
    f->SetSize(width,height);
    return ArrayVector();
  }

  void ClosePlotHelper(int fig) {
    if (fig == -1) return;
    if (plots[fig] == NULL) return;
    CloseXWindow(plots[fig]);
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

  //!
  //@Module NEWPLOT New Plot Window Function
  //@@Usage
  //Creates a new window for the display of plots using the @|plot| commands.
  //The general syntax for its use is
  //@[
  //   y = newplot
  //@]
  //where @|y| is the handle (or plot number) of the newly created window.  
  //Plot handles are sequential, starting with 1, unless one is closed, in 
  //which case the smallest unused handle is returned.
  //!
  ArrayVector NewPlotFunction(int nargout,const ArrayVector& arg) {  
    NewPlot();
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(currentPlot+1));
    return retval;
  }

  //!
  //@Module USEPLOT Use Plot Window Function
  //@@Usage
  //Changes the active plot window to the specified handle (or plot number).  
  //The general syntax for its use is 
  //@[
  //  useplot(handle)
  //@]
  //where @|handle| is the handle to use.  If the plot window corresponding to
  //@|x| does not already exist, a new window with this handle number is 
  //created.
  //@@Example
  //In this example, we create two plot windows, and then use the @|useplot|
  //command to activate the first window.
  //@<
  //newplot
  //newplot
  //useplot(1)
  //@>  
  //!  
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

  //!
  //@Module CLOSEPLOT Close Plot Window
  //@@Usage
  //Closes an plot window, either the currently active window, a 
  //window with a specific handle, or all plot windows.  The general
  //syntax for its use is
  //@[
  //   closeplot(handle)
  //@]
  //in which case the plot window with the speicified @|handle| is
  //closed.  Alternately, issuing the command with no argument
  //@[
  //   closeplot
  //@]
  //is equivalent to closing the currently active plot window.  Finally
  //the command
  //@[
  //   closeplot('all')
  //@]
  //closes all plot windows currently open.
  //!
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
    FlushWindowEvents();
    return ArrayVector();
  }

  //!
  //@Module XLABEL Plot X-axis Label Function
  //@@Usage
  //This command adds a label to the x-axis of the plot.  The general syntax
  //for its use is
  //@[
  //  xlabel('label')
  //@]
  //or in the alternate form
  //@[
  //  xlabel 'label'
  //@]
  //or simply
  //@[
  //  xlabel label
  //@]
  //Here @|label| is a string variable.
  //@@Example
  //Here is an example of a simple plot with a label on the @|x|-axis.
  //@<
  //x = linspace(-1,1);
  //y = cos(2*pi*x);
  //plot(x,y,'r-');
  //xlabel('time');
  //mprintplot xlabel1
  //@>
  //which results in the following plot.
  //@figure xlabel1
  //!
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

  //!
  //@Module YLABEL Plot Y-axis Label Function
  //@@Usage
  //This command adds a label to the y-axis of the plot.  The general syntax
  //for its use is
  //@[
  //  ylabel('label')
  //@]
  //or in the alternate form
  //@[
  //  ylabel 'label'
  //@]
  //or simply
  //@[
  //  ylabel label
  //@]
  //Here @|label| is a string variable.
  //@@Example
  //Here is an example of a simple plot with a label on the @|y|-axis.
  //@<
  //x = linspace(-1,1);
  //y = cos(2*pi*x);
  //plot(x,y,'r-');
  //ylabel('cost');
  //mprintplot ylabel1
  //@>
  //which results in the following plot.
  //@figure ylabel1
  //!
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

  //!
  //@Module TITLE Plot Title Function
  //@@Usage
  //This command adds a title to the plot.  The general syntax
  //for its use is
  //@[
  //  title('label')
  //@]
  //or in the alternate form
  //@[
  //  title 'label'
  //@]
  //or simply
  //@[
  //  title label
  //@]
  //Here @|label| is a string variable.
  //@@Example
  //Here is an example of a simple plot with a title.
  //@<
  //x = linspace(-1,1);
  //y = cos(2*pi*x);
  //plot(x,y,'r-');
  //title('cost over time');
  //mprintplot title1
  //@>
  //which results in the following plot.
  //@figure title1
  //!
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

  //!
  //@Module PRINTPLOT Print an Plot To A File
  //@@Usage
  //This function ``prints'' the currently active plot to a file.  The 
  //generic syntax for its use is
  //@[
  //  printplot(filename)
  //@]
  //or, alternately,
  //@[
  //  printplot filename
  //@]
  //where @|filename| is the (string) filename of the destined file.  The current
  //plot is then saved to the output file using a format that is determined
  //by the extension of the filename.  The exact output formats may vary on
  //different platforms, but generally speaking, the following extensions
  //should be supported cross-platform:
  //\begin{itemize}
  //\item @|jpg|, @|jpeg|  --  JPEG file 
  //\item @|ps|, @|eps| -- Encapsulated Postscript file 
  //\item @|png| -- Portable Net Graphics file
  //\end{itemize}
  //Note that only the plot is printed, not the window displaying
  //the plot.  If you want something like that (essentially a window-capture)
  //use a seperate utility or your operating system's built in screen
  //capture ability.
  //@@Example
  //Here is a simple example of how the figures in this manual are generated.
  //@<
  //x = linspace(-1,1);
  //y = cos(5*pi*x);
  //plot(x,y,'r-');
  //printplot printplot1.eps
  //printplot printplot1.jpg
  //mprintplot printplt
  //@>
  //which creates two plots @|printplot1.eps|, which is an Encapsulated
  //Postscript file, and @|printplot1.jpg| which is a JPEG file.
  //@figure printplt
  //!
  ArrayVector PrintPlotFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("printplot function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("printplot function takes a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
	std::string outname(t.getContentsAsCString());
    f->PrintMe(outname.c_str());
    return ArrayVector();
  }

  //!
  //@Module GRID Plot Grid Toggle Function
  //@@Usage
  //Toggles the drawing of grid lines on the currently active plot.  The
  //general syntax for its use is
  //@[
  //   grid(state)
  //@]
  //where @|state| is either
  //@[
  //   grid('on')
  //@]
  //to activate the grid lines, or
  //@[
  //   grid('off')
  //@]
  //to deactivate the grid lines.
  //@@Example
  //Here is a simple plot without grid lines.
  //@<
  //x = linspace(-1,1);
  //y = cos(3*pi*x);
  //plot(x,y,'r-');
  //mprintplot grid1
  //@>
  //@figure grid1
  //
  //Next, we activate the grid lines.
  //@<
  //plot(x,y,'r-');
  //grid on
  //mprintplot grid2
  //@>
  //@figure grid2
  //!
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

  //!
  //@Module HOLD Plot Hold Toggle Function
  //@@Usage
  //Toggles the hold state on the currently active plot.  The
  //general syntax for its use is
  //@[
  //   grid(state)
  //@]
  //where @|state| is either
  //@[
  //   hold('on')
  //@]
  //to turn hold on, or
  //@[
  //   hold('off')
  //@]
  //to turn hold off.
  //@@Function Internals
  //The @|hold| function allows one to construct a plot sequence
  //incrementally, instead of issuing all of the plots simultaneously
  //using the @|plot| command.
  //@@Example
  //Here is an example of using both the @|hold| command and the
  //multiple-argument @|plot| command to construct a plot composed
  //of three sets of data.  The first is a plot of a modulated Gaussian.
  //@<
  //x = linspace(-5,5,500);
  //t = exp(-x.^2);
  //y = t.*cos(2*pi*x*3);
  //plot(x,y);
  //mprintplot hold1
  //@>
  //@figure hold1
  //
  //We now turn the hold state to @|'on'|, and add another plot
  //sequence, this time composed of the top and bottom envelopes of
  //the modulated Gaussian.  We add the two envelopes simultaneously
  //using a single @|plot| command.  The fact that @|hold| is
  //@|'on'| means that these two envelopes are added to (instead of
  //replace) the current contents of the plot.
  //@<
  //plot(x,y);
  //hold on
  //plot(x,t,'g-',x,-t,'b-')
  //mprintplot hold2
  //@>
  //@figure hold2
  //!
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

  //!
  //@Module PLOT Plot Function
  //@@Usage
  //This is the basic plot command for FreeMat.  The general syntax for its
  //use is
  //@[
  //  plot(<data 1>,{linespec 1},<data 2>,{linespec 2}...)
  //@]
  //where the @|<data>| arguments can have various forms, and the
  //@|linespec| arguments are optional.  We start with the
  //@|<data>| term, which can take on one of multiple forms:
  //\begin{itemize}
  //  \item \emph{Vector Matrix Case} -- In this case the argument data is a pair
  //    of variables.  A set of @|x| coordinates in a numeric vector, and a 
  //    set of @|y| coordinates in the columns of the second, numeric matrix.
  //    @|x| must have as many elements as @|y| has columns (unless @|y|
  //    is a vector, in which case only the number of elements must match).  Each
  //    column of @|y| is plotted sequentially against the common vector @|x|.
  //  \item \emph{Unpaired Matrix Case} -- In this case the argument data is a 
  //    single numeric matrix @|y| that constitutes the @|y|-values
  //    of the plot.  An @|x| vector is synthesized as @|x = 1:length(y)|,
  //    and each column of @|y| is plotted sequentially against this common @|x|
  //    axis.
  //  \item \emph{Complex Matrix Case} -- Here the argument data is a complex
  //    matrix, in which case, the real part of each column is plotted against
  //    the imaginary part of each column.  All columns receive the same line
  //    styles.
  //\end{itemize}
  //Multiple data arguments in a single plot command are treated as a \emph{sequence}, meaning
  //that all of the plots are overlapped on the same set of axes.
  //The @|linespec| is a string used to change the characteristics of the line.  In general,
  //the @|linespec| is composed of three optional parts, the @|colorspec|, the 
  //@|symbolspec| and the @|linestylespec| in any order.  Each of these specifications
  //is a single character that determines the corresponding characteristic.  First, the 
  //@|colorspec|:
  //\begin{itemize}
  //  \item @|'r'| - Color Red
  //  \item @|'g'| - Color Green
  //  \item @|'b'| - Color Blue
  //  \item @|'k'| - Color Black
  //  \item @|'c'| - Color Cyan
  //  \item @|'m'| - Color Magenta
  //  \item @|'y'| - Color Yellow
  //\end{itemize}
  //The @|symbolspec| specifies the (optional) symbol to be drawn at each data point:
  //\begin{itemize}
  //  \item @|'.'| - Dot symbol
  //  \item @|'o'| - Circle symbol
  //  \item @|'x'| - Times symbol
  //  \item @|'+'| - Plus symbol
  //  \item @|'*'| - Asterisk symbol
  //  \item @|'s'| - Square symbol
  //  \item @|'d'| - Diamond symbol
  //  \item @|'v'| - Downward-pointing triangle symbol
  //  \item @|'^'| - Upward-pointing triangle symbol
  //  \item @|'<'| - Left-pointing triangle symbol
  //  \item @|'>'| - Right-pointing triangle symbol
  //\end{itemize}
  //The @|linestylespec| specifies the (optional) line style to use for each data series:
  //\begin{itemize}
  //  \item @|'-'| - Solid line style
  //  \item @|':'| - Dotted line style
  //  \item @|';'| - Dot-Dash-Dot-Dash line style
  //  \item @|'||'| - Dashed line style
  //\end{itemize}
  //For sequences of plots, the @|linespec| is recycled with colors taken sequentially from
  //the palette.
  //@@Example
  //The most common use of the @|plot| command probably involves the vector-matrix
  //paired case.  Here, we generate a simple cosine, and plot it using a red line, with
  //no symbols (i.e., a @|linespec| of @|'r-'|).
  //@<
  //x = linspace(-pi,pi);
  //y = cos(x);
  //plot(x,y,'r-');
  //mprintplot plot1
  //@>
  //which results in the following plot.
  //@figure plot1
  //
  //Next, we plot multiple sinusoids (at different frequencies).  First, we construct
  //a matrix, in which each column corresponds to a different sinusoid, and then plot
  //them all at once.
  //@<
  //x = linspace(-pi,pi);
  //y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
  //plot(x,y);
  //mprintplot plot2
  //@>
  //In this case, we do not specify a @|linespec|, so that we cycle through the
  //colors automatically (in the order listed in the previous section).
  //@figure plot2
  //
  //This time, we produce the same plot, but as we want to assign individual
  //@|linespec|s to each line, we use a sequence of arguments in a single plot
  //command, which has the effect of plotting all of the data sets on a common 
  //axis, but which allows us to control the @|linespec| of each plot. In 
  //the following example, the first line (harmonic) has red, solid lines with 
  //times symbols
  //marking the data points, the second line (third harmonic) has blue, solid lines
  //with right-pointing triangle symbols, and the third line (fifth harmonic) has
  //green, dotted lines with asterisk symbols.
  //@<
  //plot(x,y(:,1),'rx-',x,y(:,2),'b>-',x,y(:,3),'g*:');
  //mprintplot plot3
  //@>
  //@figure plot3
  //
  //The second most frequently used case is the unpaired matrix case.  Here, we need
  //to provide only one data component, which will be automatically plotted against
  //a vector of natural number of the appropriate length.  Here, we use a plot sequence
  //to change the style of each line to be dotted, dot-dashed, and dashed.
  //@<
  //plot(y(:,1),'r:',y(:,2),'b;',y(:,3),'g|');
  //mprintplot plot4
  //@>
  //Note in the resulting plot that the @|x|-axis no longer runs from @|[-pi,pi]|, but 
  //instead runs from @|[1,100]|.
  //@figure plot4
  //
  //The final case is for complex matrices.  For complex arguments, the real part is
  //plotted against the imaginary part.  Hence, we can generate a 2-dimensional plot
  //from a vector as follows.
  //@<
  //y = cos(2*x) + i * cos(3*x);
  //plot(y);
  //mprintplot plot5
  //@>
  //@figure plot5
  //!

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

  //!
  //@Module AXIS Plot Axis Set/Get Function
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
  //mprintplot axis1
  //@>
  //@figure axis1
  //
  //Suppose we now want to make the axis fit the plot exactly.  We can issue an
  //@|axis('tight')| command, which results in the following plot.
  //@<
  //plot(x,y,'r-');
  //grid on
  //axis tight
  //mprintplot axis2
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
  //mprintplot axis3
  //@>
  //@figure axis3
  //
  //Finally, we can restore the original plot by issuing an @|axis('auto')|
  //command.
  //@<
  //plot(x,y,'r-');
  //axis auto
  //mprintplot axis4
  //@>
  //@figure axis4
  //!
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
