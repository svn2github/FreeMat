#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "HandleText.hpp"
#include "Core.hpp"
#include <qapplication.h>
#include <math.h>
#include <qpainter.h>
#include "GLRenderEngine.hpp"
#include "HandleCommands.hpp"

// Property list & status
//    activepositionproperty
//    alim
//    alimmode
//    ambientlightcolor
//    box
//    cameraposition - done
//    camerapositionmode  - done
//    cameratarget - done
//    cameratargetmode - done
//    cameraupvector - done
//    cameraviewangle 
//    cameraviewanglemode
//    children
//    clim
//    climmode
//    clipping
//    color - done (does 'none' work?)
//    colororder - done
//    dataaspectratio
//    dataaspectratiomode
//    fontangle - done
//    fontname - done
//    fontsize - done
//    fontunits - not implemented
//    fontweight - done
//    gridlinestyle - done
//    handlevisibility
//    hittest
//    interruptible
//    layer
//    //    linestyleorder
//    linewidth
//    minorgridlinestyle - done
//    nextplot
//    outerposition - done - need linkage to position
//    parent - done
//    plotboxaspectratio
//    plotboxaspectratiomode
//    position - done
//    projection
//    selected
//    selectionhighlight
//    tag - done
//    tickdir - done - need labels to not follow tick direction
//    tickdirmode - done
//    ticklength - need 2d support
//    tightinset
//    title
//    type
//    units
//    //    userdata
//    visible
//    xaxislocation - done
//    yaxislocation - done
//    xcolor - done
//    ycolor - done
//    zcolor - done
//    xdir - done
//    ydir - done
//    zdir - done
//    xgrid - done
//    ygrid - done
//    zgrid - done
//    xlabel - done
//    ylabel - done
//    zlabel - done
//    xlim - done
//    ylim - done
//    zlim - done
//    xlimmode - done
//    ylimmode - done
//    zlimmode - done
//    xminorgrid - done
//    yminorgrid - done
//    zminorgrid - done
//    xscale - done
//    yscale - done
//    zscale - done
//    xtick - done
//    ytick - done
//    ztick - done
//    xticklabel - done
//    yticklabel - done
//    zticklabel - done
//    xtickmode - done
//    ytickmode - done
//    ztickmode - done
//    xticklabelmode - done
//    yticklabelmode - done
//    zticklabelmode - done
 

// Need to build the transformation matrix...
// Given a position rectangle and a camera matrix,
//   1.  Map the 8 corners of the data space into the camera plane
//   1b. Use the data scaling factors in DataAspectRatio when
//       mapping the coordinates.
//   2.  Calculate scale factor & offsets appropriately to 
//       stretch the data to fit the position rectangle
//   Create a special 4x4 matrix that is the composite of 
//   the data scale, camera and xy scale matrices
//  
//   This is the final (view) matrix.
//
// Check out gluLookAt
//
// Actually, not...  it is more complicated than this.
// The view matrix maps _rotated_ x,y,z coordinates
// to the screen.  We want to specify the clipping
// volume in _unrotated_ coordinates.  So, what needs
// to happen is that we must transform the 8 corners
// of the clipped volume using the current modelview
// matrix, use them to establish new clipping planes
// in x, y, z, and then map the clipped volume to viewer
// space.
namespace FreeMat {

  // Probably a better way to do this...

  class pt3d {
  public:
    double x;
    double y;
    double z;
    pt3d() : x(0), y(0), z(0) {}
    pt3d(double a, double b, double c) : x(a), y(b), z(c) {}
    pt3d operator-(const pt3d& t) {return pt3d(x-t.x,y-t.y,z-t.z);}
  };

  static pt3d crossprod(pt3d v1, pt3d v2) {
    pt3d temp;
    temp.x = (v1.y * v2.z) - (v1.z * v2.y);
    temp.y = (v1.z * v2.x) - (v1.x * v2.z);
    temp.z = (v1.x * v2.y) - (v1.y * v2.x);
    return temp;
  }

  static std::string TrimPrint(double val, bool scientificNotation) {
    char buffer[1000];
    char *p;
    if (!scientificNotation) {
      sprintf(buffer,"%f",val);
      p = buffer + strlen(buffer) - 1;
      while (*p == '0') {
	*p = 0;
	p--;
      }
      if ((*p == '.') || (*p == ',')) {
	*p = 0;
	//	*(p+1) = '0';
	//	*(p+2) = 0;
      }
      return std::string(buffer);
    } else {
      sprintf(buffer,"%e",val);
      std::string label(buffer);
      unsigned int ePtr;
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr--;
      while (label[ePtr] == '0') {
	label.erase(ePtr,1);
	ePtr--;
      }
      if ((label[ePtr] == '.') || (label[ePtr] == ','))
	label.insert(ePtr+1, 1,'0');
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr+=2;
      while ((label[ePtr] == '0') && ePtr < label.size()) {
	label.erase(ePtr,1);
      }
      if (ePtr == label.size())
	label.append("0");
      return label;
    }
  }

  double tlog(double x) {
    if (x>0) 
      return log10(x);
    else
      return -10;
  }

