#include "GraphicsContext.hpp"

void GraphicsContext::DrawTextStringAligned(std::string text, Point2D pos, 
					    XALIGNTYPE xalign, 
					    YALIGNTYPE yalign){
  Point2D txtSize(GetTextExtent(text));
  if (yalign == TB_BOTTOM)
    pos.y -= txtSize.y;
  if (yalign == TB_CENTER)
    pos.y -= txtSize.y/2;
  if (xalign == LR_RIGHT)
    pos.x -= txtSize.x;
  if (xalign == LR_CENTER)
    pos.x -= txtSize.x/2;
  DrawTextString(text,pos);
}
