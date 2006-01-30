#include "HandleLineSeries.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "IEEEFP.hpp"

namespace FreeMat {

  HandleLineSeries::HandleLineSeries() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleLineSeries::~HandleLineSeries() {
  }

  // Calculate the limits - should return a vector of the
  // form...
  // [xmin, xmax, ymin, ymax, zmin, zmax, cmin, cmax, amin, amax]
  std::vector<double> HandleLineSeries::GetLimits() {
    std::vector<double> xs(VectorPropertyLookup("xdata"));
    std::vector<double> ys(VectorPropertyLookup("ydata"));
    std::vector<double> zs(VectorPropertyLookup("zdata"));
    std::vector<double> limits;
    limits.push_back(VecMin(xs));
    limits.push_back(VecMax(xs));
    limits.push_back(VecMin(ys));
    limits.push_back(VecMax(ys));
    limits.push_back(VecMin(zs));
    limits.push_back(VecMax(zs));
    limits.push_back(0);
    limits.push_back(0);
    limits.push_back(0);
    limits.push_back(0);    
    return limits;
  }

  void HandleLineSeries::UpdateState() {
    // Check that x, y and z data are the same size
    // Generate the x coordinates if necessary
    std::vector<double> xs(VectorPropertyLookup("xdata"));
    std::vector<double> ys(VectorPropertyLookup("ydata"));
    std::vector<double> zs(VectorPropertyLookup("zdata"));
    if (IsAuto("xdatamode")) {
      xs.clear();
      for (int i=0;i<ys.size();i++)
	xs.push_back(i+1.0);
    }
    if (zs.size() == 0)
      for (int i=0;i<ys.size();i++)
	zs.push_back(0.0);
    HPVector *sp;
    sp = (HPVector*) LookupProperty("xdata");
    sp->Data(xs);
    sp = (HPVector*) LookupProperty("zdata");
    sp->Data(zs);
    HandleAxis* parent = GetParentAxis();
    parent->UpdateState();
  }

  void HandleLineSeries::PaintMe(RenderEngine& gc) {
    if (StringCheck("visible","off"))
      return;
    // Draw the line...
    double width(ScalarPropertyLookup("linewidth"));
    gc.lineWidth(width);
    HPColor *lc = (HPColor*) LookupProperty("color");
    // remap...
    std::vector<double> xs(VectorPropertyLookup("xdata"));
    std::vector<double> ys(VectorPropertyLookup("ydata"));
    std::vector<double> zs(VectorPropertyLookup("zdata"));
    if (!((xs.size() == ys.size()) && (ys.size() == zs.size())))
      return;
    std::vector<double> mxs, mys, mzs;
    HandleAxis *parent = (HandleAxis*) GetParentAxis();
    parent->ReMap(xs,ys,zs,mxs,mys,mzs);
    if (!lc->IsNone()) {
      gc.color(lc->Data());
      gc.setLineStyle(StringPropertyLookup("linestyle"));
      // Partition it into segments of finite entries..
      int n = 0;
      while (n < mxs.size()) {
	std::vector<double> local_mxs, local_mys, local_mzs;
	while ((n < mxs.size()) && IsFinite(mxs[n]) && IsFinite(mys[n]) && (IsFinite(mzs[n]))) {
	  local_mxs.push_back(mxs[n]);
	  local_mys.push_back(mys[n]);
	  local_mzs.push_back(mzs[n]);
	  n++;
	}
	gc.lineSeries(local_mxs,local_mys,local_mzs);
	while ((n < mxs.size()) && !(IsFinite(mxs[n]) && IsFinite(mys[n]) && (IsFinite(mzs[n])))) n++;
      }
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
	DrawSymbol(gc,typ,uc[i],vc[i],0,sze,ec->Data(),fc->Data(),width);
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
    AddProperty(new HPHandles,"children");
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
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPVector,"ydata");
    AddProperty(new HPVector,"zdata");
  }
}
