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

#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"


#include <qgl.h>
#include <QtGui>
#include <QtSvg>
#include <ctype.h>
#include <algorithm>
#include "HandleLineSeries.hpp"
#include "HandleObject.hpp"
#include "HandleText.hpp"
#include "HandleAxis.hpp"
#include "HandleImage.hpp"
#include <qapplication.h>
#include "HandleList.hpp"
#include "HandleSurface.hpp"
#include "HandleWindow.hpp"
#include "HandleContour.hpp"
#include "HandleUIControl.hpp"
#include "QTRenderEngine.hpp"
#include "Interpreter.hpp"
#include <math.h>

// Subplot
// labels don't always appear properly.
// linestyle/symbol specification
// images

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

static bool HGInitialized = false;

void InitializeHandleGraphics() {
  if (HGInitialized) return;
  for (int i=0;i<MAX_FIGS;i++) Hfigs[i] = NULL;
  HcurrentFig = -1;
  HGInitialized = true;
}

void ShutdownHandleGraphics() {
  for (int i=0;i<MAX_FIGS;i++) {
    if  (Hfigs[i]) {
      Hfigs[i]->hide();
      delete Hfigs[i];
    }
    Hfigs[i] = NULL;
  }
  HcurrentFig = -1;
}

// Magic constant - limits the number of figures you can have...
  
HandleList<HandleObject*> objectset;

void NotifyFigureClosed(unsigned figNum) {
  //    delete Hfigs[figNum];
  Hfigs[figNum] = NULL;
  if (((int)figNum) == HcurrentFig)
    HcurrentFig = -1;
  // Check for all figures closed
  bool allClosed = true;
  for (int i=0;allClosed && i<MAX_FIGS;i++)
    allClosed = Hfigs[i] == NULL;
}

void NotifyFigureActive(unsigned figNum) {
  HcurrentFig = figNum;
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

static HandleWindow* CurrentWindow() {
  if (HcurrentFig == -1)
    NewFig();
  return (Hfigs[HcurrentFig]);
}

static HandleFigure* CurrentFig() {
  if (HcurrentFig == -1)
    NewFig();
  return (Hfigs[HcurrentFig]->HFig());
}


static void SelectFig(int fignum) {
  if (fignum < 0)
    throw Exception("Illegal argument to SelectFig");
  if (Hfigs[fignum] == NULL)
    Hfigs[fignum] = new HandleWindow(fignum);
  SaveFocus();
  Hfigs[fignum]->show();
  Hfigs[fignum]->raise();
  RestoreFocus();
  HcurrentFig = fignum;
}

//!
//@Module DRAWNOW Flush the Event Queue
//@@Section HANDLE
//@@Usage
//The @|drawnow| function can be used to process the events in the
//event queue of the FreeMat application.  The syntax for its use
//is
//@[
//   drawnow
//@]
//Now that FreeMat is threaded, you do not generally need to call this
//function, but it is provided for compatibility.
//!

bool AnyDirty(bool issueUpdates) {
  bool retval = false;
  if (!HGInitialized) return false;
  for (int i=0;i<MAX_FIGS;i++) {
    if (Hfigs[i] && (Hfigs[i]->isDirty()))  {
      retval = true;
      if (issueUpdates)
	Hfigs[i]->repaint();
    }
  }
  return retval;
}

static bool in_DoDrawNow = false;

void DoDrawNow() {
  if (in_DoDrawNow) return;
  in_DoDrawNow = true;
  if (AnyDirty(true))
    while (AnyDirty(false))
      qApp->processEvents();
  in_DoDrawNow = false;
}

ArrayVector DrawNowFunction(int nargout, const ArrayVector& arg) {
  GfxEnableRepaint();
  DoDrawNow();
  GfxDisableRepaint();
  return ArrayVector();
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
//Changes the active figure window to the specified figure
//number.  The general syntax for its use is 
//@[
//  figure(number)
//@]
//where @|number| is the figure number to use. If the figure window 
//corresponding to @|number| does not already exist, a new 
//window with this number is created.  If it does exist
//then it is brought to the forefront and made active.
//You can use @|gcf| to obtain the number of the current figure.
//
//Note that the figure number is also the handle for the figure.
//While for most graphical objects (e.g., axes, lines, images), the
//handles are large integers, for figures, the handle is the same
//as the figure number.  This means that the figure number can be
//passed to @|set| and @|get| to modify the properties of the
//current figure, (e.g., the colormap).  So, for figure @|3|, for 
//example, you can use @|get(3,'colormap')| to retrieve the colormap
//for the current figure.
//!
ArrayVector HFigureFunction(int nargout,const ArrayVector& arg) {
  if (arg.size() == 0) {
    NewFig();
    return SingleArrayVector(Array::int32Constructor(HcurrentFig+1));
  } else {
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_FIGS))
      throw Exception("figure number is out of range - it must be between 1 and 50");
    SelectFig(fignum-1);
    return ArrayVector();
  }
}

