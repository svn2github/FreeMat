#ifndef __HandleObject_hpp__
#define __HandleObject_hpp__

#include <string>
#include "SymbolTable.hpp"
#include "HandleProperty.hpp"

namespace FreeMat {

  class HandleObject {
    SymbolTable<HandleProperty*> m_properties;
  public:
    HandleObject();
    virtual ~HandleObject() {}
    virtual void RegisterProperties() {}
    bool HasChanged(std::vector<std::string> names);
    void ClearChanged(std::vector<std::string> names);
    void AddProperty(HandleProperty* prop, std::string name);
    HandleProperty* LookupProperty(std::string name);
    void SetConstrainedStringDefault(std::string name, std::string value);
    void SetTwoVectorDefault(std::string name, double x, double y);
    void SetThreeVectorDefault(std::string name, double x, double y, double z);
    void SetFourVectorDefault(std::string name, double x, double y, 
			      double z, double w);
    void SetStringDefault(std::string name, std::string value);
    void SetScalarDefault(std::string name, double value);
    bool IsAuto(std::string mode);
    virtual void paintGL() = 0;
  };
}

#endif
