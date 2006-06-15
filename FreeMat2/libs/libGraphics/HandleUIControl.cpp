#include "HandleUIControl.hpp"
#include "HandleWindow.hpp"
#include "WalkTree.hpp"
#include <QtGui>

namespace FreeMat {
  HandleUIControl::HandleUIControl() {
    ConstructProperties();
    SetupDefaults();
    widget = NULL;
  }
  
  HandleUIControl::~HandleUIControl() {
  }

  void HandleUIControl::SetEvalEngine(WalkTree *eval) {
    m_eval = eval;
  }

  void HandleUIControl::ConstructWidget(HandleWindow *f) {
    if (widget) delete widget;
    widget = NULL;
    if (StringCheck("style","pushbutton")) {
      widget = new QPushButton(QString::fromStdString(StringPropertyLookup("string")),
			       f->GetQtWidget());
      connect(widget,SIGNAL(clicked()),this,SLOT(clicked()));
    }
    std::vector<double> bgcolor(VectorPropertyLookup("backgroundcolor"));
    if (!widget) return;
    std::vector<double> sizevec(VectorPropertyLookup("position"));
    widget->setGeometry(sizevec[0],sizevec[1],sizevec[2],sizevec[3]);
    widget->show();
  }

  void HandleUIControl::clicked() {
    char *tmp = strdup(StringPropertyLookup("callback").c_str());
    m_eval->evaluateString(tmp,false);
  }

