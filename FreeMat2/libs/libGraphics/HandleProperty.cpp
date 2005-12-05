#include "HandleProperty.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleCommands.hpp"
#include "Core.hpp"

namespace FreeMat {
  static const char *auto_manual_dict[3] = {"auto","manual",0};
  static const char *on_off_dict[3] = {"on","off",0};
  static const char *font_angle_dict[4] = {"normal","italic","oblique",0};
  static const char *font_units_dict[6] = {"points","normalized","inches",
					   "centimeters","pixels",0};
  static const char *font_weight_dict[5] = {"normal","bold","light","demi",0};
  static const char *line_style_dict[6] = {"-","--",":","-.","none",0};
  static const char *top_bottom_dict[3] = {"top","bottom",0};
  static const char *left_right_dict[3] = {"left","right",0};
  static const char *normal_reverse_dict[3] = {"normal","reverse",0};
  static const char *linear_log_dict[3] = {"linear","log",0};
  static const char *next_plot_dict[4] = {"add","replace","replacechildren",0};
  static const char *projection_mode_dict[3] = {"orthographic","perspective",0};
  static const char *in_out_dict[3] = {"in","out",0};
  static const char *units_dict[8] = {"inches","centimeters","normalized",
				      "points","pixels","characters","data",0};
  static const char *position_dict[3] = {"outerposition","position",0};
  static const char *horiz_dict[4] = {"left","center","right",0};
  static const char *vert_dict[6] = {"top","cap","middle","baseline","bottom",0};
  static const char *symb_dict[19] = {"+","o","*",".","x","square","s","diamond","d","^","v",">","<","pentagram","p","hexagram","h","none",0};
  
  HPAutoManual::HPAutoManual() : HPConstrainedString(auto_manual_dict) {}
  HPOnOff::HPOnOff() : HPConstrainedString(on_off_dict) {}
  HPFontAngle::HPFontAngle() : HPConstrainedString(font_angle_dict) {}
  HPFontUnits::HPFontUnits() : HPConstrainedString(font_units_dict) {}
  HPFontWeight::HPFontWeight() : HPConstrainedString(font_weight_dict) {}
  HPLineStyle::HPLineStyle() : HPConstrainedString(line_style_dict) {}
  HPTopBottom::HPTopBottom() : HPConstrainedString(top_bottom_dict) {}
  HPLeftRight::HPLeftRight() : HPConstrainedString(left_right_dict) {}
  HPNormalReverse::HPNormalReverse() : HPConstrainedString(normal_reverse_dict) {}
  HPLinearLog::HPLinearLog() : HPConstrainedString(linear_log_dict) {}
  HPNextPlotMode::HPNextPlotMode() : HPConstrainedString(next_plot_dict) {}
  HPProjectionMode::HPProjectionMode() : HPConstrainedString(projection_mode_dict) {}
  HPInOut::HPInOut() : HPConstrainedString(in_out_dict) {}
  HPUnits::HPUnits() : HPConstrainedString(units_dict) {}
  HPPosition::HPPosition() : HPConstrainedString(position_dict) {}
  HPAlignHoriz::HPAlignHoriz() : HPConstrainedString(horiz_dict) {}
  HPAlignVert::HPAlignVert() : HPConstrainedString(vert_dict) {}
  HPSymbol::HPSymbol() : HPConstrainedString(symb_dict) {}
  HPLineStyleOrder::HPLineStyleOrder() : HPConstrainedStringSet(line_style_dict) {}

  HPHandles::HPHandles() {
  }
  
  Array HPHandles::Get() {
    Array ret(Array::uint32VectorConstructor(data.size()));
    unsigned *dp = (unsigned*) ret.getReadWriteDataPointer();
    for (int i=0;i<data.size();i++)
      dp[i] = data[i];
    return ret;
  }

  void HPHandles::Set(Array arg) {
    if (arg.isEmpty()) {
      data.clear();
      HandleProperty::Set(arg);
      return;
    }
    if (!arg.isReal())
      throw Exception("expecting handle for property");
    arg.promoteType(FM_UINT32);
    const unsigned *dp = (const unsigned*) arg.getDataPointer();
    // make sure they are all valid handles
    for (int i=0;i<arg.getLength();i++) 
      ValidateHandle(dp[i]);
    data.clear();
    for (int i=0;i<arg.getLength();i++) 
      data.push_back(dp[i]);
    HandleProperty::Set(arg); 
  }

  Array HPString::Get() {
    return Array::stringConstructor(data);
  }

  void HPString::Set(Array arg) {
    HandleProperty::Set(arg);
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
    HandleProperty::Set(num);
    num.promoteType(FM_DOUBLE);
    const double *dp = (const double*) num.getDataPointer();
    data.clear();
    for (int i=0;i<num.getLength();i++)
      data.push_back(dp[i]);
  }
  
