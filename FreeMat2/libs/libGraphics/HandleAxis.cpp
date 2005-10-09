#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "Core.hpp"

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
namespace FreeMat {
  class HandleProperty {
    bool modified;
  public:
    HandleProperty() {modified = false;}
    virtual ~HandleProperty() {}
    virtual Array Get() = 0;
    virtual void Set(Array) {modified = true;}
    void ClearModified() {modified = false;}
    bool isModified() {return modified;}
  };

  class HPHandles : public HandleProperty {
  protected:
    int m_len;
    std::vector<unsigned> data;
  public:
    HPHandles(int len);
    HPHandles();
    virtual ~HPHandles() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<unsigned> Data() {return data;}
    void Data(std::vector<unsigned> m) {data = m;}
  };

  class HPHandle : public HPHandles {
  public:
    HPHandle() : HPHandles(1) {}
    ~HPHandle() {}
  };

  class HPVector : public HandleProperty {
  protected:
    std::vector<double> data;
  public:
    HPVector() {}
    virtual ~HPVector() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<double> Data() {return data;}
    void Data(std::vector<double> m) {data = m;}
    double& operator[](int ndx);
    double& At(int ndx);
  };

  class HPFixedVector : public HPVector {
  protected:
    unsigned m_len;
  public:
    HPFixedVector(unsigned len) : m_len(len) {for (int i=0;i<len;i++) data.push_back(0);}
    virtual ~HPFixedVector() {}
    virtual void Set(Array);    
  };

  class HPString : public HandleProperty {
  protected:
    std::string data;
  public:
    HPString() {}
    virtual ~HPString() {}
    virtual Array Get();
    virtual void Set(Array);
    std::string Data() {return data;}
    void Data(std::string m) {data = m;}
    void Value(std::string m) {data = m;}
  };

  class HPStringSet : public HandleProperty {
  protected:
    std::vector<std::string> data;
  public:
    HPStringSet() {}
    virtual ~HPStringSet() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<std::string> Data() {return data;}
    void Data(std::vector<std::string> m) {data = m;}
  };

  class HPConstrainedString : public HPString {
  protected:
    std::vector<std::string> m_dictionary;
  public:
    HPConstrainedString(std::vector<std::string> dict) : m_dictionary(dict) {
      data = dict[0];
    }
    HPConstrainedString(const char **dict) {
      while (*dict) {
	m_dictionary.push_back(*dict);
	dict++;
      }
    }
    virtual void Set(Array);    
  };

  class HandleObject {
    SymbolTable<HandleProperty*> m_properties;
  public:
    HandleObject();
    virtual ~HandleObject() {}
    virtual void RegisterProperties() {}
    void AddProperty(HandleProperty* prop, std::string name);
    HandleProperty* LookupProperty(std::string name);
    void SetConstrainedStringDefault(std::string name, std::string value);
    void SetTwoVectorDefault(std::string name, double x, double y);
    void SetThreeVectorDefault(std::string name, double x, double y, double z);
    void SetStringDefault(std::string name, std::string value);
    void SetScalarDefault(std::string name, double value);
    virtual void DrawMe(DrawEngine& gc) = 0;
    virtual void UpdateState() = 0;
  };
  
  class HPTwoVector : public HPFixedVector {
  public:
    HPTwoVector() : HPFixedVector(2) {}
    virtual ~HPTwoVector() {}
    void Value(double x, double y) {At(0) = x; At(1) = y;}
  };

  class HPThreeVector : public HPFixedVector {
  public:
    HPThreeVector() : HPFixedVector(3) {}
    virtual ~HPThreeVector() {}
    void Value(double x, double y, double z) {At(0) = x; At(1) = y; At(2) = z;}
  };
  
  class HPFourVector : public HPFixedVector {
  public:
    HPFourVector() : HPFixedVector(4) {}
    virtual ~HPFourVector() {}
  };

  const char *auto_manual_dict[3] = {"auto","manual",0};

  class HPAutoManual : public HPConstrainedString {
  public:
    HPAutoManual() : HPConstrainedString(auto_manual_dict) {}
    virtual ~HPAutoManual() {}
  };

