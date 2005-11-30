#include "HandleText.hpp"
#include "HandleProperty.hpp"
#include "HandleList.hpp"
#include <math.h>

namespace FreeMat {

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
    fnt = QFont(fontname->Data().c_str(),fontsize->Data()[0]);
    fnt.setStyle(fstyle);
    fnt.setWeight(fweight);
    HPString *txt = (HPString*) LookupProperty("string");
    text = txt->Data();
  }

  int HandleText::GetTextHeightInPixels() {
    QFontMetrics fm(fnt);
    QRect sze(fm.boundingRect("|"));
    return sze.height();
  }

  void HandleText::PaintMe(RenderEngine& gc) {
    UpdateState();
    // Get handleaxis parent
    HandleAxis* axis = GetParentAxis();
    if (!axis) return;
    // Map position -> pixel location
    int x, y;
    std::vector<double> pos(VectorPropertyLookup("position"));
    // remap it
    std::vector<double> mapped(axis->ReMap(pos));
    gc.toPixels(mapped[0],mapped[1],mapped[2],x,y);
    gc.setupDirectDraw();
    // Retrieve the margin...
    double margin(ScalarPropertyLookup("margin"));
    margin = margin + 1;
    RenderEngine::AlignmentFlag xalign, yalign;
    HPAlignVert *hv = (HPAlignVert*) LookupProperty("verticalalignment");
    if (hv->Is("top"))
      yalign = RenderEngine::Max;
    else if (hv->Is("middle"))
      yalign = RenderEngine::Mean;
    else
      yalign = RenderEngine::Min;
    HPAlignHoriz *hh = (HPAlignHoriz*) LookupProperty("horizontalalignment");
    if (hh->Is("left"))
      xalign = RenderEngine::Min;
    else if (hh->Is("center"))
      xalign = RenderEngine::Mean;
    else
      xalign = RenderEngine::Max;
    // Get the width and height of the label
    int textwidth;
    int textheight;
    // Get the corner offsets for the label
    int textxoffset;
    int textyoffset;
    gc.measureText(text,fnt,xalign,yalign,textwidth,textheight,textxoffset,textyoffset);
    // Construct the coordinates of the text rectangle
    int x1, y1;
    double costheta, sintheta;
    double rotation = ScalarPropertyLookup("rotation");
    costheta = cos(-rotation*M_PI/180.0);
    sintheta = sin(-rotation*M_PI/180.0);
    x1 = x + (textxoffset-margin)*costheta + (textyoffset-margin)*sintheta;
    y1 = y - (textxoffset-margin)*sintheta + (textyoffset-margin)*costheta;
    double hdelx, hdely, vdelx, vdely;
    hdelx = (textwidth+2*margin)*costheta;
    hdely = -(textwidth+2*margin)*sintheta;
    vdelx = (textheight+2*margin)*sintheta;
    vdely = (textheight+2*margin)*costheta;
    // fill background rectangle
    HPColor *bc = (HPColor*) LookupProperty("backgroundcolor");
    if (!bc->IsNone()) {
      gc.color(bc->Data());
      gc.quad(x1,y1,0,
	      x1+hdelx,y1+hdely,0,
	      x1+hdelx+vdelx,y1+hdely+vdely,0,
	      x1+vdelx,y1+vdely,0);
    }
    // draw bounding rectangle
    HPColor *ec = (HPColor*) LookupProperty("edgecolor");
    if (!ec->IsNone()) {
      gc.color(ec->Data());
      gc.setLineStyle(((HPLineStyle*) LookupProperty("linestyle"))->Data());
      gc.lineWidth(ScalarPropertyLookup("linewidth"));
      // draw bounding rectangle
      gc.quadline(x1,y1,0,
		  x1+hdelx,y1+hdely,0,
		  x1+hdelx+vdelx,y1+hdely+vdely,0,
		  x1+vdelx,y1+vdely,0);
    }
    // draw the text
    HPColor *tc = (HPColor*) LookupProperty("color");
    gc.putText(x,y,text,tc->Data(),xalign,yalign,fnt,rotation);
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
    SetScalarDefault("fontsize",10);
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
