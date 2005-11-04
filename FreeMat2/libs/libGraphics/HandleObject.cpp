#include "HandleObject.hpp"

namespace FreeMat {

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
    HandleProperty** hp = m_properties.findSymbol(name);
    if (!hp)
      throw Exception("invalid property " + name);
    return *hp;
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
