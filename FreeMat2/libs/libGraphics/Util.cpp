#include "Exception.hpp"
#include "Util.hpp"
#include <qimage.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qfontmetrics.h>
#ifndef QT3
#include <qimagewriter.h>
#include <qgridlayout.h>
#else
#include <qlayout.h>
#include <qobjectlist.h>
#endif

namespace FreeMat {
  Point2D GetTextExtent(DrawEngine& gc, std::string label) {
    QRect sze(gc.fontMetrics().boundingRect(label.c_str()));
    return Point2D(sze.width(),sze.height());    
  }

  Point2D GetTextExtentNoGC(std::string label, int size) {
    QFontMetrics fm(QFont("Helvetica",size));
    QRect sze(fm.boundingRect(label.c_str()));
    return Point2D(sze.width(),sze.height());        
  }

  void DrawTextString(DrawEngine& gc, std::string txt, Point2D pos, 
		      int orient) {
    gc.save();
    gc.translate(pos.x, pos.y);
    gc.rotate(-orient);
    gc.drawText(0, 0, txt.c_str());
    gc.restore();    
  }

  void DrawTextStringAligned(DrawEngine& gc, std::string text, Point2D pos, 
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

  void DrawLines(DrawEngine& gc, std::vector<Point2D> pts) {
    std::vector<QPoint> op;
    for (int i=0;i<pts.size();i++)
      op.push_back(QPoint(pts[i].x,pts[i].y));
    gc.drawPolyline(op);
  }

  void SetFontSize(DrawEngine& gc, int size) {
    gc.setFont(QFont("Helvetica",size));
  }
  
  void BlitImage(DrawEngine& gc, unsigned char *data, int width, int height, int x0, int y0) {
#ifdef QT3
    QImage qimg(width, height, 32);
#else
    QImage qimg(width, height, QImage::Format_RGB32);
#endif
    for (int i=0;i<height;i++) {
      uint *p = (uint*) qimg.scanLine(i);
      for (int j=0;j<width;j++)
	p[j] = qRgb(data[i*width*3 + j*3],data[i*width*3 + j*3 + 1],data[i*width*3 + j*3 + 2]);
    }
    gc.drawImage(x0,y0,qimg);
  }

  QWidget *save = NULL;
  
  void SaveFocus() {
    save = qApp->focusWidget();
  }
  
  void RestoreFocus() {
    if (save)
      save->setFocus();
  }
  
  std::string NormalizeImageExtension(std::string ext) {
    std::string upperext(ext);
    std::string lowerext(ext);
    std::transform(upperext.begin(),upperext.end(),upperext.begin(),toupper);
    std::transform(lowerext.begin(),lowerext.end(),lowerext.begin(),tolower);
    if (upperext == "JPG") return std::string("JPEG");
    if ((upperext == "PDF") || (upperext == "PS") || (upperext == "EPS")) return upperext;
#ifdef QT3
    QStrList formats(QImage::outputFormats());
    for (int i=0;i<formats.count();i++) {
      if (formats.at(i) == upperext) return upperext;
      if (formats.at(i) == lowerext) return lowerext;
    }
#else
    QList<QByteArray> formats(QImageWriter::supportedImageFormats());
    for (int i=0;i<formats.count();i++) {
      if (formats.at(i).data() == upperext) return upperext;
      if (formats.at(i).data() == lowerext) return lowerext;
    }
#endif
    return std::string();
  }
  
  std::string FormatListAsString() {
    std::string ret_text = "Supported Formats: ";
#ifdef QT3
    QStrList formats(QImage::outputFormats());
    for (int i=0;i<formats.count();i++)
      ret_text = ret_text + formats.at(i) + " ";
#else
    QList<QByteArray> formats(QImageWriter::supportedImageFormats());
    for (int i=0;i<formats.count();i++)
      ret_text = ret_text + formats.at(i).data() + " ";
#endif
    return ret_text;
  }

  std::string TrimPrint(double val, bool scientificNotation) {
    char buffer[1000];
    char *p;
    if (!scientificNotation) {
      sprintf(buffer,"%f",val);
      p = buffer + strlen(buffer) - 1;
      while (*p == '0') {
	*p = 0;
	p--;
      }
      if ((*p == '.') || (*p == ',')) {
	*(p+1) = '0';
	*(p+2) = 0;
      }
      return std::string(buffer);
    } else {
      sprintf(buffer,"%e",val);
      std::string label(buffer);
      unsigned int ePtr;
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr--;
      while (label[ePtr] == '0') {
	label.erase(ePtr,1);
	ePtr--;
      }
      if ((label[ePtr] == '.') || (label[ePtr] == ','))
	label.insert(ePtr+1, 1,'0');
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr+=2;
      while ((label[ePtr] == '0') && ePtr < label.size()) {
	label.erase(ePtr,1);
      }
      if (ePtr == label.size())
	label.append("0");
      return label;
    }
  }

  void ClearGridWidget(QWidget* w, const char *name) {
    // Look for a child widget in the position (row,col)
    QGridLayout *l = dynamic_cast<QGridLayout*>(w->layout());
    if (!l) throw Exception("ClearGridWidget failed... this is unexpected - please file a bug report at http://freemat.sf.net detailing what happened");
    const QObjectList children = w->children();
    for (int i = 0; i < children.size(); ++i) {
      QWidget *p = dynamic_cast<QWidget*>(children.at(i));
    }
  }

  void SetGridWidget(QWidget* w, QPWidget* m, int row, int col) {
    // Look for a child widget in the position (row,col)
    QGridLayout *l = dynamic_cast<QGridLayout*>(w->layout());
    if (!l) throw Exception("SetPlotWidget failed... this is unexpected - please file a bug report at http://freemat.sf.net detailing what happened");
    l->addWidget(m,row,col);
  }
  
}
