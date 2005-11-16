#include "HandleText.hpp"
#include "HandleProperty.hpp"
#include "HandleList.hpp"


namespace FreeMat {

extern HandleList<HandleObject*> handleset;

HandleText::HandleText() {
  ConstructProperties();
  SetupDefaults();
}

HandleText::~HandleText() {
}

void HandleText::UpdateState() {
  QFont::Style fstyle;
  QFont::Weight fweight;
  HPString *fontname = (HPString*) LookupProperty("fontname");
  HPFontAngle *fontangle = (HPFontAngle*) LookupProperty("fontangle");
  HPFontWeight *fontweight = (HPFontWeight*) LookupProperty("fontweight");
  HPScalar *fontsize = (HPScalar*) LookupProperty("fontsize");
  if (fontangle->Is("normal"))
    fstyle = QFont::StyleNormal;
  if (fontangle->Is("italic"))
    fstyle = QFont::StyleItalic;
  if (fontangle->Is("oblique"))
    fstyle = QFont::StyleOblique;
  if (fontweight->Is("normal"))
    fweight = QFont::Normal;
  if (fontweight->Is("bold"))
    fweight = QFont::Bold;
  if (fontweight->Is("light"))
    fweight = QFont::Light;
  if (fontweight->Is("demi"))
    fweight = QFont::DemiBold;
  // Lookup the font
  QFont fnt(fontname->Data().c_str(),fontsize->Data()[0]);
  fnt.setStyle(fstyle);
  fnt.setWeight(fweight);
  HPColor *col = (HPColor*) LookupProperty("color");
  HPString *txt = (HPString*) LookupProperty("string");
  std::string text = txt->Data();
  glab = GLLabel(fnt,text,col->Data()[0]*255,col->Data()[1]*255,col->Data()[2]*255);
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
  HandleAxis *axis = (HandleAxis*) fp;
  return axis;
}

void HandleText::paintGL() {
  UpdateState();
  // Get handleaxis parent
  HandleAxis* axis = GetParentAxis();
  if (!axis) return;
  // Map position -> pixel location
  int x, y;
  HPThreeVector* hp = (HPThreeVector*) LookupProperty("position");
  axis->ToPixels(hp->Data()[0],hp->Data()[1],hp->Data()[2],x,y);
  axis->SetupDirectDraw();
  // Retrieve the margin...
  double margin(ScalarPropertyLookup("margin"));
  // Get the width and height of the label
  int textwidth = glab.twidth();
  int textheight = glab.theight();
  // Get the corner offsets for the label
  int textxoffset = glab.xoffset(xalign);
  int textyoffset = glab.yoffset(yalign);
  // Construct the coordinates of the text rectangle
  int x1, y1, x2, y2;
  x1 = x + textxoffset - margin;
  y1 = y + textyoffset - margin;
  x2 = x1 + textwidth + 2*margin;
  y2 = y1 + textheight + 2*margin;
  // fill background rectangle
  HPColor *bc = (HPColor*) LookupProperty("backgroundcolor");
  glColor3f(bc->Data()[0],bc->Data()[1],bc->Data()[2]);
  glRectf(x1,y1,x2,y2);
  // draw bounding rectangle
  axis->SetLineStyle(((HPLineStyle*) LookupProperty("linestyle"))->Data());
  HPColor *ec = (HPColor*) LookupProperty("edgecolor");
  glColor3f(ec->Data()[0],ec->Data()[1],ec->Data()[2]);
  glLineWidth(ScalarPropertyLookup("linewidth"));
  // draw bounding rectangle
  glBegin(GL_LINES);
  glVertex2f(x1,y1);
  glVertex2f(x1,y2);
  glVertex2f(x1,y2);
  glVertex2f(x2,y2);
  glVertex2f(x2,y2);
  glVertex2f(x2,y1);
  glVertex2f(x2,y1);
  glVertex2f(x1,y1);
  glEnd();
  // draw text label
  glab.DrawMe(x,y,xalign,yalign);
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
  SetConstrainedStringDefault("visible","on");
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
}