  // Construct an axis 
  void FormatAxisManual(double t1, double t2, int tickcount,
			bool isLogarithmic,
			double& tStart, double &tStop,
			std::vector<double> &tickLocations,
			std::vector<std::string> &tlabels) {
    double tBegin, tEnd;
    double delt = (t2-t1)/tickcount;
    int n = ceil(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = floor(log2(rdelt));
    double tDelt = pow(10.0,(double) n)*pow(2.0,(double) p);
    if (isLogarithmic)
      tDelt = ceil(tDelt);
    tStart = t1;
    tStop = t2;
    tBegin = tDelt*ceil(t1/tDelt);
    tEnd = floor(t2/tDelt)*tDelt;
    int mprime;
    mprime = ceil((tEnd-tBegin)/tDelt);
    if ((tBegin+mprime*tDelt) > t2)
      mprime--;
    int tCount = mprime+1;
    tickLocations.clear();
    tlabels.clear();
    bool exponentialForm;
    exponentialForm = false;
    for (int i=0;i<tCount;i++) {
      double tloc = tBegin+i*tDelt;
      tickLocations.push_back(tloc);
      if (tloc != 0.0)
	exponentialForm |= (fabs(log10(fabs(tloc))) >= 4.0);
    }
    for (int i=0;i<tCount;i++) 
      tlabels.push_back(TrimPrint(tBegin+i*tDelt,exponentialForm));
  }
  
  void FormatAxisAuto(double tMin, double tMax, int tickcount,
		      bool isLogarithmic,
		      double& tStart, double &tStop,
		      std::vector<double> &tickLocations,
		      std::vector<std::string> &tlabels) {
    double tBegin, tEnd;
    double delt = (tMax-tMin)/tickcount;
    int n = ceil(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = floor(log2(rdelt));
    double tDelt = pow(10.0,(double) n)*pow(2.0,(double) p);
    if (isLogarithmic) 
      tDelt = ceil(tDelt);
    tStart = floor(tMin/tDelt)*tDelt;
    tStop = ceil(tMax/tDelt)*tDelt;
    tBegin = tStart;
    tEnd = tStop;
    int mprime;
    mprime = ceil((tEnd-tBegin)/tDelt);
    if ((tBegin+(mprime-1)*tDelt) > tMax)
      mprime--;
    int tCount = mprime+1;
    tickLocations.clear();
    tlabels.clear();
    bool exponentialForm;
    exponentialForm = false;
    for (int i=0;i<tCount;i++) {
      double tloc = tBegin+i*tDelt;
      if (!isLogarithmic)
	tickLocations.push_back(tloc);
      else
	tickLocations.push_back(pow(10.0,tloc));
      if (tloc != 0.0)
	exponentialForm |= (fabs(log10(fabs(tloc))) >= 4.0);
    }
    for (int i=0;i<tCount;i++) 
      if (!isLogarithmic)
	tlabels.push_back(TrimPrint(tBegin+i*tDelt,exponentialForm));
      else
	tlabels.push_back(TrimPrint(pow(10.0,tBegin+i*tDelt),true));
  }

  void HandleAxis::GetMaxTickMetric(RenderEngine &gc,
				    std::vector<std::string> labs,
				    double &maxx, double &maxy) {
    maxx = 0;
    maxy = 0;
    for (int i=0;i<labs.size();i++) {
      int width, height, xoffset, yoffset;
      gc.measureText(labs[i],m_font,
		     RenderEngine::Min,RenderEngine::Min,
		     width, height, xoffset, yoffset);
      maxx = qMax(maxx,(double)width);
      maxy = qMax(maxy,(double)height);
    }      
  }
  
  void HandleAxis::ConstructProperties() {
    // These are all the properties of the axis
    AddProperty(new HPPosition, "activepositionproperty");
    AddProperty(new HPTwoVector,"alim");
    AddProperty(new HPAutoManual,"alimmode");
    AddProperty(new HPColor,"ambientlightcolor");
    AddProperty(new HPOnOff,"box");
    AddProperty(new HPThreeVector,"cameraposition");
    AddProperty(new HPAutoManual,"camerapositionmode");
    AddProperty(new HPThreeVector,"cameratarget");
    AddProperty(new HPAutoManual,"cameratargetmode");
    AddProperty(new HPThreeVector,"cameraupvector");
    AddProperty(new HPAutoManual,"cameraupvectormode");
    AddProperty(new HPScalar,"cameraviewangle");
    AddProperty(new HPAutoManual,"cameraviewanglemode");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPTwoVector,"clim");
    AddProperty(new HPAutoManual,"climmode");
    AddProperty(new HPOnOff,"clipping");
    AddProperty(new HPColor,"color");
    AddProperty(new HPColorVector,"colororder");
    AddProperty(new HPThreeVector,"dataaspectratio");
    AddProperty(new HPAutoManual,"dataaspectratiomode");
    AddProperty(new HPFontAngle,"fontangle");
    AddProperty(new HPString,"fontname");
    AddProperty(new HPScalar,"fontsize");
    AddProperty(new HPFontUnits,"fontunits");
    AddProperty(new HPFontWeight,"fontweight");
    AddProperty(new HPLineStyle,"gridlinestyle");
    AddProperty(new HPOnOff,"handlevisibility");
    AddProperty(new HPOnOff,"hittest");
    AddProperty(new HPOnOff,"interruptible");
    AddProperty(new HPTopBottom,"layer");
    AddProperty(new HPLineStyleOrder,"linestyleorder");
    AddProperty(new HPScalar,"linewidth");
    AddProperty(new HPLineStyle,"minorgridlinestyle");
    AddProperty(new HPNextPlotMode,"nextplot");
    AddProperty(new HPFourVector,"outerposition");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPThreeVector,"plotboxaspectratio");
    AddProperty(new HPAutoManual,"plotboxaspectratiomode");
    AddProperty(new HPFourVector,"position");
    AddProperty(new HPProjectionMode,"projection");
    AddProperty(new HPOnOff,"selected");
    AddProperty(new HPOnOff,"selectionhighlight");
    AddProperty(new HPString,"tag");
    AddProperty(new HPInOut,"tickdir");
    AddProperty(new HPAutoManual,"tickdirmode");
    AddProperty(new HPTwoVector,"ticklength");
    AddProperty(new HPFourVector,"tightinset");
    AddProperty(new HPHandles,"title");
    AddProperty(new HPString,"type");
    AddProperty(new HPUnits,"units");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPOnOff,"visible");
    AddProperty(new HPTopBottom,"xaxislocation");
    AddProperty(new HPLeftRight,"yaxislocation");
    AddProperty(new HPColor,"xcolor");
    AddProperty(new HPColor,"ycolor");
    AddProperty(new HPColor,"zcolor");
    AddProperty(new HPNormalReverse,"xdir");
    AddProperty(new HPNormalReverse,"ydir");
    AddProperty(new HPNormalReverse,"zdir");
    AddProperty(new HPOnOff,"xgrid");
    AddProperty(new HPOnOff,"ygrid");
    AddProperty(new HPOnOff,"zgrid");
    AddProperty(new HPHandles,"xlabel");
    AddProperty(new HPHandles,"ylabel");
    AddProperty(new HPHandles,"zlabel");
    AddProperty(new HPTwoVector,"xlim");
    AddProperty(new HPTwoVector,"ylim");
    AddProperty(new HPTwoVector,"zlim");
    AddProperty(new HPAutoManual,"xlimmode");
    AddProperty(new HPAutoManual,"ylimmode");
    AddProperty(new HPAutoManual,"zlimmode");
    AddProperty(new HPOnOff,"xminorgrid");
    AddProperty(new HPOnOff,"yminorgrid");
    AddProperty(new HPOnOff,"zminorgrid");
    AddProperty(new HPLinearLog,"xscale");
    AddProperty(new HPLinearLog,"yscale");
    AddProperty(new HPLinearLog,"zscale");
    AddProperty(new HPVector,"xtick");
    AddProperty(new HPVector,"ytick");
    AddProperty(new HPVector,"ztick");
    AddProperty(new HPStringSet,"xticklabel");
    AddProperty(new HPStringSet,"yticklabel");
    AddProperty(new HPStringSet,"zticklabel");
    AddProperty(new HPAutoManual,"xtickmode");
    AddProperty(new HPAutoManual,"ytickmode");
    AddProperty(new HPAutoManual,"ztickmode");
    AddProperty(new HPAutoManual,"xticklabelmode");
    AddProperty(new HPAutoManual,"yticklabelmode");
    AddProperty(new HPAutoManual,"zticklabelmode");
  }

  GLubyte* GetTextAsBits(QFont fnt, std::string text, int &width, 
			 int &height, GLubyte red, GLubyte green, 
			 GLubyte blue) {
    // Get the font
    QFontMetrics fm(fnt);
    QRect sze(fm.boundingRect(text.c_str()));
    width = sze.width();
    height = sze.height();
    QImage img(width,height,QImage::Format_RGB32);
    QPainter pnt(&img);
    pnt.setBackground(QColor(255,255,255));
    pnt.eraseRect(0,0,width,height);
    pnt.setFont(fnt);
    pnt.setPen(QColor(0,0,0));
    pnt.drawText(0,height,text.c_str());
    pnt.end();
    // Now, we generate a synthetic image that is of the same size
    GLubyte *bits = new GLubyte[width*height*4];
    unsigned char *ibits = img.bits();
    // Set the color bits to all be the same color as specified
    // in the argument list, and use the grey scale to modulate
    // the transparency
    for (int i=0;i<height;i++) {
      for (int j=0;j<width;j++) {
	int dptr = 4*(i*width+j);
	int sptr = 4*((height-1-i)*width+j);
	bits[dptr] = red;
	bits[dptr+1] = green;
	bits[dptr+2] = blue;
	bits[dptr+3] = 255-ibits[sptr];
      }
    }
    return bits;
  }

  HandleAxis::HandleAxis() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleAxis::~HandleAxis() {
  }

  void HandleAxis::SetupDefaults() {
    SetConstrainedStringDefault("activepositionproperty","outerposition");
    SetConstrainedStringDefault("alimmode","auto");
    SetConstrainedStringDefault("box","off");
    SetThreeVectorDefault("cameraposition",0,0,1);
    SetConstrainedStringDefault("camerapositionmode","auto");
    SetThreeVectorDefault("cameratarget",0,0,0);
    SetConstrainedStringDefault("cameratargetmode","auto");
    SetThreeVectorDefault("cameraupvector",0,1,0);
    SetConstrainedStringDefault("cameraupvectormode","auto");
    SetConstrainedStringDefault("cameraviewanglemode","auto");
    SetConstrainedStringDefault("climmode","auto");
    SetConstrainedStringDefault("clipping","on");
    SetThreeVectorDefault("color",1,1,1);
    // Set up the default color order
    std::vector<double> colors;
    colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0); 
    colors.push_back(1.0); colors.push_back(1.0); colors.push_back(0.0); 
    colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
    colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
    colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);
    colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0);
    colors.push_back(1.0); colors.push_back(1.0); colors.push_back(0.0);
    HPVector *hp = (HPVector*) LookupProperty("colororder");
    hp->Data(colors);
    SetConstrainedStringDefault("dataaspectratiomode","auto");
    //    SetConstrainedStringDefault("drawmode","normal");
    SetConstrainedStringDefault("fontangle","normal");
    SetStringDefault("fontname","helvetica");
    SetScalarDefault("fontsize",10);
    SetConstrainedStringDefault("fontunits","points");
    SetConstrainedStringDefault("fontweight","normal");
    SetConstrainedStringDefault("gridlinestyle",":");
    SetConstrainedStringDefault("handlevisibility","on");
    SetConstrainedStringDefault("hittest","on");
    SetConstrainedStringDefault("interruptible","on");
    SetConstrainedStringDefault("layer","bottom");
    SetScalarDefault("linewidth",1.0);
    SetConstrainedStringSetDefault("linestyleorder","-|--|:|-.");
    SetConstrainedStringDefault("minorgridlinestyle",":");
    SetFourVectorDefault("outerposition",0,0,1,1);
    SetConstrainedStringDefault("nextplot","replace");
    SetConstrainedStringDefault("plotboxaspectratiomode","auto");
    SetFourVectorDefault("position",0.13,0.11,0.775,0.815);
    SetConstrainedStringDefault("projection","orthographic");
    SetConstrainedStringDefault("selected","off");
    SetConstrainedStringDefault("selectionhighlight","on");
    SetConstrainedStringDefault("tickdir","in");
    SetConstrainedStringDefault("tickdirmode","auto");
    SetTwoVectorDefault("ticklength",0.01,0.025);
    SetStringDefault("type","axes");
    SetConstrainedStringDefault("units","normalized");
    SetConstrainedStringDefault("visible","on");
    SetConstrainedStringDefault("xaxislocation","bottom");
    SetConstrainedStringDefault("yaxislocation","left");
    SetThreeVectorDefault("xcolor",0,0,0);
    SetThreeVectorDefault("ycolor",0,0,0);
    SetThreeVectorDefault("zcolor",0,0,0);
    SetConstrainedStringDefault("xdir","normal");
    SetConstrainedStringDefault("ydir","normal");
    SetConstrainedStringDefault("zdir","normal");
    SetConstrainedStringDefault("xgrid","off");
    SetConstrainedStringDefault("ygrid","off");
    SetConstrainedStringDefault("zgrid","off");
    SetTwoVectorDefault("xlim",0,1);
    SetTwoVectorDefault("ylim",0,1);
    SetTwoVectorDefault("zlim",0,1);
    SetConstrainedStringDefault("xlimmode","auto");
    SetConstrainedStringDefault("ylimmode","auto");
    SetConstrainedStringDefault("zlimmode","auto");
    SetConstrainedStringDefault("xminorgrid","off");
    SetConstrainedStringDefault("yminorgrid","off");
    SetConstrainedStringDefault("zminorgrid","off");
    SetConstrainedStringDefault("xscale","linear");
    SetConstrainedStringDefault("yscale","linear");
    SetConstrainedStringDefault("zscale","linear");
    SetConstrainedStringDefault("xtickmode","auto");
    SetConstrainedStringDefault("ytickmode","auto");
    SetConstrainedStringDefault("ztickmode","auto");
    SetConstrainedStringDefault("xticklabelmode","auto");
    SetConstrainedStringDefault("yticklabelmode","auto");
    SetConstrainedStringDefault("zticklabelmode","auto");
  }

  

  std::vector<double> HandleAxis::GetAxisLimits() {
    HPTwoVector *hp;
    std::vector<double> lims;
    HPLinearLog *sp;
    hp = (HPTwoVector*) LookupProperty("xlim");
    sp = (HPLinearLog*) LookupProperty("xscale");
    if (sp->Is("linear")) {
      lims.push_back(hp->Data()[0]);
      lims.push_back(hp->Data()[1]);
    } else {
      lims.push_back(tlog(hp->Data()[0]));
      lims.push_back(tlog(hp->Data()[1]));
    }
    hp = (HPTwoVector*) LookupProperty("ylim");
    sp = (HPLinearLog*) LookupProperty("yscale");
    if (sp->Is("linear")) {
      lims.push_back(hp->Data()[0]);
      lims.push_back(hp->Data()[1]);
    } else {
      lims.push_back(tlog(hp->Data()[0]));
      lims.push_back(tlog(hp->Data()[1]));
    }
    hp = (HPTwoVector*) LookupProperty("zlim");
    sp = (HPLinearLog*) LookupProperty("zscale");
    if (sp->Is("linear")) {
      lims.push_back(hp->Data()[0]);
      lims.push_back(hp->Data()[1]);
    } else {
      lims.push_back(tlog(hp->Data()[0]));
      lims.push_back(tlog(hp->Data()[1]));
    }
    return lims;
  }

  HandleFigure* HandleAxis::GetParentFigure() {
    // Get our parent - should be a figure
    HPHandles *parent = (HPHandles*) LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    HandleFigure *fig = LookupHandleFigure(parent_handle);
    return fig;
  }

  std::vector<double> HandleAxis::UnitsReinterpret(std::vector<double> a) {
    HandleFigure *fig = GetParentFigure();
    unsigned width = fig->GetWidth();
    unsigned height = fig->GetHeight();
    HPUnits *hp = (HPUnits*) LookupProperty("units");
    if (hp->Is("normalized")) {
      for (int i=0;i<a.size();i+=2) {
	a[i] *= width;
	a[i+1] *= height;
      }
      return a;
    } else if (hp->Is("pixels")) {
      return a;
    } else {
      throw Exception("Units of " + hp->Data() + " not yet implemented - please file a Request For Enhancement (RFE) on the FreeMat web site");
    }
  }
  
  std::vector<double> HandleAxis::GetPropertyVectorAsPixels(std::string name) {
    HPFourVector *hp = (HPFourVector*) LookupProperty(name);
    return (UnitsReinterpret(hp->Data()));
  }

  static void MinMaxVector(double *vals, int len, double &vmin, double &vmax) {
    vmin = vmax = vals[0];
    for (int i=0;i<len;i++) {
      vmin = (vals[i] < vmin) ? vals[i] : vmin;
      vmax = (vals[i] > vmax) ? vals[i] : vmax;
    }
  }

  std::vector<double> HandleAxis::ReMap(std::vector<double> t) {
    std::vector<double> s;
    for (int i=0;i<t.size();i+=3) {
      s.push_back(MapX(t[i]));
      s.push_back(MapY(t[i+1]));
      s.push_back(MapZ(t[i+2]));
    }
    return s;
  }

  void HandleAxis::ReMap(std::vector<double> xs, std::vector<double> ys,
			 std::vector<double> zs, std::vector<double> &ax,
			 std::vector<double> &ay, std::vector<double> &az) {
    for (int i=0;i<xs.size();i++) {
      ax.push_back(MapX(xs[i]));
      ay.push_back(MapY(ys[i]));
      az.push_back(MapZ(zs[i]));
    }    
  }
  
  // x in [a,b]
  // a->b
  // b->a
  // y = a-x+b = (a+b) - x
  double HandleAxis::MapX(double x) {
    HPNormalReverse *hp;    
    hp = (HPNormalReverse*) LookupProperty("xdir");
    HPTwoVector *xlim;
    xlim = (HPTwoVector*) LookupProperty("xlim");
    std::vector<double> lims(xlim->Data());
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("xscale");
    if (sp->Is("log"))
      x = tlog(x);
    double xmin(lims[0]);
    double xmax(lims[1]);
    if (hp->Is("reverse")) 
      return(xmin+xmax-x);
    else
      return(x);
  }

  double HandleAxis::MapY(double y) {
    HPNormalReverse *hp;    
    hp = (HPNormalReverse*) LookupProperty("ydir");
    HPTwoVector *ylim;
    ylim = (HPTwoVector*) LookupProperty("ylim");
    std::vector<double> lims(ylim->Data());
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("yscale");
    if (sp->Is("log"))
      y = tlog(y);
    double ymin(lims[0]);
    double ymax(lims[1]);
    if (hp->Is("reverse")) 
      return(ymin+ymax-y);
    else
      return(y);
  }

  double HandleAxis::MapZ(double z) {
    HPNormalReverse *hp;    
    hp = (HPNormalReverse*) LookupProperty("zdir");
    HPTwoVector *zlim;
    zlim = (HPTwoVector*) LookupProperty("zlim");
    std::vector<double> lims(zlim->Data());
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("zscale");
    if (sp->Is("log"))
      z = tlog(z);
    double zmin(lims[0]);
    double zmax(lims[1]);
    if (hp->Is("reverse")) 
      return(zmin+zmax-z);
    else
      return(z);
  }

  void HandleAxis::SetupProjection(RenderEngine &gc) {
    HPThreeVector *tv1, *tv2, *tv3;
    tv1 = (HPThreeVector*) LookupProperty("cameraposition");
    tv2 = (HPThreeVector*) LookupProperty("cameratarget");
    tv3 = (HPThreeVector*) LookupProperty("cameraupvector");
    gc.lookAt(tv1->Data()[0],tv1->Data()[1],tv1->Data()[2],
	      tv2->Data()[0],tv2->Data()[1],tv2->Data()[2],
	      tv3->Data()[0],tv3->Data()[1],tv3->Data()[2]);
    // Get the axis limits
    std::vector<double> limits(GetAxisLimits());
    // Map the 8 corners of the clipping cube to rotated space
    double xvals[8];
    double yvals[8];
    double zvals[8];
    gc.mapPoint(limits[0],limits[2],limits[4],xvals[0],yvals[0],zvals[0]);
    gc.mapPoint(limits[0],limits[2],limits[5],xvals[1],yvals[1],zvals[1]);
    gc.mapPoint(limits[0],limits[3],limits[4],xvals[2],yvals[2],zvals[2]);
    gc.mapPoint(limits[0],limits[3],limits[5],xvals[3],yvals[3],zvals[3]);
    gc.mapPoint(limits[1],limits[2],limits[4],xvals[4],yvals[4],zvals[4]);
    gc.mapPoint(limits[1],limits[2],limits[5],xvals[5],yvals[5],zvals[5]);
    gc.mapPoint(limits[1],limits[3],limits[4],xvals[6],yvals[6],zvals[6]);
    gc.mapPoint(limits[1],limits[3],limits[5],xvals[7],yvals[7],zvals[7]);
    // Get the min and max x, y and z coordinates
    double xmin, xmax, ymin, ymax, zmin, zmax;
    MinMaxVector(xvals,8,xmin,xmax);
    MinMaxVector(yvals,8,ymin,ymax);
    MinMaxVector(zvals,8,zmin,zmax);
    if (zmin == zmax)
      zmax = zmin+1;   
    // Invert the signs of zmin and zmax
    gc.project(xmin,xmax,ymin,ymax,-zmax,-zmin);
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
    gc.viewport(position[0],position[1],position[2],position[3]);
  }

  void HandleAxis::DrawBox(RenderEngine &gc) {
    // Q: Is this outerposition that's supposed to be colored?
    // Get the limits
    HPColor *hp = (HPColor*) LookupProperty("color");
    if (hp->IsNone()) return;
    std::vector<double> limits(GetAxisLimits());
    gc.color(hp->Data());
    gc.depth(false);
    gc.quad( limits[0], limits[2], limits[4],
	     limits[1], limits[2], limits[4],
	     limits[1], limits[3], limits[4],
	     limits[0], limits[3], limits[4]);
    gc.quad( limits[0], limits[2], limits[5],
	     limits[0], limits[3], limits[5],
	     limits[1], limits[3], limits[5],
	     limits[1], limits[2], limits[5]);
    gc.quad( limits[0], limits[2], limits[4],
	     limits[0], limits[3], limits[4],
	     limits[0], limits[3], limits[5],
	     limits[0], limits[2], limits[5]);
    gc.quad( limits[1], limits[2], limits[4],
	     limits[1], limits[2], limits[5],
	     limits[1], limits[3], limits[5],
	     limits[1], limits[3], limits[4]);
    gc.quad( limits[0], limits[2], limits[4],
	     limits[0], limits[2], limits[5],
	     limits[1], limits[2], limits[5],
	     limits[1], limits[2], limits[4]);
    gc.quad( limits[0], limits[3], limits[4],
	     limits[1], limits[3], limits[4],
	     limits[1], limits[3], limits[5],
	     limits[0], limits[3], limits[5]);
    gc.depth(true);
  }


  void HandleAxis::DrawGridLines(RenderEngine &gc) {
    std::vector<double> limits(GetAxisLimits());
    gc.depth(false);
    // The normals of interest are 
    // [0,0,1],[0,0,-1],
    // [0,1,0],[0,-1,0],
    // [1,0,0],[-1,0,0]
    // We will multiply the transformation matrix 
    // by a directional vector.  Then we test the
    // sign of the z component.  This sequence of
    // operations is equivalent to simply taking the
    // 2, 6, 10 elements of m, and drawing the corresponding
    // Select the set of grids to draw based on these elements
    // Draw the grid
    gc.lineWidth(ScalarPropertyLookup("linewidth"));
    gc.setLineStyle(((HPLineStyle*) LookupProperty("gridlinestyle"))->Data());
    HPVector *hp;
    hp = (HPVector*) LookupProperty("xtick");
    std::vector<double> xticks(hp->Data());
    hp = (HPVector*) LookupProperty("ytick");
    std::vector<double> yticks(hp->Data());
    hp = (HPVector*) LookupProperty("ztick");
    std::vector<double> zticks(hp->Data());
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    if (xvisible && ((HPOnOff*) LookupProperty("xgrid"))->AsBool()) {
      gc.color(xc->Data());
      for (int i=0;i<xticks.size();i++) {
	GLfloat t = MapX(xticks[i]);
	DrawXGridLine(gc,t,limits);
      }
    }
    if (yvisible && ((HPOnOff*) LookupProperty("ygrid"))->AsBool()) {
      gc.color(yc->Data());
      for (int i=0;i<yticks.size();i++) {
	GLfloat t = MapY(yticks[i]);
	DrawYGridLine(gc,t,limits);
      }
    }
    if (zvisible && ((HPOnOff*) LookupProperty("zgrid"))->AsBool()) {
      gc.color(zc->Data());
      for (int i=0;i<zticks.size();i++) {
	GLfloat t = MapZ(zticks[i]);
	DrawZGridLine(gc,t,limits);
      }
    }
    gc.depth(true);
  }

  void HandleAxis::DrawXGridLine(RenderEngine &gc, double t, 
				 std::vector<double> limits) {
    double m[16];
    gc.getModelviewMatrix(m);
    if (m[10] > 0) {
      gc.line(t,limits[2],limits[4],
	      t,limits[3],limits[4]);
    } else if (m[10] < 0) {
      gc.line(t,limits[2],limits[5],
	      t,limits[3],limits[5]);
    }
    if (m[6] > 0) {
      gc.line(t,limits[2],limits[4],
	      t,limits[2],limits[5]);
    } else if (m[6] < 0) {
      gc.line(t,limits[3],limits[4],
	      t,limits[3],limits[5]);
    }
  }
  
  void HandleAxis::DrawYGridLine(RenderEngine &gc, double t,
				 std::vector<double> limits) {
    double m[16];
    gc.getModelviewMatrix(m);
    if (m[10] > 0) {
      gc.line(limits[0],t,limits[4],
	      limits[1],t,limits[4]);
    } else if (m[10] < 0) {
      gc.line(limits[0],t,limits[5],
	      limits[1],t,limits[5]);
    }
    if (m[2] > 0) {
      gc.line(limits[0],t,limits[4],
	      limits[0],t,limits[5]);
    } else if (m[2] < 0) {
      gc.line(limits[1],t,limits[4],
	      limits[1],t,limits[5]);
    }
  }

  void HandleAxis::DrawZGridLine(RenderEngine &gc, double t,
				 std::vector<double> limits) {
    double m[16];
    gc.getModelviewMatrix(m);
    if (m[6] > 0) {
      gc.line(limits[0],limits[2],t,
	      limits[1],limits[2],t);
    } else if (m[6] < 0) {
      gc.line(limits[0],limits[3],t,
	      limits[1],limits[3],t);
    }
    if (m[2] > 0) {
      gc.line(limits[0],limits[2],t,
	      limits[0],limits[3],t);
    } else if (m[2] < 0) {
      gc.line(limits[1],limits[2],t,
	      limits[1],limits[3],t);
    }
  }				 

  void HandleAxis::DrawMinorGridLines(RenderEngine &gc) {
    std::vector<double> limits(GetAxisLimits());
    gc.setLineStyle(((HPLineStyle*) LookupProperty("minorgridlinestyle"))->Data());
    gc.depth(false);
    HPVector *hp;
    hp = (HPVector*) LookupProperty("xtick");
    std::vector<double> xticks(hp->Data());
    hp = (HPVector*) LookupProperty("ytick");
    std::vector<double> yticks(hp->Data());
    hp = (HPVector*) LookupProperty("ztick");
    std::vector<double> zticks(hp->Data());
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    HPLinearLog *sp;
    if (((HPOnOff*) LookupProperty("xminorgrid"))->AsBool()) {
      gc.color(xc->Data());
      sp = (HPLinearLog*) LookupProperty("xscale");
      if (sp->Is("linear")) {
	for (int i=0;i<xticks.size()-1;i++) {
	  double t = MapX((xticks[i]+xticks[i+1])/2);
	  DrawXGridLine(gc,t,limits);
	}
      } else {
	for (int i=0;i<xticks.size()-1;i++) {
	  // Ticks should be in integer divisions
	  double t1 = xticks[i];
	  double t2 = xticks[i+1];
	  if (t2 == (t1 + 1)) {
	    int n = 2;
	    while ((t1*n)<t2) {
	      double t = MapX(n*t1);
	      n++;
	      DrawXGridLine(gc,t,limits);
	    }
	  }
	}
      }
    }
    if (((HPOnOff*) LookupProperty("yminorgrid"))->AsBool()) {
      gc.color(yc->Data());
      sp = (HPLinearLog*) LookupProperty("yscale");
      if (sp->Is("linear")) {
	for (int i=0;i<yticks.size()-1;i++) {
	  GLfloat t = MapY((yticks[i]+yticks[i+1])/2);
	  DrawYGridLine(gc,t,limits);
	}
      } else {
	for (int i=0;i<yticks.size()-1;i++) {
	  // Ticks should be in integer divisions
	  double t1 = yticks[i];
	  double t2 = yticks[i+1];
	  if (t2 == (t1 + 1)) {
	    int n = 2;
	    while ((t1*n)<t2) {
	      double t = MapY(n*t1);
	      n++;
	      DrawYGridLine(gc,t,limits);
	    }
	  }
	}
      }
    }
    if (((HPOnOff*) LookupProperty("zminorgrid"))->AsBool()) {
      gc.color(zc->Data());
      sp = (HPLinearLog*) LookupProperty("zscale");
      if (sp->Is("linear")) {
	for (int i=0;i<zticks.size()-1;i++) {
	  GLfloat t = MapZ((zticks[i]+zticks[i+1])/2);
	  DrawZGridLine(gc,t,limits);
	}
      } else {
	for (int i=0;i<zticks.size()-1;i++) {
	  // Ticks should be in integer divisions
	  double t1 = zticks[i];
	  double t2 = zticks[i+1];
	  if (t2 == (t1 + 1)) {
	    int n = 2;
	    while ((t1*n)<t2) {
	      double t = MapZ(n*t1);
	      n++;
	      DrawZGridLine(gc,t,limits);
	    }
	  }
	}
      }
    }
    gc.depth(true);
  }
  
  double HandleAxis::flipX(double t) {
    std::vector<double> limits(GetAxisLimits());
    if (t == limits[0])
      return limits[1];
    return limits[0];
  }

  double HandleAxis::flipY(double t) {
    std::vector<double> limits(GetAxisLimits());
    if (t == limits[2])
      return limits[3];
    return limits[2];
  }

  double HandleAxis::flipZ(double t) {
    std::vector<double> limits(GetAxisLimits());
    if (t == limits[4])
      return limits[5];
    return limits[4];
  }

  void HandleAxis::SetupAxis(RenderEngine &gc) {
    double model[16];
    gc.getModelviewMatrix(model);
    std::vector<double> limits(GetAxisLimits());
    // Query the axisproperties to set the z-position of the
    // x and y axis
    if (((HPTopBottom*)LookupProperty("xaxislocation"))->Is("bottom")) {
      x1pos[2] = limits[4];
    } else
      x1pos[2] = limits[5];
    if (((HPLeftRight*)LookupProperty("yaxislocation"))->Is("left")) {
      y1pos[2] = limits[4];
    } else
      y1pos[2] = limits[5];

    if ((model[10] > 0) && (model[6] > 0)) {
      if (x1pos[2] == limits[4])
	x1pos[1] = limits[3];
      else
	x1pos[1] = limits[2];
    } else if ((model[10] > 0) && (model[6] <= 0)) {
      if (x1pos[2] == limits[4])
	x1pos[1] = limits[2];
      else
	x1pos[1] = limits[3];
    } else if ((model[10] <= 0) && (model[6] > 0)) {
      if (x1pos[2] == limits[4])
	x1pos[1] = limits[2];
      else
	x1pos[1] = limits[3];
    } else if ((model[10] <= 0) && (model[6] <= 0)) {
      if (x1pos[2] == limits[4])
	x1pos[1] = limits[3];
      else
	x1pos[1] = limits[2];
    } 

    // There are two possibilities for where the opposite x axis is
    //   - one option is to use the opposite axis in the y direction
    //   - the other option is to use the opposite position in the z direction
    //   - we have to decide which one to use.  What we can do is take
    //   - the longer axis
    double px0, py0, px1, py1, px2, py2;
    gc.toPixels(limits[0],x1pos[1],x1pos[2],px0,py0);
    gc.toPixels(limits[0],flipY(x1pos[1]),x1pos[2],px1,py1);
    gc.toPixels(limits[0],x1pos[1],flipZ(x1pos[2]),px2,py2);
    double len1, len2;
    len1 = ((px1-px0)*(px1-px0) + (py1-py0)*(py1-py0));
    len2 = ((px2-px0)*(px2-px0) + (py2-py0)*(py2-py0));
    if ((len1 > len2) && (len1 > 0)) {
      x2pos[1] = flipY(x1pos[1]);
      x2pos[2] = x1pos[2];
    } else {
      x2pos[1] = x1pos[1];
      x2pos[2] = flipZ(x1pos[2]);
    }

    //     if (x1pos[1] == limits[3])
    //       x2pos[1] = limits[2];
    //     else if (x1pos[1] == limits[2])
    //       x2pos[1] = limits[3];

    if ((model[10] > 0) && (model[2] > 0)) {
      if (y1pos[2] == limits[4])
	y1pos[0] = limits[1];
      else
	y1pos[0] = limits[0];
    } else if ((model[10] <= 0) && (model[2] > 0)) {
      if (y1pos[2] == limits[4])
	y1pos[0] = limits[0];
      else
	y1pos[0] = limits[1];
    } else if ((model[10] > 0) && (model[2] <= 0)) {
      if (y1pos[2] == limits[4])
	y1pos[0] = limits[0];
      else
	y1pos[0] = limits[1];
    } else if ((model[10] <= 0) && (model[2] <= 0)) {
      if (y1pos[2] == limits[4])
	y1pos[0] = limits[1];
      else
	y1pos[0] = limits[0];
    } 
    gc.toPixels(y1pos[0],limits[2],y1pos[2],px0,py0);
    gc.toPixels(flipX(y1pos[0]),limits[2],y1pos[2],px1,py1);
    gc.toPixels(y1pos[0],limits[2],flipZ(y1pos[2]),px2,py2);
    len1 = ((px1-px0)*(px1-px0) + (py1-py0)*(py1-py0));
    len2 = ((px2-px0)*(px2-px0) + (py2-py0)*(py2-py0));
    if ((len1 > len2) && (len1 > 0)) {
      y2pos[0] = y1pos[0];
      y2pos[2] = flipZ(y1pos[2]);
    } else {
      y2pos[0] = flipX(y1pos[0]);
      y2pos[2] = y1pos[2];
    }

    //     if (y1pos[0] == limits[1])
    //       y2pos[0] = limits[0];
    //     else if (y1pos[0] == limits[0])
    //       y2pos[0] = limits[1];
    
    if (model[6] > 0)
      z1pos[0] = limits[1];
    else if (model[6] <= 0)
      z1pos[0] = limits[0];
    if (model[2] > 0)
      z1pos[1] = limits[2];
    else if (model[2] <= 0)
      z1pos[1] = limits[3];

    //sgn - x - y
    //111 - H - H
    //110 - L - H
    //101 - H - L
    //100 - L - L
    //011 - L - L
    //010 - H - L
    //001 - L - H
    //000 - H - H
    //
    // so, x=H if (!10 ^ 2), and y = H if (!10 ^ 6)
    if ((model[10] > 0) && (model[6] > 0) && (model[2] > 0)) {
      z2pos[0] = limits[1];
      z2pos[1] = limits[3];
    } else if ((model[10] > 0) && (model[6] > 0) && (model[2] < 0)) {
      z2pos[0] = limits[0];
      z2pos[1] = limits[3];
    } else if ((model[10] > 0) && (model[6] < 0) && (model[2] > 0)) {
      z2pos[0] = limits[1];
      z2pos[1] = limits[2];
    } else if ((model[10] > 0) && (model[6] < 0) && (model[2] < 0)) {
      z2pos[0] = limits[0];
      z2pos[1] = limits[2];
    } else if ((model[10] < 0) && (model[6] > 0) && (model[2] > 0)) {
      z2pos[0] = limits[0];
      z2pos[1] = limits[2];
    } else if ((model[10] < 0) && (model[6] > 0) && (model[2] < 0)) {
      z2pos[0] = limits[1];
      z2pos[1] = limits[2];
    } else if ((model[10] < 0) && (model[6] < 0) && (model[2] > 0)) {
      z2pos[0] = limits[0];
      z2pos[1] = limits[3];
    } else if ((model[10] < 0) && (model[6] < 0) && (model[2] < 0)) {
      z2pos[0] = limits[1];
      z2pos[1] = limits[3];
    }

    // Check for ordinal views
    // Z axis isn't visible
    if ((model[2] == 0) && (model[6] == 0)) {
      x2pos[1] = flipY(x1pos[1]);
      x2pos[2] = x1pos[2];
      y2pos[0] = flipX(y1pos[0]);
      y2pos[2] = y2pos[2];
    }
    // X axis isn't visible
    if ((model[6] == 0) && (model[10] == 0)) {
      y2pos[0] = y1pos[0];
      y2pos[2] = flipZ(y1pos[2]);
      z2pos[0] = z1pos[0];
      z2pos[1] = flipY(z1pos[1]);
    }
    // Y axis isn't visible
    if ((model[2] == 0) && (model[10] == 0)) {
      x2pos[1] = x1pos[1];
      x2pos[2] = flipZ(x1pos[2]);
      z2pos[0] = flipX(z1pos[0]);
      z2pos[1] = z1pos[1];
    }

    double x1, y1, x2, y2;
    gc.toPixels(limits[0],x1pos[1],x1pos[2],x1,y1);
    gc.toPixels(limits[1],x1pos[1],x1pos[2],x2,y2);
    xvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);
    gc.toPixels(y1pos[0],limits[2],y1pos[2],x1,y1);
    gc.toPixels(y1pos[0],limits[3],y1pos[2],x2,y2);
    yvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);
    gc.toPixels(z1pos[0],z1pos[1],limits[4],x1,y1);
    gc.toPixels(z1pos[0],z1pos[1],limits[5],x2,y2);
    zvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);  
  }

  bool HandleAxis::Is2DView() {
    return (!(xvisible && yvisible && zvisible));
  }

  void HandleAxis::DrawAxisLines(RenderEngine &gc) { 
    std::vector<double> limits(GetAxisLimits());
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    gc.setLineStyle("-");
    gc.lineWidth(ScalarPropertyLookup("linewidth"));
    gc.setupDirectDraw();
    glBegin(GL_LINES);
    if (xvisible) {
      gc.color(xc->Data());
      double px0, py0, px1, py1;
      gc.toPixels(limits[0],x1pos[1],x1pos[2],px0,py0);
      gc.toPixels(limits[1],x1pos[1],x1pos[2],px1,py1);
      gc.line(px0,py0,px1,py1);
      if (Is2DView()) {
	gc.toPixels(limits[0],x2pos[1],x2pos[2],px0,py0);
	gc.toPixels(limits[1],x2pos[1],x2pos[2],px1,py1);
	gc.line(px0,py0,px1,py1);
      }
    }
    if (yvisible) {
      gc.color(yc->Data());
      double px0, py0, px1, py1;
      gc.toPixels(y1pos[0],limits[2],y1pos[2],px0,py0);
      gc.toPixels(y1pos[0],limits[3],y1pos[2],px1,py1);
      gc.line(px0,py0,px1,py1);
      if (Is2DView()) {
	gc.toPixels(y2pos[0],limits[2],y2pos[2],px0,py0);
	gc.toPixels(y2pos[0],limits[3],y2pos[2],px1,py1);
	gc.line(px0,py0,px1,py1);
      }
    } 
    if (zvisible) {
      gc.color(zc->Data());
      double px0, py0, px1, py1;
      gc.toPixels(z1pos[0],z1pos[1],limits[4],px0,py0);
      gc.toPixels(z1pos[0],z1pos[1],limits[5],px1,py1);
      gc.line(px0,py0,px1,py1);
      if (Is2DView()) {
	gc.toPixels(z2pos[0],z2pos[1],limits[4],px0,py0);
	gc.toPixels(z2pos[0],z2pos[1],limits[5],px1,py1);
	gc.line(px0,py0,px1,py1);
      }
    }
    gc.releaseDirectDraw();
  }

  // Assemble a font for the axis
  void HandleAxis::UpdateAxisFont() {
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
    m_font = fnt;
  }

  int HandleAxis::GetTickCount(RenderEngine &gc,
			       double x1, double y1, double z1,
			       double x2, double y2, double z2) {
    double u1, v1, u2, v2;
    gc.toPixels(x1,y1,z1,u1,v1);
    gc.toPixels(x2,y2,z2,u2,v2);
    double axlen;
    axlen = sqrt((u2-u1)*(u2-u1) + (v2-v1)*(v2-v1));
    int numtics = qMax(2.0,axlen/100.0);
    return numtics;
  }

  void HandleAxis::RecalculateTicks(RenderEngine &gc) {
    // We have to calculate the tick sets for each axis...
    std::vector<double> limits(GetAxisLimits());
    std::vector<double> xticks;
    std::vector<std::string> xlabels;
    std::vector<double> yticks;
    std::vector<std::string> ylabels;
    std::vector<double> zticks;
    std::vector<std::string> zlabels;
    int xcnt, ycnt, zcnt;
    xcnt = GetTickCount(gc,limits[0],x1pos[1],x1pos[2],
			limits[1],x1pos[1],x1pos[2]);
    ycnt = GetTickCount(gc,y1pos[0],limits[2],y1pos[2],
			y1pos[0],limits[3],y1pos[2]);
    zcnt = GetTickCount(gc,z1pos[0],z1pos[1],limits[4],
			z1pos[0],z1pos[1],limits[5]);
    double xStart, xStop;
    double yStart, yStop;
    double zStart, zStop;
    HPTwoVector *tp;
    HPLinearLog *lp;
    lp = (HPLinearLog*)LookupProperty("xscale");
    if (IsAuto("xlimmode")) {
      FormatAxisAuto(limits[0],limits[1],xcnt,
		     lp->Is("log"),xStart,xStop,xticks,xlabels);
      tp = (HPTwoVector*) LookupProperty("xlim");
      std::vector<double> lims; 
      if (lp->Is("linear")) {
	lims.push_back(xStart);
	lims.push_back(xStop);
      } else {
	lims.push_back(pow(10.0,xStart));
	lims.push_back(pow(10.0,xStop));
      }
      tp->Data(lims);
    } else
      FormatAxisManual(limits[0],limits[1],xcnt,
		       lp->Is("log"),xStart,xStop,xticks,xlabels);

    lp = (HPLinearLog*)LookupProperty("yscale");
    if (IsAuto("ylimmode")) {
      FormatAxisAuto(limits[2],limits[3],ycnt,
		     lp->Is("log"),yStart,yStop,yticks,ylabels);
      tp = (HPTwoVector*) LookupProperty("ylim");
      std::vector<double> lims; 
      if (lp->Is("linear")) {
	lims.push_back(yStart);
	lims.push_back(yStop);
      } else {
	lims.push_back(pow(10.0,yStart));
	lims.push_back(pow(10.0,yStop));
      }
      tp->Data(lims);
    } else
      FormatAxisManual(limits[2],limits[3],ycnt,
		       lp->Is("log"),yStart,yStop,yticks,ylabels);

    lp = (HPLinearLog*)LookupProperty("zscale");
    if (IsAuto("zlimmode")) {
      FormatAxisAuto(limits[4],limits[5],zcnt,
		     lp->Is("log"),zStart,zStop,zticks,zlabels);
      tp = (HPTwoVector*) LookupProperty("zlim");
      std::vector<double> lims; 
      if (lp->Is("linear")) {
	lims.push_back(zStart);
	lims.push_back(zStop);
      } else {
	lims.push_back(pow(10.0,zStart));
	lims.push_back(pow(10.0,zStop));
      }
      tp->Data(lims);
    } else
      FormatAxisManual(limits[4],limits[5],zcnt,
		       lp->Is("log"),zStart,zStop,zticks,zlabels);
    // Update the limits...
    
    HPVector *hp;
    HPStringSet *qp;
    if (IsAuto("xtickmode")) {
      hp = (HPVector*) LookupProperty("xtick");
      hp->Data(xticks);
    }
    if (IsAuto("xticklabelmode")) {
      qp = (HPStringSet*) LookupProperty("xticklabel");
      qp->Data(xlabels);
    }
    if (IsAuto("ytickmode")) {
      hp = (HPVector*) LookupProperty("ytick");
      hp->Data(yticks);
    }
    if (IsAuto("yticklabelmode")) {
      qp = (HPStringSet*) LookupProperty("yticklabel");
      qp->Data(ylabels);
    }
    if (IsAuto("ztickmode")) {
      hp = (HPVector*) LookupProperty("ztick");
      hp->Data(zticks);
    }
    if (IsAuto("zticklabelmode")) {
      qp = (HPStringSet*) LookupProperty("zticklabel");
      qp->Data(zlabels);
    }
  }

  void HandleAxis::RePackFigure() {
    int titleHeight = 0;
    int xlabelHeight = 0;
    int ylabelHeight = 0;
    int zlabelHeight = 0;
    int maxLabelHeight = 0;
    int tickHeight = 0;
    HPHandles *lbl;
    if (xvisible) {
      lbl = (HPHandles*) LookupProperty("xlabel");
      if (!lbl->Data().empty()) {
	HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
	xlabelHeight = fp->GetTextHeightInPixels();
      }
    }
    if (yvisible) {
      lbl = (HPHandles*) LookupProperty("ylabel");
      if (!lbl->Data().empty()) {
	HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
	ylabelHeight = fp->GetTextHeightInPixels();
      }
    }
    if (zvisible) {
      lbl = (HPHandles*) LookupProperty("zlabel");
      if (!lbl->Data().empty()) {
	HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
	zlabelHeight = fp->GetTextHeightInPixels();
      }
    }
    lbl = (HPHandles*) LookupProperty("title");
    if (!lbl->Data().empty()) {
      HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
      titleHeight = fp->GetTextHeightInPixels();
    }
    QFontMetrics fm(m_font);
    QRect sze(fm.boundingRect("|"));
    tickHeight =  sze.height();
    // Take the maximum of the title, and label sizes to compute
    // the padding...
    maxLabelHeight = qMax(titleHeight,xlabelHeight);
    maxLabelHeight = qMax(maxLabelHeight,ylabelHeight);
    maxLabelHeight = qMax(maxLabelHeight,zlabelHeight);
    // Get the outer position vector...
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    // Generate a candidate position vector based on the default
    double posx0,posy0,poswidth,posheight;
    poswidth = 0.775*outerpos[2];
    posheight = 0.815*outerpos[3];
    posx0 = 0.13*outerpos[2];
    posy0 = 0.11*outerpos[3];
    // Pad the label height
    maxLabelHeight = maxLabelHeight*1.2 + tickHeight;
    // Check posx0 against maxLabelHeight..
    if (posx0 < maxLabelHeight)
      posx0 = maxLabelHeight;
    // Check posy0 against maxLabelHeight...
    if (posy0 < maxLabelHeight)
      posy0 = maxLabelHeight;
    // Check the width against maxLabelHeight...
    if ((outerpos[2] - poswidth) < 2*maxLabelHeight) {
      poswidth = outerpos[2] - 2*maxLabelHeight;
    }
    if ((outerpos[3] - posheight) < 2*maxLabelHeight) {
      posheight = outerpos[3] - 2*maxLabelHeight;
    }
    HandleFigure *fig = GetParentFigure();
    unsigned width = fig->GetWidth();
    unsigned height = fig->GetHeight();
    // Normalize
    poswidth = poswidth/width;
    posheight = posheight/height;
    posx0 = (posx0+outerpos[0])/width;
    posy0 = (posy0+outerpos[1])/height;
    HPFourVector *hp = (HPFourVector*) LookupProperty("position");
    hp->Value(posx0,posy0,poswidth,posheight);
  }

  void HandleAxis::UpdateLimits(bool x, bool y, bool z, bool a, bool c) {
    if (!x && !y && !z && !a && !c) return;
    // Get our set of children
    std::vector<double> limits;
    bool first = true;
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      std::vector<double> child_limits(fp->GetLimits());
      if (!child_limits.empty()) {
	if (first) {
	  limits = child_limits;
	  first = false;
	} else {
	  for (int i=0;i<qMin(limits.size(),child_limits.size());i+=2) {
	    limits[i] = qMin(limits[i],child_limits[i]);
	    limits[i+1] = qMax(limits[i+1],child_limits[i+1]);
	  }
	}
      }
    }
    if (first) return;
    if (limits[1] == limits[0]) {
      limits[0] = limits[0] - 0.5;
      limits[1] = limits[0] + 1;
    }
    if (limits[3] == limits[2]) {
      limits[2] = limits[2] - 0.5;
      limits[5] = limits[2] + 1;
    }
    if (limits[5] == limits[4]) {
      limits[4] = limits[4] - 0.5;
      limits[5] = limits[4] + 1;
    }
    if (x) SetTwoVectorDefault("xlim",limits[0],limits[1]);
    if (y) SetTwoVectorDefault("ylim",limits[2],limits[3]);
    if (z) SetTwoVectorDefault("zlim",limits[4],limits[5]);
    if (c) SetTwoVectorDefault("clim",limits[6],limits[7]);
    if (a) SetTwoVectorDefault("alim",limits[8],limits[9]);
  }

  void HandleAxis::UpdateState(RenderEngine &gc) {
    std::vector<std::string> tset;
    if (HasChanged("xlim")) ToManual("xlimmode");
    if (HasChanged("ylim")) ToManual("ylimmode");
    if (HasChanged("zlim")) ToManual("zlimmode");
    if (HasChanged("alim")) ToManual("alimmode");
    if (HasChanged("clim")) ToManual("climmode");
    if (HasChanged("xtick")) ToManual("xtickmode");
    if (HasChanged("ytick")) ToManual("ytickmode");
    if (HasChanged("ztick")) ToManual("ztickmode");
    if (HasChanged("xticklabel")) ToManual("xticklabelmode");
    if (HasChanged("yticklabel")) ToManual("yticklabelmode");
    if (HasChanged("zticklabel")) ToManual("zticklabelmode");
    tset.push_back("fontangle");  tset.push_back("fontname");
    tset.push_back("fontsize");   tset.push_back("fontunits");
    tset.push_back("fontweight"); tset.push_back("xticklabel");
    tset.push_back("yticklabel"); tset.push_back("zticklabel");
    tset.push_back("xcolor");     tset.push_back("ycolor"); 
    tset.push_back("zcolor"); 
    if (HasChanged(tset)) {
      UpdateAxisFont();
      ClearChanged(tset);
    }
    // Repack the figure...
    // To repack the figure, we get the heights of the
    // three labels (title, xlabel and ylabel)
    RePackFigure();
    // if ticklabels changed --> tickmode = manual
    // if tickdir set --> tickdirmode = manual
    // if resize || position chng && tickmode = auto --> recalculate tick marks
    // if resize || position chng && ticlabelmode = auto --> recalculate tick labels
    HandleFigure* fig = GetParentFigure();
    if (fig->Resized() || HasChanged("position")) {
      RecalculateTicks(gc);
    }
    // Limits
    bool xflag, yflag, zflag, aflag, cflag;
    xflag = IsAuto("xlimmode");
    yflag = IsAuto("ylimmode");
    zflag = IsAuto("zlimmode");
    aflag = IsAuto("alimmode");
    cflag = IsAuto("climmode");
    UpdateLimits(xflag,yflag,zflag,aflag,cflag);
    // Camera properties...
    if (HasChanged("cameratarget")) 
      ToManual("cameratargetmode");
    if (IsAuto("cameratargetmode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameratarget");
      std::vector<double> limits(GetAxisLimits());
      tv->Value((limits[0]+limits[1])/2.0,
		(limits[2]+limits[3])/2.0,
		(limits[4]+limits[5])/2.0);
    }
    if (HasChanged("cameraposition"))
      ToManual("camerapositionmode");
    if (IsAuto("camerapositionmode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraposition");
      std::vector<double> limits(GetAxisLimits());
      tv->Value((limits[0]+limits[1])/2.0,
		(limits[2]+limits[3])/2.0,
		limits[5]+1);
    }
    if (HasChanged("cameraupvector"))
      ToManual("cameraupvectormode");
    if (IsAuto("cameraupvectormode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraupvector");
      tv->Value(0,1,0);
    }
    RecalculateTicks(gc);
  }

  // The orientation of the label depends on the angle of the
  // tick
  void HandleAxis::DrawLabel(RenderEngine& gc,
			     double dx, double dy, 
			     double x2, double y2, 
			     std::vector<double> color,
			     std::string txt) {
    double angle = atan2(dy,dx)*180.0/M_PI;
    RenderEngine::AlignmentFlag xalign;
    RenderEngine::AlignmentFlag yalign;
    if (fabs(angle) < 10) {
      xalign = RenderEngine::Min;
      yalign = RenderEngine::Mean;
    } else if (fabs(angle) > 170) {
      xalign = RenderEngine::Max;
      yalign = RenderEngine::Mean;
    } else if ((angle >= 10) && (angle < 80)) {
      xalign = RenderEngine::Min;
      yalign = RenderEngine::Min;
    } else if ((angle >= 80) && (angle < 100)) {
      xalign = RenderEngine::Mean;
      yalign = RenderEngine::Min;
    } else if ((angle >= 100) && (angle < 170)) {
      xalign = RenderEngine::Max;
      yalign = RenderEngine::Min;
    } else if ((angle <= -10) && (angle > -80)) {
      xalign = RenderEngine::Min;
      yalign = RenderEngine::Max;
    } else if ((angle <= -80) && (angle > -100)) {
      xalign = RenderEngine::Mean;
      yalign = RenderEngine::Max;
    } else if ((angle <= -100) && (angle > -170)) {
      xalign = RenderEngine::Max;
      yalign = RenderEngine::Max;
    }
    gc.putText(x2,y2,txt,color,xalign,yalign,m_font,0);
  }

  //
  // Look at the z axis... if T*[0;0;1;0] y component is positive,
  // put x and y axis at the bottom.  otherwise, put them at the top.
  //
  void HandleAxis::DrawTickMarks(RenderEngine &gc) {
    // The trick here is to determine where to attach the 
    // three axes - there should be two possibilities for
    // each axis.  We start with four possibilities for
    // each axis.  Now each axis sits on the boundary of
    // two facets.  If exactly one of the two facets is
    // visible, then the axis line is visible.
    HandleFigure *fig = GetParentFigure();
    unsigned width = fig->GetWidth();
    unsigned height = fig->GetHeight();    
    HPVector *hp;
    hp = (HPVector*) LookupProperty("xtick");
    std::vector<double> xticks(hp->Data());
    hp = (HPVector*) LookupProperty("ytick");
    std::vector<double> yticks(hp->Data());
    hp = (HPVector*) LookupProperty("ztick");
    std::vector<double> zticks(hp->Data());
    gc.lineWidth(ScalarPropertyLookup("linewidth"));
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    // Compute the longest 
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
    int maxlen = (position[2] > position[3]) ? position[2] : position[3];
    HPTwoVector *kp = (HPTwoVector*) LookupProperty("ticklength");
    std::vector<double> ticklen(kp->Data());
    int ticlen;
    if (Is2DView())
      ticlen = (int) (maxlen*ticklen[0]);
    else
      ticlen = (int) (maxlen*ticklen[1]);
    float ticdir;
    if (IsAuto("tickdirmode")) {
      if (Is2DView())
	ticdir = 1;
      else
	ticdir = -1;
    } else {
      if (((HPInOut*) LookupProperty("tickdir"))->Is("in")) 
	ticdir = 1;
      else
	ticdir = -1;
    }
    HPStringSet *qp;
    qp = (HPStringSet*) LookupProperty("xticklabel");
    std::vector<std::string> xlabeltxt(qp->Data());
    qp = (HPStringSet*) LookupProperty("yticklabel");
    std::vector<std::string> ylabeltxt(qp->Data());
    qp = (HPStringSet*) LookupProperty("zticklabel");
    std::vector<std::string> zlabeltxt(qp->Data());
    // Draw the ticks
    std::vector<double> limits(GetAxisLimits());
    // Next step - calculate the tick directions...
    // We have to draw the tics in flat space
    //
    // To keep the label from touching the tick labels, we need
    // that the tick label box (x0,y0) --> (x0+maxx,y0+maxy)
    // and we need to advance by (x0+n*dx,y0+n*dy) so that
    // n = max(maxx/dx,maxy/dy)
    //
    gc.setupDirectDraw();
    gc.setLineStyle("-");
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    if (xvisible) {
      std::vector<double> mapticks;
      for (int i=0;i<xticks.size();i++)
	mapticks.push_back(MapX(xticks[i]));
      DrawTickLabels(gc,xc->Data(),
		     0,x1pos[1],x1pos[2],
		     0,x2pos[1],x2pos[2],
		     limits[0],limits[1],
		     1,0,0,
		     mapticks,xlabeltxt,
		     "xlabel",ticlen,ticdir);
    }
    if (yvisible) {
      std::vector<double> mapticks;
      for (int i=0;i<yticks.size();i++)
	mapticks.push_back(MapY(yticks[i]));
      DrawTickLabels(gc,yc->Data(),
		     y1pos[0],0,y1pos[2],
		     y2pos[0],0,y2pos[2],
		     limits[2],limits[3],
		     0,1,0,
		     mapticks,ylabeltxt,
		     "ylabel",ticlen,ticdir);
    }
    if (zvisible) {
      std::vector<double> mapticks;
      for (int i=0;i<zticks.size();i++)
	mapticks.push_back(MapZ(zticks[i]));
      DrawTickLabels(gc,zc->Data(),
		     z1pos[0],z1pos[1],0,
		     z2pos[0],z2pos[1],0,
		     limits[4],limits[5],
		     0,0,1,
		     mapticks,zlabeltxt,
		     "zlabel",ticlen,ticdir);
    }
    HPHandles *lbl = (HPHandles*) LookupProperty("title");
//     if (!lbl->Data().empty()) {
//       HandleText *fp = handleset.lookupHandle(lbl->Data()[0]);
//       HPThreeVector *gp = (HPThreeVector*) fp->LookupProperty("position");
//       // Put the title in the right spot
//       //      fp->PaintMe(gc);
//     }
    gc.releaseDirectDraw();
  }

  void HandleAxis::DrawTickLabels(RenderEngine& gc,
				  std::vector<double> color,
				  double px1, double py1, double pz1,
				  double px2, double py2, double pz2,
				  double limmin, double limmax,
				  double unitx, double unity, double unitz,
				  std::vector<double>  maptics,
				  std::vector<std::string> labels,
				  std::string labelname,
				  int ticlen, double ticdir) {
    gc.color(color);
    // Calculate the tick direction vector
    double dx1, dy1, dx2, dy2;
    gc.toPixels(limmin*unitx+px1,
		limmin*unity+py1,
		limmin*unitz+pz1,dx1,dy1);
    gc.toPixels(limmin*unitx+px2,
		limmin*unity+py2,
		limmin*unitz+pz2,dx2,dy2);
    double delx, dely;
    delx = dx2-dx1; dely = dy2-dy1;
    // normalize the tick length
    double norm = sqrt(delx*delx + dely*dely);
    delx /= norm; dely /= norm;
    for (int i=0;i<maptics.size();i++) {
      double t = maptics[i];
      // Map the coords ourselves
      double x1, y1, x2, y2;
      gc.toPixels(t*unitx+px1,
		  t*unity+py1,
		    t*unitz+pz1,x1,y1);
      x2 = delx*ticlen*ticdir + x1;
      y2 = dely*ticlen*ticdir + y1;
      gc.line(x1,y1,x2,y2);
      double x3, y3;
      if (ticdir > 0) {
	x3 = -delx*0.015*norm + x1;
	y3 = -dely*0.015*norm + y1;
      } else {
	x3 = -delx*0.015*norm + x2;
	y3 = -dely*0.015*norm + y2;
      }
      if (!labels.empty())
	DrawLabel(gc,-delx,-dely,x3,y3,color,
		  labels[i % labels.size()]);
      // For a 2D view, draw the opposite tick marks too
      if (Is2DView()) {
	gc.toPixels(t*unitx+px2,
		    t*unity+py2,
		    t*unitz+pz2,x1,y1);
	x2 = -delx*ticlen*ticdir + x1;
	y2 = -dely*ticlen*ticdir + y1;
	gc.line(x1,y1,x2,y2);
      }
    }
    // Get the maximum tick metric
    double maxx, maxy;
    GetMaxTickMetric(gc,labels,maxx,maxy);
    // Draw the x label
    // Calculate the center of the x axis...
    double x1, x2, x3, y1, y2, y3;
    double meanval;
    meanval = (limmin+limmax)/2.0;
    gc.toPixels(meanval*unitx+px1,
		meanval*unity+py1,
		meanval*unitz+pz1,x1,y1);
    // Calculate the tick offset
    x2 = delx*ticlen*ticdir + x1;
    y2 = dely*ticlen*ticdir + y1;
    // Offset by the top of the label
    if (ticdir > 0) {
      x3 = -delx*0.015*norm + x1;
      y3 = -dely*0.015*norm + y1;
    } else {
      x3 = -delx*0.015*norm + x2;
      y3 = -dely*0.015*norm + y2;
    }
    double lx, ly;
    if (delx != 0)
      lx = fabs(maxx/delx);
    else
      lx = 1e10;
    if (dely != 0)
      ly = fabs(maxy/dely);
    else
      ly = 1e10;
    double lmax;
    lmax = qMin(lx,ly);
    // Set the position of the label
    HPHandles *lbl = (HPHandles*) LookupProperty(labelname);
    if (!lbl->Data().empty()) {
      HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
      // To calculate the angle, we have to look at the axis
      // itself.  The direction of the axis is determined by
      // the projection of [1,0,0] onto the screen plane
      double axx1, axy1, axx2,axy2;
      gc.toPixels(0,0,0,axx1,axy1);
      gc.toPixels(unitx,unity,unitz,axx2,axy2);
      double angle = atan2(axy2-axy1,axx2-axx1)*180.0/M_PI;
      if (angle < -90) angle += 180;
      if (angle > 90) angle -= 180;
      HPScalar *sp = (HPScalar*) fp->LookupProperty("rotation");
      // The angle we want is no the rotation angle of the axis, but
      // the angle of the origin to label position.  We get this
      // taking the mean limit along the unit direction, and the
      // average of the two projected axes.
      double origx, origy;
      gc.toPixels(meanval*unitx+(px1+px2)/2.0,
		  meanval*unity+(py1+py2)/2.0,
		  meanval*unitz+(pz1+pz2)/2.0,origx,origy);
      double meanx, meany;
      gc.toPixels(meanval*unitx+px1,
		  meanval*unity+py1,
		  meanval*unitz+pz1,meanx,meany); 
     
      //       int pixpad = 1.5*fp->GetTextHeightInPixels();
      //       // Offset by the labelsize
      //       if (lmax == lx)
      // 	lmax += fabs(pixpad/delx);
      //       else
      // 	lmax += fabs(pixpad/dely);
      double xl1, yl1;
      xl1 = x3 - lmax*delx;
      yl1 = y3 - lmax*dely;
      double angle2 = atan2(y3-origy,x3-origx)*180.0/M_PI;
      if ((angle == 90) && (angle2 > -90)) {
 	angle = -90;
      }
      if (angle2 == 180)
	angle2 = -180;
      if (angle2 < 0)
	if (fabs(angle) != 90)
	  ((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("top");
	else
	  ((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("bottom");
      else
	((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("bottom");
      if ((angle == -90) && (angle2 == -180))
	angle = 90;
      sp->Value(angle);
      // Move another couple of percent along the radial line
      xl1 += (x3-origx)*0.04;
      yl1 += (y3-origy)*0.04;
      HPThreeVector *gp = (HPThreeVector*) fp->LookupProperty("position");
      // We now have the position of the label in absolute (pixel)
      // coordinates.  Need to translate this to normalized coordinates
      // relative to outerposition.
      std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
      double xnorm, ynorm;
      xnorm = (xl1-outerpos[0])/outerpos[2];
      ynorm = (yl1-outerpos[1])/outerpos[3];
      HandleFigure *fig = GetParentFigure();
      gp->Value(xnorm,ynorm,0.0);
    }      
  }

  void HandleAxis::DrawAxisLabels(RenderEngine& gc) {
    // Set up the "annotation axis"
    gc.lookAt(0,0,1,0.0,0.0,0,0,1,0);
    gc.project(0,1,0,1,-1,1);
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    gc.viewport(outerpos[0],outerpos[1],outerpos[2],outerpos[3]);
    HPHandles *lbl;
    if (xvisible) {
      lbl = (HPHandles*) LookupProperty("xlabel");
      if (!lbl->Data().empty()) {
	HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
	fp->PaintMe(gc);
      }
    }
    if (yvisible) {
      lbl = (HPHandles*) LookupProperty("ylabel");
      if (!lbl->Data().empty()) {
	HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
	fp->PaintMe(gc);
      }      
    }
    if (zvisible) {
      lbl = (HPHandles*) LookupProperty("zlabel");
      if (!lbl->Data().empty()) {
	HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
	fp->PaintMe(gc);
      }      
    }
    lbl = (HPHandles*) LookupProperty("title");
    if (!lbl->Data().empty()) {
      HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
      fp->PaintMe(gc);
    }      
    SetupProjection(gc);
  }

  void HandleAxis::DrawChildren(RenderEngine& gc) {
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      fp->PaintMe(gc);
    }
  }

  void HandleAxis::PaintMe(RenderEngine& gc) {
    if (GetParentFigure() == NULL) return;
    SetupProjection(gc);
    SetupAxis(gc);
    UpdateState(gc);
    DrawBox(gc);
    DrawGridLines(gc);
    DrawMinorGridLines(gc);
    DrawChildren(gc);
    DrawAxisLines(gc);
    DrawTickMarks(gc);
    DrawAxisLabels(gc);
  }
}
