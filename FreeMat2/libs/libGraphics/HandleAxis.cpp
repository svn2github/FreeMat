#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "HandleText.hpp"
#include "Core.hpp"
#include <QMouseEvent>
#include <qapplication.h>
#include <math.h>
#include <qpainter.h>
#include "GLRenderEngine.hpp"

// Need to add the labels...


// These are globals for now... ultimately, they need to be handled
// differently...
int azim = 0;
int elev = 0;
int arot = 0;

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
//    childrenint
//    clim
//    climmode
//    clipping
//    color - done (does 'none' work?)
//    colororder
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
//    xlabel
//    ylabel
//    zlabel
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
  class BaseFigure : public QGLWidget {
    float beginx, beginy;
    bool elevazim;
  public:
    HandleFigure *hfig;
    BaseFigure(QWidget* parent, const char *Name);
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    // Support dragging...
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
  };

  // Probably a better way to do this...
  HandleList<HandleObject*> handleset;

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
	*(p+1) = '0';
	*(p+2) = 0;
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
    AddProperty(new HPThreeVector,"cameraupvectormode");
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
    //    AddProperty(new HPLineStyleSet,"linestyleorder");
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
    AddProperty(new HPHandle,"title");
    AddProperty(new HPString,"type");
    AddProperty(new HPUnits,"units");
    //    AddProperty(new Array,"userdata");
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
    AddProperty(new HPHandle,"xlabel");
    AddProperty(new HPHandle,"ylabel");
    AddProperty(new HPHandle,"zlabel");
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
    SetThreeVectorDefault("color",-1,-1,-1);
    SetConstrainedStringDefault("dataaspectratiomode","auto");
    //    SetConstrainedStringDefault("drawmode","normal");
    SetConstrainedStringDefault("fontangle","normal");
    SetStringDefault("fontname","helvetica");
    SetScalarDefault("fontsize",12);
    SetConstrainedStringDefault("fontunits","points");
    SetConstrainedStringDefault("fontweight","normal");
    SetConstrainedStringDefault("gridlinestyle",":");
    SetConstrainedStringDefault("handlevisibility","on");
    SetConstrainedStringDefault("hittest","on");
    SetConstrainedStringDefault("interruptible","on");
    SetConstrainedStringDefault("layer","bottom");
    SetScalarDefault("linewidth",1.0);
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

  // Construct an axis object 
  ArrayVector AxesFunction(int nargout, const ArrayVector& arg) {
    HandleObject *fp = new HandleAxis;
    unsigned int handle = handleset.assignHandle(fp);
    ArrayVector t(arg);
    while (t.size() >= 2) {
      std::string propname(ArrayToString(t[0]));
      fp->LookupProperty(propname)->Set(t[1]);
      t.erase(t.begin(),t.begin()+2);
    }
    //    fp->UpdateState();
    return singleArrayVector(Array::uint32Constructor(handle));
  }

  ArrayVector SetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("set doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    std::string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleObject *fp = handleset.lookupHandle(handle);
    // Use the address and property name to lookup the Get/Set handler
    fp->LookupProperty(propname)->Set(arg[2]);
    //    fp->UpdateState();
    return ArrayVector();
  }

  ArrayVector GetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("get doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    std::string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleObject *fp = handleset.lookupHandle(handle);
    // Use the address and property name to lookup the Get/Set handler
    return singleArrayVector(fp->LookupProperty(propname)->Get());
  }

  ArrayVector DmoFunction(int nargout, const ArrayVector& arg) {
    // Create a figure...
    BaseFigure* fp = new BaseFigure(NULL,NULL);
    // Show it
    fp->show();
    return ArrayVector();
  }

  ArrayVector TextFunction(int nargout, const ArrayVector& arg) {
    HandleObject *fp = new HandleText;
    unsigned int handle = handleset.assignHandle(fp);
    ArrayVector t(arg);
    while (t.size() >= 2) {
      std::string propname(ArrayToString(t[0]));
      fp->LookupProperty(propname)->Set(t[1]);
      t.erase(t.begin(),t.begin()+2);
    }
    //    fp->UpdateState();
    return singleArrayVector(Array::uint32Constructor(handle));
  }
  
  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("axes",AxesFunction,-1,1);
    context->addFunction("text",TextFunction,-1,1);
    context->addFunction("set",SetFunction,-1,0);
    context->addFunction("get",GetFunction,2,1,"handle","propname");
    context->addFunction("dmo",DmoFunction,0,0);
  };

  BaseFigure::BaseFigure(QWidget* parent, const char *name) :
    QGLWidget(parent) {
      hfig = new HandleFigure;
      handleset.assignHandle(hfig);
  }

  void BaseFigure::initializeGL() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
  }

  void BaseFigure::paintGL() {
    GLRenderEngine gc(this,0,0,width(),height());
    gc.clear();
    hfig->PaintMe(gc);
  }

  void BaseFigure::resizeGL(int width, int height) {
    hfig->resizeGL(width,height);
  }

  void BaseFigure::mousePressEvent(QMouseEvent* e) {
    if (e->button() & Qt::LeftButton)
      elevazim = true;
    else
      elevazim = false;
    beginx = e->x();
    beginy = e->y();
  }

  void BaseFigure::mouseMoveEvent(QMouseEvent* e) {
    if (elevazim) {
      elev -= (e->y() - beginy);
      azim += (e->x() - beginx);
      elev = (elev + 360) % 360;
      azim = (azim + 360) % 360;
    } else {
      arot += (e->y() - beginy);
      arot = (arot + 360) % 360;
    }
    beginx = e->x();
    beginy = e->y();
    updateGL();
  }
  
  void BaseFigure::mouseReleaseEvent(QMouseEvent* e) {
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
    HPHandle *parent = (HPHandle*) LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    HandleObject *fp = handleset.lookupHandle(parent_handle);
    HPString *name = (HPString*) fp->LookupProperty("type");
    if (!name) return NULL;
    if (!name->Is("figure")) return NULL;
    HandleFigure *fig = (HandleFigure*) fp;
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
    if (((HPOnOff*) LookupProperty("xgrid"))->AsBool()) {
      gc.color(xc->Data());
      for (int i=0;i<xticks.size();i++) {
	GLfloat t = MapX(xticks[i]);
	DrawXGridLine(gc,t,limits);
      }
    }
    if (((HPOnOff*) LookupProperty("ygrid"))->AsBool()) {
      gc.color(yc->Data());
      for (int i=0;i<yticks.size();i++) {
	GLfloat t = MapY(yticks[i]);
	DrawYGridLine(gc,t,limits);
      }
    }
    if (((HPOnOff*) LookupProperty("zgrid"))->AsBool()) {
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
    gc.depth(false);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    if (xvisible) {
      gc.color(xc->Data());
      gc.line(limits[0],x1pos[1],x1pos[2],
	      limits[1],x1pos[1],x1pos[2]);
    }
    if (yvisible) {
      gc.color(yc->Data());
      gc.line(y1pos[0],limits[2],y1pos[2],
	      y1pos[0],limits[3],y1pos[2]);
    } 
    if (zvisible) {
      gc.color(zc->Data());
      gc.line(z1pos[0],z1pos[1],limits[4],
	      z1pos[0],z1pos[1],limits[5]);
    }
    gc.depth(true);
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

  void HandleAxis::UpdateState(RenderEngine &gc) {
    std::vector<std::string> tset;
    if (HasChanged("xticklabel")) 
      ToManual("xticklabelmode");
    if (HasChanged("yticklabel"))
      ToManual("yticklabelmode");
    if (HasChanged("zticklabel")) 
      ToManual("zticklabelmode");
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
    // if ticklabels changed --> tickmode = manual
    // if tickdir set --> tickdirmode = manual
    // if resize || position chng && tickmode = auto --> recalculate tick marks
    // if resize || position chng && ticlabelmode = auto --> recalculate tick labels
    HandleFigure* fig = GetParentFigure();
    if (fig->Resized() || HasChanged("position")) {
      RecalculateTicks(gc);
    }
    // Camera properties...
    if (HasChanged("cameratarget")) 
      ToManual("cameratargetmode");
    if (IsAuto("cameratargetmode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameratarget");
      std::vector<double> limits(GetAxisLimits());
      std::vector<double> center;
      center.push_back((limits[0]+limits[1])/2.0);
      center.push_back((limits[2]+limits[3])/2.0);
      center.push_back((limits[4]+limits[5])/2.0);
      tv->Data(center);
    }
    if (HasChanged("cameraposition"))
      ToManual("camerapositionmode");
    if (IsAuto("camerapositionmode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraposition");
      std::vector<double> limits(GetAxisLimits());
      std::vector<double> center;
      center.push_back((limits[0]+limits[1])/2.0);
      center.push_back((limits[2]+limits[3])/2.0);
      center.push_back(limits[5]+1);
      tv->Data(center);
    }
    if (HasChanged("cameraupvector"))
      ToManual("cameraupvectormode");
    if (IsAuto("cameraupvectormode")) {
      // Default to 2D
      HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraupvector");
      std::vector<double> center;
      center.push_back(0);
      center.push_back(1);
      center.push_back(0);
      tv->Data(center);      
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
    gc.setupDirectDraw();
    gc.setLineStyle("-");
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    if (xvisible) {
      gc.color(xc->Data());
      for (int i=0;i<xticks.size();i++) {
	double t = MapX(xticks[i]);
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	gc.toPixels(t,x1pos[1],x1pos[2],x1,y1);
	gc.toPixels(t,x2pos[1],x2pos[2],x2,y2);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
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
	if (~xlabeltxt.empty())
	  DrawLabel(gc,-delx,-dely,x3,y3,xc->Data(),xlabeltxt[i % xlabeltxt.size()]);
      }
    }
    if (yvisible) {
      gc.color(yc->Data());
      for (int i=0;i<yticks.size();i++) {
	double t = MapY(yticks[i]);
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	gc.toPixels(y1pos[0],t,y1pos[2],x1,y1);
	gc.toPixels(y2pos[0],t,y2pos[2],x2,y2);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
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
	if (~ylabeltxt.empty())
	  DrawLabel(gc,-delx,-dely,x3,y3,yc->Data(),ylabeltxt[i % ylabeltxt.size()]);
      }
    }
    if (zvisible) {
      gc.color(zc->Data());
      for (int i=0;i<zticks.size();i++) {
	double t = MapZ(zticks[i]);
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	gc.toPixels(z1pos[0],z1pos[1],t,x1,y1);
	gc.toPixels(z2pos[0],z2pos[1],t,x2,y2);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
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
	if (~zlabeltxt.empty())
	  DrawLabel(gc,-delx,-dely,x3,y3,zc->Data(),zlabeltxt[i % zlabeltxt.size()]);
      }
    }
    gc.releaseDirectDraw();
  }

  void HandleAxis::DrawTickLabels() {
  }

  void HandleAxis::DrawAxisLabels() {
  }

  void HandleAxis::DrawChildren(RenderEngine& gc) {
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = handleset.lookupHandle(handles[i]);
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
    DrawAxisLines(gc);
    DrawTickMarks(gc);
    DrawAxisLabels();
    DrawChildren(gc);
  }
}
