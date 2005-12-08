#include "Figure.hpp"
#include "Exception.hpp"
#include "GraphicsCore.hpp"
#include <algorithm>
#include "HandleList.hpp"
#include "WalkTree.hpp"
#include "SurfPlot.hpp"
#include "QPWidget.hpp"
#include "Util.hpp"
#include "Label.hpp"
#define MAX_FIGS 100

#include <qapplication.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qclipboard.h>

#include <QMouseEvent>
#include <QImageWriter>

#include "PSDrawEngine.hpp"

namespace FreeMat {
  typedef struct {
    int type;
    QWidget* w;
  } widget;

  HandleList<widget*> guiHandles;

  Figure* figs[MAX_FIGS];
  int currentFig;

  typedef struct {
    int handle;
    FunctionDef *fdef;
    WalkTree *eval;
    Array payload;
  } cbstruct;
  
  void NotifyFigClose(int fig) {
    figs[fig] = NULL;
    if (currentFig == fig)
      currentFig = -1;
  }

  void Figure::closeEvent(QCloseEvent* e) {
    NotifyFigClose(m_num);
    QWidget::closeEvent(e);
  }
  
  Figure::Figure(int fignum) : 
    QWidget(NULL) {
    m_num = fignum;
    char buffer[1000];
    sprintf(buffer,"Figure %d",fignum+1);
    setWindowTitle(buffer);
    m_layout = new QGridLayout(this);
    //    setLayout(m_layout);
    m_type = new figType[1];
    m_wid = new QWidget*[1];
    m_wid[0] = new QWidget(this);
    m_type[0] = fignone;
    m_rows = 1;
    m_cols = 1;
    m_active_slot = 0;
  }
  
  Figure::~Figure() {
    NotifyFigClose(m_num);
  }

  figType Figure::getType() {
    return m_type[m_active_slot];
  }
  
  QWidget* Figure::GetChildWidget() {
    if (m_type[m_active_slot] == fignone) {
      QWidget *w = new QWidget(this);
      QGridLayout *l = new QGridLayout(w);
      l->addWidget(new QWidget(w),1,1);
      l->setColumnStretch(1,1);
      l->setRowStretch(1,1);
      m_type[m_active_slot] = figblank;
      m_wid[m_active_slot] = w;
      m_layout->addWidget(w,m_active_slot % m_rows,m_active_slot / m_rows);
      w->show();
    } 
    return m_wid[m_active_slot];
  }

  void Figure::SetFigureChild(QWidget *widget, figType typ) {
    if (m_wid[m_active_slot]) {
      m_wid[m_active_slot]->hide();
      delete m_wid[m_active_slot];
    }
    m_type[m_active_slot] = typ;
    m_wid[m_active_slot] = widget;
    m_layout->addWidget(widget,m_active_slot % m_rows,m_active_slot / m_rows);
    widget->show();
  }

  void Figure::ReconfigurePlotMatrix(int rows, int cols) {
    for (int i=0;i<m_rows*m_cols;i++) {
      if (m_wid[i]) {
	m_wid[i]->hide();
	delete m_wid[i];
      }
    }
    delete m_wid;
    delete m_type;
    m_rows = rows;
    m_cols = cols;
    m_type = new figType[rows*cols];
    m_wid = new QWidget*[rows*cols];
    for (int i=0;i<rows*cols;i++) {
      m_wid[i] = NULL;
      m_type[i] = fignone;
    }
  }

  void Figure::ActivateMatrixEntry(int slot) {
    m_active_slot = slot;
  }

  void Figure::GetPlotMatrix(int& rows, int& cols) {
    rows = m_rows;
    cols = m_cols;
  }

  void Figure::Copy() {
    Figure* f = GetCurrentFig();
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(QPixmap::grabWidget(f));
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
    SaveFocus();
    figs[figNum]->show();
    RestoreFocus();
    currentFig = figNum;
  }
  
  void SelectFig(int fignum) {
    if (figs[fignum] == NULL) {
      figs[fignum] = new Figure(fignum);
    }
    SaveFocus();
    figs[fignum]->show();
    RestoreFocus();
    currentFig = fignum;
  } 

  Figure* GetCurrentFig() {
    if (currentFig == -1)
      NewFig();
    SaveFocus();
    figs[currentFig]->show();
    RestoreFocus();
    return figs[currentFig];
  }


