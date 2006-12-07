/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "QTRenderEngine.hpp"
#include <math.h>
#include <algorithm>

bool operator<(const quad3d& a, const quad3d& b) {
  return (a.meanz < b.meanz);
}

void cross(double ux, double uy, double uz, 
	   double vx, double vy, double vz,
	   double &sx, double &sy, double &sz) {
  sx = uy*vz-uz*vy;
  sy = uz*vx-ux*vz;
  sz = ux*vy-uy*vx;
}

QTRenderEngine::QTRenderEngine(QPainter *painter, double x1, double y1,
			       double width, double height) {
  pnt = painter;
  pnt->setRenderHint(QPainter::TextAntialiasing);
  m_x1 = x1; m_y1 = y1; m_width = width; m_height = height;
  inDirect = false;
  pnt->setPen(QColor(0,0,0));
  pnt->setBrush(QColor(0,0,0));
  debugFlag = false;
}

QTRenderEngine::~QTRenderEngine() {
}

void QTRenderEngine::clear(std::vector<double> col) {
  pnt->save();
  pnt->setPen(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->setBrush(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->drawRect((int)m_x1,(int)m_y1,(int)m_width,(int)m_height);
  pnt->restore();
}

QPointF QTRenderEngine::Map(double x, double y, double z) {
  double a, b;
  toPixels(x,y,z,a,b);
  b = m_height-1-b;
  return QPointF(rint(a),rint(b));
}

QVector<QPointF> QTRenderEngine::Map(std::vector<double> xs, 
				     std::vector<double> ys, 
				     std::vector<double> zs) {
  QVector<QPointF> retval;
  for (int i=0;i<xs.size();i++)
    retval.push_back(Map(xs[i],ys[i],zs[i]));
  return retval;
}

std::vector<quad3d> QTRenderEngine::MapQuadsFacesOnly(std::vector<std::vector<cpoint> > &faces) {
  std::vector<quad3d> retval;
  for (int i=0;i<faces.size();i++) {
    std::vector<cpoint> qlist(faces[i]);
    for (int j=2;j<qlist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(qlist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = cpt.a;
	qx.er = cpt.r;
	qx.eg = cpt.g;
	qx.eb = cpt.b;
	qx.ea = 0;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}

std::vector<quad3d> QTRenderEngine::MapQuadsEdgesOnly(std::vector<std::vector<cpoint> > &edges) {
  std::vector<quad3d> retval;
  for (int i=0;i<edges.size();i++) {
    std::vector<cpoint> elist(edges[i]);
    for (int j=2;j<elist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(elist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = 0;
	qx.er = cpt.r;
	qx.eg = cpt.g;
	qx.eb = cpt.b;
	qx.ea = cpt.a;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}


std::vector<quad3d> QTRenderEngine::MapQuads(std::vector<std::vector<cpoint> > &faces,
					     std::vector<std::vector<cpoint> > &edges) {
  std::vector<quad3d> retval;
  if (edges.size() == 0) return MapQuadsFacesOnly(faces);
  if (faces.size() == 0) return MapQuadsEdgesOnly(edges);
  for (int i=0;i<faces.size();i++) {
    std::vector<cpoint> qlist(faces[i]);
    std::vector<cpoint> elist(edges[i]);
    for (int j=2;j<qlist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(qlist[j-2+k]);
	cpoint ept(elist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = cpt.a;
	qx.er = ept.r;
	qx.eg = ept.g;
	qx.eb = ept.b;
	qx.ea = ept.a;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}
  
void QTRenderEngine::toPixels(double x, double y, double z, int &a, int &b) {
  double aval, bval;
  bool clipped;
  toPixels(x,y,z,aval,bval,clipped);
  a = (int) aval;
  b = (int) bval;
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b) {
  bool clipped;
  toPixels(x,y,z,a,b,clipped);
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b, bool &clipped) {
  double xprime, yprime, zprime, wprime;
  double xclip, yclip, zclip, wclip;
  wprime = 1;
  mapPoint(x,y,z,xprime,yprime,zprime);
  xclip = proj[0]*xprime + proj[4]*yprime + proj[8]*zprime + proj[12];
  yclip = proj[1]*xprime + proj[5]*yprime + proj[9]*zprime + proj[13];
  zclip = proj[2]*xprime + proj[6]*yprime + proj[10]*zprime + proj[14];
  wclip = proj[3]*xprime + proj[7]*yprime + proj[11]*zprime + proj[15];
  xclip /= wclip;
  yclip /= wclip;
  zclip /= wclip;
  clipped = ((xclip < -1) | (xclip > 1) | (yclip < -1) | (yclip > 1) | (zclip < -1) | (zclip > 1));
  a = viewp[0] + (1+xclip)/2.0*viewp[2];
  b = viewp[1] + (1+yclip)/2.0*viewp[3];
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b, double &c, bool &clipped) {
  double xprime, yprime, zprime, wprime;
  double xclip, yclip, zclip, wclip;
  wprime = 1;
  mapPoint(x,y,z,xprime,yprime,zprime);
  xclip = proj[0]*xprime + proj[4]*yprime + proj[8]*zprime + proj[12];
  yclip = proj[1]*xprime + proj[5]*yprime + proj[9]*zprime + proj[13];
  zclip = proj[2]*xprime + proj[6]*yprime + proj[10]*zprime + proj[14];
  wclip = proj[3]*xprime + proj[7]*yprime + proj[11]*zprime + proj[15];
  xclip /= wclip;
  yclip /= wclip;
  zclip /= wclip;
  clipped = ((xclip < -1) | (xclip > 1) | (yclip < -1) | (yclip > 1) | (zclip < -1) | (zclip > 1));
  a = viewp[0] + (1+xclip)/2.0*viewp[2];
  b = viewp[1] + (1+yclip)/2.0*viewp[3];
  c = zclip;
}

void QTRenderEngine::lookAt(double eyex, double eyey, double eyez,
			    double centerx, double centery, double centerz,
			    double upx, double upy, double upz) {
  double fx, fy, fz;
  fx = centerx - eyex;
  fy = centery - eyey;
  fz = centerz - eyez;
  double fnorm = sqrt(fx*fx+fy*fy+fz*fz);
  fx /= fnorm;
  fy /= fnorm;
  fz /= fnorm;
  double upnorm = sqrt(upx*upx+upy*upy+upz*upz);
  upx /= upnorm;
  upy /= upnorm;
  upz /= upnorm;
  double sx, sy, sz;
  cross(fx,fy,fz,upx,upy,upz,sx,sy,sz);
  double ux, uy, uz;
  cross(sx,sy,sz,fx,fy,fz,ux,uy,uz);
  model[0] = sx; model[4] = sy; model[8] = sz; model[12] = -eyex;
  model[1] = ux; model[5] = uy; model[9] = uz; model[13] = -eyey;
  model[2] =-fx; model[6] =-fy; model[10]=-fz; model[14]= -eyez;
  model[3] = 0;  model[7] = 0;  model[11] = 0; model[15] = 1;
}

void QTRenderEngine::scale(double sx, double sy, double sz) {
  model[0] *= sx; model[4] *= sy; model[8] *= sz; 
  model[1] *= sx; model[5] *= sy; model[9] *= sz; 
  model[2] *= sx; model[6] *= sy; model[10] *= sz;
  model[3] *= sx; model[7] *= sy; model[11] *= sz;
}

void QTRenderEngine::mapPoint(double x, double y, double z,
			      double &a, double &b, double &c) {
  a = model[0]*x + model[4]*y + model[8]*z + model[12];
  b = model[1]*x + model[5]*y + model[9]*z + model[13];
  c = model[2]*x + model[6]*y + model[10]*z + model[14];
}
  
void QTRenderEngine::project(double left, double right, 
			     double bottom, double top,
			     double near, double far) {
  double tx, ty, tz;
  tx = -(right+left)/(right-left);
  ty = -(top+bottom)/(top-bottom);
  tz = -(far+near)/(far-near);
  for (int i=0;i<16;i++)
    proj[i] = 0;
  proj[0] = 2/(right-left);
  proj[5] = 2/(top-bottom);
  proj[10] = -2/(far-near);
  proj[12] = tx;
  proj[13] = ty;
  proj[14] = tz;
  //    proj[15] = -1;
  proj[15] = 1; // To match GL
//   if (left != 0)
//     qDebug("Project %f %f %f %f %f %f",left,right,bottom,top,near,far);
}
  
void QTRenderEngine::viewport(double x0, double y0, double width, double height) {
  viewp[0] = (int)x0; viewp[1] = (int)y0; viewp[2] = (int)width; viewp[3] = (int)height;
  pnt->setClipRect((int)x0,(int)(m_height-(y0+height)),(int)width,(int)height);
}

void QTRenderEngine::quad(double x1, double y1, double z1,
			  double x2, double y2, double z2,
			  double x3, double y3, double z3,
			  double x4, double y4, double z4) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  poly.push_back(Map(x4,y4,z4));
  pnt->drawPolygon(poly);
}

void QTRenderEngine::quadline(double x1, double y1, double z1,
			      double x2, double y2, double z2,
			      double x3, double y3, double z3,
			      double x4, double y4, double z4) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  poly.push_back(Map(x4,y4,z4));
  poly.push_back(Map(x1,y1,z1));
  pnt->drawPolyline(poly);
}

void QTRenderEngine::tri(double x1, double y1, double z1,
			 double x2, double y2, double z2,
			 double x3, double y3, double z3) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  pnt->drawPolygon(poly);
}

void QTRenderEngine::triLine(double x1, double y1, double z1,
			     double x2, double y2, double z2,
			     double x3, double y3, double z3) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  pnt->drawPolyline(poly);
}

void QTRenderEngine::color(std::vector<double> col) {
  QPen pen(pnt->pen());
  QBrush brush(pnt->brush());
  pen.setColor(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  brush.setColor(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->setPen(pen);
  pnt->setBrush(brush);
}

void QTRenderEngine::setLineStyle(std::string style) {
  QPen pen(pnt->pen());
  if (style == "-") 
    pen.setStyle(Qt::SolidLine);
  else if (style == "--") 
    pen.setStyle(Qt::DashLine);
  else if (style == ":") 
    pen.setStyle(Qt::DotLine);
  else if (style == "-.")
    pen.setStyle(Qt::DashDotLine);
  else if (style == "none")
    pen.setStyle(Qt::NoPen);
  pnt->setPen(pen);
}

void QTRenderEngine::lineWidth(double n) {
  QPen pen(pnt->pen());
  pen.setWidthF(n);
  pnt->setPen(pen);
}

void QTRenderEngine::line(double x1, double y1, double z1,
			  double x2, double y2, double z2) {
  pnt->drawLine(Map(x1,y1,z1),Map(x2,y2,z2));
}

void QTRenderEngine::line(double x1, double y1,
			  double x2, double y2) {
  pnt->drawLine(Map(x1,y1,0),Map(x2,y2,0));    
}

void QTRenderEngine::lineSeries(std::vector<double> xs, 
				std::vector<double> ys,
				std::vector<double> zs) {
  if (xs.size() < 2) return;
  pnt->drawPolyline(Map(xs,ys,zs));
}
  
void QTRenderEngine::debug() {
  debugFlag = !debugFlag;
//   qDebug("Projection diagonal: %f %f %f %f",
// 	 proj[0],proj[5],proj[10],proj[15]);
  return;
//   qDebug("QT Modelview matrix (before setupdirect)");
//   qDebug("%f %f %f %f",model[0],model[4],model[8],model[12]);
//   qDebug("%f %f %f %f",model[1],model[5],model[9],model[13]);
//   qDebug("%f %f %f %f",model[2],model[6],model[10],model[14]);
//   qDebug("%f %f %f %f",model[3],model[7],model[11],model[15]);
//   qDebug("QT Projection matrix (before setupdirect)");
//   qDebug("%f %f %f %f",proj[0],proj[4],proj[8],proj[12]);
//   qDebug("%f %f %f %f",proj[1],proj[5],proj[9],proj[13]);
//   qDebug("%f %f %f %f",proj[2],proj[6],proj[10],proj[14]);
//   qDebug("%f %f %f %f",proj[3],proj[7],proj[11],proj[15]);
//   qDebug("QT Viewport (before setupdirect)");
//   qDebug("%d %d %d %d",viewp[0],viewp[1],viewp[2],viewp[3]);  
}

void QTRenderEngine::setupDirectDraw() {
//   if (inDirect)
//     qDebug("DirectDraw is not reentrant!!!");
  // save the relevant matrices
  for (int i=0;i<16;i++) {
    save_model[i] = model[i];
    model[i] = 0;
    save_proj[i] = proj[i];
  }
  for (int i=0;i<4;i++)
    save_viewp[i] = viewp[i];
  model[0] = 1;
  model[5] = 1;
  model[10] = 1;
  model[15] = 1;
  viewport(m_x1,m_y1,m_width,m_height);
  project(m_x1,m_x1+m_width,m_y1,m_y1+m_height,-1,1);
  inDirect = true;
}

void QTRenderEngine::releaseDirectDraw() {
//   if (!inDirect)
//     qDebug("releaseDirectDraw called unmatched!!!");
  for (int i=0;i<16;i++) {
    model[i] = save_model[i];
    proj[i] = save_proj[i];
  }
  for (int i=0;i<4;i++)
    viewp[i] = save_viewp[i];
  inDirect = false;
}

void QTRenderEngine::getProjectionMatrix(double aproj[16]) {
  for (int i=0;i<16;i++)
    aproj[i] = proj[16];
}

void QTRenderEngine::getModelviewMatrix(double amodel[16]) {
  for (int i=0;i<16;i++)
    amodel[i] = model[i];
}

void QTRenderEngine::getViewport(int aviewp[4]) {
  for (int i=0;i<4;i++)
    aviewp[i] = viewp[i];    
}

void QTRenderEngine::putText(double x, double y, std::string txt, 
			     std::vector<double> color, 
			     AlignmentFlag xflag, AlignmentFlag yflag,
			     QFont fnt, double rotation) {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(txt.c_str()));
  int x0 = sze.left();
  int y0 = sze.bottom();
  int width = sze.width();
  int height = sze.height();
  // Adjust the raster position based on the alignment offsets
  double xdelta, ydelta;
  xdelta = 0;
  ydelta = 0;
  if (xflag == Mean)
    xdelta = -width/2.0;
  if (xflag == Max)
    xdelta = -width;
  if (yflag == Mean)
    ydelta = -height/2.0;
  if (yflag == Max)
    ydelta = -height;
  // I don't understand this...
#ifdef WIN32 
  ydelta += fm.descent();
#endif
#ifdef __APPLE__
  ydelta += fm.descent();
#endif
  double costhet, sinthet;
  costhet = cos(rotation*M_PI/180.0);
  sinthet = sin(rotation*M_PI/180.0);
  double xpos, ypos;
  xpos = x+xdelta*costhet-ydelta*sinthet;
  ypos = y+xdelta*sinthet+ydelta*costhet;
  QPointF pos(Map(xpos,ypos,0));
  QPen pen(pnt->pen());
  pnt->setPen(QColor((int)(color[0]*255),(int)(color[1]*255),(int)(color[2]*255)));
  pnt->setFont(fnt);
  pnt->save();
  pnt->translate(pos);
  pnt->rotate(-rotation);
  pnt->drawText(0,0,txt.c_str());
  pnt->restore();
  pnt->setPen(pen);
}

void QTRenderEngine::measureText(std::string txt, QFont fnt, AlignmentFlag xflag, 
				 AlignmentFlag yflag,int &width, int &height,
				 int &xoffset, int &yoffset) {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(txt.c_str()));
  width = sze.width();
  height = sze.height();
  yoffset = -height;
  xoffset = 0;
  if (xflag == Mean)
    xoffset -= width/2;
  else if (xflag == Max)
    xoffset -= width;
  if (yflag == Mean)
    yoffset += height/2;
  else if (yflag == Min)
    yoffset += height;
}

void QTRenderEngine::depth(bool) {
}

void QTRenderEngine::rect(double x1, double y1, double x2, double y2) {
  quadline(x1,y1,0,
	   x2,y1,0,
	   x2,y2,0,
	   x1,y2,0);
}
  
void QTRenderEngine::rectFill(double x1, double y1, double x2, double y2) {
  quad(x1,y1,0,
       x2,y1,0,
       x2,y2,0,
       x1,y2,0);
}

void QTRenderEngine::circle(double x1, double y1, double radius) {
  QPointF uleft(Map(x1-radius,y1+radius,0));
  QPointF lright(Map(x1+radius,y1-radius,0));
  QRectF rect;
  rect.setBottomRight(lright);
  rect.setTopLeft(uleft);
  QBrush brsh(pnt->brush());
  pnt->setBrush(Qt::NoBrush);
  pnt->drawEllipse(rect);
  pnt->setBrush(brsh);
}

void QTRenderEngine::circleFill(double x1, double y1, double radius) {
  QPointF uleft(Map(x1-radius,y1+radius,0));
  QPointF lright(Map(x1+radius,y1-radius,0));
  QRectF rect;
  rect.setBottomRight(lright);
  rect.setTopLeft(uleft);
  QPen pen(pnt->pen());
  pnt->setPen(Qt::NoPen);
  pnt->drawEllipse(rect);
  pnt->setPen(pen);
}

void QTRenderEngine::drawImage(double x1, double y1, double x2, double y2,
			       QImage pic) {
  QPointF pt(Map(x1,y1,0));
  pt.setY(pt.y()-pic.height());
  pnt->drawImage(pt,pic);
}
  

void QTRenderEngine::quadStrips(std::vector<std::vector<cpoint> > faces, bool flatfaces,
				std::vector<std::vector<cpoint> > edges, bool flatedges) {
  std::vector<quad3d> mapqds(MapQuads(faces,edges));
  std::sort(mapqds.begin(),mapqds.end());
  for (int i=0;i<mapqds.size();i++) {
    QPolygonF poly;
    poly.push_back(QPointF(mapqds[i].pts[0].x,mapqds[i].pts[0].y));
    poly.push_back(QPointF(mapqds[i].pts[1].x,mapqds[i].pts[1].y));
    poly.push_back(QPointF(mapqds[i].pts[3].x,mapqds[i].pts[3].y));
    poly.push_back(QPointF(mapqds[i].pts[2].x,mapqds[i].pts[2].y));
    pnt->setBrush(QColor((int)(mapqds[i].r*255),
			 (int)(mapqds[i].g*255),
			 (int)(mapqds[i].b*255),
			 (int)(mapqds[i].a*255)));
    pnt->setPen(QColor((int)(mapqds[i].er*255),(int)(mapqds[i].eg*255),
		       (int)(mapqds[i].eb*255),(int)(mapqds[i].ea*255)));
    pnt->drawPolygon(poly);
  }
}
//};
