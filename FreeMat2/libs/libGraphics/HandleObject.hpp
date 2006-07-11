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
#ifndef __HandleObject_hpp__
#define __HandleObject_hpp__

#include <string>
#include "SymbolTable.hpp"
#include "HandleProperty.hpp"
#include "RenderEngine.hpp"


class HandleAxis;
class HandleFigure;

double VecMin(std::vector<double> &v);
double VecMax(std::vector<double> &v);
double ArrayMin(Array x);
double ArrayMax(Array x);

class HandleObject {
  SymbolTable<HandleProperty*> m_properties;
  unsigned ref_count;
public:
  HandleObject();
  virtual ~HandleObject();
  void Reference() {ref_count++;}
  void Dereference() {ref_count--;}
  unsigned RefCount() {return ref_count;}
  virtual void RegisterProperties() {}
  virtual void UpdateState() {}
  virtual std::vector<double> GetLimits() {return std::vector<double>();};
  bool HasChanged(std::vector<std::string> names);
  bool HasChanged(std::string name);
  void ToManual(std::string name);
  bool IsType(std::string name);
  void ClearChanged(std::vector<std::string> names);
  void ClearAllChanged();
  void AddProperty(HandleProperty* prop, std::string name);
  HandleProperty* LookupProperty(std::string name);
  double ScalarPropertyLookup(std::string name);
  unsigned HandlePropertyLookup(std::string name);
  std::vector<double> VectorPropertyLookup(std::string name);
  Array ArrayPropertyLookup(std::string name);
  std::string StringPropertyLookup(std::string name);
  bool StringCheck(std::string name, std::string value);
  void SetConstrainedStringDefault(std::string name, std::string value);
  void SetConstrainedStringSetDefault(std::string name, std::string value);
  void SetConstrainedStringScalarDefault(std::string name, std::string value, double scalar);
  void SetConstrainedStringColorDefault(std::string name, std::string value, double r, double g, double b);
  void SetTwoVectorDefault(std::string name, double x, double y);
  void SetThreeVectorDefault(std::string name, double x, double y, double z);
  void SetFourVectorDefault(std::string name, double x, double y, 
			    double z, double w);
  void SetStringDefault(std::string name, std::string value);
  void SetScalarDefault(std::string name, double value);
  void SetPropertyHandle(std::string name, unsigned value);
  bool IsAuto(std::string mode);
  virtual void PaintMe(RenderEngine &gc) = 0;
  HandleAxis* GetParentAxis();
  HandleFigure* GetParentFigure();
};

#endif