void AddToCurrentFigChildren(unsigned handle) {
  HandleFigure *fig = CurrentFig();
  HPHandles *cp = (HPHandles*) fig->LookupProperty("children");
  QVector<unsigned> children(cp->Data());
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
      t.pop_front();
      t.pop_front();
    }
    // Get the current figure
    HandleFigure *fig = CurrentFig();
    fp->SetPropertyHandle("parent",HcurrentFig+1);
    fig->SetPropertyHandle("currentaxes",handle);
    // Add us to the children...
    AddToCurrentFigChildren(handle);
    fp->UpdateState();
    return SingleArrayVector(Array::uint32Constructor(handle));
  } else {
    unsigned int handle = (unsigned int) ArrayToInt32(arg[0]);
    HandleObject* hp = LookupHandleObject(handle);
    if (!hp->IsType("axes"))
      throw Exception("single argument to axes function must be handle for an axes"); 
    AddToCurrentFigChildren(handle);
    // Get the current figure
    CurrentFig()->SetPropertyHandle("currentaxes",handle);     
    CurrentFig()->UpdateState();
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
  QVector<unsigned> my_children(hp->Data());
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
	if (gp->StringPropertyLookup("type") == "uicontrol")
	  ((HandleUIControl*) gp)->Hide();
	FreeHandleObject(handle);
	delete gp;
      } 
    }
  }
  // Call the generic set function now
  hp->Set(children);
}


//!
//@Module SET Set Object Property
//@@Section HANDLE
//@@Usage
//This function allows you to change the value associated
//with a property.  The syntax for its use is
//@[
//  set(handle,property,value,property,value,...)
//@]
//where @|property| is a string containing the name of the
//property, and @|value| is the value for that property. The
//type of the variable @|value| depends on the property being
//set.  See the help for the properties to see what values
//you can set.
//!
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
    // Special case 'set' for 'figsize' - this requires help from the OS
    if (propname == "children")
      HSetChildrenFunction(fp,arg[ptr+1]);
    else if ((handle < HANDLE_OFFSET_OBJECT) && (propname == "figsize")) {
      fp->LookupProperty(propname)->Set(arg[ptr+1]);
      HandleFigure *fig = (HandleFigure*) fp;
      fig->SetSize();
    } else {
      try {
	fp->LookupProperty(propname)->Set(arg[ptr+1]);
      } catch (Exception &e) {
	throw Exception(std::string("Got error ") + std::string(e.getMessageCopy()) + std::string(" for property ") + propname);
      }
    }
    ptr+=2;
  }
  fp->UpdateState();
  if (!fp->IsType("figure") && !fp->IsType("uicontrol")) {
    HandleFigure *fig = fp->GetParentFigure();
    fig->UpdateState();
    fig->Repaint();
  }
  return ArrayVector();
}

//!
//@Module GET Get Object Property
//@@Section HANDLE
//@@Usage
//This function allows you to retrieve the value associated
//with a property.  The syntax for its use is
//@[
//  value = get(handle,property)
//@]
//where @|property| is a string containing the name of the
//property, and @|value| is the value for that property. The
//type of the variable @|value| depends on the property being
//set.  See the help for the properties to see what values
//you can set.
//!
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
  return SingleArrayVector(fp->LookupProperty(propname)->Get());
}

unsigned GenericConstructor(HandleObject* fp, const ArrayVector& arg,
			    bool autoParentGiven = true) {
  unsigned int handle = AssignHandleObject(fp);
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
    t.pop_front();
    t.pop_front();
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
    QVector<unsigned> children(cp->Data());
    children.push_back(handle);
    cp->Data(children);
    cp = (HPHandles*) fp->LookupProperty("parent");
    QVector<unsigned> parent;
    parent.push_back(current);
    cp->Data(parent);
    axis->UpdateState();
  }
  fp->UpdateState();
  return handle;
}


