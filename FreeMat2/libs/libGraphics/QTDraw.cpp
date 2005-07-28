#include "QTDraw.hpp"
#include "GraphicsContext.hpp"
#include <qpainter.h>
#include "QTGC.hpp"
#ifndef QT3
#include <QMouseEvent>
#include <QImageWriter>
#include <Q3MemArray>
#endif

namespace FreeMat {

  GraphicsContext *the_gc = NULL;

#define ARGTEST(n,name) { if (arg.size() != n) { \
      char buffer[1000]; \
      sprintf(buffer,"%s requires at least %d arguments",name,n); \
      throw Exception(buffer);\
    } \
    if (!the_gc) throw Exception("core graphics routines only valid inside a drawing function"); \
    }
  
  void QTDraw::SetWalkTree(WalkTree* tree) {
    m_tree = tree;
  }
  
  void QTDraw::SetCallback(FunctionDef* fdef, ArrayVector arg) {
    m_fdef = fdef;
    m_args = arg;
  }

  QTDraw::QTDraw() : 
#ifdef QT3
    QWidget(NULL,NULL,WRepaintNoErase) 
#else
      QWidget(NULL,NULL,Qt::WNoAutoErase)
#endif
  {
  }

  QTDraw::~QTDraw() {
  }

  void QTDraw::paintEvent(QPaintEvent* e) {
#ifndef QT3
  Q3MemArray<QRect> rects = e->region().rects();
#else
  QMemArray<QRect> rects = e->region().rects();
#endif
    QPainter painter(this);
    for ( uint i = 0; i < rects.count(); i++ ) {
#ifndef QT3
    painter.drawPixmap(rects[(int) i],m_pixmap,rects[(int) i]);
#else
    painter.drawPixmap(QPoint(rects[(int) i].left(),rects[(int) i].top()),m_pixmap,rects[(int) i]);
#endif
    }
  }

  void QTDraw::resizeEvent(QResizeEvent* e) {
    m_pixmap = QPixmap(width(),height());
    OnDraw();
  }
  
  void QTDraw::OnDraw() {
    QPainter *paint = new QPainter(&m_pixmap);
    QTGC gc(*paint,width(),height());
    the_gc = &gc;
    m_fdef->updateCode();
    bool eflag(m_tree->GUIEventFlag());
    m_tree->GUIEventFlag(false);
    ArrayVector cval = m_fdef->evaluateFunction(m_tree,m_args,0);
    m_tree->GUIEventFlag(eflag);
    the_gc = NULL;
    delete paint;
  }
  
  Array Point2Array(Point2D pt) {
    Array retvec(Array::floatVectorConstructor(2));
    float*dp = (float*) retvec.getReadWriteDataPointer();
    dp[0] = pt.x;
    dp[1] = pt.y;
    return retvec;
  }
  
  Point2D Array2Point(Array a) {
    a.promoteType(FM_FLOAT);
    if (a.getLength() != 2) throw Exception("expected a length 2 vector to represent a 2D point");
    float *dp = (float*) a.getDataPointer();
    return Point2D(dp[0],dp[1]);
  }

  std::vector<Point2D> Array2PointVector(Array a) {
    a.promoteType(FM_FLOAT);
    if (a.getLength() % 2) throw Exception("expected an even length vector to represent a list of 2D points");
    float *dp = (float*) a.getDataPointer();
    std::vector<Point2D> ptlist;
    int n = a.getLength()/2;
    for (int i=0;i<n;i++)
      ptlist.push_back(Point2D(dp[2*i],dp[2*i+1]));
    return ptlist;
  }

  Array Color2Array(Color a) {
    Array retvec(Array::floatVectorConstructor(3));
    float *dp = (float*) retvec.getReadWriteDataPointer();
    dp[0] = a.red;
    dp[1] = a.green;
    dp[2] = a.blue;
    return retvec;
  }

  Color Array2Color(Array a) {
    a.promoteType(FM_FLOAT);
    if (a.getLength() != 3) throw Exception("expected a length 3 vector to represent a color");
    float *dp = (float*) a.getDataPointer();
    return Color(dp[0],dp[1],dp[2]);
  }

  XALIGNTYPE Array2XAlign(Array a) {
    char *str = a.getContentsAsCString();
    if ((str[0] == 'l') || (str[0] == 'L'))
      return LR_LEFT;
    if ((str[0] == 'r') || (str[0] == 'R'))
      return LR_RIGHT;
    if ((str[0] == 'c') || (str[0] == 'C'))
      return LR_CENTER;
    throw Exception("unrecognized x-align spec");
  }

  YALIGNTYPE Array2YAlign(Array a) {
    char *str = a.getContentsAsCString();
    if ((str[0] == 't') || (str[0] == 'T'))
      return TB_TOP;
    if ((str[0] == 'b') || (str[0] == 'B'))
      return TB_BOTTOM;
    if ((str[0] == 'c') || (str[0] == 'C'))
      return TB_CENTER;
    throw Exception("unrecognized y-align spec");
  }

  OrientationType Array2Orient(Array a) {
    int angle = a.getContentsAsIntegerScalar();
    if (angle == 0)
      return ORIENT_0;
    if (angle == 90)
      return ORIENT_90;
    if (angle == 180)
      return ORIENT_180;
    if (angle == 270)
      return ORIENT_270;
    throw Exception("illegal value for text orientation");
  }

  LineStyleType Array2LineStyle(Array a) {
    char *str = a.getContentsAsCString();
    if ((strcmp(str,"solid") == 0) || (strcmp(str,"SOLID") == 0))
      return LINE_SOLID;
    if ((strcmp(str,"dashed") == 0) || (strcmp(str,"DASHED") == 0))
      return LINE_DASHED;
    if ((strcmp(str,"dotted") == 0) || (strcmp(str,"DOTTED") == 0))
      return LINE_DOTTED;
    if ((strcmp(str,"dashdot") == 0) || (strcmp(str,"DASHDOT") == 0))
      return LINE_DASH_DOT;
    if ((strcmp(str,"none") == 0) || (strcmp(str,"NONE") == 0))
      return LINE_NONE;
    throw Exception("illegal value for line style");
  }

  Array LineStyle2Array(LineStyleType a) {
    switch (a) {
    case LINE_SOLID:
      return Array::stringConstructor("solid");
    case LINE_DASHED:
      return Array::stringConstructor("dashed");
    case LINE_DOTTED:
      return Array::stringConstructor("dotted");
    case LINE_DASH_DOT:
      return Array::stringConstructor("dashdot");
    case LINE_NONE:
      return Array::stringConstructor("none");
    }
    return Array::stringConstructor("unknown");
  }

  double Array2Double(Array a) {
    return a.getContentsAsDoubleScalar();
  }

  int Array2Int(Array a) {
    return a.getContentsAsIntegerScalar();
  }

  std::string Array2String(Array a) {
    return std::string(a.getContentsAsCString());
  }

  Rect2D Array2Rect(Array a) {
    a.promoteType(FM_FLOAT);
    if (a.getLength() != 4) throw Exception("expected a length 4 vector to represent a 2D rectangle");
    float *dp = (float*) a.getDataPointer();
    return Rect2D(dp[0],dp[1],dp[2],dp[3]);
  }

  Array Rect2Array(Rect2D a) {
    Array retvec(Array::floatVectorConstructor(4));
    float *dp = (float*) retvec.getReadWriteDataPointer();
    dp[0] = a.x1;
    dp[1] = a.y1;
    dp[2] = a.width;
    dp[3] = a.height;
    return retvec;
  }

  void Array2Image(Array a, unsigned char * &data, int &width, int &height) {
    a.promoteType(FM_UINT8);
    // a must be N x N x 3
    Dimensions dims(a.getDimensions());
    if (dims.getLength() != 3) throw Exception("image data must be M x N x 3");
    if (dims[2] != 3) throw Exception("image data must be M x N x 3");
    height = dims[0];
    width = dims[1];
    data = (unsigned char *) a.getDataPointer();
  }

  ArrayVector GetCanvasSizeFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(0,"getcanvassize");
    return singleArrayVector(Point2Array(the_gc->GetCanvasSize()));
  }
  
  ArrayVector GetTextExtentFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"gettextextent");
    Array txt(arg[0]);
    Point2D pt(the_gc->GetTextExtent(txt.getContentsAsCString()));
    return singleArrayVector(Point2Array(pt));
  }

  ArrayVector DrawTextStringAlignedFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(5,"drawtextstringaligned");
    Array a_txt(arg[0]);
    Array a_pos(arg[1]);
    Array a_xalign(arg[2]);
    Array a_yalign(arg[3]);
    Array a_orient(arg[4]);
    char *txt = a_txt.getContentsAsCString();
    Point2D pos(Array2Point(arg[1]));
    XALIGNTYPE xalign(Array2XAlign(arg[2]));
    YALIGNTYPE yalign(Array2YAlign(arg[3]));
    OrientationType orient(Array2Orient(arg[4]));
    the_gc->DrawTextStringAligned(txt,pos,xalign,yalign,orient);
    return ArrayVector();
  }

  ArrayVector DrawTextStringFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(3,"drawtextstring");
    Array a_txt(arg[0]);
    Array a_pos(arg[1]);
    Array a_orient(arg[2]);
    char *txt = a_txt.getContentsAsCString();
    Point2D pos(Array2Point(arg[1]));
    OrientationType orient(Array2Orient(arg[2]));
    the_gc->DrawTextString(txt,pos,orient);
    return ArrayVector();
  }

  ArrayVector SetFontFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"setfontsize");
    Array a_size(arg[0]);
    the_gc->SetFont(a_size.getContentsAsIntegerScalar());
    return ArrayVector();
  }

  ArrayVector SetForeGroundColorFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"setforegroundcolor");
    Color color(Array2Color(arg[0]));
    Color old_color(the_gc->SetForeGroundColor(color));
    return singleArrayVector(Color2Array(old_color));
  }
  
  ArrayVector SetLineStyleFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"setlinestyle");
    LineStyleType lstype(Array2LineStyle(arg[0]));
    LineStyleType old_lstype(the_gc->SetLineStyle(lstype));
    return singleArrayVector(LineStyle2Array(old_lstype));
  }

  ArrayVector DrawLineFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(2,"drawline");
    the_gc->DrawLine(Array2Point(arg[0]),Array2Point(arg[1]));
    return ArrayVector();
  }

  ArrayVector DrawPointFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"drawpoint");
    the_gc->DrawPoint(Array2Point(arg[0]));
    return ArrayVector();
  }

  ArrayVector DrawCircleFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(2,"drawcircle");
    the_gc->DrawCircle(Array2Point(arg[0]),Array2Int(arg[1]));
    return ArrayVector();
  }

  ArrayVector DrawRectangleFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"drawrectangle");
    the_gc->DrawRectangle(Array2Rect(arg[0]));
    return ArrayVector();
  }

  ArrayVector FillRectangleFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"fillrectangle");
    the_gc->FillRectangle(Array2Rect(arg[0]));
    return ArrayVector();
  }

  ArrayVector FillQuadFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(4,"fillquad");
    the_gc->FillQuad(Array2Point(arg[0]),Array2Point(arg[1]),
		     Array2Point(arg[2]),Array2Point(arg[3]));
    return ArrayVector();
  }

  ArrayVector DrawQuadFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(4,"drawquad");
    the_gc->DrawQuad(Array2Point(arg[0]),Array2Point(arg[1]),
		     Array2Point(arg[2]),Array2Point(arg[3]));
    return ArrayVector();
  }
  
  void DrawSymbol(Point2D pos, std::string symbol, int len) {
    int len2 = (int) (len / sqrt(2.0));
    if (symbol == ".")
      the_gc->DrawPoint(pos);
    else if (symbol == "o")
      the_gc->DrawCircle(pos, len);
    else if (symbol == "x") {
      the_gc->DrawLine(Point2D(pos.x - len2, pos.y - len2), 
		       Point2D(pos.x + len2 + 1, pos.y + len2 + 1));
      the_gc->DrawLine(Point2D(pos.x + len2, pos.y - len2), 
			Point2D(pos.x - len2 - 1, pos.y + len2 + 1));
    }
    else if (symbol == "+") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y), 
		       Point2D(pos.x + len + 1, pos.y));
      the_gc->DrawLine(Point2D(pos.x, pos.y - len), 
		       Point2D(pos.x, pos.y + len + 1));
    }
    else if (symbol == "*") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y), 
		       Point2D(pos.x + len + 1, pos.y));
      the_gc->DrawLine(Point2D(pos.x, pos.y - len), 
		       Point2D(pos.x, pos.y + len + 1));
      the_gc->DrawLine(Point2D(pos.x - len2, pos.y - len2), 
		       Point2D(pos.x + len2 + 1, pos.y + len2 + 1));
      the_gc->DrawLine(Point2D(pos.x + len2, pos.y - len2), 
		       Point2D(pos.x - len2 - 1, pos.y + len2 + 1));
    }
    else if (symbol == "s") 
      the_gc->DrawRectangle(Rect2D(pos.x - len/2, pos.y - len/2, len + 1, len + 1));
    else if (symbol == "d") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y), Point2D(pos.x, pos.y - len));
      the_gc->DrawLine(Point2D(pos.x, pos.y - len), Point2D(pos.x + len, pos.y));
      the_gc->DrawLine(Point2D(pos.x + len, pos.y), Point2D(pos.x, pos.y + len));
      the_gc->DrawLine(Point2D(pos.x, pos.y + len), Point2D(pos.x - len, pos.y));
    }
    else if (symbol == "v") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y - len), 
		       Point2D(pos.x + len, pos.y - len));
      the_gc->DrawLine(Point2D(pos.x + len, pos.y - len), 
		       Point2D(pos.x, pos.y + len));
      the_gc->DrawLine(Point2D(pos.x, pos.y + len), 
		       Point2D(pos.x - len, pos.y - len));
    }
    else if (symbol == "^") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y + len), 
		       Point2D(pos.x + len, pos.y + len));
      the_gc->DrawLine(Point2D(pos.x + len, pos.y + len), 
		       Point2D(pos.x, pos.y - len));
      the_gc->DrawLine(Point2D(pos.x, pos.y - len), 
		       Point2D(pos.x - len, pos.y + len));
    }
    else if (symbol == "<") {
      the_gc->DrawLine(Point2D(pos.x + len, pos.y - len), 
		       Point2D(pos.x - len, pos.y));
      the_gc->DrawLine(Point2D(pos.x - len, pos.y), 
		       Point2D(pos.x + len, pos.y + len));
      the_gc->DrawLine(Point2D(pos.x + len, pos.y + len), 
		       Point2D(pos.x + len, pos.y - len));
    }
    else if (symbol == ">") {
      the_gc->DrawLine(Point2D(pos.x - len, pos.y - len), 
		       Point2D(pos.x + len, pos.y));
      the_gc->DrawLine(Point2D(pos.x + len, pos.y), 
		       Point2D(pos.x - len, pos.y + len));
      the_gc->DrawLine(Point2D(pos.x - len, pos.y + len), 
		       Point2D(pos.x - len, pos.y - len));
    }    
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

  ArrayVector TrimPrintFunction(int /* nargout */, const ArrayVector& arg) {
    if (arg.size() != 2) 
      throw Exception("trimprint requires at least 2 arguments");
    double val(Array2Double(arg[0]));
    int boolflag(Array2Int(arg[1]));
    return singleArrayVector(Array::stringConstructor(TrimPrint(val,boolflag)));
  }

  ArrayVector DrawSymbolsFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(3,"drawsymbols");
    std::vector<Point2D> pos(Array2PointVector(arg[0]));
    std::string symbol(Array2String(arg[1]));
    int length(Array2Int(arg[2]));
    for (int i=0;i<pos.size();i++)
      if (pos[i].isFinite())
	DrawSymbol(pos[i],symbol,length);
    return ArrayVector();
  }
  
  ArrayVector DrawLinesFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"drawlines");
    the_gc->DrawLines(Array2PointVector(arg[0]));
    return ArrayVector();
  }

  ArrayVector PushClipFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(1,"pushclip");
    the_gc->PushClippingRegion(Array2Rect(arg[0]));
    return ArrayVector();
  }

  ArrayVector PopClipFunction(int /* nargout */, const ArrayVector& arg) {
    return singleArrayVector(Rect2Array(the_gc->PopClippingRegion()));
  }

  ArrayVector BlitImageFunction(int /* nargout */, const ArrayVector& arg) {
    ARGTEST(2,"blitimage");
    unsigned char *data;
    int width, height;
    Array2Image(arg[0],data,width,height);
    Point2D pt(Array2Point(arg[1]));
    the_gc->BlitImage(data,width,height,pt.x,pt.y);
    return ArrayVector();
  }

  // cfigure(callback, data)
  ArrayVector CFigureFunction(int /*nargout*/, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() == 0) throw Exception("cfigure function needs at least 1 argument (callback)");
    FuncPtr callback;
    ArrayVector args(arg);
    args.erase(args.begin(),args.begin()+1);
    if (!eval->lookupFunction(Array2String(arg[0]),callback,args))
      throw Exception("unable to find callback function for custom figure");
    QTDraw* p = new QTDraw;
    p->SetWalkTree(eval);
    p->SetCallback(callback,args);
    p->show();
    return ArrayVector();
  }

  void LoadQTDraw(Context* context) {
    context->addFunction("getcanvassize",GetCanvasSizeFunction,0,1);
    context->addFunction("gettextextent",GetTextExtentFunction,1,1,"string");
    context->addFunction("drawtextstringaligned",DrawTextStringAlignedFunction,5,0,
			 "string","pos","xalign","yalign","orient");
    context->addFunction("drawtextstring",DrawTextStringFunction,3,0,
			 "string","pos","orient");
    context->addFunction("setfontsize",SetFontFunction,1,0,"size");
    context->addFunction("setforegroundcolor",SetForeGroundColorFunction,1,1,"color");
    context->addFunction("setlinestyle",SetLineStyleFunction,1,1,"linestyle");
    context->addFunction("drawline",DrawLineFunction,2,0,"start","stop");
    context->addFunction("drawsymbols",DrawSymbolsFunction,3,0,"pos","symbol","length");
    context->addFunction("drawlines",DrawLinesFunction,1,0,"points");
    context->addFunction("drawpoint",DrawPointFunction,1,0,"pos");
    context->addFunction("drawcircle",DrawCircleFunction,2,0,"pos","radius");
    context->addFunction("drawrectangle",DrawRectangleFunction,1,0,"rect");
    context->addFunction("fillrectangle",FillRectangleFunction,1,0,"rect");
    context->addFunction("fillquad",FillQuadFunction,4,0,"x1","x2","x3","x4");
    context->addFunction("drawquad",DrawQuadFunction,4,0,"x1","x2","x3","x4");
    context->addFunction("pushclip",PushClipFunction,1,0,"rect");
    context->addFunction("popclip",PopClipFunction,0,1);
    context->addFunction("blitimage",BlitImageFunction,1,0,"imagedata","pos");
    context->addFunction("trimprint",TrimPrintFunction,2,1,"val","scientific");
    context->addSpecialFunction("cfigure",CFigureFunction,-1,0);
  }
}
