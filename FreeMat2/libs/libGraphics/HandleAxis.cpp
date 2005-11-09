#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "Core.hpp"
#include <qgl.h>
#include <QMouseEvent>
#include <qapplication.h>
#include <math.h>
#include <qpainter.h>
#include "GLLabel.hpp"

// Tick direction still doesn't work right...

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
//    cameraposition
//    camerapositionmode
//    cameratarget
//    cameratargetmode
//    cameraupvector
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
//    minorgridlinestyle 
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
//    xdir
//    ydir
//    zdir
//    xgrid - done
//    ygrid - done
//    zgrid - done
//    xlabel
//    ylabel
//    zlabel
//    xlim - done
//    ylim - done
//    zlim - done
//    xlimmode
//    ylimmode
//    zlimmode
//    xminorgrid
//    yminorgrid
//    zminorgrid
//    xscale
//    yscale
//    zscale
//    xtick - done
//    ytick - done
//    ztick - done
//    xticklabel - done
//    yticklabel - done
//    zticklabel - done
//    xtickmode - done
//    ytickmode - done
//    ztickmode - done
//    xticklabelmode
//    yticklabelmode
//    zticklabelmode
 

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
  BaseFigure *drawing;

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

  // Construct an axis 
  void FormatAxisManual(double t1, double t2, int tickcount,
			double& tStart, double &tStop,
			std::vector<double> &tickLocations,
			std::vector<std::string> &tlabels) {
    double tBegin, tEnd;
    double delt = (t2-t1)/tickcount;
    int n = ceil(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = floor(log2(rdelt));
    double tDelt = pow(10.0,(double) n)*pow(2.0,(double) p);
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
		      double& tStart, double &tStop,
		      std::vector<double> &tickLocations,
		      std::vector<std::string> &tlabels) {
    double tBegin, tEnd;
    double delt = (tMax-tMin)/tickcount;
    int n = ceil(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = floor(log2(rdelt));
    double tDelt = pow(10.0,(double) n)*pow(2.0,(double) p);
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
      tickLocations.push_back(tloc);
      if (tloc != 0.0)
	exponentialForm |= (fabs(log10(fabs(tloc))) >= 4.0);
    }
    for (int i=0;i<tCount;i++) 
      tlabels.push_back(TrimPrint(tBegin+i*tDelt,exponentialForm));
  }

  void HandleAxis::Transform(double x, double y, double z, 
			     double &i, double &j) {
    // multiply the point by the transformation matrix
    i = camera[0][0]*x + camera[0][1]*y + camera[0][2]*z + camera[0][3];
    j = camera[1][0]*x + camera[1][1]*y + camera[1][2]*z + camera[1][3];
    HPFourVector *Position = (HPFourVector*) LookupProperty("position");
    i = Position->At(0) + (Position->At(2)/2.0) + i*50;
    j = Position->At(1) + (Position->At(3)/2.0) + j*50;
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
    SetScalarDefault("linewidth",0.5);
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
  
  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("axes",AxesFunction,-1,1);
    context->addFunction("set",SetFunction,-1,0);
    context->addFunction("get",GetFunction,2,1,"handle","propname");
    context->addFunction("dmo",DmoFunction,0,0);
  };

  BaseFigure::BaseFigure(QWidget* parent, const char *name) :
    QGLWidget(parent,name) {
      hfig = new HandleFigure;
      handleset.assignHandle(hfig);
  }

  void BaseFigure::initializeGL() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
    glClearDepth(1e10f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }

  void BaseFigure::paintGL() {
    drawing = this;
    hfig->paintGL();
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
    hp = (HPTwoVector*) LookupProperty("xlim");
    lims.push_back(hp->Data()[0]);
    lims.push_back(hp->Data()[1]);
    hp = (HPTwoVector*) LookupProperty("ylim");
    lims.push_back(hp->Data()[0]);
    lims.push_back(hp->Data()[1]);
    hp = (HPTwoVector*) LookupProperty("zlim");
    lims.push_back(hp->Data()[0]);
    lims.push_back(hp->Data()[1]);
    return lims;
  }

  HandleFigure* HandleAxis::GetParentFigure() {
    // Get our parent - should be a figure
    HPHandle *parent = (HPHandle*) LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    HandleObject *fp = handleset.lookupHandle(parent_handle);
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

  static void MapPoint4(double m[16], double x, double y, 
		       double z, double w, double &tx,
		       double &ty, double &tz, double &tw) {
    tx = m[0]*x+m[4]*y+m[8]*z+m[12];
    ty = m[1]*x+m[5]*y+m[9]*z+m[13];
    tz = m[2]*x+m[6]*y+m[10]*z+m[14];
    tw = m[3]*x+m[7]*y+m[11]*z+m[15];
  }

  static void ToPixels(double model[16], double proj[16], double x, double y,
		       double z, double &a, double &b, int viewp[4]) {
    double c1, c2, c3;
    gluProject(x,y,z,model,proj,viewp,&c1,&c2,&c3);
    a = c1;
    b = c2;
  }

  static void GetDirection(double model[16], double proj[16], double x1, double y1,
			   double z1, double x2, double y2, double z2, 
			   int viewp, double &xdir, double &ydir,
			   double ticlen) {
    double qx1, qy1, qx2, qy2;
    ToPixels(model,proj,x1,y1,z1,qx1,qy1,viewp);
    ToPixels(model,proj,x2,y2,z2,qx2,qy2,viewp);
    xdir = (x2-x1);
    ydir = (y2-y1);
    double norm = sqrt(xdir*xdir+ydir*ydir);
    xdir *= ticlen/norm;
    ydir *= ticlen/norm;
  }

  static void MapPoint(double m[16], double x, double y, double z, 
		       double *tx, double *ty, double *tz) {
    *tx = m[0]*x+m[4]*y+m[8]*z+m[12];
    *ty = m[1]*x+m[5]*y+m[9]*z+m[13];
    *tz = m[2]*x+m[6]*y+m[10]*z+m[14];
  }

  static void MinMaxVector(double *vals, int len, double &vmin, double &vmax) {
    vmin = vmax = vals[0];
    for (int i=0;i<len;i++) {
      vmin = (vals[i] < vmin) ? vals[i] : vmin;
      vmax = (vals[i] > vmax) ? vals[i] : vmax;
    }
  }
  void HandleAxis::SetupProjection() {
    // Build the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(arot,0,0,1);
    glRotatef(elev,1,0,0);
    glRotatef(azim,0,0,1);
    // Retrieve it
    double m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,m);
    // Get the axis limits
    std::vector<double> limits(GetAxisLimits());
    // Map the 8 corners of the clipping cube to rotated space
    double xvals[8];
    double yvals[8];
    double zvals[8];
    MapPoint(m,limits[0],limits[2],limits[4],xvals+0,yvals+0,zvals+0);
    MapPoint(m,limits[0],limits[2],limits[5],xvals+1,yvals+1,zvals+1);
    MapPoint(m,limits[0],limits[3],limits[4],xvals+2,yvals+2,zvals+2);
    MapPoint(m,limits[0],limits[3],limits[5],xvals+3,yvals+3,zvals+3);
    MapPoint(m,limits[1],limits[2],limits[4],xvals+4,yvals+4,zvals+4);
    MapPoint(m,limits[1],limits[2],limits[5],xvals+5,yvals+5,zvals+5);
    MapPoint(m,limits[1],limits[3],limits[4],xvals+6,yvals+6,zvals+6);
    MapPoint(m,limits[1],limits[3],limits[5],xvals+7,yvals+7,zvals+7);
    // Get the min and max x, y and z coordinates
    double xmin, xmax, ymin, ymax, zmin, zmax;
    MinMaxVector(xvals,8,xmin,xmax);
    MinMaxVector(yvals,8,ymin,ymax);
    MinMaxVector(zvals,8,zmin,zmax);
    // Get the position vector
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
    glViewport(position[0],position[1],position[2],position[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xmin,xmax,ymin,ymax,zmin,zmax);
  }

  void HandleAxis::DrawBox() {
    // Q: Is this outerposition that's supposed to be colored?
    // Get the limits
    HPColor *hp = (HPColor*) LookupProperty("color");
    if (hp->IsNone()) return;
    std::vector<double> limits(GetAxisLimits());
    glEnable(GL_CULL_FACE);
    glBegin(GL_QUADS);
    glColor3f(hp->Data()[0],hp->Data()[1],hp->Data()[2]);

    glVertex3f( limits[0], limits[2], limits[4]);
    glVertex3f( limits[1], limits[2], limits[4]);
    glVertex3f( limits[1], limits[3], limits[4]);
    glVertex3f( limits[0], limits[3], limits[4]);
    
    glVertex3f( limits[0], limits[2], limits[5]);
    glVertex3f( limits[0], limits[3], limits[5]);
    glVertex3f( limits[1], limits[3], limits[5]);
    glVertex3f( limits[1], limits[2], limits[5]);

    glVertex3f(limits[0], limits[2], limits[4]);
    glVertex3f(limits[0], limits[3], limits[4]);
    glVertex3f(limits[0], limits[3], limits[5]);
    glVertex3f(limits[0], limits[2], limits[5]);

    glVertex3f(limits[1], limits[2], limits[4]);
    glVertex3f(limits[1], limits[2], limits[5]);
    glVertex3f(limits[1], limits[3], limits[5]);
    glVertex3f(limits[1], limits[3], limits[4]);

    glVertex3f(limits[0], limits[2], limits[4]);
    glVertex3f(limits[0], limits[2], limits[5]);
    glVertex3f(limits[1], limits[2], limits[5]);
    glVertex3f(limits[1], limits[2], limits[4]);

    glVertex3f(limits[0], limits[3], limits[4]);
    glVertex3f(limits[1], limits[3], limits[4]);
    glVertex3f(limits[1], limits[3], limits[5]);
    glVertex3f(limits[0], limits[3], limits[5]);

    glEnd();
    glDisable(GL_CULL_FACE);
  }

  void HandleAxis::SetLineStyle(std::string style) {
    if (style == "-") {
      glDisable(GL_LINE_STIPPLE);
      return;
    }
    if (style == "--") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0x00FF);
      return;
    }
    if (style == ":") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0xDDDD);
      return;
    }
    if (style == "-.") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0xF0D0);
      return;
    }
    if (style == "none") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0x0000);
      return;
    }
  }

  void HandleAxis::DrawGridLines() {
    std::vector<double> limits(GetAxisLimits());
    glDisable(GL_DEPTH_TEST);
    // Retrieve the current transformation matrix
    double m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,m);
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
    SetLineStyle(((HPLineStyle*) LookupProperty("gridlinestyle"))->Data());
    glBegin(GL_LINES);
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
      glColor3f(xc->Data()[0],xc->Data()[1],xc->Data()[2]);
      for (int i=0;i<xticks.size();i++) {
	GLfloat t = xticks[i];
	if (m[10] > 0) {
	  glVertex3f(t,limits[2],limits[4]);
	  glVertex3f(t,limits[3],limits[4]);
	} else if (m[10] < 0) {
	  glVertex3f(t,limits[2],limits[5]);
	  glVertex3f(t,limits[3],limits[5]);
	}
	if (m[6] > 0) {
	  glVertex3f(t,limits[2],limits[4]);
	  glVertex3f(t,limits[2],limits[5]);
	} else if (m[6] < 0) {
	  glVertex3f(t,limits[3],limits[4]);
	  glVertex3f(t,limits[3],limits[5]);
	}
      }
    }
    if (((HPOnOff*) LookupProperty("ygrid"))->AsBool()) {
      glColor3f(yc->Data()[0],yc->Data()[1],yc->Data()[2]);
      for (int i=0;i<yticks.size();i++) {
	GLfloat t = yticks[i];
	if (m[10] > 0) {
	  glVertex3f(limits[0],t,limits[4]);
	  glVertex3f(limits[1],t,limits[4]);
	} else if (m[10] < 0) {
	  glVertex3f(limits[0],t,limits[5]);
	  glVertex3f(limits[1],t,limits[5]);
	}
	if (m[2] > 0) {
	  glVertex3f(limits[0],t,limits[4]);
	  glVertex3f(limits[0],t,limits[5]);
	} else if (m[2] < 0) {
	  glVertex3f(limits[1],t,limits[4]);
	  glVertex3f(limits[1],t,limits[5]);
	}
      }
    }
    if (((HPOnOff*) LookupProperty("zgrid"))->AsBool()) {
      glColor3f(zc->Data()[0],zc->Data()[1],zc->Data()[2]);
      for (int i=0;i<zticks.size();i++) {
	GLfloat t = zticks[i];
	if (m[6] > 0) {
	  glVertex3f(limits[0],limits[2],t);
	  glVertex3f(limits[1],limits[2],t);
	} else if (m[6] < 0) {
	  glVertex3f(limits[0],limits[3],t);
	  glVertex3f(limits[1],limits[3],t);
	}
	if (m[2] > 0) {
	  glVertex3f(limits[0],limits[2],t);
	  glVertex3f(limits[0],limits[3],t);
	} else if (m[2] < 0) {
	  glVertex3f(limits[1],limits[2],t);
	  glVertex3f(limits[1],limits[3],t);
	}
      }
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LINE_STIPPLE);
  }
  
  void HandleAxis::ReleaseDirectDraw() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();    
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
    glViewport(position[0],position[1],position[2],position[3]);
  }

  void HandleAxis::SetupDirectDraw() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    glViewport(outerpos[0],outerpos[1],outerpos[2],outerpos[3]);
    glOrtho(outerpos[0],outerpos[0]+outerpos[2],
	    outerpos[1],outerpos[1]+outerpos[3],-1,1);
    glDisable(GL_DEPTH_TEST);
  }

  bool HandleAxis::IsVisibleLine(float nx1, float ny1, float nz1,
				 float nx2, float ny2, float nz2) {
    // Retrieve the modelview matrix
    double model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,model);
    // Transform the normals, and take the z component
    double tx, ty, tz, tw;
    bool plane1visible;
    bool plane2visible;
    MapPoint4(model,nx1,ny1,nz1,0,tx,ty,tz,tw);
    plane1visible = (tz > 0);
    MapPoint4(model,nx2,ny2,nz2,0,tx,ty,tz,tw);
    plane2visible = (tz > 0);
    return (plane1visible ^ plane2visible);
  }

  void HandleAxis::SetupAxis() {
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
   // Project the visible axis positions
    double model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,model);
    std::vector<double> limits(GetAxisLimits());
    // Query the axisproperties to set the z-position of the
    // x and y axis
    if (((HPTopBottom*)LookupProperty("xaxislocation"))->Is("bottom")) {
      xzval = limits[4];
    } else
      xzval = limits[5];
    if (((HPLeftRight*)LookupProperty("yaxislocation"))->Is("left")) {
      yzval = limits[4];
    } else
      yzval = limits[5];
    if ((model[10] > 0) && (model[6] > 0)) {
      if (xzval == limits[4])
	xyval = limits[3];
      else
	xyval = limits[2];
    } else if ((model[10] > 0) && (model[6] < 0)) {
      if (xzval == limits[4])
	xyval = limits[2];
      else
	xyval = limits[3];
    } else if ((model[10] < 0) && (model[6] > 0)) {
      if (xzval == limits[4])
	xyval = limits[2];
      else
	xyval = limits[3];
    } else if ((model[10] < 0) && (model[6] < 0)) {
      if (xzval == limits[4])
	xyval = limits[3];
      else
	xyval = limits[2];
    } 

    if (xyval == limits[3])
      xyval_opposite = limits[2];
    else if (xyval == limits[2])
      xyval_opposite = limits[3];

    if ((model[10] > 0) && (model[2] > 0)) {
      if (yzval == limits[4])
	yxval = limits[1];
      else
	yxval = limits[0];
    } else if ((model[10] < 0) && (model[2] > 0)) {
      if (yzval == limits[4])
	yxval = limits[0];
      else
	yxval = limits[1];
    } else if ((model[10] > 0) && (model[2] < 0)) {
      if (yzval == limits[4])
	yxval = limits[0];
      else
	yxval = limits[1];
    } else if ((model[10] < 0) && (model[2] < 0)) {
      if (yzval == limits[4])
	yxval = limits[1];
      else
	yxval = limits[0];
    } 

    if (yxval == limits[1])
      yxval_opposite = limits[0];
    else if (yxval == limits[0])
      yxval_opposite = limits[1];

    if (model[6]>0)
      zxval = limits[1];
    else if (model[6]<0)
      zxval = limits[0];
    if (model[2]>0)
      zyval = limits[2];
    else if (model[2]<0)
      zyval = limits[3];

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
      zxval_opposite = limits[1];
      zyval_opposite = limits[3];
    } else if ((model[10] > 0) && (model[6] > 0) && (model[2] < 0)) {
      zxval_opposite = limits[0];
      zyval_opposite = limits[3];
    } else if ((model[10] > 0) && (model[6] < 0) && (model[2] > 0)) {
      zxval_opposite = limits[1];
      zyval_opposite = limits[2];
    } else if ((model[10] > 0) && (model[6] < 0) && (model[2] < 0)) {
      zxval_opposite = limits[0];
      zyval_opposite = limits[2];
    } else if ((model[10] < 0) && (model[6] > 0) && (model[2] > 0)) {
      zxval_opposite = limits[0];
      zyval_opposite = limits[2];
    } else if ((model[10] < 0) && (model[6] > 0) && (model[2] < 0)) {
      zxval_opposite = limits[1];
      zyval_opposite = limits[2];
    } else if ((model[10] < 0) && (model[6] < 0) && (model[2] > 0)) {
      zxval_opposite = limits[0];
      zyval_opposite = limits[3];
    } else if ((model[10] < 0) && (model[6] < 0) && (model[2] < 0)) {
      zxval_opposite = limits[1];
      zyval_opposite = limits[3];
    }
    double proj[16];
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
    int viewp[4];
    glGetIntegerv(GL_VIEWPORT,viewp);
    double x1, y1, x2, y2;
    ToPixels(model,proj,limits[0],xyval,xzval,x1,y1,viewp);
    ToPixels(model,proj,limits[1],xyval,xzval,x2,y2,viewp);
    xvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);
    ToPixels(model,proj,yxval,limits[2],yzval,x1,y1,viewp);
    ToPixels(model,proj,yxval,limits[3],yzval,x2,y2,viewp);
    yvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);
    ToPixels(model,proj,zxval,zyval,limits[4],x1,y1,viewp);
    ToPixels(model,proj,zxval,zyval,limits[5],x2,y2,viewp);
    zvisible = (abs(x1-x2) > 2) || (abs(y1-y2) > 2);
  }

  bool HandleAxis::Is2DView() {
    return (!(xvisible && yvisible && zvisible));
  }

  void HandleAxis::DrawAxisLines() { 
    std::vector<double> limits(GetAxisLimits());
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    if (xvisible) {
      glColor3f(xc->Data()[0],xc->Data()[1],xc->Data()[2]);
      glVertex3f(limits[0],xyval,xzval);
      glVertex3f(limits[1],xyval,xzval);
    }
    if (yvisible) {
      glColor3f(yc->Data()[0],yc->Data()[1],yc->Data()[2]);
      glVertex3f(yxval,limits[2],yzval);
      glVertex3f(yxval,limits[3],yzval);
    } 
    if (zvisible) {
      glColor3f(zc->Data()[0],zc->Data()[1],zc->Data()[2]);
      glVertex3f(zxval,zyval,limits[4]);
      glVertex3f(zxval,zyval,limits[5]);
    }
    glEnd();
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

  int HandleAxis::GetTickCount(double x1, double y1, double z1,
			       double x2, double y2, double z2) {
    // Retrieve the transformation matrix
    double model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,model);
    // Map the points from the grid...
    double proj[16];
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
    int viewp[4];
    glGetIntegerv(GL_VIEWPORT,viewp);
    std::vector<double> position(GetPropertyVectorAsPixels("position"));
    double u1, v1, u2, v2;
    ToPixels(model,proj,x1,y1,z1,u1,v1,viewp);
    ToPixels(model,proj,x2,y2,z2,u2,v2,viewp);
    double axlen;
    axlen = sqrt((u2-u1)*(u2-u1) + (v2-v1)*(v2-v1));
    int numtics = QMAX(2.0,axlen/100.0);
    return numtics;
  }

  void HandleAxis::RecalculateTicks() {
    // We have to calculate the tick sets for each axis...
    std::vector<double> limits(GetAxisLimits());
    std::vector<double> xticks;
    std::vector<std::string> xlabels;
    std::vector<double> yticks;
    std::vector<std::string> ylabels;
    std::vector<double> zticks;
    std::vector<std::string> zlabels;
    int xcnt, ycnt, zcnt;
    xcnt = GetTickCount(limits[0],xyval,xzval,
			limits[1],xyval,xzval);
    ycnt = GetTickCount(yxval,limits[2],yzval,
			yxval,limits[3],yzval);
    zcnt = GetTickCount(zxval,zyval,limits[4],
			zxval,zyval,limits[5]);
    double xStart, xStop;
    double yStart, yStop;
    double zStart, zStop;
    HPTwoVector *tp;
    if (IsAuto("xlimmode")) {
      FormatAxisAuto(limits[0],limits[1],xcnt,xStart,xStop,xticks,xlabels);
      tp = (HPTwoVector*) LookupProperty("xlim");
      std::vector<double> lims; 
      lims.push_back(xStart);
      lims.push_back(xStop);
      tp->Data(lims);
    } else
      FormatAxisManual(limits[0],limits[1],xcnt,xStart,xStop,xticks,xlabels);

    if (IsAuto("ylimmode")) {
      FormatAxisAuto(limits[2],limits[3],ycnt,yStart,yStop,yticks,ylabels);
      tp = (HPTwoVector*) LookupProperty("ylim");
      std::vector<double> lims; 
      lims.push_back(yStart);
      lims.push_back(yStop);
      tp->Data(lims);
    } else
      FormatAxisManual(limits[2],limits[3],ycnt,yStart,yStop,yticks,ylabels);

    if (IsAuto("zlimmode")) {
      FormatAxisAuto(limits[4],limits[5],zcnt,zStart,zStop,zticks,zlabels);
      tp = (HPTwoVector*) LookupProperty("zlim");
      std::vector<double> lims; 
      lims.push_back(zStart);
      lims.push_back(zStop);
      tp->Data(lims);
    } else
      FormatAxisManual(limits[4],limits[5],zcnt,zStart,zStop,zticks,zlabels);
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
      qp->Data(xlabels);
    }
    if (IsAuto("ztickmode")) {
      hp = (HPVector*) LookupProperty("ztick");
      hp->Data(zticks);
    }
    if (IsAuto("zticklabelmode")) {
      qp = (HPStringSet*) LookupProperty("zticklabel");
      qp->Data(xlabels);
    }
  }

  void HandleAxis::UpdateState() {
    std::vector<std::string> tset;
    tset.push_back("fontangle");  tset.push_back("fontname");
    tset.push_back("fontsize");   tset.push_back("fontunits");
    tset.push_back("fontweight"); tset.push_back("xticklabel");
    tset.push_back("yticklabel"); tset.push_back("zticklabel");
    tset.push_back("xcolor");     tset.push_back("ycolor"); 
    tset.push_back("zcolor"); 
    if (HasChanged(tset)) {
      UpdateAxisFont();
      GenerateLabels();
      ClearChanged(tset);
    }
    // if ticklabels changed --> tickmode = manual
    // if tickdir set --> tickdirmode = manual
    // if resize || position chng && tickmode = auto --> recalculate tick marks
    // if resize || position chng && ticlabelmode = auto --> recalculate tick labels
    HandleFigure* fig = GetParentFigure();
    if (fig->Resized() || HasChanged("position")) {
      RecalculateTicks();
      GenerateLabels();
    }
    // Need to to test for camera position here...
    RecalculateTicks();
    GenerateLabels();
  }

  void HandleAxis::GenerateLabels() {
    HPStringSet *qp;
    qp = (HPStringSet*) LookupProperty("xticklabel");
    std::vector<std::string> xlabeltxt(qp->Data());
    qp = (HPStringSet*) LookupProperty("yticklabel");
    std::vector<std::string> ylabeltxt(qp->Data());
    qp = (HPStringSet*) LookupProperty("zticklabel");
    std::vector<std::string> zlabeltxt(qp->Data());
    QFont fnt(m_font);
    HPColor *xc = (HPColor*) LookupProperty("xcolor");
    HPColor *yc = (HPColor*) LookupProperty("ycolor");
    HPColor *zc = (HPColor*) LookupProperty("zcolor");
    xlabels.clear();
    for (int i=0;i<xlabeltxt.size();i++)
      xlabels.push_back(GLLabel(fnt,xlabeltxt[i],xc->Data()[0]*255,
				xc->Data()[1]*255,xc->Data()[2]*255));
    ylabels.clear();
    for (int i=0;i<ylabeltxt.size();i++)
      ylabels.push_back(GLLabel(fnt,ylabeltxt[i],yc->Data()[0]*255,
				yc->Data()[1]*255,yc->Data()[2]*255));
    zlabels.clear();
    for (int i=0;i<zlabeltxt.size();i++)
      zlabels.push_back(GLLabel(fnt,zlabeltxt[i],zc->Data()[0]*255,
				zc->Data()[1]*255,zc->Data()[2]*255));
  }

  // The orientation of the label depends on the angle of the
  // tick
  void HandleAxis::DrawLabel(double dx, double dy, 
			     double x2, double y2, GLLabel& a) {
    double angle = atan2(dy,dx)*180.0/M_PI;
    GLLabel::AlignmentFlag xalign;
    GLLabel::AlignmentFlag yalign;
    if (fabs(angle) < 10) {
      xalign = GLLabel::Min;
      yalign = GLLabel::Mean;
    } else if (fabs(angle) > 170) {
      xalign = GLLabel::Max;
      yalign = GLLabel::Mean;
    } else if ((angle >= 10) && (angle < 80)) {
      xalign = GLLabel::Min;
      yalign = GLLabel::Min;
    } else if ((angle >= 80) && (angle < 100)) {
      xalign = GLLabel::Mean;
      yalign = GLLabel::Min;
    } else if ((angle >= 100) && (angle < 170)) {
      xalign = GLLabel::Max;
      yalign = GLLabel::Min;
    } else if ((angle <= -10) && (angle > -80)) {
      xalign = GLLabel::Min;
      yalign = GLLabel::Max;
    } else if ((angle <= -80) && (angle > -100)) {
      xalign = GLLabel::Mean;
      yalign = GLLabel::Max;
    } else if ((angle <= -100) && (angle > -170)) {
      xalign = GLLabel::Max;
      yalign = GLLabel::Max;
    }
    a.DrawMe(x2,y2,xalign,yalign);
  }

  //
  // Look at the z axis... if T*[0;0;1;0] y component is positive,
  // put x and y axis at the bottom.  otherwise, put them at the top.
  //
  void HandleAxis::DrawTickMarks() {
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
    // Draw the ticks
    // Retrieve the transformation matrix
    double model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,model);
    // Map the points from the grid...
    double proj[16];
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
    int viewp[4];
    glGetIntegerv(GL_VIEWPORT,viewp);
    std::vector<double> limits(GetAxisLimits());
    // Assemble the font we need to draw the labels
    QFont fnt(m_font);
    // Next step - calculate the tick directions...
    // We have to draw the tics in flat space
    SetupDirectDraw();
    
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    glBegin(GL_LINES);
    glVertex2f(0,0);
    glVertex2f(0,10);
    glVertex2f(0,0);
    glVertex2f(10,0);
    glVertex2f(outerpos[2]-1,outerpos[3]-1);
    glVertex2f(outerpos[2]-10,outerpos[3]-1);
    glVertex2f(outerpos[2]-1,outerpos[3]-1);
    glVertex2f(outerpos[2]-1,outerpos[3]-10);
    glEnd();

    if (xvisible) {
      glColor3f(xc->Data()[0],xc->Data()[1],xc->Data()[2]);
      for (int i=0;i<xticks.size();i++) {
	GLfloat t = xticks[i];
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	ToPixels(model,proj,t,xyval,xzval,x1,y1,viewp);
	ToPixels(model,proj,t,xyval_opposite,xzval,x2,y2,viewp);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
	x2 = delx*ticlen*ticdir + x1;
	y2 = dely*ticlen*ticdir + y1;
	glBegin(GL_LINES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glEnd();
	double x3, y3;
	x3 = -delx*0.01*1.25 + x1;
	y3 = -dely*0.01*1.25 + y1;
	if (~xlabels.empty())
	  DrawLabel(-delx,-dely,x3,y3,xlabels[i % xlabels.size()]);
      }
    }
    if (yvisible) {
      glColor3f(yc->Data()[0],yc->Data()[1],yc->Data()[2]);
      for (int i=0;i<yticks.size();i++) {
	GLfloat t = yticks[i];
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	ToPixels(model,proj,yxval,t,yzval,x1,y1,viewp);
	ToPixels(model,proj,yxval_opposite,t,yzval,x2,y2,viewp);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
	x2 = delx*ticlen*ticdir + x1;
	y2 = dely*ticlen*ticdir + y1;
	glBegin(GL_LINES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glEnd();
	double x3, y3;
	x3 = -delx*0.01*1.25 + x1;
	y3 = -dely*0.01*1.25 + y1;
	if (~ylabels.empty())
	  DrawLabel(-delx,-dely,x3,y3,ylabels[i % ylabels.size()]);
      }
    }
    if (zvisible) {
      glColor3f(zc->Data()[0],zc->Data()[1],zc->Data()[2]);
      for (int i=0;i<zticks.size();i++) {
	GLfloat t = zticks[i];
	// Map the coords ourselves
	double x1, y1, x2, y2, delx, dely;
	double norm;
	ToPixels(model,proj,zxval,zyval,t,x1,y1,viewp);
	ToPixels(model,proj,zxval_opposite,zyval_opposite,t,x2,y2,viewp);
	delx = x2-x1; dely = y2-y1;
	// normalize the tick length
	norm = sqrt(delx*delx + dely*dely);
	delx /= norm; dely /= norm;
	x2 = delx*ticlen*ticdir + x1;
	y2 = dely*ticlen*ticdir + y1;
	glBegin(GL_LINES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glEnd();
	double x3, y3;
	x3 = -delx*0.01*1.25 + x1;
	y3 = -dely*0.01*1.25 + y1;
	if (~zlabels.empty())
	  DrawLabel(-delx,-dely,x3,y3,zlabels[i % zlabels.size()]);
      }
    }
    ReleaseDirectDraw();
  }

  void HandleAxis::DrawTickLabels() {
  }

  void HandleAxis::DrawAxisLabels() {
  }

  void HandleAxis::DrawChildren() {
  }

  void HandleAxis::paintGL() {
    if (GetParentFigure() == NULL) return;
    // Time to draw the axis...  
    SetupProjection();
    SetupAxis();
    UpdateState();
    DrawBox();
    DrawGridLines();
    DrawAxisLines();
    DrawTickMarks();
    DrawTickLabels();
    DrawAxisLabels();
    DrawChildren();
  }
}
