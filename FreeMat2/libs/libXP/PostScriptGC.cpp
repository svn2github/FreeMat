#include "PostScriptGC.hpp"
#include "helv_table.h"

Point2D PostScriptGC::ToPS(Point2D p) {
  return Point2D(p.x,m_height-1-p.y);
}

void PostScriptGC::DoRect(Rect2D p) {
  fprintf(fp,"%d %d moveto\n",p.x1,m_height-1-p.y1);
  fprintf(fp,"%d %d lineto\n",p.x1+p.width-1,m_height-1-p.y1);
  fprintf(fp,"%d %d lineto\n",p.x1+p.width-1,m_height-1-(p.y1+p.height-1));
  fprintf(fp,"%d %d lineto\n",p.x1,m_height-1-(p.y1+p.height-1));
  fprintf(fp,"%d %d lineto\n",p.x1,m_height-1-p.y1);
}

void PostScriptGC::RefreshGS() {
  SetForeGroundColor(m_fg);
  SetFont(m_fontsize);
  SetLineStyle(m_lst);
}

PostScriptGC::PostScriptGC(std::string filename, int width, int height) {
  InitializeFontTable();
  m_width = width;
  m_height = height;
  m_bg = Color(255,255,25);
  m_fg = Color(0,0,0);
  m_fontsize = 12;
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
  fprintf(fp,"%%%%EOF\n");
  fclose(fp);
}

Point2D PostScriptGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

int GetKerningData(int glyph1, int glyph2) {
  int i;
  i = 0;
  while (i<KERNCOUNT) {
    if ((kernglyph1[i] == glyph1) && (kernglyph2[i] == glyph2))
      return kerndelta[i];
    i++;
  }
  return 0;
}

Point2D PostScriptGC::GetTextExtent(std::string text) {
  int penx;
  int stringheight, charheight;
  int len, i, g1, g2;
  int scalefact;
  
  penx = 0;
  len = text.size();
  stringheight = 0;
  scalefact = 64*12;
  for (i=0;i<len-1;i++) {
    g1 = text[i];
    g2 = text[i+1];
    penx += (glyph_x[g1] + GetKerningData(g1,g2));
  }
  g1 = text[len-1];
  penx += glyph_x[g1];
  return Point2D(penx*m_fontsize/scalefact,m_fontsize);
}

void PostScriptGC::DrawTextString(std::string text, Point2D pos, OrientationType orient) {
  pos = ToPS(pos);
  fprintf(fp,"newpath\n%d %d moveto\n(%s)\n",pos.x,pos.y,text.c_str());
  fprintf(fp,"gsave\n");
  switch(orient) {
  case ORIENT_0:
    break;
  case ORIENT_90:
    fprintf(fp,"90 rotate\n");
    break;
  case ORIENT_180:
    fprintf(fp,"180 rotate\n");
    break;
  case ORIENT_270:
    fprintf(fp,"270 rotate\n");
    break;
  }
  fprintf(fp,"show\n");
  fprintf(fp,"grestore\n");
}

void PostScriptGC::SetFont(int fontsize) {
  fprintf(fp,"/Helvetica findfont\n%d scalefont\nsetfont\n",fontsize);
  m_fontsize = fontsize;
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
  switch(m_lst) {
  case LINE_SOLID:
    fprintf(fp,"[] 0 setdash\n");
    break;    
  case LINE_DASHED:
    fprintf(fp,"[4 4] 0 setdash\n");
    break;
  case LINE_DOTTED:
    fprintf(fp,"[2 2] 0 setdash\n");
    break;
  case LINE_DASH_DOT:
    fprintf(fp,"[6 4 2 4] 0 setdash\n");
    break;
  }
  return tmp;
}

void PostScriptGC::DrawLine(Point2D pos1, Point2D pos2) {
  if (m_lst != LINE_NONE) {
    pos1 = ToPS(pos1);
    pos2 = ToPS(pos2);
    fprintf(fp,"newpath\n%d %d moveto\n%d %d lineto\n1 setlinewidth\nstroke\n",
	    pos1.x,pos1.y,pos2.x,pos2.y);
  }
}

void PostScriptGC::DrawPoint(Point2D pos) {
  if (m_lst != LINE_NONE) {
    DrawCircle(ToPS(pos),1);
  }
}

void PostScriptGC::DrawCircle(Point2D pos, int radius) {
  if (m_lst != LINE_NONE) {
    pos = ToPS(pos);
    fprintf(fp,"newpath\n");
    fprintf(fp,"%d %d %d 0 360 arc\n",pos.x,pos.y,radius);
    fprintf(fp,"closepath\n1 setlinewidth\nstroke\n");
  }
}

void PostScriptGC::DrawRectangle(Rect2D rect) { 
  if (m_lst != LINE_NONE) {
    fprintf(fp,"newpath\n");
    DoRect(rect);
    fprintf(fp,"closepath\n1 setlinewidth\nstroke\n");
  }
}

void PostScriptGC::FillRectangle(Rect2D rect) {
  fprintf(fp,"newpath\n");
  DoRect(rect);
  fprintf(fp,"closepath\n1 setlinewidth\nfill\n");
}

void PostScriptGC::DrawLines(std::vector<Point2D> pts) {
  if (m_lst != LINE_NONE) {
    if (pts.size() >= 2) {
      fprintf(fp,"newpath\n");
      Point2D t(ToPS(pts[0]));
      fprintf(fp,"%d %d moveto\n",t.x,t.y);
      for (int i=1;i<pts.size();i++) {
	Point2D t(ToPS(pts[i]));
	fprintf(fp,"%d %d lineto\n",t.x,t.y);
      }
      fprintf(fp,"1 setlinewidth\nstroke\n");
    }
  }
}

void PostScriptGC::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  fprintf(fp,"gsave\n");
  fprintf(fp,"newpath\n");
  DoRect(rect);
  fprintf(fp,"closepath\nclip\n");
}

Rect2D PostScriptGC::PopClippingRegion() {
  Rect2D ret(clips.back());
  clips.pop_back();
  fprintf(fp,"grestore\n");
  RefreshGS();
  return ret;
}

void PostScriptGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  int linelen;
  int outcount;
  int remaining;
  int n;
  fprintf(fp,"/picstr %d string def\n",3*width);
  fprintf(fp,"gsave\n");
  fprintf(fp,"%d %d translate\n",x0,y0);
  fprintf(fp,"%d %d scale\n",width,height);
  fprintf(fp,"%d %d 8 [%d 0 0 -%d 0 %d]\n",width,height,width,height,height);
  fprintf(fp,"{currentfile picstr readhexstring pop} \n");
  fprintf(fp,"false 3 colorimage\n");
  outcount = 0;
  remaining = width*height;
  while (remaining>0) {
    linelen = 10;
    if (linelen>remaining)
      linelen = remaining;
    remaining -= linelen;
    for (n=outcount;n<outcount+linelen;n++) 
      fprintf(fp,"%02x%02x%02x",data[3*n],data[3*n+1],data[3*n+2]);
    outcount += linelen;
    fprintf(fp,"\n");
  }
  fprintf(fp,"grestore\n");  
}
