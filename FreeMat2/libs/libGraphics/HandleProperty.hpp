#ifndef __HandleProperty_hpp__
#define __HandleProperty_hpp__

#include "Array.hpp"
#include <string>
#include <vector>

namespace FreeMat {
  class HandleProperty {
    bool modified;
  public:
    HandleProperty() {modified = false;}
    virtual ~HandleProperty() {}
    virtual Array Get() = 0;
    virtual void Set(Array) {modified = true;}
    void ClearModified() {modified = false;}
    bool isModified() {return modified;}
  };

  class HPHandles : public HandleProperty {
  protected:
    int m_len;
    std::vector<unsigned> data;
  public:
    HPHandles(int len);
    HPHandles();
    virtual ~HPHandles() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<unsigned> Data() {return data;}
    void Data(std::vector<unsigned> m) {data = m;}
  };

  class HPHandle : public HPHandles {
  public:
    HPHandle() : HPHandles(1) {}
    ~HPHandle() {}
  };

  class HPVector : public HandleProperty {
  protected:
    std::vector<double> data;
  public:
    HPVector() {}
    virtual ~HPVector() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<double> Data() {return data;}
    void Data(std::vector<double> m) {data = m;}
    double& operator[](int ndx);
    double& At(int ndx);
  };

  class HPFixedVector : public HPVector {
  protected:
    unsigned m_len;
  public:
    HPFixedVector(unsigned len) : m_len(len) {for (int i=0;i<len;i++) data.push_back(0);}
    virtual ~HPFixedVector() {}
    virtual void Set(Array);    
  };

  class HPString : public HandleProperty {
  protected:
    std::string data;
  public:
    HPString() {}
    virtual ~HPString() {}
    virtual Array Get();
    virtual void Set(Array);
    std::string Data() {return data;}
    void Data(std::string m) {data = m;}
    void Value(std::string m) {data = m;}
    bool Is(std::string m) {return (data == m);}
  };

  class HPStringSet : public HandleProperty {
  protected:
    std::vector<std::string> data;
  public:
    HPStringSet() {}
    virtual ~HPStringSet() {}
    virtual Array Get();
    virtual void Set(Array);
    std::vector<std::string> Data() {return data;}
    void Data(std::vector<std::string> m) {data = m;}
  };

  class HPConstrainedString : public HPString {
  protected:
    std::vector<std::string> m_dictionary;
  public:
    HPConstrainedString(std::vector<std::string> dict) : m_dictionary(dict) {
      data = dict[0];
    }
    HPConstrainedString(const char **dict) {
      while (*dict) {
	m_dictionary.push_back(*dict);
	dict++;
      }
    }
    virtual void Set(Array);    
  };

  class HPConstrainedStringSet : public HPStringSet {
    std::vector<std::string> m_dictionary;
  public:
    HPConstrainedStringSet(const char **dict) {
      while (*dict) {
	m_dictionary.push_back(*dict);
	dict++;
      }
    }
    virtual ~HPConstrainedStringSet() {}
    virtual void Set(Array);
  };
  
  class HPTwoVector : public HPFixedVector {
  public:
    HPTwoVector() : HPFixedVector(2) {}
    virtual ~HPTwoVector() {}
    void Value(double x, double y) {At(0) = x; At(1) = y;}
  };

  class HPThreeVector : public HPFixedVector {
  public:
    HPThreeVector() : HPFixedVector(3) {}
    virtual ~HPThreeVector() {}
    void Value(double x, double y, double z) {At(0) = x; At(1) = y; At(2) = z;}
  };
  
  class HPFourVector : public HPFixedVector {
  public:
    HPFourVector() : HPFixedVector(4) {}
    virtual ~HPFourVector() {}
    void Value(double x, double y, double z, double w) {At(0) = x; At(1) = y; At(2) = z; At(3) = w;}
  };

  class HPAutoManual : public HPConstrainedString {
  public:
    HPAutoManual();
    virtual ~HPAutoManual() {}
  };

  class HPColor : public HPFixedVector {
  public:
    HPColor() : HPFixedVector(3) {}
    virtual ~HPColor() {}
    virtual void Set(Array);    
    Array Get();    
    bool IsNone() {return (data[0]<0);}
  };

  class HPColorVector : public HPVector {
  public:
    HPColorVector() : HPVector() {}
    virtual ~HPColorVector() {}
    virtual void Set(Array);    
    Array Get();
  };

  class HPOnOff : public HPConstrainedString {
  public:
    HPOnOff();
    virtual ~HPOnOff() {}
    bool AsBool() {return Is("on");}
  };

  class HPScalar : public HPFixedVector {
  public:
    HPScalar() : HPFixedVector(1) {}
    virtual ~HPScalar() {}
    void Value(double x);
  };

    
  class HPFontAngle : public HPConstrainedString {
  public:
    HPFontAngle();
    virtual ~HPFontAngle() {}
  };
  
  
  class HPFontUnits : public HPConstrainedString {
  public:
    HPFontUnits();
    virtual ~HPFontUnits() {}
  };
  

  class HPFontWeight : public HPConstrainedString {
  public:
    HPFontWeight();
    virtual ~HPFontWeight() {}
  };


  class HPLineStyle : public HPConstrainedString {
  public:
    HPLineStyle();
    virtual ~HPLineStyle() {}
  };


  class HPTopBottom : public HPConstrainedString {
  public:
    HPTopBottom();
    virtual ~HPTopBottom() {}
  };


  class HPLeftRight : public HPConstrainedString {
  public:
    HPLeftRight();
    virtual ~HPLeftRight() {}
  };


  class HPNormalReverse : public HPConstrainedString {
  public:
    HPNormalReverse();
    virtual ~HPNormalReverse() {}
  };


  class HPLinearLog : public HPConstrainedString {
  public:
    HPLinearLog();
    virtual ~HPLinearLog() {}
  };
  

  class HPNextPlotMode : public HPConstrainedString {
  public:
    HPNextPlotMode();
    virtual ~HPNextPlotMode() {}
  };


  class HPProjectionMode : public HPConstrainedString {
  public:
    HPProjectionMode();
    virtual ~HPProjectionMode() {}
  };
  

  class HPInOut : public HPConstrainedString {
  public:
    HPInOut();
    virtual ~HPInOut() {}
  };

  
  class HPUnits : public HPConstrainedString {
  public:
    HPUnits();
    virtual ~HPUnits() {}
  };
  
  
  class HPPosition : public HPConstrainedString {
  public:
    HPPosition();
    virtual ~HPPosition() {}
  };

  class HPAlignHoriz : public HPConstrainedString {
  public:
    HPAlignHoriz();
    virtual ~HPAlignHoriz() {}
  };

  class HPAlignVert : public HPConstrainedString {
  public:
    HPAlignVert();
    virtual ~HPAlignVert() {}
  };

  class HPSymbol : public HPConstrainedString {
  public:
    HPSymbol();
    virtual ~HPSymbol() {}
  };

  class HPLineStyleOrder : public HPConstrainedStringSet {
  public:
    HPLineStyleOrder();
    virtual ~HPLineStyleOrder() {}
  };

}

#endif
