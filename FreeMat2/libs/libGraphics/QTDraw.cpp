#include "QTDraw.hpp"

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
  
  void QTDraw::OnDraw(GraphicsContext &gc) {
    the_gc = &gc;
    ArrayVector tocall;
    m_fdef->updateCode();
    bool eflag(m_tree->GUIEventFlag());
    m_tree->GUIEventFlag(false);
    ArrayVector cval = m_fdef->evaluateFunction(m_tree,tocall,0);
    m_tree->GUIEventFlag(eflag);
    the_gc = NULL;
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
    p->Show();
    return ArrayVector();
  }

  void LoadQTDraw(Context* context) {
    context->addFunction("getcanvassize",GetCanvasSizeFunction,0,1);
    context->addFunction("gettextextent",GetTextExtentFunction,1,1,"string");
    context->addFunction("drawtextstringaligned",DrawTextStringAlignedFunction,5,0,
			 "string","pos","xalign","yalign","orient");
    context->addFunction("drawtextstring",DrawTextStringFunction,3,0,"string","pos","orient");
    context->addFunction("setfontsize",SetFontFunction,1,0,"size");
    context->addFunction("setforegroundcolor",SetForeGroundColorFunction,1,1,"color");
    context->addFunction("setlinestyle",SetLineStyleFunction,1,1,"linestyle");
    context->addFunction("drawline",DrawLineFunction,2,0,"start","stop");
    context->addFunction("drawpoint",DrawPointFunction,1,0,"pos");
    context->addFunction("drawcircle",DrawCircleFunction,2,0,"pos","radius");
    context->addFunction("drawrectangle",DrawRectangleFunction,1,0,"rect");
    context->addFunction("fillrectangle",FillRectangleFunction,1,0,"rect");
    context->addFunction("fillquad",FillQuadFunction,4,0,"x1","x2","x3","x4");
    context->addFunction("drawquad",DrawQuadFunction,4,0,"x1","x2","x3","x4");
    context->addFunction("pushclip",PushClipFunction,1,0,"rect");
    context->addFunction("popclip",PopClipFunction,0,1);
    context->addFunction("blitimage",BlitImageFunction,1,0,"imagedata","pos");
    context->addSpecialFunction("cfigure",CFigureFunction,-1,0);
  }
}
