#include <qpainter.h>
#include <vector>
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
  Point2D GetTextExtentNoGC(std::string label, int size);
  void DrawTextString(QPainter& gc, std::string txt, Point2D pos, 
		      int orient = 0);
  void DrawTextStringAligned(QPainter& gc, std::string txt, Point2D pos, 
			     XALIGNTYPE xalign, YALIGNTYPE yalign, 
			     int orient = 0);
  void DrawLines(QPainter& gc, std::vector<Point2D> pts);
  void SetFontSize(QPainter& gc, int size);
  void BlitImage(QPainter& gc, unsigned char *data, int width, int height, int x0, int y0);
  
  void SaveFocus();
  void RestoreFocus();
  
  std::string NormalizeImageExtension(std::string ext);
  std::string FormatListAsString();

  /**
   * Print the given floating point argument, truncating superfluous
   * zeros, and optionally, print it in exponential format.
   */
  std::string TrimPrint(double val, bool scientificNotation);
  
}
