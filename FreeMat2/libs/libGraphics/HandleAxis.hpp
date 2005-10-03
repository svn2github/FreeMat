#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Array.hpp"
#include "QPWidget.hpp"

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
    std::map<std::string, HandleProperty*, std::less<std::string> > m_properties;
  protected:
    HPHandles children;
    HPHandles parent;
  public:
    HandleObject();
    virtual ~HandleObject() {}
    virtual void RegisterProperties();
    void RegisterProperty(std::string name, HandleProperty* prop);
    HandleProperty* LookupProperty(std::string name);
    virtual void DrawMe(DrawEngine& gc) = 0;
    virtual void UpdateState() = 0;
  };
  
  class HPTwoVector : HPFixedVector {
  public:
    HPTwoVector() : HPFixedVector(2) {}
    virtual ~HPTwoVector() {}
  };

  class HPThreeVector : HPFixedVector {
  public:
    HPThreeVector() : HPFixedVector(3) {}
    virtual ~HPThreeVector() {}
  };
  
  class HPFourVector : HPFixedVector {
  public:
    HPFourVector() : HPFixedVector(4) {}
    virtual ~HPFourVector() {}
  };

  const char *auto_manual_dict[3] = {"auto","manual",0};

  class HPAutoManual : HPConstrainedString {
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

  class HPOnOff : HPConstrainedString {
  public:
    HPOnOff() : HPConstrainedString(on_off_dict) {}
    virtual ~HPOnOff() {}
  };

  class HPScalar : public HPFixedVector {
  public:
    HPScalar() : HPFixedVector(1) {}
    virtual ~HPScalar() {}
  };

  const char *font_angle_dict[4] = {"normal","italic","oblique",0};
    
  class HPFontAngle : HPConstrainedString {
  public:
    HPFontAngle() : HPConstrainedString(font_angle_dict) {}
    virtual ~HPFontAngle() {}
  };
  
  const char *font_units_dict[6] = {"points","normalized","inches",
				    "centimeters","pixels",0};
  
  class HPFontUnits : HPConstrainedString {
  public:
    HPFontUnits() : HPConstrainedString(font_units_dict) {}
    virtual ~HPFontUnits() {}
  };
  
  const char *font_weight_dict[5] = {"normal","bold","light","demi",0};

  class HPFontWeight : HPConstrainedString {
  public:
    HPFontWeight() : HPConstrainedString(font_weight_dict) {}
    virtual ~HPFontWeight() {}
  };

  const char *line_style_dict[6] = {"-","--",":","-.","none",0};

  class HPLineStyle : HPConstrainedString {
  public:
    HPLineStyle() : HPConstrainedString(line_style_dict) {}
    virtual ~HPLineStyle() {}
  };

  const char *top_bottom_dict[3] = {"top","bottom",0};

  class HPTopBottom : HPConstrainedString {
  public:
    HPTopBottom() : HPConstrainedString(top_bottom_dict) {}
    virtual ~HPTopBottom() {}
  };

  const char *left_right_dict[3] = {"left","right",0};

  class HPLeftRight : HPConstrainedString {
  public:
    HPLeftRight() : HPConstrainedString(left_right_dict) {}
    virtual ~HPLeftRight() {}
  };

  const char *normal_reverse_dict[3] = {"normal","reverse",0};

  class HPNormalReverse : HPConstrainedString {
  public:
    HPNormalReverse() : HPConstrainedString(normal_reverse_dict) {}
    virtual ~HPNormalReverse() {}
  };

  const char *linear_log_dict[3] = {"linear","log",0};

  class HPLinearLog : HPConstrainedString {
  public:
    HPLinearLog() : HPConstrainedString(linear_log_dict) {}
    virtual ~HPLinearLog() {}
  };
  
  const char *next_plot_dict[4] = {"add","replace","replacechildren",0};

  class HPNextPlotMode : HPConstrainedString {
  public:
    HPNextPlotMode() : HPConstrainedString(next_plot_dict) {}
    virtual ~HPNextPlotMode() {}
  };

  const char *projection_mode_dict[3] = {"orthographic","perspective",0};

  class HPProjectionMode : HPConstrainedString {
  public:
    HPProjectionMode() : HPConstrainedString(projection_mode_dict) {}
    virtual ~HPProjectionMode() {}
  };
  
  const char *in_out_dict[3] = {"in","out",0};

  class HPInOut : HPConstrainedString {
  public:
    HPInOut() : HPConstrainedString(in_out_dict) {}
    virtual ~HPInOut() {}
  };

  const char *units_dict[7] = {"inches","centimeters","normalized",
			       "points","pixels","characters",0};
  
  class HPUnits : HPConstrainedString {
  public:
    HPUnits() : HPConstrainedString(units_dict) {}
    virtual ~HPUnits() {}
  };
  
  const char *position_dict[3] = {"outerposition","position",0};
  
  class HPPosition : HPConstrainedString {
  public:
    HPPosition() : HPConstrainedString(position_dict) {}
    virtual ~HPPosition() {}
  };
  
  //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis : public HandleObject {
  public:
    // These are all the properties of the axis
    HPPosition ActivePositionProperty;
    HPTwoVector ALim;
    HPAutoManual ALimMode;
    HPColor AmbientLightColor;
    HPOnOff Box;
    HPThreeVector CameraPosition;
    HPAutoManual CameraPositionMode;
    HPThreeVector CameraTarget;
    HPAutoManual CameraTargetMode;
    HPThreeVector CameraUpVector;
    HPScalar CameraViewAngle;
    HPAutoManual CameraViewAngleMode;
    HPHandles Children;
    HPTwoVector CLim;
    HPAutoManual CLimMode;
    HPOnOff Clipping;
    HPColor Color;
    HPColorVector ColorOrder;
    HPThreeVector DataAspectRatio;
    HPAutoManual DataAspectRatioMode;
    HPFontAngle FontAngle;
    HPString FontName;
    HPScalar FontSize;
    HPFontUnits FontUnits;
    HPFontWeight FontWeight;
    HPLineStyle GridLineStyle;
    HPOnOff HandleVisibility;
    HPOnOff HitTest;
    HPOnOff Interruptible;
    HPTopBottom Layer;
    //    HPLineStyleSet LineStyleOrder;
    HPScalar LineWidth;
    HPLineStyle MinorGridLineStyle;
    HPNextPlotMode NextPlot;
    HPFourVector OuterPosition;
    HPHandle Parent;
    HPThreeVector PlotBoxAspectRatio;
    HPAutoManual PlotBoxAspectRatioMode;
    HPFourVector Position;
    HPProjectionMode Projection;
    HPOnOff Selected;
    HPOnOff SelectionHighlight;
    HPString Tag;
    HPInOut TickDir;
    HPAutoManual TickDirMode;
    HPTwoVector TickLength;
    HPFourVector TightInset;
    HPHandle Title;
    HPString Type;
    HPUnits Units;
    Array UserData;
    HPOnOff Visible;
    HPTopBottom XAxisLocation;
    HPLeftRight YAxisLocation;
    HPColor XColor;
    HPColor YColor;
    HPColor ZColor;
    HPNormalReverse XDir;
    HPNormalReverse YDir;
    HPNormalReverse ZDir;
    HPOnOff XGrid;
    HPOnOff YGrid;
    HPOnOff ZGrid;
    HPHandle XLabel;
    HPHandle YLabel;
    HPHandle ZLabel;
    HPTwoVector XLim;
    HPTwoVector YLim;
    HPTwoVector ZLim;
    HPAutoManual XLimMode;
    HPAutoManual YLimMode;
    HPAutoManual ZLimMode;
    HPOnOff XMinorGrid;
    HPOnOff YMinorGrid;
    HPOnOff ZMinorGrid;
    HPLinearLog XScale;
    HPLinearLog YScale;
    HPLinearLog ZScale;
    HPVector XTick;
    HPVector YTick;
    HPVector ZTick;
    HPStringSet XTickLabel;
    HPStringSet YTickLabel;
    HPStringSet ZTickLabel;
    HPAutoManual XTickMode;
    HPAutoManual YTickMode;
    HPAutoManual ZTickMode;
    HPAutoManual XTickLabelMode;
    HPAutoManual YTickLabelMode;
    HPAutoManual ZTickLabelMode;
    double camera[4][4];

    HandleAxis();
    ~HandleAxis();
    virtual void RegisterProperties();  
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
    HPFixedVector position;
    
    HandleFigure();
    virtual ~HandleFigure() {}
    virtual void RegisterProperties();
    virtual void DrawMe(DrawEngine& gc);
    virtual void UpdateState();
  };

  class BaseFigure : public QPWidget {
  public:
    HandleFigure *hfig;
    BaseFigure(QPWidget* parent, const char *Name);
    virtual void DrawMe(DrawEngine& gc);
  };

  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
