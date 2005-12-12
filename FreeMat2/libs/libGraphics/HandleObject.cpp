#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "HandleCommands.hpp"
#include "Core.hpp"
namespace FreeMat {

  bool HandleObject::IsType(std::string name) {
    HPString* sp = (HPString*) LookupProperty("type");
    return (sp->Is(name));
  }

  void HandleObject::ToManual(std::string name) {
    HPAutoManual *qp = (HPAutoManual*) LookupProperty(name);
    qp->Data("manual");
  }

  bool HandleObject::HasChanged(std::vector<std::string> names) {
    HandleProperty *hp;
    for (int i=0;i<names.size();i++) {
      hp = LookupProperty(names[i]);
      if (hp->isModified()) return true;
    }
    return false;
  }

  bool HandleObject::HasChanged(std::string name) {
    std::vector<std::string> names;
    names.push_back(name);
    return HasChanged(names);
  }

  void HandleObject::ClearChanged(std::vector<std::string> names) {
    HandleProperty *hp;
    for (int i=0;i<names.size();i++) {
      hp = LookupProperty(names[i]);
      hp->ClearModified();
    }    
  }

  HandleObject::HandleObject() {
  }

  HandleProperty* HandleObject::LookupProperty(std::string name) {
    std::transform(name.begin(),name.end(),name.begin(),tolower);
    // First look for the property to match (avoids the problem
    // with prefix names)
    HandleProperty** hp = m_properties.findSymbol(name);
    if (hp) return (*hp);
    //complete it
    stringVector completes(m_properties.getCompletions(name));
    if (completes.empty())
      throw Exception("invalid property " + name);
    if (completes.size() != 1)
      throw Exception("ambiguous property name " + name);
    hp = m_properties.findSymbol(completes[0]);
    return *hp;
  }

  void HandleObject::SetPropertyHandle(std::string name, unsigned value) {
    HPHandles* hp = (HPHandles*) LookupProperty(name);
    std::vector<unsigned> newval;
    newval.push_back(value);
    hp->Data(newval);
  }

  HandleAxis* HandleObject::GetParentAxis() {
    // Get our parent - must be an axis
    HPHandles *parent = (HPHandles*) LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    HandleObject *fp = LookupHandleObject(parent_handle);
    HPString *name = (HPString*) fp->LookupProperty("type");
    if (!name) return NULL;
    if (!name->Is("axes")) return NULL;
    HandleAxis *axis = (HandleAxis*) fp;
    return axis;
  }

  HandleFigure* HandleObject::GetParentFigure() {
    HandleAxis* hp;
    if (StringCheck("type","axes")) 
      hp = this;
    else
      hp = GetParentAxis();
    // Get our parent - must be an axis
    HPHandles *parent = (HPHandles*) hp->LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    return LookupHandleFigure(parent_handle);
  }

  std::string HandleObject::StringPropertyLookup(std::string name) {
    HPString* sp = (HPString*) LookupProperty(name);
    return (sp->Data());
  }

  std::vector<double> HandleObject::VectorPropertyLookup(std::string name) {
    HPVector* sp = (HPVector*) LookupProperty(name);
    return (sp->Data());
  }

  Array HandleObject::ArrayPropertyLookup(std::string name) {
    HPArray* hp = (HPArray*) LookupProperty(name);
    return (hp->Data());
  }
  
  double HandleObject::ScalarPropertyLookup(std::string name) {
    HPScalar* sp = (HPScalar*) LookupProperty(name);
    return (sp->Data()[0]);
  }

  unsigned HandleObject::HandlePropertyLookup(std::string name) {
    HPHandles* sp = (HPHandles*) LookupProperty(name);
    if (sp->Data().empty())
      return 0;
    else
      return (sp->Data()[0]);
  }

  void HandleObject::AddProperty(HandleProperty* hp, std::string name) {
    m_properties.insertSymbol(name,hp);
  }

  void HandleObject::SetConstrainedStringScalarDefault(std::string name,
						       std::string value,
						       double sval) {
    HPConstrainedStringScalar *hp = 
      (HPConstrainedStringScalar*) LookupProperty(name);
    if (!hp)
      throw Exception("constrained string/scalar default failed lookup of <" + 
		      name + ">");
    hp->Value(value);
    hp->Scalar(sval);
  }

  void HandleObject::SetConstrainedStringColorDefault(std::string name,
						      std::string value,
						      double red, 
						      double green,
						      double blue) {
    HPConstrainedStringColor *hp = 
      (HPConstrainedStringColor*) LookupProperty(name);
    if (!hp)
      throw Exception("constrained string/color default failed lookup of <" +
		      name + ">");
    hp->Value(value);
    hp->ColorSpec(red,green,blue);
  }
						     

  void HandleObject::SetConstrainedStringDefault(std::string name, std::string value) {
    HPConstrainedString *hp = (HPConstrainedString*) LookupProperty(name);
    if (!hp)
      throw Exception("set constrained string default failed lookup of <" + name + ">");
    hp->Value(value);
  }

  void HandleObject::SetConstrainedStringSetDefault(std::string name, std::string values) {
    HPConstrainedStringSet *hp = (HPConstrainedStringSet*) LookupProperty(name);
    std::vector<std::string> data;
    Tokenize(values,data,"|");
    ((HPStringSet*)hp)->Data(data);
  }

  void HandleObject::SetThreeVectorDefault(std::string name, double x, double y, double z) {
    HPThreeVector *hp = (HPThreeVector*) LookupProperty(name);
    hp->Value(x,y,z);
  }

  void HandleObject::SetFourVectorDefault(std::string name, double x, double y,
					  double z, double w) {
    HPFourVector *hp = (HPFourVector*) LookupProperty(name);
    hp->Value(x,y,z,w);
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

  bool HandleObject::IsAuto(std::string mode) {
    HPAutoManual *hp = (HPAutoManual*) LookupProperty(mode);
    return hp->Is("auto");
  }
  
  bool HandleObject::StringCheck(std::string name, std::string value) {
    HPString *hp = (HPString*) LookupProperty(name);
    return hp->Is(value);
  }

}
