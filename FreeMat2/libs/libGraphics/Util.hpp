#include <qpainter.h>
#include <vector>
#include "Point2D.hpp"
#include "DrawEngine.hpp"
#include "QPWidget.hpp"

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
  Point2D GetTextExtent(DrawEngine& gc, std::string label);
  Point2D GetTextExtentNoGC(std::string label, int size);
  void DrawTextString(DrawEngine& gc, std::string txt, Point2D pos, 
		      int orient = 0);
  void DrawTextStringAligned(DrawEngine& gc, std::string txt, Point2D pos, 
			     XALIGNTYPE xalign, YALIGNTYPE yalign, 
			     int orient = 0);
  void DrawLines(DrawEngine& gc, std::vector<Point2D> pts);
  void SetFontSize(DrawEngine& gc, int size);
  void BlitImage(DrawEngine& gc, unsigned char *data, int width, int height, int x0, int y0);
  
  void SaveFocus();
  void RestoreFocus();
  
  std::string NormalizeImageExtension(std::string ext);
  std::string FormatListAsString();

  void ClearGridWidget(QWidget* w, const char *name);
  void SetGridWidget(QWidget* w, QPWidget* m, int row, int col);

  /**
   * Print the given floating point argument, truncating superfluous
   * zeros, and optionally, print it in exponential format.
   */
  std::string TrimPrint(double val, bool scientificNotation);
  
}
