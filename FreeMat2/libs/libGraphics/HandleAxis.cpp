#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "Core.hpp"
#include <qgl.h>

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
  public:
    HandleFigure *hfig;
    BaseFigure(QWidget* parent, const char *Name);
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
  };

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

  void HandleAxis::Transform(double x, double y, double z, 
			     double &i, double &j) {
    // multiply the point by the transformation matrix
    i = camera[0][0]*x + camera[0][1]*y + camera[0][2]*z + camera[0][3];
    j = camera[1][0]*x + camera[1][1]*y + camera[1][2]*z + camera[1][3];
    HPFourVector *Position = (HPFourVector*) LookupProperty("position");
    i = Position->At(0) + (Position->At(2)/2.0) + i*50;
    j = Position->At(1) + (Position->At(3)/2.0) + j*50;
  }

#if 0
  void HandleAxis::DrawMe(DrawEngine& gc) {
    // Draw the box extents - to do something...
    // Load our transformation matrix... this
    // should be part of gc... for now, we 
    // transform the point directly
    HPTwoVector *XLim = (HPTwoVector*) LookupProperty("xlim");
    HPTwoVector *YLim = (HPTwoVector*) LookupProperty("ylim");
    HPTwoVector *ZLim = (HPTwoVector*) LookupProperty("zlim");
    double x1, x2, x3, x4, x5, x6, x7, x8;
    double y1, y2, y3, y4, y5, y6, y7, y8;
    Transform(XLim->At(0),YLim->At(0),ZLim->At(0),x1,y1);
    Transform(XLim->At(0),YLim->At(0),ZLim->At(1),x2,y2);
    Transform(XLim->At(0),YLim->At(1),ZLim->At(1),x3,y3);
    Transform(XLim->At(0),YLim->At(1),ZLim->At(0),x4,y4);
    Transform(XLim->At(1),YLim->At(0),ZLim->At(0),x5,y5);
    Transform(XLim->At(1),YLim->At(0),ZLim->At(1),x6,y6);
    Transform(XLim->At(1),YLim->At(1),ZLim->At(1),x7,y7);
    Transform(XLim->At(1),YLim->At(1),ZLim->At(0),x8,y8);
    // Draw the cube
    gc.setPen(Qt::black);
    gc.drawLine(x1,y1,x2,y2);
    gc.drawLine(x2,y2,x3,y3);
    gc.drawLine(x3,y3,x4,y4);
    gc.drawLine(x4,y4,x1,y1);
    gc.drawLine(x5,y5,x6,y6);
    gc.drawLine(x6,y6,x7,y7);
    gc.drawLine(x7,y7,x8,y8);
    gc.drawLine(x8,y8,x4,y4);
  }
#endif

#if 0
  void HandleAxis::UpdateState() {
    // If any of the axis limits have changed, or the camera parameters
    // have changed, we must recompute the transformation matrices
    // For now, we just do it always...
    // Camera is at position M
    // Looking at target T
    // Up vector is given
    // We want to map the optical axis to the -z axis
    HPThreeVector *CameraTarget = (HPThreeVector*) LookupProperty("cameratarget");
    HPThreeVector *CameraPosition = (HPThreeVector*) LookupProperty("cameraposition");
    HPThreeVector *CameraUpVector = (HPThreeVector*) LookupProperty("cameraupvector");
    pt3d ctarget(CameraTarget->At(0),CameraTarget->At(1),CameraTarget->At(2));
    pt3d cpos(CameraPosition->At(0),CameraPosition->At(1),CameraPosition->At(2));
    pt3d caxis(ctarget - cpos);
    pt3d cup(CameraUpVector->At(0),CameraUpVector->At(1),CameraUpVector->At(2));
    pt3d cright(crossprod(caxis,cup));
    // stuff these into a matrix
    camera[0][0] = cright.x; camera[0][1] = cright.y; camera[0][2] = cright.z;
    camera[1][0] = cup.x; camera[1][1] = cup.y; camera[1][2] = cup.z;
    camera[2][0] = caxis.x; camera[2][1] = caxis.y; camera[2][2] = caxis.z;
    camera[3][0] = 0; camera[3][1] = 0; camera[3][2] = 0; camera[3][3] = 1;
    camera[0][3] = -cpos.x; camera[1][3] = -cpos.y; camera[2][3] = -cpos.z;
  }