  void PrintWidgetHelper(QWidget* g, DrawEngine &gc) {
    if (!g) return;
    QPWidget *w = dynamic_cast<QPWidget *>(g);
    if (w) {
      gc.save();
      gc.translate(w->pos().x(),w->pos().y());
      w->DrawMe(gc);
      gc.restore();
    }
    const QObjectList children = g->children();
    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = static_cast<QWidget*>(children.at(i));
        if (!child->isWidgetType() || child->isWindow()
            || child->isHidden())
            continue;
	PrintWidgetHelper((QWidget*) child, gc);
    }
   }
    

  bool PrintWidget(QWidget* g, std::string filename, std::string type) {
    if (type == "EPS" || type == "PS") {
      PSDrawEngine pE(filename,g->width(),g->height());
      PrintWidgetHelper(g, pE);
      return true;
    } else {
      // Binary print - use grabWidget
      QPixmap pxmap(QPixmap::grabWidget(g));
      QImage img(pxmap.toImage());
      return img.save(filename.c_str(),type.c_str());
    }
  }

  //!
  //@Module PRINT Print a Figure To A File
  //@@Section FIGURE
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
  //plot(x,y,'r-');
  //print printfig1.eps
  //print printfig1.jpg
  //@>
  //which creates two plots @|printfig1.eps|, which is an Encapsulated
  //Postscript file, and @|printfig1.jpg| which is a JPEG file.
  //@figure printfig1
  //!
  ArrayVector PrintFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("print function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("print function takes a single, string argument");
    Array t(arg[0]);
    Figure* f = GetCurrentFig();
    std::string outname(t.getContentsAsCString());
    if (f) {
      int pos = outname.rfind(".");
      if (pos < 0)
	throw Exception("print function argument must contain an extension - which is used to figure out the format for the output");
      std::string original_extension(outname.substr(pos+1,outname.size()));
      std::string modified_extension = 
	NormalizeImageExtension(original_extension);
      if (modified_extension.empty())
	throw Exception(std::string("unsupported output format ") + 
			original_extension + " for print.\n" + 
			FormatListAsString());
      if (!PrintWidget(f,outname,modified_extension))
	throw Exception("Printing failed!");
    }
    return ArrayVector();
  }

