#include "PostScriptGC.hpp"

PostScriptGC::PostScriptGC(std::string filename, int width, int height) {
  m_width = width;
  m_height = height;
  m_bg = Color(255,255,25);
  m_fg = Color(0,0,0);
  fp = fopen(filename.c_str(),"w");
  if (!fp) {
    fprintf(stderr,"Error: unable to open file %s for writing\n",filename.c_str());
    exit(1);
  }  
  // Emit standard header stuff
  fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(fp,"%%%%Creator: FreeMat\n");
  fprintf(fp,"%%%%Title: %s\n",filename.c_str());
  fprintf(fp,"%%%%DocumentData: Clean7Bit\n");
  fprintf(fp,"%%%%Origin 0 0\n");
  fprintf(fp,"%%%%BoundingBox: 0 0 %d %d\n",width,height);
  fprintf(fp,"%%%%LanguageLevel: 2\n");
  fprintf(fp,"%%%%Pages: 1\n");
  fprintf(fp,"%%%%Page: 1 1\n");
}

PostScriptGC::~PostScriptGC() {
  fprintf(fp,"%%EOF\n");
  fclose(fp);
}

Point2D PostScriptGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

Point2D PostScriptGC::GetTextExtent(std::string label) {
  return Point2D(0,0);
}

void PostScriptGC::DrawText(std::string text, Point2D pos) {
  fprintf(fp,"newpath\n%d %d moveto\n(%s) show\n",pos.x,pos.y,text.c_str());
}

void PostScriptGC::DrawRotatedText(std::string text, Point2D pos, OrientationType orient) {
}

void PostScriptGC::SetFont(std::string fontname, int fontsize) {
  if (fontname == "swiss") {
    fprintf(fp,"/Helvetica findfont\n%d scalefont\nsetfont\n",fontsize);
  } else {
    fprintf(stderr,"Unrecognized font name %s\n",fontname.c_str());
    exit(1);
  }
}

Color PostScriptGC::SetBackGroundColor(Color col) {
  Color tmp;
  tmp = m_bg;
  m_bg = col;
  return tmp;
}

Color PostScriptGC::SetForeGroundColor(Color col) {
  Color tmp;
  tmp = m_fg;
  m_fg = col;
  fprintf(fp,"%f %f %f setrgbcolor\n",col.red/255.0,col.green/255.0,col.blue/255.0);
  return tmp;
}

LineStyleType PostScriptGC::SetLineStyle(LineStyleType style) {
  LineStyleType tmp;
  tmp = m_lst;
  m_lst = style;
  return tmp;
}

void PostScriptGC::DrawLine(Point2D pos1, Point2D pos2) {
  fprintf(fp,"newpath\n%d %d moveto\n%d %d lineto\n1 setlinewidth\nstroke\n",
	  pos1.x,pos1.y,pos2.x,pos2.y);
}

void PostScriptGC::DrawPoint(Point2D pos) {
  DrawCircle(pos,1);
}

void PostScriptGC::DrawCircle(Point2D pos, int radius) {
  fprintf(fp,"newpath\n");
  fprintf(fp,"%d %d %d 0 360 arc\n",pos.x,pos.y,radius);
  fprintf(fp,"closepath\n1 setlinewidth\nstroke\n");
}

void PostScriptGC::DrawRectangle(Rect2D rect) {
  fprintf(fp,"newpath\n%d %d moveto\n",rect.x1,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1);
  fprintf(fp,"closepath\n1 setlinewidth\nstroke\n");
}

void PostScriptGC::FillRectangle(Rect2D rect) {
  fprintf(fp,"newpath\n%d %d moveto\n",rect.x1,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1);
  fprintf(fp,"closepath\n1 setlinewidth\nfill\n");
}

void PostScriptGC::DrawLines(std::vector<Point2D> pts) {
  if (pts.size() >= 2) {
    fprintf(fp,"newpath\n");
    fprintf(fp,"%d %d moveto\n",pts[0].x,pts[0].y);
    for (int i=1;i<pts.size();i++) 
      fprintf(fp,"%d %d lineto\n",pts[i].x,pts[i].y);
    fprintf(fp,"1 setlinewidth\nstroke\n");
  }
}

void PostScriptGC::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  fprintf(fp,"gsave\n");
  fprintf(fp,"newpath\n%d %d moveto\n",rect.x1,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1);
  fprintf(fp,"%d %d lineto\n",rect.x1+rect.width,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1+rect.height);
  fprintf(fp,"%d %d lineto\n",rect.x1,rect.y1);
  fprintf(fp,"closepath\nclip");
}

Rect2D PostScriptGC::PopClippingRegion() {
  Rect2D ret(clips.back());
  clips.pop_back();
  fprintf(fp,"grestore\n");
}

void PostScriptGC::BlitGrayscaleImage(Point2D pos, GrayscaleImage &img) {
}

void PostScriptGC::BlitRGBImage(Point2D pos, RGBImage &img) {
}