  class HPColor : public HPFixedVector {
  public:
    HPColor() : HPFixedVector(3) {}
    virtual ~HPColor() {}
    virtual void Set(Array);    
    Array Get();    
  };

  class HPColorVector : public HPVector {
  public:
    HPColorVector() : HPVector() {}
    virtual ~HPColorVector() {}
    virtual void Set(Array);    
    Array Get();    
  };

  const char *on_off_dict[3] = {"on","off",0};

  class HPOnOff : public HPConstrainedString {
  public:
    HPOnOff() : HPConstrainedString(on_off_dict) {}
    virtual ~HPOnOff() {}
  };

  class HPScalar : public HPFixedVector {
  public:
    HPScalar() : HPFixedVector(1) {}
    virtual ~HPScalar() {}
    void Value(double x);
  };

  const char *font_angle_dict[4] = {"normal","italic","oblique",0};
    
  class HPFontAngle : public HPConstrainedString {
  public:
    HPFontAngle() : HPConstrainedString(font_angle_dict) {}
    virtual ~HPFontAngle() {}
  };
  
  const char *font_units_dict[6] = {"points","normalized","inches",
				    "centimeters","pixels",0};
  
  class HPFontUnits : public HPConstrainedString {
  public:
    HPFontUnits() : HPConstrainedString(font_units_dict) {}
    virtual ~HPFontUnits() {}
  };
  
  const char *font_weight_dict[5] = {"normal","bold","light","demi",0};

  class HPFontWeight : public HPConstrainedString {
  public:
    HPFontWeight() : HPConstrainedString(font_weight_dict) {}
    virtual ~HPFontWeight() {}
  };

  const char *line_style_dict[6] = {"-","--",":","-.","none",0};

  class HPLineStyle : public HPConstrainedString {
  public:
    HPLineStyle() : HPConstrainedString(line_style_dict) {}
    virtual ~HPLineStyle() {}
  };

  const char *top_bottom_dict[3] = {"top","bottom",0};

  class HPTopBottom : public HPConstrainedString {
  public:
    HPTopBottom() : HPConstrainedString(top_bottom_dict) {}
    virtual ~HPTopBottom() {}
  };

  const char *left_right_dict[3] = {"left","right",0};

  class HPLeftRight : public HPConstrainedString {
  public:
    HPLeftRight() : HPConstrainedString(left_right_dict) {}
    virtual ~HPLeftRight() {}
  };

  const char *normal_reverse_dict[3] = {"normal","reverse",0};

  class HPNormalReverse : public HPConstrainedString {
  public:
    HPNormalReverse() : HPConstrainedString(normal_reverse_dict) {}
    virtual ~HPNormalReverse() {}
  };

  const char *linear_log_dict[3] = {"linear","log",0};

  class HPLinearLog : public HPConstrainedString {
  public:
    HPLinearLog() : HPConstrainedString(linear_log_dict) {}
    virtual ~HPLinearLog() {}
  };
  
  const char *next_plot_dict[4] = {"add","replace","replacechildren",0};

  class HPNextPlotMode : public HPConstrainedString {
  public:
    HPNextPlotMode() : HPConstrainedString(next_plot_dict) {}
    virtual ~HPNextPlotMode() {}
  };

  const char *projection_mode_dict[3] = {"orthographic","perspective",0};

  class HPProjectionMode : public HPConstrainedString {
  public:
    HPProjectionMode() : HPConstrainedString(projection_mode_dict) {}
    virtual ~HPProjectionMode() {}
  };
  
  const char *in_out_dict[3] = {"in","out",0};

  class HPInOut : public HPConstrainedString {
  public:
    HPInOut() : HPConstrainedString(in_out_dict) {}
    virtual ~HPInOut() {}
  };

  const char *units_dict[7] = {"inches","centimeters","normalized",
			       "points","pixels","characters",0};
  
  class HPUnits : public HPConstrainedString {
  public:
    HPUnits() : HPConstrainedString(units_dict) {}
    virtual ~HPUnits() {}
  };
  
  const char *position_dict[3] = {"outerposition","position",0};
  
  class HPPosition : public HPConstrainedString {
  public:
    HPPosition() : HPConstrainedString(position_dict) {}
    virtual ~HPPosition() {}
  };
  
