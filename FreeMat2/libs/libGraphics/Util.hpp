#include <qpainter.h>
#include "Point2D.hpp"

namespace FreeMat {
  
typedef enum {
  LR_LEFT,
  LR_RIGHT,
  LR_CENTER
} XALIGNTYPE;

typedef enum {
  TB_TOP,
  TB_BOTTOM,
  TB_CENTER
} YALIGNTYPE;

  // Helper functions
  Point2D GetTextExtent(QPainter& gc, std::string label);
  void DrawTextString(QPainter& gc, std::string txt, Point2D pos, 
		      int orient = 0);
  void DrawTextStringAligned(QPainter& gc, std::string txt, Point2D pos, 
			     XALIGNTYPE xalign, YALIGNTYPE yalign, 
			     int orient = 0);
  void DrawLines(QPainter& gc, std::vector<Point2D> pts);
  void SetFontSize(QPainter& gc, int size);
}