  void HandleUIControl::ConstructProperties() {
    //!
    //@Module UICONTROLPROPERTIES UI Control Properties
    //@@Section HANDLE
    //@@Usage
    //Below is a summary of the properties for user interface
    //controls.
    //\begin{itemize}
    //\item @|backgroundcolor| - @|colorspec| - The background color
    // for the widget.
    //\item @|busyaction| - Not used.
    //\item @|buttondownfcn| - Not used.
    //\item @|callback| - @|string| - the callback to execute when the 
    //GUI control does its action.  Clicking a button or moving a 
    //scroller will cause the callback to be executed.  Also, pressing 
    //enter in a text box causes the callback to be executed.
    //\item @|cdata| - an @|M x N x 3| array that represents an RGB image
    //to use as the truecolor image displayed on push bottons or toggle
    //buttons.  The values must be between 0 and 1.
    //\item @|children| - Not used.
    //\item @|createfcn| - Not used.
    //\item @|deletefcn| - Not used;
    //\item @|enable| - @|{'on','inactive','off'}| - For @|on| (the
    //default) the uicontrol behaves normally.  For inactive, it is not
    //operational, but looks the same as @|on|.  For @|off|, the
    //control is grayed out.
    //\item @|extent| - a read only property that contains the extent of
    //the text for the control.
    //\item @|fontangle| - @|{'normal','italic','oblique'}| - The angle of the fonts used
    // for text labels (e.g., tick labels).
    //\item @|fontsize| - @|scalar| - The size of fonts used for text labels (tick labels).
    //\item @|fontunits| - Not used.
    //\item @|fontname| - @|string| - The name of the font to use for the widget.
    //\item @|fontweight| - @|{'normal','bold','light','demi'}| - The weight of the font used
    //\item @|foregroundcolor| - @|colorspec| - the foreground color for text.
    //  \item @|handlevisibility| - Not used.
    //  \item @|hittest| - Not used.
    //\item @|horizontalalignment| - @|{'left','center','right}| - determines
    //the justification of text.
    //\item @|interruptible| - Not used.
    //\item @|keypressfcn| - @|functionspec| - a string or function handle
    // that is called when a key is pressed and a uicontrol object has focus.
    //\item @|listboxtop| - a scalar (used only by the listbox style of
    //uicontrols) that specifies which string appears at the top of the list
    //box.
    //\item @|max| - a scalar that specifies the largest value allowed
    //for the @|value| property.  The interpretation varies depending on
    //the type of the control
    //\begin{itemize}
    //\item @|check boxes| - specifies what @|value| is set to when the
    //check box is selected.
    //\item @|edit box| - if @|max-min>1| then the text box allows for
    //multiple lines of input.  Otherwise, it is a single line only.
    //\item @|list box| - if @|max-min>1| then multiple item selections
    //are allowed.  Otherwise, only single item selections are allowed.
    //\item @|radio buttons| - specifies what @|value| is set to when the
    //radio button is selected.
    //\item @|slider| - the maximum value the slider can take.
    //\item @|toggle button| - specifies what @|value| is set to when 
    //the toggle button is selected.
    //\end{itemize}
    //\item @|min| - a scalar that specifies the smallest value for the
    //@|value| property.  The interpretation of it depends on the type
    //of the control
    //\begin{itemize}
    //\item @|check boxes| - specifies what @|value| is set to when the
    //check box is not selected.
    //\item @|edit box| - if @|max-min>1| then the text box allows for
    //multiple lines of input.  Otherwise, it is a single line only.
    //\item @|list box| - if @|max-min>1| then multiple item selections
    //are allowed.  Otherwise, only single item selections are allowed.
    //\item @|radio buttons| - specifies what @|value| is set to when the
    //radio button is not selected.
    //\item @|slider| - the minimum value the slider can take.
    //\item @|toggle button| - specifies what @|value| is set to when 
    //the toggle button is not selected.
    //\end{itemize}
    //\item @|parent| - the handle of the parent object.
    //\item @|position| - size and location of the uicontrol as a 
    //four vector @|[left, bottom, width, height]|.  If @|width>height|
    //then sliders are horizontal, otherwise the slider is oriented
    //vertically.
    //\item @|selected| - @|{'on','off'}| - not used.
    //\item @|selectionhighlight| - @|{'on','off'}| - not used.
    //\item @|sliderstep| - a two vector @|[min_step max_step]|
    //that controls the amount the slider @|value| changes when
    //you click the mouse on the control.  If you click the arrow
    //for the slider, the value changes by @|min_step|, while if
    //you click the trough, the value changes by @|max_step|.  
    //Each value must be in the range @|[0,1]|, and is a percentage
    //of the range @|max-min|.
    //\item @|string| - @|string| - the text for the control.
    //\item @|style| - @|{'pushbutton','togglebutton','radiobutton','checkbox',
    //'edit','text','slider','frame','listbox','popupmenu'}|.
    //\item @|tag| - @|string| - user specified label.
    //\item @|tooltipstring| - @|string| the tooltip for the control.
    //\item @|type| - @|string| - the text is set to @|'uicontrol'|.
    //\item @|uicontextmenu| - @|handle| the handle of the @|uicontextmenu|
    //that shows up when you right-click over the control.
    //\item @|units| - not used.
    //\item @|userdata| - @|array| - any data you want to associate with the
    //control.
    //\item @|value| - The meaning of this property depends on the type of the
    //control:
    //\begin{itemize}
    //\item check box - set to @|max| when checked, and @|min| when off.
    //\item list box - set to a vector of indices corresponding to selected
    //items, with @|1| corresponding to the first item in the list.
    //\item pop up menu - set to the index of the item selected.
    //\item radio buttons - set to @|max| when selected, and set to @|min| when
    //not selected.
    //\item sliders - set to the value of the slider
    //\item toggle buttons - set to @|max| when selected, and set to @|min| when
    //not selected.
    //\item text controls, push buttons - do not use this property.
    //\end{itemize}
    //\item @|visible| - @|{'on','off'}| - controls whether the control is 
    //visible or not
    //\end{itemize}
    //!
    AddProperty(new HPColor,"backgroundcolor");
    AddProperty(new HPString,"busyaction");
    AddProperty(new HPString,"buttondownfcn");
    AddProperty(new HPString,"callback");
    AddProperty(new HPArray,"cdata");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPString,"createfcn");
    AddProperty(new HPString,"deletefcn");
    AddProperty(new HPOnOffInactive,"enable");
    AddProperty(new HPFontAngle,"fontangle");
    AddProperty(new HPString,"fontname");
    AddProperty(new HPScalar,"fontsize");
    AddProperty(new HPFontUnits,"fontunits");
    AddProperty(new HPFontWeight,"fontweight");
    AddProperty(new HPColor,"foregroundcolor");
    AddProperty(new HPOnOff,"handlevisibility");
    AddProperty(new HPOnOff,"hittest");
    AddProperty(new HPAlignHoriz,"horizontalalignment");
    AddProperty(new HPOnOff,"interruptible");
    AddProperty(new HPString,"keypressfcn");
    AddProperty(new HPScalar,"listboxtop");
    AddProperty(new HPScalar,"max");
    AddProperty(new HPScalar,"min");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPFourVector,"position");
    AddProperty(new HPOnOff,"selected");
    AddProperty(new HPOnOff,"selectionhighlight");
    AddProperty(new HPTwoVector,"sliderstep");
    AddProperty(new HPString,"string");
    AddProperty(new HPWidgetString,"style");
    AddProperty(new HPString,"tag");
    AddProperty(new HPString,"tooltipstring");
    AddProperty(new HPString,"type");
    AddProperty(new HPHandles,"uicontextmenu");
    AddProperty(new HPUnits,"units");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPVector,"value");
  }

  void HandleUIControl::SetupDefaults() {
    SetThreeVectorDefault("backgroundcolor",0.6,0.6,0.6);
    SetStringDefault("enable","on");
    SetConstrainedStringDefault("fontangle","normal");
    SetStringDefault("fontname","helvetica");
    SetScalarDefault("fontsize",10);
    SetConstrainedStringDefault("fontunits","points");
    SetConstrainedStringDefault("fontweight","normal");
    SetThreeVectorDefault("foregroundcolor",0,0,0);
    SetConstrainedStringDefault("horizontalalignment","left");
    SetScalarDefault("listboxtop",1);
    SetScalarDefault("max",1);
    SetScalarDefault("min",1);
    SetFourVectorDefault("position",0,0,10,10);
    SetTwoVectorDefault("sliderstep",0.01,0.1);
    SetStringDefault("type","uicontrol");
    SetStringDefault("style","pushbutton");
  }
}
