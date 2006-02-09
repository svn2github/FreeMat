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
#include "HandleCommands.hpp"
#include "HandleFigure.hpp"
#include <qgl.h>
#include <QClipboard>
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

// Subplot
// labels don't always appear properly.
// linestyle/symbol specification
// images

namespace FreeMat {

  QWidget *save = NULL;

  void SaveFocus() {
    save = qApp->focusWidget();
  }
  
  void RestoreFocus() {
    if (save)
      save->setFocus();
  }

  HandleWindow* Hfigs[MAX_FIGS];
  int HcurrentFig = -1;
  

  void InitializeHandleGraphics() {
    for (int i=0;i<MAX_FIGS;i++) Hfigs[i] = NULL;
    HcurrentFig = -1;
  }

  void ShutdownHandleGraphics() {
    for (int i=0;i<MAX_FIGS;i++) {
      if  (Hfigs[i]) {
	Hfigs[i]->hide();
	delete Hfigs[i];
      }
      Hfigs[i] = NULL;
    }
  }

  // Magic constant - limits the number of figures you can have...
  
  HandleList<HandleObject*> objectset;

  void NotifyFigureClosed(unsigned figNum) {
    //    delete Hfigs[figNum];
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
    if (handle == 0) return;
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


  //!
  //@Module FIGURE Figure Window Select and Create Function
  //@@Section HANDLE
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

  void HSetChildrenFunction(HandleObject *fp, Array children) {
    children.promoteType(FM_UINT32);
    const unsigned *dp = (const unsigned*) children.getDataPointer();
    // make sure they are all valid handles
    for (int i=0;i<children.getLength();i++) 
      ValidateHandle(dp[i]);
    // Retrieve the current list of children
    HandleObject *gp;
    HPHandles *hp = (HPHandles*) fp->LookupProperty("children");
    std::vector<unsigned> my_children(hp->Data());
    for (int i=0;i<my_children.size();i++) {
      unsigned handle = my_children[i];
      if (handle >= HANDLE_OFFSET_OBJECT) {
	gp = LookupHandleObject(handle);
	gp->Dereference();
      }
    }
    // Loop through the new list of children
    for (int i=0;i<children.getLength();i++) {
      unsigned handle = dp[i];
      if (handle >= HANDLE_OFFSET_OBJECT) {
	gp = LookupHandleObject(handle);
	gp->Reference();
      }
    }
    // Check for anyone with a zero reference count - it should
    // be deleted
    for (int i=0;i<my_children.size();i++) {
      unsigned handle = my_children[i];
      if (handle >= HANDLE_OFFSET_OBJECT) {
	gp = LookupHandleObject(handle);
	if (gp->RefCount() <= 0) {
	  //	  qDebug("Deleting handle %d\n",handle);
	  FreeHandleObject(handle);
	  delete gp;
	}
      }
    }
    // Call the generic set function now
    hp->Set(children);
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
      // Special case 'set' for 'children' - this can change reference counts
      if (propname == "children")
	HSetChildrenFunction(fp,arg[ptr+1]);
      else {
	try {
	  fp->LookupProperty(propname)->Set(arg[ptr+1]);
	} catch (Exception &e) {
	  throw Exception(std::string("Got error ") + std::string(e.getMessageCopy()) + std::string(" for property ") + propname);
	}
      }
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
    bool autoParentGiven = true;
    ArrayVector t(arg);
    while (t.size() >= 2) {
      std::string propname(ArrayToString(t[0]));
      if (propname == "autoparent") {
	std::string pval(ArrayToString(t[1]));
	autoParentGiven = (pval == "on");
      }	else {
	try {
	  fp->LookupProperty(propname)->Set(t[1]);
	} catch (Exception &e) {
	  throw Exception(std::string("Got error ") + std::string(e.getMessageCopy()) + std::string(" for property ") + propname);
	}
      }
      t.erase(t.begin(),t.begin()+2);
    }
    if (autoParentGiven) {
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
      axis->UpdateState();
    }
    fp->UpdateState();
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

  bool PrintBaseFigure(HandleWindow* g, std::string filename, 
		       std::string type) {
    bool retval;
    HPColor *color = (HPColor*) g->HFig()->LookupProperty("color");
    double cr, cg, cb;
    cr = color->At(0); cg = color->At(1); cb = color->At(2);
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
    g->HFig()->SetThreeVectorDefault("color",cr,cg,cb);
    return retval;
  }
  
  void CloseHelper(int fig) {
    if (fig == -1) return;
    if (Hfigs[fig] == NULL) return;
    Hfigs[fig]->hide();
    delete Hfigs[fig];
    Hfigs[fig] = NULL;
    if (HcurrentFig == fig)
      HcurrentFig = -1;
  }

  //!
  //@Module CLOSE Close Figure Window
  //@@Section HANDLE
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
  ArrayVector HCloseFunction(int nargout, const ArrayVector& arg) {
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
      if (HcurrentFig != -1) 
	CloseHelper(HcurrentFig);
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
  //@Module COPY Copy Figure Window
  //@@Section HANDLE
  //@@Usage
  //Copies the currently active figure window to the clipboard.
  //The syntax for its use is:
  //@[
  //   copy
  //@]
  //The resulting figure is copied as a bitmap to the clipboard, 
  //and can then be pasted into any suitable application.
  //!
  ArrayVector HCopyFunction(int nargout, const ArrayVector& arg) {
    if (HcurrentFig == -1)
      return ArrayVector();
    HandleWindow *f = Hfigs[HcurrentFig];
    // use grabWidget - doesnt work for openGL yet
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(QPixmap::grabWidget(f));
    return ArrayVector();
  }
  
  std::string NormalizeImageExtension(std::string ext) {
    std::string upperext(ext);
    std::string lowerext(ext);
    std::transform(upperext.begin(),upperext.end(),upperext.begin(),toupper);
    std::transform(lowerext.begin(),lowerext.end(),lowerext.begin(),tolower);
    if (upperext == "JPG") return std::string("JPEG");
    if ((upperext == "PDF") || (upperext == "PS") || (upperext == "EPS")) return upperext;
    QList<QByteArray> formats(QImageWriter::supportedImageFormats());
    for (int i=0;i<formats.count();i++) {
      if (formats.at(i).data() == upperext) return upperext;
      if (formats.at(i).data() == lowerext) return lowerext;
    }
    return std::string();
  }

  std::string FormatListAsString() {
    std::string ret_text = "Supported Formats: ";
    QList<QByteArray> formats(QImageWriter::supportedImageFormats());
    for (int i=0;i<formats.count();i++)
      ret_text = ret_text + formats.at(i).data() + " ";
    return ret_text;
  }

  //!
  //@Module PRINT Print a Figure To A File
  //@@Section HANDLE
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
  //\item @|pdf| -- Portable Document Format file
  //\item @|png| -- Portable Net Graphics file
  //\end{itemize}
  //Postscript (PS, EPS) is supported on non-Mac-OSX Unix only.
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
  //print help/latex/printfig1.jpg
  //print help/html/printfig1.png
  //@>
  //which creates two plots @|printfig1.png|, which is a Portable
  //Net Graphics file, and @|printfig1.jpg| which is a JPEG file.
  //@figure printfig1
  //!
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

  ArrayVector HPointFunction(int nargout, const ArrayVector& arg) {
    if (HcurrentFig == -1)
      return ArrayVector();
    HandleWindow *f = Hfigs[HcurrentFig];
    f->raise();
    f->activateWindow();
    f->setFocus(Qt::OtherFocusReason);
    int x, y;
    f->GetClick(x,y);
    Array retval(Array::doubleVectorConstructor(2));
    double *d_ip;
    d_ip = (double*) retval.getReadWriteDataPointer();
    d_ip[0] = (double) x;
    d_ip[1] = (double) y;
    return singleArrayVector(retval);
  }

  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("axes",HAxesFunction,-1,1);
    context->addFunction("line",HLineFunction,-1,1);
    context->addFunction("htext",HTextFunction,-1,1);
    context->addFunction("himage",HImageFunction,-1,1);
    context->addFunction("surface",HSurfaceFunction,-1,1);
    context->addFunction("set",HSetFunction,-1,0);
    context->addFunction("get",HGetFunction,2,1,"handle","propname");
    context->addFunction("figure",HFigureFunction,1,1,"number");
    context->addFunction("gca",HGCAFunction,0,1);
    context->addFunction("gcf",HGCFFunction,0,1);
    context->addFunction("pvalid",HPropertyValidateFunction,2,1,"type","property");
    context->addFunction("print",HPrintFunction,-1,0);
    context->addFunction("close",HCloseFunction,1,0,"handle");
    context->addFunction("copy",HCopyFunction,0,0);
    context->addFunction("hpoint",HPointFunction,0,1);
    // Need colormap, zoom, colorbar, addtxt, sizefig, winlev, point, legend
    InitializeHandleGraphics();
  };
}