#if 0
  void CheckBoxArgument(Array t) {
    if ((t.isReferenceType() || t.isComplex()) || (t.getLength() != 4))
      throw Exception("box argument invalid (must be real, numeric and of length 4)");
  }

  widget* GetWidgetFromHandle(Array arg) {
    static widget p;
    int32 handle = arg.getContentsAsIntegerScalar();
    if (handle < MAX_FIGS) {
      SelectFig(handle);
      Fl_Group *ptr;
      ptr = figs[handle];
      p.w = (Fl_Widget*) ptr;
      p.type = WIDGET_GROUP;
      return (&p);
    } else 
      return guiHandles.lookupHandle(handle-MAX_FIGS);
  }
  
  // value = wigdetvalue(handle)
  ArrayVector WidgetValueFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("widgetvalue requires at least a handle argument");
    widget* p = GetWidgetFromHandle(arg[0]);
    switch (p->type) {
    case WIDGET_GROUP:
      return singleArrayVector(Array::emptyConstructor());
    case WIDGET_INPUT:
      return singleArrayVector(Array::stringConstructor(((Fl_Input*) p->w)->value()));
    case WIDGET_SCROLL:
      return singleArrayVector(Array::doubleConstructor(((Fl_Valuator*) p->w)->value()));
    }
    return singleArrayVector(Array::emptyConstructor());
  }
  
  // handle = slider(parent,box,type)
  ArrayVector SliderFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("slider requires two arguments: handle, box");
    widget* p = GetWidgetFromHandle(arg[0]);
    CheckBoxArgument(arg[1]);
    Array box(arg[1]);
    int fl_input_type;
    fl_input_type = FL_VERTICAL;
    if ((arg.size() > 2) && arg[2].isString()) {
      char *sp = arg[2].getContentsAsCString();
      if (strcmp(sp,"vertical")==0)
	fl_input_type = FL_VERTICAL;
      if (strcmp(sp,"horizontal")==0)
	fl_input_type = FL_HORIZONTAL;
      if (strcmp(sp,"vert_fill")==0)
	fl_input_type = FL_VERT_FILL_SLIDER;
      if (strcmp(sp,"horiz_fill")==0)
	fl_input_type = FL_HOR_FILL_SLIDER;
      if (strcmp(sp,"vert_nice_slider")==0)
	fl_input_type = FL_VERT_NICE_SLIDER;
      if (strcmp(sp,"horiz_nice_slider")==0)
	fl_input_type = FL_HOR_NICE_SLIDER;
    }
    box.promoteType(FM_INT32);
    int32 *dp = (int32*)box.getDataPointer();
    ptr->begin();
    Fl_Slider *nput = new Fl_Slider(dp[0],dp[1],dp[2],dp[3]);
    nput->type(fl_input_type);
    int newhandle = guiHandles.assignHandle(nput) + MAX_FIGS;
    ptr->end();
    ptr->Redraw();
    return singleArrayVector(Array::int32Constructor(newhandle));    
  }


  // type = 'normal', 'float', 'int', 'multiline'
  // handle = inputfield(parent,box,type)
  ArrayVector InputFieldFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("input field requires two arguments: handle, box");
    Array hnd(arg[0]);
    Fl_Group *ptr;
    int32 handle = hnd.getContentsAsIntegerScalar();
    CheckBoxArgument(arg[1]);
    if (handle < MAX_FIGS) {
      SelectFig(handle);
      ptr = figs[handle];
    } else {
      ptr = (Fl_Group*)guiHandles.lookupHandle(handle-MAX_FIGS);
    }
    Array box(arg[1]);
    int fl_input_type;
    fl_input_type = FL_NORMAL_INPUT;
    if ((arg.size() > 2) && arg[2].isString()) {
      char *sp = arg[2].getContentsAsCString();
      if (strcmp(sp,"normal")==0)
	fl_input_type = FL_NORMAL_INPUT;
      if (strcmp(sp,"float")==0)
	fl_input_type = FL_FLOAT_INPUT;
      if (strcmp(sp,"int")==0)
	fl_input_type = FL_INT_INPUT;
      if (strcmp(sp,"multiline")==0)
	fl_input_type = FL_MULTILINE_INPUT;
    }
    box.promoteType(FM_INT32);
    int32 *dp = (int32*)box.getDataPointer();
    ptr->begin();
    Fl_Input *nput = new Fl_Input(dp[0],dp[1],dp[2],dp[3]);
    nput->type(fl_input_type);
    int newhandle = guiHandles.assignHandle(nput) + MAX_FIGS;
    ptr->end();
    ptr->Redraw();
    return singleArrayVector(Array::int32Constructor(newhandle));
  }

  // how does the button work?  we can just add a button to the
  // current figure.  something like:
  // handle = button(parent,box,label,callback,data)
  // where callback is the function to execute, and data
  // contains any data needed by the callback.
  ArrayVector ButtonFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() < 4)
      throw Exception("button function requires at least four arguments: handle, box, label, callback");
    Array hnd(arg[0]);
    Fl_Group *ptr;
    int32 handle = hnd.getContentsAsIntegerScalar();
    CheckBoxArgument(arg[1]);
    if (!(arg[2].isString()))
      throw Exception("second argument to button must be a string label");
    if (!(arg[3].isString()))
      throw Exception("third argument to button must be a string callback function");
    if (handle < MAX_FIGS) {
      SelectFig(handle);
      ptr = figs[handle];
    } else {
      ptr = (Fl_Group*)guiHandles.lookupHandle(handle-MAX_FIGS);
    }
    Array box(arg[1]);
    box.promoteType(FM_INT32);
    int32 *dp = (int32*)box.getDataPointer();
    char *label = arg[2].getContentsAsCString();
    char *callback = arg[3].getContentsAsCString();
    eval->getInterface()->rescanPath();
    Context *context = eval->getContext();
    FunctionDef *funcDef;
    if (!context->lookupFunction(callback,funcDef))
      throw Exception(std::string("function ") + callback + " undefined!");
    funcDef->updateCode();
    ptr->begin();
    Fl_Button *ok = new Fl_Button(dp[0],dp[1],dp[2],dp[3],label);
    int newhandle = guiHandles.assignHandle(ok) + MAX_FIGS;
    cbstruct *cb = new cbstruct;
    cb->eval = eval;
    cb->handle = newhandle;
    cb->fdef = funcDef;
    if (arg.size() == 4)
      cb->payload = arg[3];
    else
      cb->payload = Array::emptyConstructor();
    ok->callback(generic_cb,cb);
    ptr->end();
    ptr->Redraw();
    return singleArrayVector(Array::int32Constructor(newhandle));
  }
