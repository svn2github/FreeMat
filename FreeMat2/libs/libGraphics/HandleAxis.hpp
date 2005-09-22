#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>

namespace FreeMat {

  class HandleObject {
    map<string, HandleProperty&, less<string> > m_properties;
  public:
    HandleObject() {}
    virtual ~HandleObject() {}
    void RegisterProperty(string name, HandleProperty& prop);
    HandleProperty& LookupProperty(string name);
  };

  class HandleProperty {
  public:
    HandleProperty() {}
    virtual ~HandleProperty() {}
    virtual Array Get() = 0;
    virtual void Set(Array) = 0;
  };
  
  class HPVector : public HandleProperty {
    vector<double> data;
  public:
    HPVector() {}
    virtual ~HPVector() {}
    virtual Array Get();
    virtual void Set(Array);
    vector<double> Data() {return data;}
    void Data(vector<double> m) {data = m;}
  };

  class HPFixedVector : public HPVector {
    unsigned m_len;
  public:
    HPFixedVector(unsigned len) : m_len(len) {}
    virtual ~HPFixedVector() {}
    virtual void Set(Array);    
  };

  class HPString : public HandleProperty {
    string data;
  public:
    HPString() {}
    virtual ~HPString() {}
    virtual Array Get();
    virtual void Set(Array);
    string Data() {return data;}
    void Data(string m) {data = m;}
  };

  class HPStringSet : public HandleProperty {
    vector<string> data;
  public:
    HPStringSet() {}
    virtual ~HPStringSet() {}
    virtual Array Get();
    virtual void Set(Array);
    vector<string> Data() {return data}
    void Data(vector<string> m) {data = m;}
  };

  class HPColor : public HPVector {
  public:
    HPColor() {}
    virtual ~HPColor() {}
    virtual void Set(Array);    
    Array Get();    
  };

  class HPConstrainedString : public HPString {
    vector<string> dictionary;
  public:
    HPConstrainedString(vector<string>);
    virtual Array Get();
    virtual void Set(Array);    
  };


  //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis : HandleObject {
  public:
    // These are all the properties of the axix
    HPColor bkcolor, xcolor, ycolor, zcolor;
    HPString xlabel, ylabel, zlabel, title;
    HPString gridlinestyle;
    HPStringSet zticklabel, xticklabel, yticklabel;
    HPFixedVector position;
    HPBool visible;
    HPFixedVector xlim, ylim, zlim, clim;
    HPVector xtick, ytick, ztick;
    HPConstrainedString xlimmode, ylimmode, zlimmode;
    HPConstrainedString xtickmode, ytickmode, ztickmode;
    HPConstrainedString xticklabelmode, yticklabelmode, zticklabelmode;
    HPConstrainedString xdir, ydir, zdir;

    HandleAxis();
    ~HandleAxis();
    void RegisterProperties();  
  };

  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
