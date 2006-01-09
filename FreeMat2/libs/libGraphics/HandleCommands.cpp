#include "HandleCommands.hpp"
#include "HandleFigure.hpp"
#include <qgl.h>
#include <QMouseEvent>
#include <QPainter>
#include <QPrinter>
#include <QImage>
#include <QImageWriter>
#include "HandleLineSeries.hpp"
#include "HandleObject.hpp"
#include "HandleText.hpp"
#include "HandleAxis.hpp"
#include "HandleImage.hpp"
#include <qapplication.h>
#include "HandleList.hpp"
#include "HandleSurface.hpp"
#include "HandleWindow.hpp"
#include "QTRenderEngine.hpp"
#include "Util.hpp"

// Subplot
// labels don't always appear properly.
// linestyle/symbol specification
// images

namespace FreeMat {
#define MAX_FIGS 100
#define HANDLE_OFFSET_OBJECT 100000
#define HANDLE_OFFSET_FIGURE 1
  
  HandleWindow* Hfigs[MAX_FIGS];
  int HcurrentFig = -1;
  
  // Magic constant - limits the number of figures you can have...
  
  HandleList<HandleObject*> objectset;

  void NotifyFigureClosed(unsigned figNum) {
    Hfigs[figNum] = NULL;
    if (figNum == HcurrentFig)
      HcurrentFig = -1;
  }

  static void NewFig() {
    // First search for an unused fig number
    int figNum = 0;
    bool figFree = false;
    while ((figNum < MAX_FIGS) && !figFree) {
      figFree = (Hfigs[figNum] == NULL);
      if (!figFree) figNum++;
    }
    if (!figFree) {
      throw Exception("No more fig handles available!  Close some figs...");
    }
    Hfigs[figNum] = new HandleWindow(figNum);
    SaveFocus();
    Hfigs[figNum]->show();
    RestoreFocus();
    HcurrentFig = figNum;
  }

  static HandleFigure* CurrentFig() {
    if (HcurrentFig == -1)
      NewFig();
    return (Hfigs[HcurrentFig]->HFig());
  }


  static void SelectFig(int fignum) {
    if (Hfigs[fignum] == NULL)
      Hfigs[fignum] = new HandleWindow(fignum);
    SaveFocus();
    Hfigs[fignum]->show();
    Hfigs[fignum]->raise();
    RestoreFocus();
    HcurrentFig = fignum;
  }

  HandleObject* LookupHandleObject(unsigned handle) {
    return (objectset.lookupHandle(handle-HANDLE_OFFSET_OBJECT));
  }

  HandleFigure* LookupHandleFigure(unsigned handle) {
    if (Hfigs[handle-HANDLE_OFFSET_FIGURE] != NULL)
      return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
    else {
      SelectFig(handle-HANDLE_OFFSET_FIGURE);
      return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
    }
  }

  void ValidateHandle(unsigned handle) {
    if (handle >= HANDLE_OFFSET_OBJECT)
      LookupHandleObject(handle);
    else
      LookupHandleFigure(handle);
  }

  unsigned AssignHandleObject(HandleObject* hp) {
    return (objectset.assignHandle(hp)+HANDLE_OFFSET_OBJECT);
  }

  void FreeHandleObject(unsigned handle) {
    objectset.deleteHandle(handle-HANDLE_OFFSET_OBJECT);
  }

