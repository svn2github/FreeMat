#include "HandleText.hpp"

HandleText::HandleText() {
  ConstructProperties();
  SetupDefaults();
}

HandleText::~HandleText() {
}

void HandleText::UpdateState() {
}

HandleAxis* HandleText::GetParentAxis() {
  // Get our parent - must be an axis
  HPHandle *parent = (HPHandle*) LookupProperty("parent");
  if (parent->Data().empty()) return NULL;
  unsigned parent_handle = parent->Data()[0];
  HandleObject *fp = handleset.lookupHandle(parent_handle);
  HPString *name = (HPString*) fp->LookupProperty("type");
  if (!name) return NULL;
  if (!name->Is("axes")) return NULL;
  HandleAxis *axis = (HandleFigure*) fp;
  return axis;
}

void HandleText::paintGL() {
  // Get handleaxis parent
  HandleAxis* axis = GetParentAxis();
  if (!axis) return;
  // Map position -> pixel location
  // Setup direct draw mode
  // fill background rectangle
  // draw bounding rectangle
  // draw text label
}

void HandleText::SetupDefaults() {
  SetConstrainedStringDefault("horizontalalignment","left");
  SetThreeVectorDefault("position",0,0,0);
  SetScalarDefault("rotation",0);
  SetConstrainedStringDefault("units","data");
  SetConstrainedStringDefault("verticalalignment","middle");
  SetThreeVectorDefault("backgroundcolor",-1,-1,-1);
  SetThreeVectorDefault("edgecolor",-1,-1,-1);
  SetScalarDefault("linewidth",0.5);
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("margin",0);
  SetConstrainedStringDefault("fontangle","normal");
  SetStringDefault("fontname","helvetica");
  SetScalarDefault("fontsize",12);
  SetConstrainedStringDefault("fontunits","points");
  SetConstrainedStringDefault("fontweight","normal");
  AddProperty("visible","on");
  SetThreeVectorDefault("color",0,0,0);
}

void HandleText::ConstructProperties() {
  AddProperty(new HPString,"string");
  AddProperty(new HPFourVector,"extent");
  AddProperty(new HPAlignHoriz,"horizontalalignment");
  AddProperty(new HPThreeVector,"position");
  AddProperty(new HPScalar,"rotation");
  AddProperty(new HPUnits,"units");
  AddProperty(new HPAlignVert,"verticalalignment");
  AddProperty(new HPColor,"backgroundcolor");
  AddProperty(new HPColor,"edgecolor");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"margin");
  AddProperty(new HPFontAngle,"fontangle");
  AddProperty(new HPString,"fontname");
  AddProperty(new HPScalar,"fontsize");
  AddProperty(new HPFontUnits,"fontunits");
  AddProperty(new HPFontWeight,"fontweight");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPColor,"color");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPString,"tag");
  //    AddProperty(new Array,"userdata");
}