  //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis : public HandleObject {
  public:
    SymbolTable<HandleProperty*> properties;
    double camera[4][4];

    HandleAxis();
    virtual ~HandleAxis();
    virtual void ConstructProperties();
    virtual void DrawMe(DrawEngine& gc);
    virtual void UpdateState();
    void SetupDefaults();
    void Transform(double x, double y, double z, double &i, double &j);
  };

  //Figure
  //   contains one or more axes
  //   to redraw the figure, we proxy our draws to the axes
  //   Axes
  //   contains one or more children
  //     to redraw the axes, we clear it with bkcolor
  //     then draw it
  //     set the transformation & clipping
  //     then draw the children
  class HandleFigure : public HandleObject {
  public:
    HandleFigure();
    virtual ~HandleFigure() {}
    virtual void ConstructProperties();
    virtual void DrawMe(DrawEngine& gc);
    virtual void UpdateState();
    void SetupDefaults();
  };

  class BaseFigure : public QPWidget {
  public:
    HandleFigure *hfig;
    BaseFigure(QPWidget* parent, const char *Name);
    virtual void DrawMe(DrawEngine& gc);
  };

  HandleList<HandleObject*> handleset;


  HandleObject::HandleObject() {
  }

  HandleProperty* HandleObject::LookupProperty(std::string name) {
    std::transform(name.begin(),name.end(),name.begin(),tolower);
    HandleProperty** hp = m_properties.findSymbol(name);
    if (!hp)
      throw Exception("invalid property " + name);
    return *hp;
  }

  HPHandles::HPHandles(int len) {
    m_len = len;
  }
  
  HPHandles::HPHandles() {
    m_len = 1;
  }
  
  Array HPHandles::Get() {
    Array ret(Array::uint32VectorConstructor(data.size()));
    unsigned *dp = (unsigned*) ret.getReadWriteDataPointer();
    for (int i=0;i<data.size();i++)
      dp[i] = data[i];
    return ret;
  }

  void HPHandles::Set(Array arg) {
    if (arg.isEmpty()) {
      data.clear();
      HandleProperty::Set(arg);
      return;
    }
    if (!(arg.isScalar() && arg.isReal()))
      throw Exception("expecting handle for property");
    arg.promoteType(FM_UINT32);
    const unsigned *dp = (const unsigned*) arg.getDataPointer();
    if (m_len > 0 && arg.getLength() != m_len)
      throw Exception("incorrect number of handles in property assignment");
    // make sure they are all valid handles
    for (int i=0;i<arg.getLength();i++) 
      handleset.lookupHandle(dp[i]);
    data.clear();
    for (int i=0;i<arg.getLength();i++) 
      data.push_back(dp[i]);
    HandleProperty::Set(arg); 
  }

  Array HPString::Get() {
    return Array::stringConstructor(data);
  }

  void HPString::Set(Array arg) {
    HandleProperty::Set(arg);
    if (!arg.isString())
      throw Exception("Expecting a string for property ");
    data = ArrayToString(arg);
  }
  
  Array HPVector::Get() {
    Array ret(Array::doubleVectorConstructor(data.size()));    
    double *dp = (double*) ret.getReadWriteDataPointer();
    for (int i=0;i<data.size();i++)
      dp[i] = data[i];
    return ret;
  }
  
  void HPVector::Set(Array num) {
    HandleProperty::Set(num);
    num.promoteType(FM_DOUBLE);
    const double *dp = (const double*) num.getDataPointer();
    data.clear();
    for (int i=0;i<num.getLength();i++)
      data.push_back(dp[i]);
  }
  
  void HPFixedVector::Set(Array num) {
    HandleProperty::Set(num);
    if (num.getLength() != m_len)
      throw Exception("expecting a vector argument of a specific length for property");
    HPVector::Set(num);
  }

  double& HPVector::At(int ndx) {
    return data[ndx];
  }

  double& HPVector::operator[](int ndx) {
    return data[ndx];
  }

  void HPColorVector::Set(Array arg) {
    // TODO...
  }

  Array HPColorVector::Get() {
    // TODO...
  }
  
  void HPColor::Set(Array arg) {
    HandleProperty::Set(arg);
    if (arg.isString() && (strcmp(arg.getContentsAsCString(),"none") == 0)) {
      data.clear();
      data.push_back(-1);
      data.push_back(-1);
      data.push_back(-1);
    } else {
      if (arg.getLength() != 3)
	throw Exception("color spec must be a length 3 array (or the string 'none')");
      arg.promoteType(FM_DOUBLE);
      const double *dp = (const double*) arg.getDataPointer();
      if (((dp[0] < 0) || (dp[0] > 1)) ||
	  ((dp[1] < 0) || (dp[1] > 1)) ||
	  ((dp[2] < 0) || (dp[2] > 1)))
	throw Exception("color spec must be a length 3 array of values between 0 and 1");
      data.clear();
      data.push_back(dp[0]);
      data.push_back(dp[1]);
      data.push_back(dp[2]);
    }
  }

  Array HPColor::Get() {
    if (data[0] == -1)
      return Array::stringConstructor("none");
    else
      return HPVector::Get();
  }
  
  Array HPStringSet::Get() {
    std::string retval;
    for (unsigned i=0;i<data.size()-1;i++) {
      retval.append(data[i]);
      retval.append("|");
    }
    retval.append(data.back());
    return Array::stringConstructor(retval);
  }

  void HPStringSet::Set(Array arg) {
    HandleProperty::Set(arg);
    if (!arg.isString()) 
      throw Exception("expecting a '|'-delimited list of strings for property argument");
    std::string args(ArrayToString(arg));
    data.clear();
    Tokenize(args,data,"|");
  }

  void HPConstrainedString::Set(Array arg) {
    HandleProperty::Set(arg);
    if (!arg.isString())
      throw Exception("expecting a string for property");
    std::string tst(ArrayToString(arg));
    if (find(m_dictionary.begin(),m_dictionary.end(),tst) == m_dictionary.end())
      throw Exception("illegal selection for property");
    HPString::Set(arg);
  }

  HandleFigure::HandleFigure() {
    ConstructProperties();
    SetupDefaults();
  }
  
  void HandleFigure::ConstructProperties() {
    AddProperty(new HPHandles,"children");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPFourVector,"position");
  }

  void HandleFigure::SetupDefaults() {
  }

  void HandleFigure::DrawMe(DrawEngine& gc) {
    // draw the children...
    HPHandles *children = LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = handleset.lookupHandle(handles[i]);
      fp->DrawMe(gc);
    }
  }

  void HandleFigure::UpdateState() {
  }

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

  void HPScalar::Value(double x) {
    At(0) = x;
  }

    void HandleObject::AddProperty(HandleProperty* hp, std::string name) {
    m_properties.insertSymbol(name,hp);
  }

  void HandleObject::SetConstrainedStringDefault(std::string name, std::string value) {
    HPConstrainedString *hp = (HPConstrainedString*) LookupProperty(name);
    hp->Value(value);
  }

  void HandleObject::SetThreeVectorDefault(std::string name, double x, double y, double z) {
    HPThreeVector *hp = (HPThreeVector*) LookupProperty(name);
    hp->Value(x,y,z);
  }

  void HandleObject::SetTwoVectorDefault(std::string name, double x, double y) {
    HPTwoVector *hp = (HPTwoVector*) LookupProperty(name);
    hp->Value(x,y);
  }

  void HandleObject::SetStringDefault(std::string name, std::string value) {
    HPString *hp = (HPString*) LookupProperty(name);
    hp->Value(value);
  }

  void HandleObject::SetScalarDefault(std::string name, double value) {
    HPScalar *hp = (HPScalar*) LookupProperty(name);
    hp->Value(value);
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
    SetConstrainedStringDefault("projection","orthographic");
    SetConstrainedStringDefault("selected","off");
    SetConstrainedStringDefault("selectionhighlight","on");
    SetConstrainedStringDefault("tickdir","in");
    SetConstrainedStringDefault("tickdirmode","auto");
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
    fp->UpdateState();
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
    fp->UpdateState();
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

  BaseFigure::BaseFigure(QPWidget* parent, const char *name) :
    QPWidget(parent,name) {
      hfig = new HandleFigure;
      handleset.assignHandle(hfig);
  }

  void BaseFigure::DrawMe(DrawEngine& gc) {
    hfig->DrawMe(gc);
  }

}