  ArrayVector HFigureFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() == 0) {
      NewFig();
      return singleArrayVector(Array::int32Constructor(HcurrentFig+1));
    } else {
      Array t(arg[0]);
      int fignum = t.getContentsAsIntegerScalar();
      if ((fignum<=0) || (fignum>MAX_FIGS))
	throw Exception("figure number is out of range - it must be between 1 and 50");
      SelectFig(fignum-1);
      return ArrayVector();
    }
  }

  //!
  //@Module AXES Create Handle Axes
  //@@Section HANDLE
  //@@Usage
  //This function has three different syntaxes.  The first takes
  //no arguments,
  //@[
  //  h = axes
  //@]
  //and creates a new set of axes that are parented to the current
  //figure (see @|gcf|).  The newly created axes are made the current
  //axes (see @|gca|) and are added to the end of the list of children 
  //for the current figure.
  //The second form takes a set of property names and values
  //@[
  //  h = axes(propertyname,value,propertyname,value,...)
  //@]
  //Creates a new set of axes, and then sets the specified properties
  //to the given value.  This is a shortcut for calling 
  //@|set(h,propertyname,value)| for each pair.
  //The third form takes a handle as an argument
  //@[
  //  axes(handle)
  //@]
  //and makes @|handle| the current axes, placing it at the head of
  //the list of children for the current figure.
  //!
  ArrayVector HAxesFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) {
      HandleObject *fp = new HandleAxis;
      unsigned int handle = AssignHandleObject(fp);
      ArrayVector t(arg);
      while (t.size() >= 2) {
	std::string propname(ArrayToString(t[0]));
	fp->LookupProperty(propname)->Set(t[1]);
	t.erase(t.begin(),t.begin()+2);
      }
      // Get the current figure
      HandleFigure *fig = CurrentFig();
      fp->SetPropertyHandle("parent",HcurrentFig+1);
      fig->SetPropertyHandle("currentaxes",handle);
      // Add us to the children...
      HPHandles *hp = (HPHandles*) fig->LookupProperty("children");
      std::vector<unsigned> children(hp->Data());
      children.push_back(handle);
      hp->Data(children);
      fp->UpdateState();
      return singleArrayVector(Array::uint32Constructor(handle));
    } else {
      unsigned int handle = (unsigned int) ArrayToInt32(arg[0]);
      HandleObject* hp = LookupHandleObject(handle);
      if (!hp->IsType("axes"))
	throw Exception("single argument to axes function must be handle for an axes"); 
      // Get the current figure
      HandleFigure *fig = CurrentFig();
      fig->SetPropertyHandle("currentaxes",handle);     
      HPHandles *cp = (HPHandles*) fig->LookupProperty("children");
      std::vector<unsigned> children(cp->Data());
      // Check to make sure that children does not contain our handle already
      int i=0;
      while (i<children.size()) {
	if (children[i] == handle)
       	  children.erase(children.begin()+i);
       	else
       	  i++;
      }
      children.insert(children.begin(),1,handle);
      cp->Data(children);
      return ArrayVector();
    }
  }

  ArrayVector HSetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 3)
      throw Exception("set doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    // Lookup the handle
    HandleObject *fp;
    if (handle >= HANDLE_OFFSET_OBJECT)
      fp = LookupHandleObject(handle);
    else
      fp = (HandleObject*) LookupHandleFigure(handle);
    int ptr = 1;
    while (arg.size() >= (ptr+2)) {
      // Use the address and property name to lookup the Get/Set handler
      std::string propname = ArrayToString(arg[ptr]);
      fp->LookupProperty(propname)->Set(arg[ptr+1]);
      ptr+=2;
    }
    fp->UpdateState();
    return ArrayVector();
  }

  ArrayVector HGetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("get doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    std::string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleObject *fp;
    if (handle >= HANDLE_OFFSET_OBJECT)
      fp = LookupHandleObject(handle);
    else
      fp = (HandleObject*) LookupHandleFigure(handle);
    // Use the address and property name to lookup the Get/Set handler
    return singleArrayVector(fp->LookupProperty(propname)->Get());
  }

  unsigned GenericConstructor(HandleObject* fp, const ArrayVector& arg) {
    unsigned int handle = AssignHandleObject(fp);
    ArrayVector t(arg);
    while (t.size() >= 2) {
      std::string propname(ArrayToString(t[0]));
      fp->LookupProperty(propname)->Set(t[1]);
      t.erase(t.begin(),t.begin()+2);
    }
    HandleFigure *fig = CurrentFig();
    unsigned current = fig->HandlePropertyLookup("currentaxes");
    if (current == 0) {
      ArrayVector arg2;
      HAxesFunction(0,arg2);
      current = fig->HandlePropertyLookup("currentaxes");
    }
    HandleAxis *axis = (HandleAxis*) LookupHandleObject(current);
    HPHandles *cp = (HPHandles*) axis->LookupProperty("children");
    std::vector<unsigned> children(cp->Data());
    children.push_back(handle);
    cp->Data(children);
    cp = (HPHandles*) fp->LookupProperty("parent");
    std::vector<unsigned> parent;
    parent.push_back(current);
    cp->Data(parent);
    fp->UpdateState();
    axis->UpdateState();
    return handle;
  }

  ArrayVector HLineFunction(int nargout, const ArrayVector& arg) {
    return singleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleLineSeries,arg)));
  }
  
  ArrayVector HImageFunction(int nargout, const ArrayVector& arg) {
    return singleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleImage,arg)));
  }

  ArrayVector HTextFunction(int nargout, const ArrayVector& arg) {
    return singleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleText,arg)));
  }

  ArrayVector HSurfaceFunction(int nargout, const ArrayVector& arg) {
    return singleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleSurface,arg)));
  }

  ArrayVector HGCFFunction(int nargout, const ArrayVector& arg) {
    if (HcurrentFig == -1)
      NewFig();      
    return singleArrayVector(Array::uint32Constructor(HcurrentFig+1));
  }

  ArrayVector HGCAFunction(int nargout, const ArrayVector& arg) {
    // Get the current figure...
    if (HcurrentFig == -1)
      NewFig();
    HandleFigure* fig = CurrentFig();
    unsigned current = fig->HandlePropertyLookup("currentaxes");
    if (current == 0) {
      ArrayVector arg2;
      HAxesFunction(0,arg2);
      current = fig->HandlePropertyLookup("currentaxes");
    }
    return singleArrayVector(Array::uint32Constructor(current));
  }

  //!
  //@Module PVALID Validate Property Name
  //@@Section HANDLE
  //@@Usage
  //This function checks to see if the given string is a valid
  //property name for an object of the given type.  The syntax
  //for its use is
  //@[
  //  b = pvalid(type,propertyname)
  //@]
  //where @|string| is a string that contains the name of a 
  // valid graphics object type, and
  //@|propertyname| is a string that contains the name of the
  //property to test for.
  //@@Example
  //Here we test for some properties on an @|axes| object.
  //@<
  //pvalid('axes','type')
  //pvalid('axes','children')
  //pvalid('axes','foobar')
  //@>
  //!
  ArrayVector HPropertyValidateFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("pvalid requires two arguments, an object type name and a property name");
    std::string objectname = ArrayToString(arg[0]);
    HandleObject *fp;
    if (objectname == "axes")
      fp = new HandleAxis;
    else if (objectname == "line")
      fp = new HandleLineSeries;
    else if (objectname == "text")
      fp = new HandleText;
    else
      throw Exception("Unrecognized object type name " + objectname);
    std::string propname = ArrayToString(arg[1]);
    bool isvalid;
    isvalid = true;
    try {
      fp->LookupProperty(propname);
    } catch (Exception& e) {
      isvalid = false;
    }
    delete fp;
    return singleArrayVector(Array::logicalConstructor(isvalid));
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
  //\item @|jpg|, @|jpeg| -- JPEG file 
  //\item @|pdf| -- Portable Document File
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
  //print printfig1.pdf
  //print printfig1.jpg
  //@>
  //which creates two plots @|printfig1.pdf|, which is a Portable
  //Document Format file, and @|printfig1.jpg| which is a JPEG file.
  //@figure printfig1
  //!

  bool PrintBaseFigure(HandleWindow* g, std::string filename, 
		       std::string type) {
    bool retval;
    HPColor *color = (HPColor*) g->HFig()->LookupProperty("color");
    g->HFig()->SetThreeVectorDefault("color",1,1,1);
    if ((type == "PDF") || (type == "PS") || (type == "EPS")){
      QPrinter prnt;
      if (type == "PDF")
	prnt.setOutputFormat(QPrinter::PdfFormat);
      prnt.setOutputFileName(filename.c_str());
      QPainter pnt(&prnt);
      QTRenderEngine gc(&pnt,0,0,g->width(),g->height());
      g->HFig()->PaintMe(gc);
      retval = true;
    } else {
      // Binary print - use grabWidget
      QPixmap pxmap(QPixmap::grabWidget(g));
      QImage img(pxmap.toImage());
      retval = img.save(filename.c_str(),type.c_str());
    }
    g->HFig()->SetThreeVectorDefault("color",color->At(0),
				     color->At(1),color->At(2));
    return retval;
  }

  ArrayVector HPrintFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("print function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("print function takes a single, string argument");
    Array t(arg[0]);
    if (HcurrentFig == -1)
      return ArrayVector();
    HandleWindow *f = Hfigs[HcurrentFig];
    std::string outname(t.getContentsAsCString());
    int pos = outname.rfind(".");
    if (pos < 0)
      throw Exception("print function argument must contain an extension - which is used to determine the format for the output");
    std::string original_extension(outname.substr(pos+1,outname.size()));
    std::string modified_extension = 
      NormalizeImageExtension(original_extension);
    if (modified_extension.empty())
      throw Exception(std::string("unsupported output format ") + 
		      original_extension + " for print.\n" + 
		      FormatListAsString());
    if (!PrintBaseFigure(f,outname,modified_extension))
      throw Exception("Printing failed!");
    return ArrayVector();
  }

  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("axes",HAxesFunction,-1,1);
    context->addFunction("line",HLineFunction,-1,1);
    context->addFunction("text",HTextFunction,-1,1);
    context->addFunction("himage",HImageFunction,-1,1);
    context->addFunction("surface",HSurfaceFunction,-1,1);
    context->addFunction("set",HSetFunction,-1,0);
    context->addFunction("get",HGetFunction,2,1,"handle","propname");
    context->addFunction("figure",HFigureFunction,1,1);
    context->addFunction("gca",HGCAFunction,0,1);
    context->addFunction("gcf",HGCFFunction,0,1);
    context->addFunction("pvalid",HPropertyValidateFunction,2,1);
    context->addFunction("print",HPrintFunction,-1,0);
  };
}
