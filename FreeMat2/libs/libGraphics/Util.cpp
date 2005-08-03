#include "Util.hpp"

namespace FreeMat {
  Point2D GetTextExtent(QPainter& gc, std::string label) {
    QRect sze(gc.fontMetrics().boundingRect(label.c_str()));
    return Point2D(sze.width(),sze.height());    
  }

  void DrawTextString(QPainter& gc, std::string txt, Point2D pos, 
		      int orient) {
    gc.save();
    gc.translate(pos.x, pos.y);
    gc.rotate(-orient);
    gc.drawText(0, 0, txt.c_str());
    gc.restore();    
  }

  void DrawTextStringAligned(QPainter& gc, std::string text, Point2D pos, 
			     XALIGNTYPE xalign, YALIGNTYPE yalign, 
			     int orient) {
    Point2D txtSize(GetTextExtent(gc,text));
    if (orient == 0) {
      if (yalign == TB_TOP)
	pos.y += txtSize.y;
      if (yalign == TB_CENTER)
	pos.y += txtSize.y/2;
      if (xalign == LR_RIGHT)
	pos.x -= txtSize.x;
      if (xalign == LR_CENTER)
	pos.x -= txtSize.x/2;
    } else if (orient == 90) {
      if (yalign == TB_TOP)
	pos.x += 0.0; //txtSize.y;
      if (yalign == TB_CENTER)
	pos.x += txtSize.y/2;
      if (xalign == LR_RIGHT)
	pos.y += txtSize.x;
      if (xalign == LR_CENTER)
	pos.y += txtSize.x/2;
    } 
    DrawTextString(gc,text,pos,orient);
  }

  void DrawLines(QPainter& gc, std::vector<Point2D> pts) {
#ifdef QT3
    QPointArray a(pts.size());
    for (int i=0;i<pts.size();i++)
      a.setPoint(i,pts[i].x,pts[i].y);
#else
    QPolygon a;
    for (int i=0;i<pts.size();i++)
      a.push_back(QPoint(pts[i].x,pts[i].y));  
#endif
    gc.drawPolyline(a);
  }

  void SetFontSize(QPainter& gc, int size) {
    gc.setFont(QFont("Helvetica",size));
  }
}