//!
//@Module HLINE Create a line object
//@@Section HANDLE
//@@Usage
//Creates a line object and parents it to the current axis.  The
//syntax for its use is 
//@[
//  handle = hline(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current axis.
//!
ArrayVector HLineFunction(int nargout, const ArrayVector& arg) {
  return SingleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleLineSeries,arg)));
}

//!
//@Module HCONTOUR Create a contour object
//@@Section HANDLE
//@@Usage
//Creates a contour object and parents it to the current axis.  The
//syntax for its use is 
//@[
//  handle = hcontour(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current axis.
//!
ArrayVector HContourFunction(int nargout, const ArrayVector& arg) {
  return SingleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleContour,arg)));
}

//!
//@Module UICONTROL Create a UI Control object
//@@Section HANDLE
//@@Usage
//Creates a UI control object and parents it to the current figure.  The
//syntax for its use is
//@[
//  handle = uicontrol(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current figure.
//!
ArrayVector HUIControlFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  HandleUIControl *o = new HandleUIControl;
  o->SetEvalEngine(eval);
  o->ConstructWidget(CurrentWindow());
  unsigned handleID = GenericConstructor(o,arg,false);
  // Parent the control to the current figure
  AddToCurrentFigChildren(handleID);
  HPHandles* cp = (HPHandles*) o->LookupProperty("parent");
  QVector<unsigned> parent;
  parent.push_back(HcurrentFig+1);
  cp->Data(parent);
  return SingleArrayVector(Array::uint32Constructor(handleID));
}

//!
//@Module HIMAGE Create a image object
//@@Section HANDLE
//@@Usage
//Creates a image object and parents it to the current axis.  The
//syntax for its use is 
//@[
//  handle = himage(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current axis.
//!
ArrayVector HImageFunction(int nargout, const ArrayVector& arg) {
  return SingleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleImage,arg)));
}

//!
//@Module HTEXT Create a text object
//@@Section HANDLE
//@@Usage
//Creates a text object and parents it to the current axis.  The
//syntax for its use is 
//@[
//  handle = htext(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current axis.
//!
ArrayVector HTextFunction(int nargout, const ArrayVector& arg) {
  return SingleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleText,arg)));
}

//!
//@Module HSURFACE Create a surface object
//@@Section HANDLE
//@@Usage
//Creates a surface object and parents it to the current axis.  The
//syntax for its use is 
//@[
//  handle = hsurface(property,value,property,value,...)
//@]
//where @|property| and @|value| are set.  The handle ID for the
//resulting object is returned.  It is automatically added to
//the children of the current axis.
//!
ArrayVector HSurfaceFunction(int nargout, const ArrayVector& arg) {
  return SingleArrayVector(Array::uint32Constructor(GenericConstructor(new HandleSurface,arg)));
}

//!
//@Module FIGRAISE Raise a Figure Window
//@@Section HANDLE
//@@Usage
//Raises a figure window indicated by the figure number.  The syntax for
//its use is
//@[
//  figraise(fignum)
//@]
//where @|fignum| is the number of the figure to raise.  The figure will
//be raised to the top of the GUI stack (meaning that it we be visible).
//Note that this function does not cause @|fignum| to become the current
//figure, you must use the @|figure| command for that.
//!
ArrayVector FigRaiseFunction(int nargout, const ArrayVector& args) {
  if (args.size() == 0)
    CurrentWindow()->raise();
  else {
    int fignum = ArrayToInt32(args[0]);
    if ((fignum >= 1) && (fignum < MAX_FIGS+1)) {
      if (Hfigs[fignum-1])
	Hfigs[fignum-1]->raise();
      else {
	Hfigs[fignum-1] = new HandleWindow(fignum-1);
	Hfigs[fignum-1]->show();
	Hfigs[fignum-1]->raise();
      }
    }
  }
  return ArrayVector();
}

//!
//@Module FIGLOWER Lower a Figure Window
//@@Section HANDLE
//@@Usage
//Lowers a figure window indicated by the figure number.  The syntax for
//its use is
//@[
//  figlower(fignum)
//@]
//where @|fignum| is the number of the figure to lower.  The figure will
//be lowerd to the bottom of the GUI stack (meaning that it we be behind other
//windows).  Note that this function does not cause @|fignum| to 
//become the current  figure, you must use the @|figure| command for that.
//Similarly, if @|fignum| is the current figure, it will remain the current
//figure (even though the figure is now behind others).
//!
ArrayVector FigLowerFunction(int nargout, const ArrayVector& args) {
  if (args.size() == 0)
    CurrentWindow()->lower();
  else {
    int fignum = ArrayToInt32(args[0]);
    if ((fignum >= 1) && (fignum < MAX_FIGS+1)) {
      if (Hfigs[fignum-1])
	Hfigs[fignum-1]->lower();
      else {
	Hfigs[fignum-1] = new HandleWindow(fignum-1);
	Hfigs[fignum-1]->show();
	Hfigs[fignum-1]->lower();
      }
    }
  }
  return ArrayVector();
}

