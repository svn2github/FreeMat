#include "HandleLineSeries.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleAxis.hpp"

namespace FreeMat {

  extern HandleList<HandleObject*> handleset;

  HandleLineSeries::HandleLineSeries() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleLineSeries::~HandleLineSeries() {
  }

  void HandleLineSeries::UpdateState() {
    // Check that x, y and z data are the same size
    // Generate the x coordinates if necessary
  }

  void HandleLineSeries::PaintMe(RenderEngine& gc) {
    // Draw the line...
    double width(ScalarPropertyLookup("linewidth"));
    gc.lineWidth(width);
    HPColor *lc = (HPColor*) LookupProperty("color");
    // remap...
    std::vector<double> xs(VectorPropertyLookup("xdata"));
    std::vector<double> ys(VectorPropertyLookup("ydata"));
    std::vector<double> zs(VectorPropertyLookup("zdata"));
    std::vector<double> mxs, mys, mzs;
    HandleAxis *parent = (HandleAxis*) GetParentAxis();
    parent->ReMap(xs,ys,zs,mxs,mys,mzs);
    if (!lc->IsNone()) {
      gc.color(lc->Data());
      gc.setLineStyle(StringPropertyLookup("linestyle"));
      gc.lineSeries(mxs,mys,mzs);
    }
    // Draw the symbols
    HPColor *ec = (HPColor*) LookupProperty("markeredgecolor");
    HPColor *fc = (HPColor*) LookupProperty("markerfacecolor");
    RenderEngine::SymbolType typ = StringToSymbol(StringPropertyLookup("marker"));
    double sze = ScalarPropertyLookup("markersize")/2.0;
    // Make sure there's something to draw...
    if ((typ != RenderEngine::None) || ec->IsNone() || fc->IsNone()) {
      // Calculate the u/v coordinates (pixels)
      std::vector<double> uc;
      std::vector<double> vc;
      for (int i=0;i<mxs.size();i++) {
	double u, v;
	bool clipped;
	gc.toPixels(mxs[i],mys[i],mzs[i],u,v,clipped);
	if (!clipped) {
	  uc.push_back(u); vc.push_back(v);
	}
      }
      gc.setupDirectDraw();
      for (int i=0;i<uc.size();i++) 
	DrawSymbol(gc,typ,uc[i],vc[i],sze,ec->Data(),fc->Data(),width);
      gc.releaseDirectDraw();
    }
  }

  void HandleLineSeries::SetupDefaults() {
    SetThreeVectorDefault("color",0,0,0);
    SetConstrainedStringDefault("linestyle","-");
    SetScalarDefault("linewidth",0.5);
    SetConstrainedStringDefault("marker","none");
    SetThreeVectorDefault("markeredgecolor",0,0,0);
    SetThreeVectorDefault("markerfacecolor",0,0,0);
    SetScalarDefault("markersize",6);
    SetStringDefault("type","line");
    SetConstrainedStringDefault("visible","on");
    SetConstrainedStringDefault("xdatamode","manual");
  }
  
  void HandleLineSeries::ConstructProperties() {
    AddProperty(new HPColor,"color");
    AddProperty(new HPString,"displayname");
    AddProperty(new HPLineStyle,"linestyle");
    AddProperty(new HPScalar,"linewidth");
    AddProperty(new HPSymbol,"marker");
    AddProperty(new HPColor,"markeredgecolor");
    AddProperty(new HPColor,"markerfacecolor");
    AddProperty(new HPScalar,"markersize");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPString,"tag");
    AddProperty(new HPString,"type");
    AddProperty(new HPOnOff,"visible");
    AddProperty(new HPVector,"xdata");
    AddProperty(new HPAutoManual,"xdatamode");
    AddProperty(new HPVector,"ydata");
    AddProperty(new HPVector,"zdata");
  }
}
