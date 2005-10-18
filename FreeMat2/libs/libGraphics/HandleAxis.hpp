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

namespace FreeMat {

    //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis : public HandleObject {
  public:
    SymbolTable<HandleProperty*> properties;
    double camera[4][4];

    HandleAxis();
    virtual ~HandleAxis();
    virtual void ConstructProperties();
    //    virtual void UpdateState();
    void SetupDefaults();
    void Transform(double x, double y, double z, double &i, double &j);
    virtual void paintGL();
    HandleFigure* GetParentFigure();
    std::vector<double> UnitsReinterpret(std::vector<double> a);
    std::vector<double> GetPropertyVectorAsPixels(std::string name);
    std::vector<double> GetAxisLimits();
    bool IsVisibleLine(float nx1, float nx2, float nx3, 
		       float ny1, float ny2, float ny3);
    void SetLineStyle(std::string style);
    void SetupDirectDraw();
    void ReleaseDirectDraw();
    void SetupProjection();
    void ClearAxes();
    void DrawBox();
    void DrawGridLines();
    void DrawAxisLines();
    void DrawTickMarks();
    void DrawTickLabels();
    void DrawAxisLabels();
    void DrawChildren();
  };


  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
