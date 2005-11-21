#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Array.hpp"
#include "QPWidget.hpp"
#include "SymbolTable.hpp"
#include "HandleObject.hpp"
#include "HandleFigure.hpp"
#include "RenderEngine.hpp"

namespace FreeMat {

    //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis : public HandleObject {
    double x1pos[3], x2pos[3];
    double y1pos[3], y2pos[3];
    double z1pos[3], z2pos[3];
    bool xvisible, yvisible, zvisible;
    QFont m_font;

    void UpdateAxisFont();
    void DrawLabel(RenderEngine& gc,
		   double dx, double dy, 
		   double x2, double y2, 
		   std::vector<double> color,
		   std::string txt);
    void SetupAxis(RenderEngine& gc);
    bool Is2DView();
    void RecalculateTicks(RenderEngine& gc);
    int GetTickCount(RenderEngine& gc,double x1, double y1, double z1, 
		     double x2, double y2, double z2);
    void DrawXGridLine(RenderEngine& gc, double t, 
		       std::vector<double> limits);
    void DrawYGridLine(RenderEngine& gc, double t, 
		       std::vector<double> limits);
    void DrawZGridLine(RenderEngine& gc, double t, 
		       std::vector<double> limits);
    double flipX(double t);
    double flipY(double t);
    double flipZ(double t);
    SymbolTable<HandleProperty*> properties;
  public:
    HandleAxis();
    virtual ~HandleAxis();
    virtual void ConstructProperties();
    virtual void UpdateState(RenderEngine& gc);
    void SetupDefaults();
    virtual void PaintMe(RenderEngine &gc);
    HandleFigure* GetParentFigure();
    std::vector<double> UnitsReinterpret(std::vector<double> a);
    std::vector<double> GetPropertyVectorAsPixels(std::string name);
    std::vector<double> GetAxisLimits();
    double MapX(double x);
    double MapY(double y);
    double MapZ(double z);
    std::vector<double> ReMap(std::vector<double>);
    void SetupProjection(RenderEngine& gc);
    void DrawBox(RenderEngine& gc);
    void DrawMinorGridLines(RenderEngine& gc);
    void DrawGridLines(RenderEngine& gc);
    void DrawAxisLines(RenderEngine& gc);
    void DrawTickMarks(RenderEngine& gc);
    void DrawTickLabels();
    void DrawAxisLabels(RenderEngine& gc);
    void DrawChildren(RenderEngine& gc);
  };


  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
