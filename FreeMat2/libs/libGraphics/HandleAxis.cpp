#include "HandleAxis.hpp"

namespace FreeMat {
  // set text...
  void SetTextProperty(string propname, Array text, string *dest) {
    if (!text.isString())
      throw Exception("Expecting a string for property " + propname);
    *dest = string(ArrayToString(text));
  }

  // get text...
  string GetTextProperty(string *src) {
    return(*src);
  }

  // set nvector...
  void SetNVectorProperty(string propname, int len, Array num, vector<double> *dest) {
    num.promoteType(FM_DOUBLE);
    if (num.getLength() != len)
      throw Exception("Expecting a vector argument for property " + propname);
    const double *dp = (const double*) num.getDataPointer();
    vector<double> ret;
    for (int i=0;i<len;i++)
      ret.push_back(dp[i]);
    *dest = ret;
  }

  // get nvector...
  Array GetNVectorProperty(vector<double> *src) {
    Array ret(Array::doubleVectorConstructor(src.size()));
    double *dp = (double*) ret.getReadWriteDataPointer();
    for (int i=0;i<src.size();i++)
      dp[i] = src[i];
    return ret;
  }

  // set selection...
  void SetSelectionProperty(string propname, Array arg, vector<string> choices, string *dest) {
    if (!text.isString())
      throw Exception("Expecting a string for property " + propname);
    string spec(ArrayToString(arg));
    // Check it against the choices
    bool foundmatch = false;
    int i=0;
    while (!foundmatch && (i < choices.size())) {
      foundmatch = (spec == choices[i]);
      if (!foundmatch) i++;
    }
    if (!foundmatch)
      throw Exception("Illegal value for property " + propname);
    *dest = choices[i];
  }

  HandleAxis::HandleAxis() {
  }

  HandleAxis::~HandleAxis() {
  }

  void HandleAxis::RegisterProperties() {
    vector<string> dirchoices;
    dirchoices.push_back("normal");
    dirchoices.push_back("reverse");
    RegisterGetSetText(this,"xlabel",&xlabel);
    RegisterGetSetText(this,"ylabel",&ylabel);
    RegisterGetSetText(this,"zlabel",&zlabel);
    RegisterGetSetNVector(this,"xlim",&xlim,2);
    RegisterGetSetNVector(this,"ylim",&ylim,2);
    RegisterGetSetNVector(this,"zlim",&zlim,2);
    RegisterGetSetNVector(this,"clim",&clim,2);
    RegisterGetSetNVector(this,"position",&position,4);
    RegisterGetSetSelection(this,"direction",&direction,dirchoices);
  }

  HandleList<HandleAxis*> handleset;

  // Construct an axis object 
  ArrayVector hAxisFunction(int nargout, const ArrayVector& arg) {
    HandleAxis *fp = new HandleAxis;
    unsigned int handle = handleset.assignHandle(fp);
    return singleArrayVector(Array::uint32Constructor(handle));
  }

  ArrayVector SetFunction(int nargout, const ArrayVector& arg) {
    // 
  }

  ArrayVector GetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("get doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleAxis *fp = handleset.lookupHandle(handle);
    // Use the address and property name to lookup the Get/Set handler
    
    // Normally, we'd have to determine which dispatch table to use now
    // Call the appropriate Get method
    return GetDispatchFunction(
  }

  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("haxis",hAxisFunction,0,1);
    context->addFunction("set",SetFunction,-1,0);
    context->addFunction("get",GetFunction,2,1,"handle","propname");
  };
}
