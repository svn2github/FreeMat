#include "GraphicsContext.hpp"

void GraphicsContext::DrawTextStringAligned(std::string text, Point2D pos, 
					    XALIGNTYPE xalign, 
					    YALIGNTYPE yalign,
					    OrientationType orient){
  Point2D txtSize(GetTextExtent(text));
  if (orient == ORIENT_0) {
    if (yalign == TB_TOP)
      pos.y += txtSize.y;
    if (yalign == TB_CENTER)
      pos.y += txtSize.y/2;
    if (xalign == LR_RIGHT)
      pos.x -= txtSize.x;
    if (xalign == LR_CENTER)
      pos.x -= txtSize.x/2;
  } else if (orient == ORIENT_90) {
    if (yalign == TB_TOP)
      pos.x += 0.0; //txtSize.y;
    if (yalign == TB_CENTER)
      pos.x += txtSize.y/2;
    if (xalign == LR_RIGHT)
      pos.y += txtSize.x;
    if (xalign == LR_CENTER)
      pos.y += txtSize.x/2;
  } 
  DrawTextString(text,pos,orient);
}