  void HPFixedVector::Set(Array num) {
    HandleProperty::Set(num);
    num.promoteType(FM_DOUBLE);
    const double *dp = (const double*) num.getDataPointer();
    data.clear();
    for (int i=0;i<qMin(m_len,(unsigned int)num.getLength());i++)
      data.push_back(dp[i]);
  }

  double& HPVector::At(int ndx) {
    return data[ndx];
  }

  double& HPVector::operator[](int ndx) {
    return data[ndx];
  }

  void HPColorVector::Set(Array arg) {
    HandleProperty::Set(arg);
    arg.promoteType(FM_DOUBLE);
    if ((!arg.is2D()) || (arg.getDimensionLength(1) != 3))
      throw Exception("Expect an m x 3 matrix for color orders");
    const double *dp = (const double *) arg.getDataPointer();
    int n = arg.getLength();
    for (int i=0;i<n;i++) 
      if ((dp[i] < 0) || (dp[i] > 1.0))
	throw Exception("Color vector must be between 0 and 1");
    for (int i=0;i<n;i++) 
      data.push_back(dp[i]);
  }

  Array HPColorVector::Get() {
    int count = data.size();
    int rows = count/3;
    double *rp = (double*) Array::allocateArray(FM_DOUBLE,count);
    for (int i=0;i<count;i++)
      rp[i] = data[i];
    return Array::Array(FM_DOUBLE,Dimensions(rows,3),rp);
  }
  
  void HPColor::Set(Array arg) {
    HandleProperty::Set(arg);
    if (arg.isString()) {
      char *cp = arg.getContentsAsCString();
      if (strcmp(cp,"none")==0) {
	data.clear(); data.push_back(-1); 
	data.push_back(-1); data.push_back(-1);
      } else if ((strcmp(cp,"y") == 0) || (strcmp(cp,"yellow")==0)) {
	data.clear(); data.push_back(1); 
	data.push_back(1); data.push_back(0);
      } else if ((strcmp(cp,"m") == 0) || (strcmp(cp,"magenta")==0)) {
	data.clear(); data.push_back(1); 
	data.push_back(0); data.push_back(1);
      } else if ((strcmp(cp,"c") == 0) || (strcmp(cp,"cyan")==0)) {
	data.clear(); data.push_back(0); 
	data.push_back(1); data.push_back(1);
      } else if ((strcmp(cp,"r") == 0) || (strcmp(cp,"red")==0)) {
	data.clear(); data.push_back(1); 
	data.push_back(0); data.push_back(0);
      } else if ((strcmp(cp,"g") == 0) || (strcmp(cp,"green")==0)) {
	data.clear(); data.push_back(0); 
	data.push_back(1); data.push_back(0);
      } else if ((strcmp(cp,"b") == 0) || (strcmp(cp,"blue")==0)) {
	data.clear(); data.push_back(0); 
	data.push_back(1); data.push_back(1);
      } else if ((strcmp(cp,"w") == 0) || (strcmp(cp,"white")==0)) {
	data.clear(); data.push_back(1); 
	data.push_back(1); data.push_back(1);
      } else if ((strcmp(cp,"k") == 0) || (strcmp(cp,"black")==0)) {
	data.clear(); data.push_back(0); 
	data.push_back(0); data.push_back(0);
      } else
	throw Exception("unrecognized color spec");
    } else {
      if (arg.getLength() != 3)
	throw Exception("color spec must be a length 3 array (or a string 'none','white',etc)");
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
    std::string retval;
    for (unsigned i=0;i<data.size()-1;i++) {
      retval.append(data[i]);
      retval.append("|");
    }
    retval.append(data.back());
    return Array::stringConstructor(retval);
  }

  void HPStringSet::Set(Array arg) {
    HandleProperty::Set(arg);
    if (!arg.isString()) 
      throw Exception("expecting a '|'-delimited list of strings for property argument");
    std::string args(ArrayToString(arg));
    data.clear();
    Tokenize(args,data,"|");
  }

  void HPConstrainedString::Set(Array arg) {
    HandleProperty::Set(arg);
    if (!arg.isString())
      throw Exception("expecting a string for property");
    std::string tst(ArrayToString(arg));
    if (find(m_dictionary.begin(),m_dictionary.end(),tst) == m_dictionary.end())
      throw Exception("illegal selection for property");
    HPString::Set(arg);
  }

  void HPConstrainedStringSet::Set(Array arg) {
    HandleProperty::Set(arg);
    HPStringSet::Set(arg);
    // Validate the result
    for (int i=0;i<data.size();i++)
      if (find(m_dictionary.begin(),m_dictionary.end(),data[i]) == m_dictionary.end())
	throw Exception("illegal selection for property");
  }

  void HPScalar::Value(double x) {
    At(0) = x;
  }
}


