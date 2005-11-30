#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "HandleList.hpp"
namespace FreeMat {

  extern HandleList<HandleObject*> handleset;

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

  HandleAxis* HandleObject::GetParentAxis() {
    // Get our parent - must be an axis
    HPHandle *parent = (HPHandle*) LookupProperty("parent");
    if (parent->Data().empty()) return NULL;
    unsigned parent_handle = parent->Data()[0];
    HandleObject *fp = handleset.lookupHandle(parent_handle);
    HPString *name = (HPString*) fp->LookupProperty("type");
    if (!name) return NULL;
    if (!name->Is("axes")) return NULL;
    HandleAxis *axis = (HandleAxis*) fp;
    return axis;
  }

  std::string HandleObject::StringPropertyLookup(std::string name) {
    HPString* sp = (HPString*) LookupProperty(name);
    return (sp->Data());
  }

  std::vector<double> HandleObject::VectorPropertyLookup(std::string name) {
    HPVector* sp = (HPVector*) LookupProperty(name);
    return (sp->Data());
  }
  
  double HandleObject::ScalarPropertyLookup(std::string name) {
    HPScalar* sp = (HPScalar*) LookupProperty(name);
    return (sp->Data()[0]);
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
}
