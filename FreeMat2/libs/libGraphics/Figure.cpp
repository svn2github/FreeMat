#include "Figure.hpp"
#include "Exception.hpp"
#include "GraphicsCore.hpp"

#define MAX_FIGS 100

namespace FreeMat {
  Figure* figs[MAX_FIGS];
  int currentFig;
  
  void NotifyFigClose(int fig) {
    figs[fig] = NULL;
    if (currentFig == fig)
      currentFig = -1;
  }
  
  Figure::Figure(int fignum) :
    Fl_Double_Window(500,400) {
    m_num = fignum;
    m_type = fignone;
    char buffer[1000];
    sprintf(buffer,"Figure %d",fignum+1);
    label(buffer);
    resizable(this);
  }
  
  Figure::~Figure() {
    NotifyFigClose(m_num);
  }
  
  Fl_Widget* Figure::GetChildWidget() {
    return m_wid;
  }

  void Figure::SetFigureChild(Fl_Widget *widget, figType typ) {
    m_type = typ;
    clear();
    add(widget);
    resizable(widget);
    m_wid = widget;
  }
  
  void InitializeFigureSubsystem() {
    currentFig = -1;
    for (int i=0;i<MAX_FIGS;i++) 
      figs[i] = NULL;
  }
  
  void NewFig() {
    // First search for an unused fig number
    int figNum = 0;
    bool figFree = false;
    while ((figNum < MAX_FIGS) && !figFree) {
      figFree = (figs[figNum] == NULL);
      if (!figFree) figNum++;
    }
    if (!figFree) {
      throw Exception("No more fig handles available!  Close some figs...");
    }
    figs[figNum] = new Figure(figNum);
    figs[figNum]->show();
    currentFig = figNum;
  }
  
  void SelectFig(int fignum) {
    if (figs[fignum] == NULL) {
      figs[fignum] = new Figure(fignum);
    }
    figs[fignum]->show();
    currentFig = fignum;
  } 

  Figure* GetCurrentFig() {
    if (currentFig == -1)
      NewFig();
    figs[currentFig]->show();
    return figs[currentFig];
  }

  //!
  //@Module PRINT Print a Figure To A File
  //@@Usage
  //This function ``prints'' the currently active fig to a file.  The 
  //generic syntax for its use is
  //@[
  //  print(filename)
  //@]
  //or, alternately,
  //@[
  //  print filename
  //@]
  //where @|filename| is the (string) filename of the destined file.  The current
  //fig is then saved to the output file using a format that is determined
  //by the extension of the filename.  The exact output formats may vary on
  //different platforms, but generally speaking, the following extensions
  //should be supported cross-platform:
  //\begin{itemize}
  //\item @|jpg|, @|jpeg|  --  JPEG file 
  //\item @|ps|, @|eps| -- Encapsulated Postscript file 
  //\item @|png| -- Portable Net Graphics file
  //\end{itemize}
  //Note that only the fig is printed, not the window displaying
  //the fig.  If you want something like that (essentially a window-capture)
  //use a seperate utility or your operating system's built in screen
  //capture ability.
  //@@Example
  //Here is a simple example of how the figures in this manual are generated.
  //@<
  //x = linspace(-1,1);
  //y = cos(5*pi*x);
  //fig(x,y,'r-');
  //print printfig1.eps
  //print printfig1.jpg
  //mprintplot printplt
  //@>
  //which creates two plots @|printplot1.eps|, which is an Encapsulated
  //Postscript file, and @|printplot1.jpg| which is a JPEG file.
  //@figure printplt
  //!
  ArrayVector PrintFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("print function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("print function takes a single, string argument");
    Array t(arg[0]);
    Figure* f = GetCurrentFig();
    std::string outname(t.getContentsAsCString());
    //    f->Print(outname);
    return ArrayVector();
  }

  ArrayVector DemoFunction(int nargout, const ArrayVector& arg) {
    Figure* f = GetCurrentFig();
#if 0
    XPContainer *c = new XPContainer(f, f->GetBoundingRect());
    XPLabel *l = new XPLabel(NULL, Rect2D(75,75,50,75), "Label!");
    XPButton *b = new XPButton(NULL, Rect2D(50,50,150,100), l);
    XPVSlider *s = new XPVSlider(NULL, Rect2D(250,50,35,200), 0.0);
    XPEditLine *e = new XPEditLine(NULL, Rect2D(200,300,200,30), "twe");
    c->AddChild(b);
    c->AddChild(s);
    c->AddChild(e);
    f->SetFigureChild(c, figgui);
    f->Refresh(f->GetBoundingRect());
#endif
    return ArrayVector();
  }

  //!
  //@Module SIZEFIG Set Size of an Fig Window
  //@@Usage
  //The @|sizefig| function changes the size of the currently
  //selected fig window.  The general syntax for its use is
  //@[
  //   sizefig(width,height)
  //@]
  //where @|width| and @|height| are the dimensions of the fig
  //window.
  //!
  ArrayVector SizeFigFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("sizefig function takes two arguments: height and width");
    Array w(arg[1]);
    Array h(arg[0]);
    int width;
    int height;
    width = w.getContentsAsIntegerScalar();
    height = h.getContentsAsIntegerScalar();
    Figure *f = GetCurrentFig();
    f->resize(0,0,width,height);
    return ArrayVector();
  }

  void CloseHelper(int fig) {
    if (fig == -1) return;
    if (figs[fig] == NULL) return;
    figs[fig]->hide();
    delete figs[fig];
    figs[fig] = NULL;
    if (currentFig == fig)
      currentFig = -1;
  }

  //!
  //@Module FIGURE Figure Window Select and Create Function
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
  ArrayVector FigureFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() == 0) {
      NewFig();
      return singleArrayVector(Array::int32Constructor(currentFig+1));
    } else {
      Array t(arg[0]);
      int fignum = t.getContentsAsIntegerScalar();
      if ((fignum<=0) || (fignum>MAX_GFX))
	throw Exception("figure number is out of range - it must be between 1 and 50");
      SelectFig(fignum-1);
      return ArrayVector();
    }
  }
  
  //!
  //@Module CLOSE Close Figure Window
  //@@Usage
  //Closes an figure window, either the currently active window, a 
  //window with a specific handle, or all figure windows.  The general
  //syntax for its use is
  //@[
  //   close(handle)
  //@]
  //in which case the figure window with the speicified @|handle| is
  //closed.  Alternately, issuing the command with no argument
  //@[
  //   close
  //@]
  //is equivalent to closing the currently active figure window.  Finally
  //the command
  //@[
  //   close('all')
  //@]
  //closes all figure windows currently open.
  //!
  ArrayVector CloseFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("close takes at most one argument - either the string 'all' to close all figures, or a scalar integer indicating which figure is to be closed.");
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
	  throw Exception("string argument to close function must be 'all'");
      } else {
	int handle = t.getContentsAsIntegerScalar();
	if (handle < 1)
	  throw Exception("Invalid figure number argument to close function");
	action = handle;
      }
    }
    if (action == 0) {
      if (currentFig != -1) 
	CloseHelper(currentFig);
    } else if (action == -1) {
      for (int i=0;i<MAX_FIGS;i++)
	CloseHelper(i);
    } else {
      if ((action < MAX_FIGS) && (action >= 1))
	CloseHelper(action-1);
    }
    return ArrayVector();
  }

  void ForceRefresh() {
    Figure* fig = GetCurrentFig();
    fig->redraw();
  }
}
