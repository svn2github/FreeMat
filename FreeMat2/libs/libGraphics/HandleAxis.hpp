/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Array.hpp"
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
    double model[16];
    double proj[16];
    int viewp[4];

    void UpdateAxisFont();
    void HandlePlotBoxFlags();
    void GetMaxTickMetric(RenderEngine& gc,
			  std::vector<std::string> labs,
			  double &maxx, double &maxy);
    void DrawLabel(RenderEngine& gc,
		   double dx, double dy, 
		   double x2, double y2, 
		   std::vector<double> color,
		   std::string txt);
    void SetupAxis(RenderEngine& gc);
    bool Is2DView(); 
    void RecalculateTicks();
    int GetTickCount(RenderEngine& gc, double x1, double y1, 
		     double z1, double x2, double y2, double z2);
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
    void RePackFigure();
    void UpdateLimits(bool x, bool y, bool z, bool a, bool c);
  public:
    HandleAxis();
    virtual ~HandleAxis();
    virtual void ConstructProperties();
    virtual void UpdateState();
    void SetupDefaults();
    virtual void PaintMe(RenderEngine &gc);
    HandleFigure* GetParentFigure();
    std::vector<double> UnitsReinterpret(std::vector<double> a);
    std::vector<double> GetPropertyVectorAsPixels(std::string name);
    std::vector<double> GetAxisLimits();
    void SetAxisLimits(std::vector<double>);
    double MapX(double x);
    double MapY(double y);
    double MapZ(double z);
    std::vector<double> ReMap(std::vector<double>);
    void ReMap(std::vector<double>,
	       std::vector<double>,
	       std::vector<double>,
	       std::vector<double>&,
	       std::vector<double>&,
	       std::vector<double>&);
    void SetupProjection(RenderEngine& gc);
    void DrawBox(RenderEngine& gc);
    void DrawMinorGridLines(RenderEngine& gc);
    void DrawGridLines(RenderEngine& gc);
    void DrawAxisLines(RenderEngine& gc);
    void DrawTickMarks(RenderEngine& gc);
    void DrawTickLabels(RenderEngine& gc,
			std::vector<double> color,
			double px1, double py1, double pz1,
			double px2, double py2, double pz2,
			double limmin, double limmax,
			double unitx, double unity, double unitz,
			std::vector<double>  maptics,
			std::vector<std::string> labels,
			std::string labelname,
			int ticlen, double ticdir);
    void DrawAxisLabels(RenderEngine& gc);
    void DrawChildren(RenderEngine& gc);
  };


  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