//!
//@Module GCF Get Current Figure
//@@Section HANDLE
//@@Usage
//Returns the figure number for the current figure (which is also its handle,
//and can be used to set properties of the current figure using @|set|).  
//The syntax for its use
//is
//@[
//  figure_number = gcf
//@]
//where @|figure_number| is the number of the active figure (also the handle of
//the figure).
//
//Note that figures have handles, just like axes, images, plots, etc.  However
//the handles for figures match the figure number (while handles for other 
//graphics objects tend to be large, somewhat arbitrary integers).  So, to 
//retrieve the colormap of the current figure, you could 
//use @|get(gcf,'colormap')|, or to obtain the colormap for figure 3, 
//use @|get(3,'colormap')|.
//!
ArrayVector HGCFFunction(int nargout, const ArrayVector& arg) {
  if (HcurrentFig == -1)
    NewFig();      
  return SingleArrayVector(Array::uint32Constructor(HcurrentFig+1));
}



//!
//@Module GCA Get Current Axis
//@@Section HANDLE
//@@Usage
//Returns the handle for the current axis.  The syntax for its use
//is
//@[
//  handle = gca
//@]
//where @|handle| is the handle of the active axis.  All object
//creation functions will be children of this axis.
//!
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
  return SingleArrayVector(Array::uint32Constructor(current));
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
  return SingleArrayVector(Array::logicalConstructor(isvalid));
}

bool PrintBaseFigure(HandleWindow* g, std::string filename, 
		     std::string type) {
  bool retval;
  HPColor *color = (HPColor*) g->HFig()->LookupProperty("color");
  double cr, cg, cb;
  cr = color->At(0); cg = color->At(1); cb = color->At(2);
  g->HFig()->SetThreeVectorDefault("color",1,1,1);
  GfxEnableRepaint();
  g->UpdateState();
  while (g->isDirty())
    qApp->processEvents();
  if ((type == "PDF") || (type == "PS") || (type == "EPS")){
    QPrinter prnt;
    if (type == "PDF")
      prnt.setOutputFormat(QPrinter::PdfFormat);
    else
      prnt.setOutputFormat(QPrinter::PostScriptFormat);
    prnt.setOutputFileName(filename.c_str());
    QPainter pnt(&prnt);
    QTRenderEngine gc(&pnt,0,0,g->width(),g->height());
    g->HFig()->PaintMe(gc);
    retval = true;
  } else if (type == "SVG") {
    QSvgGenerator gen;
    gen.setFileName(QString::fromStdString(filename));
    gen.setSize(QSize(g->width(),g->height()));
    QPainter pnt(&gen);
    QTRenderEngine gc(&pnt,0,0,g->width(),g->height());
    g->HFig()->PaintMe(gc);
    retval = true;
  } else {
    // Binary print - use grabWidget
    QPixmap pxmap(QPixmap::grabWidget(g->GetQtWidget()));
    QImage img(pxmap.toImage());
    retval = img.save(filename.c_str(),type.c_str());
  }
  g->HFig()->SetThreeVectorDefault("color",cr,cg,cb);
  g->UpdateState();
  while (g->isDirty())
    qApp->processEvents();
  GfxDisableRepaint();
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
      string allflag = t.getContentsAsString();
      if (allflag == "all") 
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
    HcurrentFig = -1;
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
  
static std::string NormalizeImageExtension(std::string ext) {
  std::string upperext(ext);
  std::string lowerext(ext);
  std::transform(upperext.begin(),upperext.end(),upperext.begin(),
		 (int(*)(int))toupper);
  std::transform(lowerext.begin(),lowerext.end(),lowerext.begin(),
		 (int(*)(int))tolower);
  if (upperext == "JPG") return std::string("JPEG");
  if ((upperext == "SVG") || (upperext == "PDF") || 
      (upperext == "PS") || (upperext == "EPS")) return upperext;
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
//\item @|svg| -- Scalable Vector Graphics file
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
//print('printfig1.jpg')
//print('printfig1.png')
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
  std::string outname(t.getContentsAsString());
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

//!
//@Module HPOINT Get Point From Window
//@@Section HANDLE
//@@Usage
//This function waits for the user to click on the current figure
//window, and then returns the coordinates of that click.  The
//generic syntax for its use is
//@[
//  [x,y] = hpoint
//@]
//!
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
  return SingleArrayVector(retval);
}

//!
//@Module IS2DVIEW Test Axes For 2D View
//@@Section HANDLE
//@@Usage
//This function returns @|true| if the current axes are in a
//2-D view, and false otherwise.  The generic syntax for its
//use is
//@[
//  y = is2dview(x)
//@]
//where @|x| is the handle of an axes object.
//!
ArrayVector HIs2DViewFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) throw Exception("is2DView expects a handle to axes");
  unsigned int handle = (unsigned int) ArrayToInt32(arg[0]);
  HandleObject* hp = LookupHandleObject(handle);
  if (!hp->IsType("axes"))
    throw Exception("single argument to axes function must be handle for an axes"); 
  HandleAxis *axis = (HandleAxis*) hp;
  return SingleArrayVector(Array::logicalConstructor(axis->Is2DView()));
}

