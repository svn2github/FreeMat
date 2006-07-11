/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "HandleProperty.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleCommands.hpp"
#include "Core.hpp"

static const char *auto_manual_dict[3] = {"auto","manual",0};
static const char *on_off_dict[3] = {"on","off",0};
static const char *on_inactive_off_dict[4] = {"on","active","off",0};
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
static const char *mapmode_dict[4] = {"none","direct","scaled",0};
static const char *datamapmode_dict[3] = {"scaled","direct",0};
static const char *rowcols_dict[4] = {"both","rows","cols",0};
static const char *autoflat_dict[5] = {"none","auto","flat","colorspec",0};
static const char *lightingmode_dict[5] = {"none","flat","gouraud","phong",0};
static const char *interpmode_dict[5] = {"none","flat","interp","colorspec",0};
static const char *facealpha_dict[5] = {"flat","interp","texturemap","scalar",0};
static const char *backface_dict[4] = {"unlit","lit","reverselit",0};
static const char *edgealpha_dict[4] = {"flat","interp","scalar",0};
static const char *render_dict[3] = {"painters","opengl",0};
static const char *widget_dict[11] = {"pushbutton","togglebutton","radiobutton","checkbox",
				      "edit","text","slider","frame","listbox","popupmenu",0};
  
HPAutoManual::HPAutoManual() : HPConstrainedString(auto_manual_dict) {}
HPOnOff::HPOnOff() : HPConstrainedString(on_off_dict) {}
HPOnOffInactive::HPOnOffInactive() : HPConstrainedString(on_inactive_off_dict) {}
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
HPMappingMode::HPMappingMode() : HPConstrainedString(mapmode_dict) {}
HPDataMappingMode::HPDataMappingMode() : HPConstrainedString(datamapmode_dict) {}
HPLineStyleOrder::HPLineStyleOrder() : HPConstrainedStringSet(line_style_dict) {}
HPRowColumns::HPRowColumns() : HPConstrainedString(rowcols_dict) {}
HPLightingMode::HPLightingMode() : HPConstrainedString(lightingmode_dict) {}
HPAutoFlatColor::HPAutoFlatColor() : HPConstrainedStringColor(autoflat_dict) {}
HPColorInterp::HPColorInterp() : HPConstrainedStringColor(interpmode_dict) {}
HPFaceAlpha::HPFaceAlpha() : HPConstrainedStringScalar(facealpha_dict) {}
HPBackFaceLighting::HPBackFaceLighting() : HPConstrainedString(backface_dict) {}
HPWidgetString::HPWidgetString() : HPConstrainedString(widget_dict) {}
HPEdgeAlpha::HPEdgeAlpha() : HPConstrainedStringScalar(edgealpha_dict) {}
HPRenderMode::HPRenderMode() : HPConstrainedString(render_dict) {}

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
  data.clear();
  int rows = n/3;
  for (int i=0;i<rows;i++) 
    for (int j=0;j<3;j++)
      data.push_back(dp[i+j*rows]);
}

Array HPColorVector::Get() {
  int count = data.size();
  int rows = count/3;
  double *rp = (double*) Array::allocateArray(FM_DOUBLE,count);
  for (int i=0;i<rows;i++)
    for (int j=0;j<3;j++)
      rp[i+j*rows] = data[3*i+j];
  return Array::Array(FM_DOUBLE,Dimensions(rows,3),rp);
}

//!
//@Module COLORSPEC Color Property Description
//@@Section HANDLE
//@@Usage
//There are a number of ways of specifying a color value for
//a color-based property.  Examples include line colors, 
//marker colors, and the like.  One option is to specify
//color as an RGB triplet
//@[
//   set(h,'color',[r,g,b])
//@]
//where @|r,g,b| are between @[0,1]@.  Alternately, you can
//use color names to specify a color.
//\begin{itemize}
//  \item @|'none'| - No color.
//  \item @|'y','yellow'| - The color @[1,1,0]@ in RGB space.
//  \item @|'m','magenta'| - The color @[1,0,1]@ in RGB space.
//  \item @|'c','cyan'| - The color @[0,1,1]@ in RGB space.
//  \item @|'r','red'| - The color @[1,0,0]@ in RGB space.
//  \item @|'g','green'| - The color @[0,1,0]@ in RGB space.
//  \item @|'b','blue'| - The color @[0,0,1]@ in RGB space.
//  \item @|'w','white'| - The color @[1,1,1]@ in RGB space.
//  \item @|'k','black'| - The color @[0,0,0]@ in RGB space.
//\end{itemize}
//!
bool ParseColorSpec(Array arg, std::vector<double> &data) {
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
      data.push_back(0); data.push_back(1);
    } else if ((strcmp(cp,"w") == 0) || (strcmp(cp,"white")==0)) {
      data.clear(); data.push_back(1); 
      data.push_back(1); data.push_back(1);
    } else if ((strcmp(cp,"k") == 0) || (strcmp(cp,"black")==0)) {
      data.clear(); data.push_back(0); 
      data.push_back(0); data.push_back(0);
    } else
      return false;
  } else {
    if (arg.getLength() != 3)
      return false;
    arg.promoteType(FM_DOUBLE);
    const double *dp = (const double*) arg.getDataPointer();
    if (((dp[0] < 0) || (dp[0] > 1)) ||
	((dp[1] < 0) || (dp[1] > 1)) ||
	((dp[2] < 0) || (dp[2] > 1)))
      return false;
    data.clear();
    data.push_back(dp[0]);
    data.push_back(dp[1]);
    data.push_back(dp[2]);
  }
  return true;
}

void HPColor::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!ParseColorSpec(arg,data))
    throw Exception("Expecting a color spec: either a color name or a 3-vector or RGB values");
}
  
Array HPColor::Get() {
  if (data[0] == -1)
    return Array::stringConstructor("none");
  else
    return HPVector::Get();
}
  
Array HPStringSet::Get() {
  if (data.size() == 0)
    return Array::stringConstructor("");
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

double HPConstrainedStringScalar::Scalar() {
  return scalar;
}

void HPConstrainedStringScalar::Scalar(double scal) {
  scalar = scal;
}

void HPConstrainedStringScalar::Set(Array arg) {
  HandleProperty::Set(arg);
  if (arg.isString())
    HPConstrainedString::Set(arg);
  else
    scalar = ArrayToDouble(arg);
}

std::vector<double> HPConstrainedStringColor::ColorSpec() {
  return colorspec;
}

void HPConstrainedStringColor::ColorSpec(std::vector<double> col) {
  colorspec = col;
}

void HPConstrainedStringColor::ColorSpec(double r, double g, double b) {
  std::vector<double> data;
  data.push_back(r);
  data.push_back(g);
  data.push_back(b);
  colorspec = data;
}

void HPConstrainedStringColor::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!ParseColorSpec(arg,colorspec)) {
    HPConstrainedString::Set(arg);
  } else {
    data = "colorspec";
  }
  if (colorspec.size() > 1 && (colorspec[0] == -1))
    data = "none";
}

Array HPConstrainedStringColor::Get() {
  if (!Is("colorspec"))
    return HPConstrainedString::Get();
  Array ret(Array::doubleVectorConstructor(3));
  double *dp = (double*) ret.getReadWriteDataPointer();
  dp[0] = colorspec[0];
  dp[1] = colorspec[1];
  dp[2] = colorspec[2];
  return ret;
}

Array HPConstrainedStringScalar::Get() {
  if (!Is("scalar"))
    return HPConstrainedString::Get();
  return Array::doubleConstructor(scalar);
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