#endif

  //!
  //@Module XLABEL Plot X-axis Label Function
  //@@Section PLOT
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
  ArrayVector XLabelFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("xlabel function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("xlabel function takes only a single, string argument");
    Array t(arg[0]);
    Figure *fig = GetCurrentFig();
    QWidget *f = fig->GetChildWidget();
    ClearGridWidget(f,"xlabel");
    ClearGridWidget(f,"colorbar_s");
    Label *l = new Label(f,"xlabel",t.getContentsAsCString(),'h');
    SetGridWidget(f,l,2,1);
    l->show();
    return ArrayVector();
  }

  //!
  //@Module YLABEL Plot Y-axis Label Function
  //@@Section PLOT
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
    Figure *fig = GetCurrentFig();
    QWidget *f = fig->GetChildWidget();
    ClearGridWidget(f,"ylabel");
    ClearGridWidget(f,"colorbar_w");
    Label *l = new Label(f,"ylabel",t.getContentsAsCString(),'v');
    SetGridWidget(f,l,1,0);
    l->show();
    return ArrayVector();
  }

  //!
  //@Module TITLE Plot Title Function
  //@@Section PLOT
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
    Figure *fig = GetCurrentFig();
    QWidget *f = fig->GetChildWidget();
    ClearGridWidget(f,"title");
    ClearGridWidget(f,"colorbar_n");
    Label *p = new Label(f,"title",t.getContentsAsCString(),'h');
    SetGridWidget(f,p,0,1);
    p->show();
    return ArrayVector();
  }

  ArrayVector DemoFunction(int nargout, const ArrayVector& arg) {
    Figure* f = GetCurrentFig();
    SurfPlot* t = new SurfPlot(f);
    Array s(arg[0]);
    s.promoteType(FM_DOUBLE);
    if (s.getLength() != 256*3)
      throw Exception("demo function argument must have exactly 768 elements");
    t->SetColormap((double*) s.getDataPointer());
    Array x(arg[1]);
    x.promoteType(FM_DOUBLE);
    Array y(arg[2]);
    y.promoteType(FM_DOUBLE);
    Array z(arg[3]);
    z.promoteType(FM_DOUBLE);
    t->SetData((const double*) x.getDataPointer(),
	       (const double*) y.getDataPointer(),
	       (const double*) z.getDataPointer(),
	       x.getLength(),
	       y.getLength());
    f->SetFigureChild(t,fig3plot);
    f->repaint();
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
  //@@Section FIGURE
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
    f->resize(width,height);
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
  //@@Section FIGURE
  //@@Usage
  //Changes the active figure window to the specified handle 
  //(or figure number).  The general syntax for its use is 
  //@[
  //  figure(handle)
  //@]
  //where @|handle| is the handle to use. If the figure window 
  //corresponding to @|handle| does not already exist, a new 
  //window with this handle number is created.  If it does exist
  //then it is brought to the forefront and made active.
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
  //@Module COPY Copy Figure Window
  //@@Section FIGURE
  //@@Usage
  //Copies the currently active figure window to the clipboard on 
  //Windows systems.  The syntax for its use is:
  //@[
  //   copy
  //@]
  //The resulting figure is copied as an Enhanced MetaFile (EMF)
  //to the clipboard, and can then be pasted into any suitable
  //application.
  //!
  ArrayVector CopyFunction(int nargout, const ArrayVector& arg) {
    Figure* f = GetCurrentFig();
    f->Copy();
    return ArrayVector();
  }

  //!
  //@Module CLOSE Close Figure Window
  //@@Section FIGURE
  //@@Usage
  //Closes a figure window, either the currently active window, a 
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

  //!
  //@Module CLF Clear Figure
  //@@Section PLOT
  //@@Usage
  //This function clears the contents of the current figure.  The
  //syntax for its use is
  //@[
  //   clf
  //@]
  //!
  ArrayVector ClearFigureFunction(int nargout, const ArrayVector& arg) {
    Figure* fig = GetCurrentFig();
    fig->ReconfigurePlotMatrix(1,1);
    fig->repaint();
    return ArrayVector();
  }
  
  //!
  //@Module SUBPLOT Subplot Function
  //@@Section PLOT
  //@@Usage
  //This function divides the current figure into a 2-dimensional
  //grid, each of which can contain a plot of some kind.  The
  //syntax for its use is
  //@[
  //   subplot(row,col,num)
  //@]
  //which either activates subplot number @|num|, or 
  //sets up a subplot grid of size @|row x col|, and then
  //activates @|num|. 
  //!
  ArrayVector SubPlotFunction(int nargout, const ArrayVector& arg) {
    int row;
    int col;
    int slot;
    if (arg.size() == 3) {
      row = ArrayToInt32(arg[0]);
      col = ArrayToInt32(arg[1]);
      slot = ArrayToInt32(arg[2]);
    } else if (arg.size() == 1) {
      int p = ArrayToInt32(arg[0]);
      row = p / 100;
      col = p / 10;
      slot = p % 10;
    } else
      throw Exception("unrecognized argument format for subplot");
    Figure* fig = GetCurrentFig();
    int crows, ccols;
    fig->GetPlotMatrix(crows,ccols);
    if ((row != crows) || (col != ccols))
      fig->ReconfigurePlotMatrix(row,col);
    fig->ActivateMatrixEntry(slot-1);
    return ArrayVector();
  }
  
  void ForceRefresh() {
    Figure* fig = GetCurrentFig();
    fig->repaint();
  }

  
}