#if 0
class contour_point {
public:
  double x;
  double y;
  inline contour_point(double a, double b) : x(a), y(b) {};
};

inline bool operator==(const contour_point& p1, const contour_point& p2) {
  return ((p1.x == p2.x) && (p1.y == p2.y));
}

typedef QList<contour_point> cline;
typedef QList<cline> lineset;

inline cline Reverse(const cline& src) {
  cline ret;
  for (int i=src.size()-1;i>=0;i--)
    ret << src.at(i);
  return ret;
}

inline bool Connected(const cline& current, const cline& test) {
  return ((current.front() == test.front()) || 
	  (current.front() == test.back()) ||
	  (current.back() == test.front()) ||
	  (current.back() == test.back()));
}

inline void Join(cline& current, const cline& toadd) {
  if (current.front() == toadd.front())
    current = Reverse(toadd) + current;
  else if (current.front() == toadd.back())
    current = toadd + current;
  else if (current.back() == toadd.front())
    current += toadd;
  else if (current.back() == toadd.back())
    current += Reverse(toadd);
}

#define FOLD(x) MAP((x),row-1)
#define FNEW(x) MAP((x),row)
#define MAP(x,y) func[(y)+(x)*numy]
#define AINTER(a,b) ((val-(a))/((b)-(a)))
#define ALEFT(i,j) (((j)-1)+AINTER(FOLD((i)-1),FNEW((i)-1)))
#define TOP(i) (((i)-1)+AINTER(FNEW((i)-1),FNEW((i))))
#define BOT(i) (((i)-1)+AINTER(FOLD((i)-1),FOLD((i))))
#define RIGHT(i,j) (((j)-1)+AINTER(FOLD((i)),FNEW((i))))
#define DRAW(a,b,c,d) {allLines << (cline() << contour_point((double)a,(double)b) << contour_point((double)c,(double)d));}

