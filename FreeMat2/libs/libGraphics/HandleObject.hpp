#ifndef __HandleObject_hpp__
#define __HandleObject_hpp__

#include <string>
#include "SymbolTable.hpp"
#include "HandleProperty.hpp"
#include "RenderEngine.hpp"

#define HANDLE_OFFSET_OBJECT 100000
#define HANDLE_OFFSET_FIGURE 0

namespace FreeMat {

  class HandleAxis;
  class HandleFigure;

  class HandleObject {
    SymbolTable<HandleProperty*> m_properties;
  public:
    HandleObject();
    virtual ~HandleObject() {}
    virtual void RegisterProperties() {}
    virtual void UpdateState() {}
    bool HasChanged(std::vector<std::string> names);
    bool HasChanged(std::string name);
    void ToManual(std::string name);
    void ClearChanged(std::vector<std::string> names);
    void AddProperty(HandleProperty* prop, std::string name);
    HandleProperty* LookupProperty(std::string name);
    double ScalarPropertyLookup(std::string name);
    std::vector<double> VectorPropertyLookup(std::string name);
    std::string StringPropertyLookup(std::string name);
    void SetConstrainedStringDefault(std::string name, std::string value);
    void SetConstrainedStringSetDefault(std::string name, std::string value);
    void SetTwoVectorDefault(std::string name, double x, double y);
    void SetThreeVectorDefault(std::string name, double x, double y, double z);
    void SetFourVectorDefault(std::string name, double x, double y, 
			      double z, double w);
    void SetStringDefault(std::string name, std::string value);
    void SetScalarDefault(std::string name, double value);
    bool IsAuto(std::string mode);
    virtual void PaintMe(RenderEngine &gc) = 0;
    HandleAxis* GetParentAxis();
  };

  HandleObject* LookupHandleObject(unsigned handle);
  HandleFigure* LookupHandleFigure(unsigned handle);
  unsigned AssignHandleObject(HandleObject*);
  unsigned AssignHandleFigure(HandleFigure*);
  void FreeHandleObject(unsigned handle);
  void FreeHandleFigure(unsigned handle);
  void ValidateHandle(unsigned handle);
}

#endif
