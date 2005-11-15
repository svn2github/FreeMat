#include "HandleText.hpp"

void HandleText::ConstructProperties() {
  AddProperty(new HPString,"string");
  AddProperty(new HPFourVector,"extent");
  AddProperty(new HPAlignHoriz,"horizontalalignment");
  AddProperty(new HPThreeVector,"position");
  AddProperty(new HPScalar,"rotation");
  AddProperty(new HPUnits,"units");
  AddProperty(new HPAlignVert,"verticalalignment");
  AddProperty(new HPColor,"backgroundcolor");
  AddProperty(new HPColor,"edgecolor");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"margin");
  AddProperty(new HPFontAngle,"fontangle");
  AddProperty(new HPString,"fontname");
  AddProperty(new HPScalar,"fontsize");
  AddProperty(new HPFontUnits,"fontunits");
  AddProperty(new HPFontWeight,"fontweight");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPColor,"color");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPString,"tag");
  //    AddProperty(new Array,"userdata");
    
}
