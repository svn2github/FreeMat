#ifndef __HandleObject_hpp__
#define __HandleObject_hpp__

#include <string>
#include "SymbolTable.hpp"
#include "HandleProperty.hpp"
#include "RenderEngine.hpp"


namespace FreeMat {

  class HandleAxis;
  class HandleFigure;

  double VecMin(std::vector<double> &v);
  double VecMax(std::vector<double> &v);
  double ArrayMin(Array x);
  double ArrayMax(Array x);

  class HandleObject {
    SymbolTable<HandleProperty*> m_properties;
  public:
    HandleObject();
    virtual ~HandleObject() {}
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

}

#endif