ArrayVector ContourCFunction(int nargout, const ArrayVector& arg) {
  lineset allLines;
  lineset bundledLines;
  if (arg.size() < 2) throw Exception("contourc expects two arguments");
  double val = ArrayToDouble(arg[1]);
  if (!arg[0].is2D())
    throw Exception("First argument to contourc must be a 2D matrix");
  Array m(arg[0]);
  m.promoteType(FM_DOUBLE);
  const double *func = (const double *) m.getDataPointer();
  int outcnt = 0;
  int numy = m.rows();
  int numx = m.columns();
  for (int row=1;row<numy;row++)
    for (int col=1;col<numx;col++) {
      int l = 0;
      if (FOLD(col) >= val) l  = l + 1;
      if (FOLD(col-1) >= val) l = l + 2;
      if (FNEW(col) >= val) l = l + 4;
      if (FNEW(col-1) >= val) l = l + 8;
      switch (l) {
      case 1:
      case 14:
	DRAW(BOT(col),row-1,col,RIGHT(col,row));
	break;
      case 2:
      case 13:
	DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	break;
      case 3:
      case 12:
	DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	break;
      case 4:
      case 11:
	DRAW(TOP(col),row,col,RIGHT(col,row));
	break;
      case 5:
      case 10:
	DRAW(BOT(col),row-1,TOP(col),row);
	break;
      case 6:
      case 9:
	{
	  double x0 = AINTER(FOLD(col-1),FOLD(col));
	  double x1 = AINTER(FNEW(col-1),FNEW(col));
	  double y0 = AINTER(FOLD(col-1),FNEW(col-1));
	  double y1 = AINTER(FOLD(col),FNEW(col));
	  double y = (x0*(y1-y0)+y0)/(1.0-(x1-x0)*(y1-y0));
	  double x = y*(x1-x0) + x0;
	  double fx1 = MAP(col-1,row-1)+x*(MAP(col,row-1)-MAP(col-1,row-1));
	  double fx2 = MAP(col-1,row)+x*(MAP(col,row)-MAP(col-1,row));
	  double f = fx1 + y*(fx2-fx1);
	  if (f==val) {
	    DRAW(BOT(col),row-1,TOP(col),row);
	    DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	  } else if (((f > val) && (FNEW(col) > val)) || 
		     ((f < val) && (FNEW(col) < val))) {
	    DRAW(col-1,ALEFT(col,row),TOP(col),row);
	    DRAW(BOT(col),row-1,col,RIGHT(col,row));
	  } else {
	    DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	    DRAW(TOP(col),row,col,RIGHT(col,row));
	  }
	}
	break;
      case 7:
      case 8:
	DRAW(col-1,ALEFT(col,row),TOP(col),row);
	break;
      }
    }
  // Now we link the line segments into longer lines.
  int allcount = allLines.size();
  while (!allLines.empty()) {
    // Start a new line segment
    cline current(allLines.takeAt(0));
    bool lineGrown = true;
    while (lineGrown) {
      lineGrown = false;
      int i = 0;
      while (i<allLines.size()) {
	if (Connected(current,allLines.at(i))) {
	  Join(current,allLines.takeAt(i));
	  lineGrown = true;
	} else
	  i++;
      }
    }
    bundledLines << current;
  }
  int outcount = bundledLines.size() + 2*allcount + 1;
  Array out(Array::doubleMatrixConstructor(2,outcount));
  double *output = (double *) out.getReadWriteDataPointer();
  for (int i=0;i<bundledLines.size();i++) {
    *output++ = val;
    *output++ = bundledLines[i].size();
    cline bline(bundledLines[i]);
    for (int j=0;j<bline.size();j++) {
      *output++ = bline[j].x;
      *output++ = bline[j].y;
    }
  }
  return ArrayVector() << out;
}
#endif 

void LoadHandleGraphicsFunctions(Context* context) {
  context->addGfxFunction("is2dview",HIs2DViewFunction,1,1,"x",NULL);
  context->addGfxFunction("axes",HAxesFunction,-1,1,NULL);
  context->addGfxFunction("hline",HLineFunction,-1,1,NULL);
  context->addGfxFunction("htext",HTextFunction,-1,1,NULL);
  context->addGfxFunction("himage",HImageFunction,-1,1,NULL);
  context->addGfxFunction("hcontour",HContourFunction,-1,1,NULL);
  context->addGfxFunction("surface",HSurfaceFunction,-1,1,NULL);
  context->addGfxFunction("set",HSetFunction,-1,0,NULL);
  context->addGfxFunction("get",HGetFunction,2,1,"handle","propname",NULL);
  context->addGfxFunction("figure",HFigureFunction,1,1,"number",NULL);
  context->addGfxSpecialFunction("uicontrol",HUIControlFunction,-1,1,NULL);
  context->addGfxFunction("gca",HGCAFunction,0,1,NULL);
  context->addGfxFunction("gcf",HGCFFunction,0,1,NULL);
  context->addGfxFunction("pvalid",HPropertyValidateFunction,2,1,"type","property",NULL);
  context->addGfxFunction("print",HPrintFunction,-1,0,NULL);
  context->addGfxFunction("close",HCloseFunction,1,0,"handle",NULL);
  context->addGfxFunction("copy",HCopyFunction,0,0,NULL);
  context->addGfxFunction("hpoint",HPointFunction,0,1,NULL);
  context->addGfxFunction("drawnow",DrawNowFunction,0,0,NULL);
  context->addGfxFunction("figraise",FigRaiseFunction,1,0,"handle",NULL);
  context->addGfxFunction("figlower",FigLowerFunction,1,0,"handle",NULL);
  //  context->addFunction("contourc",ContourCFunction,2,1,"z","v",NULL);
  //  context->addSpecialFunction("demo",HDemoFunction,1,1,NULL);
  InitializeHandleGraphics();
};