#endif
  
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
#if 0
    // set axes area color to white
    bkcolor[0] = 1.0; bkcolor[1] = 1.0; bkcolor[2] = 1.0;
    // set x, y and z colors to black
    XColor[0] = 0.0; XColor[1] = 0.0; XColor[2] = 0.0;
    YColor[0] = 0.0; YColor[1] = 0.0; YColor[2] = 0.0;
    ZColor[0] = 0.0; ZColor[1] = 0.0; ZColor[2] = 0.0;
    // set the x, y and z limits to 0,1
    XLim[0] = 0.0; XLim[1] = 1.0;
    YLim[0] = 0.0; YLim[1] = 1.0;
    ZLim[0] = 0.0; ZLim[1] = 1.0;
    // set the camera position, etc..
    CameraPosition[0] = 0.0; CameraPosition[1] = 0.0; CameraPosition[2] = 1.0;
    CameraTarget[0] = 0.0; CameraTarget[1] = 0.0; CameraTarget[2] = 0.0;
    CameraUpVector[0] = 0.0; CameraUpVector[1] = 1.0; CameraUpVector[2] = 0.0;
    Position[0] = 0; Position[1] = 0; Position[2] = 100; Position[3] = 100;
#endif
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
  }

  void BaseFigure::paintGL() {
    hfig->paintGL();
  }

  void BaseFigure::resizeGL(int width, int height) {
    hfig->resizeGL(width,height);
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
  
  std::vector<double> HandleAxis::GetPositionVectorAsPixels() {
    HPFourVector *hp = (HPFourVector*) LookupProperty("position");
    return (UnitsReinterpret(hp->Data()));
  }

  static void MapPoint4(float m[16], double x, double y, 
		       double z, double w, double &tx,
		       double &ty, double &tz, double &tw) {
    tx = m[0]*x+m[4]*y+m[8]*z+m[12];
    ty = m[1]*x+m[5]*y+m[9]*z+m[13];
    tz = m[2]*x+m[6]*y+m[10]*z+m[14];
    tw = m[3]*x+m[7]*y+m[11]*z+m[15];
  }

  static void MapPoint(float m[16], double x, double y, double z, 
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
    glRotatef(30,0,1,0);
    glRotatef(60,1,0,0);
    // Retrieve it
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,m);
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
    std::vector<double> position(GetPositionVectorAsPixels());
    glViewport(position[0],position[1],position[2],position[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //    glOrtho(xmin,xmax,ymin,ymax,zmax,zmin);
    // Not sure why the 2*zmin is needed...
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
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,m);
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
  
  void HandleAxis::DrawTickMarks() {
    // Get the data on tick locations and the colors...
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
    std::vector<double> position(GetPositionVectorAsPixels());
    int maxlen = (position[2] > position[3]) ? position[2] : position[3];
    HPTwoVector *kp = (HPTwoVector*) LookupProperty("ticklength");
    std::vector<double> ticklen(kp->Data());
    // Have to decide if this is a 2D view
    int ticlen = (int) (maxlen*ticklen[1]);
    // Draw the ticks
    // Retrieve the transformation matrix
    float model[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,model);
    // Map the points from the grid...
    float proj[16];
    glGetFloatv(GL_PROJECTION_MATRIX,proj);
    std::vector<double> limits(GetAxisLimits());
    // map (xi,ymin,zmin) --> (ai,bi)
    for (int i=0;i<xticks.size();i++) {
      double x, y, z, w, tx, ty, tz, tw, wx, wy, wz, ww;
      double a, b;
      x = xticks[i];
      y = limits[2];
      z = limits[4];
      w = 1;
      MapPoint4(model,x,y,z,w,tx,ty,tz,tw);
      MapPoint4(proj,tx,ty,tz,tw,wx,wy,wz,ww);
      wx /= ww;
      wy /= ww;
      wz /= ww;
      a = (wx+1)*(1.0/2)*position[2] + position[0];
      b = (wy+1)*(1.0/2)*position[3] + position[1];
      std::cout << "a = " << a << " b = " << b << "\n";
      glDisable(GL_DEPTH_TEST);
      glBegin(GL_LINES);
      glVertex2f(wx,wy);
      glVertex2f(wx,wy+.1);
      glEnd();
    }    
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
    DrawBox();
    DrawGridLines();
    DrawTickMarks();
    DrawTickLabels();
    DrawAxisLabels();
    DrawChildren();
  }
}
