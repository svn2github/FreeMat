#include "HandleAxis.hpp"
#include "Core.hpp"

namespace FreeMat {
  void HandleObject::RegisterProperty(string name, HandleProperty& prop) {
    m_properties[name] = prop;
  }

  HandleProperty& HandleObject::LookupProperty(string name) {
    if (m_properties.count(name) == 0)
      throw Exception("invalid property " + name);
    return m_properties[name];
  }

  Array HPString::Get() {
    return Array::stringConstructor(data);
  }

  void HPString::Set(Array arg) {
    if (!arg.isString())
      throw Exception("Expecting a string for property ");
    data = ArrayToString(arg);
  }
  
  Array HPVector::Get() {
    Array ret(Array::doubleVectorConstructor(data.size()));
    double *dp = (double*) ret.getReadWriteDataPointer();
    for (int i=0;i<data.size();i++)
      dp[i] = data[i];
    return ret;
  }
  
  void HPVector::Set(Array num) {
    num.promoteType(FM_DOUBLE);
    const double *dp = (const double*) num.getDataPointer();
    data.clear();
    for (int i=0;i<len;i++)
      data.push_back(dp[i]);
  }
  
  void HPFixedVector::Set(Array num) {
    if (num.getLength() != m_len)
      throw Exception("expecting a vector argument of a specific length for property");
    HPVector::Set(arg);
  }

  void HPColor::Set(Array arg) {
    if (ar.isString() && (strcmp(arg.getContentsAsCString(),"none") == 0)) {
      data.clear();
      data.push_back(-1);
    } else {
      if (arg.getLength() != 3)
	throw Exception("color spec must be a length 3 array (or the string 'none')");
      arg.promoteType(FM_DOUBLE);
      const double *dp = (const double*) arg.getDataPointer();
      if (((dp[0] < 0) || (dp[0] > 1)) ||
	  ((dp[1] < 0) || (dp[1] > 1)) ||
	  ((dp[2] < 0) || (dp[2] > 1)))
	throw Exception("color spec must be a length 3 array of values between 0 and 1");
      data.clear();
      data.push_back(dp[0]);
      data.push_back(dp[1]);
      data.push_back(dp[2]);
    }
  }

  Array HPColor::Get() {
    if (data[0] == -1)
      return Array::stringConstructor("none");
    else
      return HPVector::Get();
  }
  
  Array HPStringSet::Get() {
    string retval;
    for (unsigned i=0;i<data.size()-1;i++) {
      retval.append(data[i]);
      retval.append("|");
    }
    retval.append(data.back());
    return Array::stringConstructor(retval);
  }

  void HPStringSet::Set(Array arg) {
    if (!arg.isString()) 
      throw Exception("expecting a '|'-delimited list of strings for property argument");
    string args(ArrayToString(arg));
    data.clear();
    Tokenize(args,data,"|");
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

  static vector<string> mode_dictionary;
  static vector<string> direction_dictionary;
  static vector<string> bool_dictionary;

  void InitializeDictionaries() {
    mode_dictionary.push_back("auto");
    mode_dictionary.push_back("manual");
    direction_dictionary.push_back("normal");
    
  }


  HandleAxis::HandleAxis() :
    position(4), xlim(2), ylim(2), zlim(2), clim(2)
  }

  HandleAxis::~HandleAxis() {
  }

  void HandleAxis::RegisterProperties() {
    RegisterProperty("xlabel",&xlabel);
    RegisterProperty("ylabel",&ylabel);
    RegisterProperty("zlabel",&zlabel);
    RegisterProperty("xlim",&xlim);
    RegisterProperty("ylim",&ylim);
    RegisterProperty("zlim",&zlim);
    RegisterProperty("clim",&clim);
    RegisterProperty("position",&position);
    RegisterProperty("direction",&direction);
  }

  HandleList<HandleAxis*> handleset;

  // Construct an axis object 
  ArrayVector hAxisFunction(int nargout, const ArrayVector& arg) {
    HandleAxis *fp = new HandleAxis;
    unsigned int handle = handleset.assignHandle(fp);
    return singleArrayVector(Array::uint32Constructor(handle));
  }

  ArrayVector SetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("set doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleObject *fp = handleset.lookupHandle(handle);
    // Use the address and property name to lookup the Get/Set handler
    ArrayVector argreduce(arg);
    argreduce.erase(0,2);
    fp->LookupProperty(propname).Set(argreduce);
    return ArrayVector();
  }

  ArrayVector GetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("get doesn't handle all cases yet!");
    int handle = ArrayToInt32(arg[0]);
    string propname = ArrayToString(arg[1]);
    // Lookup the handle
    HandleObject *fp = handleset.lookupHandle(handle);
    // Use the address and property name to lookup the Get/Set handler
    return fp->LookupProperty(propname).Get();
  }

  void LoadHandleGraphicsFunctions(Context* context) {
    context->addFunction("haxis",hAxisFunction,0,1);
    context->addFunction("set",SetFunction,-1,0);
    context->addFunction("get",GetFunction,2,1,"handle","propname");
  };
}
