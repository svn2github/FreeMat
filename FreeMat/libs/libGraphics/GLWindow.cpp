/*
 * Copyright (c) 2009 Samit Basu
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
#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "GLWindow.hpp"
#include "Array.hpp"
#include "HandleList.hpp"
#include "trackball.h"

class GLNode {
public:
  QString material;
  QVector<double> points;
};

class GLLines {
public:
  QVector<double> points;
  double color[3];
};

class GLClump {
public:
  QVector<double> points;
};

class GLPart {
public:
  QString name;
  Array transform;
};

class GLAssembly {
public:
  QVector<GLPart> parts;
};

class GLMaterial {
public:
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
};

QMap<QString,GLMaterial> material_dictionary;
QMap<QString,GLAssembly> assemblymap;
QMap<QString,GLNode> nodemap;
QMap<QString,GLClump> clumpmap;
QMap<QString,GLLines> linesmap;


GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
}

GLWidget::~GLWidget()  {
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(400, 400);
}

GLfloat LightAmbient[] = {.2,.2,.2,1};
GLfloat LightDiffuse[] = {1,1,1,1};
GLfloat LightSpecular[] = {1,1,1,1};
GLfloat LightPosition[] = {2,2,0,1};

void GLWidget::initializeGL() {
  qglClearColor(Qt::lightGray);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  //  glEnable(GL_CULL_FACE);
  trackball(curquat, 0, 0, 0, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  glEnable(GL_LIGHT1);
}

void getNormal(double p1[3], double p2[3], double p3[3], double pn[3]) {
  double a[3];
  double b[3];
  for (int i=0;i<3;i++) {
    a[i] = p1[i] - p2[i];
    b[i] = p3[i] - p2[i];
  }
  pn[0] = -(a[1] * b[2] - a[2] * b[1]);
  pn[1] = -(a[2] * b[0] - a[0] * b[2]);
  pn[2] = -(a[0] * b[1] - a[1] * b[0]);
  double len = sqrt(pn[0]*pn[0] + pn[1]*pn[1] + pn[2]*pn[2]);
  if (len) {
    pn[0] /= len;
    pn[1] /= len;
    pn[2] /= len;
  }
}

void GLWidget::paintAssembly(QString aname) {
  //  qDebug() << "drawing assembly " << aname;
  // Render this assembly
  GLAssembly &asy(assemblymap[aname]);
  for (int i=0;i<asy.parts.size();i++) {
    GLfloat m[4][4];
    const GLPart &t = asy.parts[i];
    Array transform = t.transform;
    for (int j=0;j<4;j++) {
      for (int k=0;k<4;k++) {
	m[j][k] = float(transform.get(NTuple(j+1,k+1)).asDouble());
      }
    }
    glPushMatrix();
    glMultMatrixf(&m[0][0]);
    if (nodemap.contains(t.name))
      paintNode(t.name);
    else if (clumpmap.contains(t.name))
      paintClump(t.name);
    else if (linesmap.contains(t.name))
      paintLines(t.name);
    else
      paintAssembly(t.name);
    glPopMatrix();
  }
}

void GLWidget::paintLines(QString aname) {
  GLLines &node(linesmap[aname]);
  int n=0;
  float color[4];
  color[0] = node.color[0];
  color[1] = node.color[1];
  color[2] = node.color[2];
  color[3] = 1.0f;
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
  while (n<node.points.size()) {
    int pcount = int(node.points[n++]);
    glBegin(GL_LINE_STRIP);
    for (int i=0;i<pcount;i++) {
      double x = node.points[n++];
      double y = node.points[n++];
      double z = node.points[n++];
      glVertex3d(x,y,z);
    }
    glEnd();
  }
}

void GLWidget::paintClump(QString aname) {
  GLClump &node(clumpmap[aname]);
  int n=0;
  double p1[3];
  double p2[3];
  double p3[3];
  double pn[3];
  float color[4];
  while (n<node.points.size()) {
    color[0] = node.points[n++];
    color[1] = node.points[n++];
    color[2] = node.points[n++];
    color[3] = 1;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    int pcount = int(node.points[n++]);
    glBegin(GL_TRIANGLES);
    for (int i=0;i<pcount;i+=9) {
      p1[0] = node.points[n++]; p1[1] = node.points[n++]; p1[2] = node.points[n++];
      p2[0] = node.points[n++]; p2[1] = node.points[n++]; p2[2] = node.points[n++];
      p3[0] = node.points[n++]; p3[1] = node.points[n++]; p3[2] = node.points[n++];
      getNormal(p1,p2,p3,pn);
      glNormal3d(pn[0],pn[1],pn[2]);
      glVertex3d(p1[0],p1[1],p1[2]);
      glVertex3d(p2[0],p2[1],p2[2]);
      glVertex3d(p3[0],p3[1],p3[2]);
    }
    glEnd();
  }
}

void GLWidget::paintNode(QString aname) {
  GLNode &node(nodemap[aname]);
  double p1[3];
  double p2[3];
  double p3[3];
  double pn[3];
  
  glBegin(GL_TRIANGLES);
  GLMaterial mat = material_dictionary.value(node.material);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
  glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess*128.0);
  for (int i=0;i<node.points.size();i+=9) {
    p1[0] = node.points[i]; p1[1] = node.points[i+1]; p1[2] = node.points[i+2];
    p2[0] = node.points[i+3]; p2[1] = node.points[i+4]; p2[2] = node.points[i+5];
    p3[0] = node.points[i+6]; p3[1] = node.points[i+7]; p3[2] = node.points[i+8];
    getNormal(p1,p2,p3,pn);
    glNormal3d(pn[0],pn[1],pn[2]);
    glVertex3d(p1[0],p1[1],p1[2]);
    glVertex3d(p2[0],p2[1],p2[2]);
    glVertex3d(p3[0],p3[1],p3[2]);
  }
  glEnd();
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  GLfloat m[4][4];
  build_rotmatrix(m,curquat);
  glTranslated(0.0, 0.0, -10.0);
  glMultMatrixf(&m[0][0]);
  glScaled(1.0/scale,1.0/scale,1.0/scale);
  paintAssembly(name);
}

void GLWidget::resizeGL(int width, int height) {
  W = width;
  H = height;
  int side = qMin(width, height);
  glViewport((width - side) / 2, (height - side) / 2, side, side);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
  glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  trackball(lastquat,
	    (2.0*lastPos.x() - W)/W,
	    (2.0*lastPos.y() - H)/H,
	    (2.0*event->x() - W)/W,
	    (2.0*event->y() - H)/H);
  
  add_quats(lastquat, curquat, curquat);
  updateGL();
  
  lastPos = event->pos();
}

HandleList<GLWidget*> glHandles;

//!
//@Module GLDEFMATERIAL Defines a GL Material
//@@Section GLWIN
//@@Usage
//Define a material.  The syntax for its use is
//@[
//  gldefmaterial(name,ambient,diffuse,specular,shininess)
//@]
//where @|name| is the name of the material, and @|ambient|
//is a @|4 x 1| vector containing the ambient component of 
//the material property, and @|diffuse| is a @|4 x 1| vector
//and @|specular| is a @|4 x 1| vector containing the specular
//component of the material properties and @|shininess| is
//the exponent that governs the shinines of the material.
//@@Signature
//gfunction gldefmaterial GLDefMaterialFunction
//input name ambient diffuse specular shininess
//output none
//!
ArrayVector GLDefMaterialFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 5) 
    throw Exception("gldefmaterial requires 5 parameters");
  QString name = arg[0].asString();
  Array ambient = arg[1].toClass(Double);
  if (arg[1].length() != 4) 
    throw Exception("ambient parameter must be length 4");
  Array diffuse = arg[2].toClass(Double);
  if (arg[2].length() != 4)
    throw Exception("diffuse parameter must be length 4");
  Array specular = arg[3].toClass(Double);
  if (arg[3].length() != 4)
    throw Exception("specular parameter must be length 4");
  double shininess = arg[4].asDouble();
  GLMaterial mat;
  mat.shininess = shininess;
  for (int i=0;i<4;i++) {
    mat.ambient[i] = ambient.get((index_t)(i+1)).asDouble();
    mat.diffuse[i] = diffuse.get((index_t)(i+1)).asDouble();
    mat.specular[i] = specular.get((index_t)(i+1)).asDouble();
  }
  material_dictionary[name] = mat;
  return ArrayVector();
}

//!
//@Module GLLINES Create a GL Lineset
//@@Section GLWIN
//@@Usage
//Defines a set of lines that can be treated as a node.
//A GL Lines is defined by a vector consisting of the
//following elements:
//@[
//   [m1 x1 y1 z1 ... xn yn zn m2 x1 y1 z1 .... ]
//@]
//i.e., a point count followed by that number of triplets.
//The usage of this function is 
//@[
//  gllines(name,vector,color)
//@]
//where @|name| is the name of the lineset and @|vector|
//is the aforementioned vector of points.
//@@Signature
//gfunction gllines GLLinesFunction
//inputs name vector color
//outputs none
//!
ArrayVector GLLinesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("gllines requires three arguments: the object name, the vector and color");
  QString name = arg[0].asString();
  Array p = arg[1].toClass(Double).asDenseArray();
  const BasicArray<double> &p_rp(p.constReal<double>());
  GLLines lines;
  for (index_t i=1;i<=p.length();i++) {
    lines.points.push_back(p_rp[NTuple(i,1)]);
  }
  Array color = arg[2].toClass(Double).asDenseArray();
  const BasicArray<double> &color_rp(color.constReal<double>());
  lines.color[0] = color_rp[NTuple(1,1)];
  lines.color[1] = color_rp[NTuple(2,1)];
  lines.color[2] = color_rp[NTuple(3,1)];
  linesmap[name] = lines;
  return ArrayVector();
}

//!
//@Module GLCLUMP Create a GL Clump
//@@Section GLWIN
//@@Usage
//Defines an aggregate clump of objects that can be treated
//as a node.  A GL Clump is defined by a vector consisting
//of the following elements:
//@[
//   [r1 g1 b1 n1 p1 p2 p3 ... r2 g2 b2 n2 p1 p2 p3 ... ]
//@]
//i.e., an RGB color spec, followed by a point count @|ni|, followed
//by a length @|ni| vector of coordinates that are @|x,y,z| triplets.
//The usage of this function is
//@[
//   glclump(name,vector)
//@]
//where @|name| is the name of the clump and @|vector| is the aforementioned
//vector of points.
//@@Signature
//gfunction glclump GLClumpFunction
//inputs name vector
//outputs none
//!
ArrayVector GLClumpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("glclump requires two arguments: the object name and the clump vector");
  QString name = arg[0].asString();
  Array p = arg[1].toClass(Double).asDenseArray();
  const BasicArray<double> &p_rp(p.constReal<double>());
  GLClump clump;
  for (index_t i=1;i<=p.length();i++) {
    clump.points.push_back(p_rp[NTuple(i,1)]);
  }
  clumpmap[name] = clump;
  return ArrayVector();
}

//!
//@Module GLASSEMBLY Create a GL Assembly
//@@Section GLWIN
//@@Usage
//Define a GL Assembly.  A GL Assembly consists of one or more
//GL Nodes or GL Assemblies that are placed relative to the 
//coordinate system of the assembly.  For example, if we have
//@|glnode| definitions for @|'bread'| and @|'cheese'|, then
//a @|glassembly| of sandwich would consist of placements of
//two @|'bread'| nodes with a @|'cheese'| node in between.
//Furthermore, a @|'lunch'| assembly could consist of a @|'sandwich'|
//a @|'chips'| and @|'soda'|.  Hopefully, you get the idea.  The
//syntax for the @|glassembly| command is
//@[
//   glassembly(name,part1,transform1,part2,transform2,...)
//@]
//where @|part1| is the name of the first part, and could be
//either a @|glnode| or itself be another @|glassembly|.  
//Here @|transform1| is the @|4 x 4 matrix| that transforms
//the part into the local reference coordinate system.
//
//WARNING!! Currently FreeMat does not detect or gracefully handle 
//self-referential assemblies (i.e, if you try to make a @|sandwich| 
//contain a @|sandwich|, which you can do by devious methods that I 
//refuse to explain).  Do not do this!  You have been warned.
//!
//@@Signature
//gfunction glassembly GLAssemblyFunction
//inputs name varargin
//outputs none
//
ArrayVector GLAssemblyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) return ArrayVector();
  QString name = arg[0].asString();
  if (arg.size() % 2 == 0)
    throw Exception("glassembly expects a name followed by object name and transform pairs");
  GLAssembly assembly;
  for (int i=1;i<arg.size();i+=2) {
    QString objectname = arg[i].asString();
    if (!assemblymap.contains(objectname) && !nodemap.contains(objectname)
	&& !clumpmap.contains(objectname) && !linesmap.contains(objectname))
      throw Exception(QString("Object ") + objectname + " is not defined");
    Array transform = arg[i+1].toClass(Double);
    if ((transform.rows() != 4) || (transform.cols() != 4))
      throw Exception("transforms must be 4 x 4 matrices");
    GLPart part;
    part.name = objectname;
    part.transform = transform;
    assembly.parts.push_back(part);
  }
  assemblymap[name] = assembly;
  return ArrayVector();
}

//!
//@Module GLNODE Create a GL Node
//@@Section GLWIN
//@@Usage
//Define a GL Node.  A GL Node is an object that can be displayed
//in a GL Window.  It is defined by a triangular mesh of vertices.
//It must also have a material that defines its appearance (i.e.
//color, shininess, etc.).  The syntax for the @|glnode| command
//is 
//@[
//  glnode(name,material,pointset)  
//@]
//where @|material| is the name of a material that has already been
//defined with @|gldefmaterial|, @|pointset| is a @|3 x N| matrix
//of points that define the geometry of the object.  Note that the points
//are assumed to be connected in triangular facts, with the points
//defined counter clock-wise as seen from the outside of the facet.
//@|FreeMat| will compute the normals.  The @|name| argument must
//be unique.  If you want multiple instances of a given @|glnode|
//in your GLWindow, that is fine, as instances of a @|glnode| are
//created through a @|glassembly|.  
//@@Signature
//gfunction glnode GLNodeFunction
//input name material pointset
//output none
//!
ArrayVector GLNodeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("glnode requires four arguments");
  QString name = arg[0].asString();
  QString material = arg[1].asString();
  Array pointset = arg[2].toClass(Double);
  const BasicArray<double> &points_rp(pointset.constReal<double>());
  if (pointset.rows() != 3)
    throw Exception("pointset argument must be a 3 x N matrix");
  GLNode p;
  if (!material_dictionary.contains(material))
    throw Exception(QString("material '") + material + QString("' is not defined"));
  p.material = material;
  for (index_t i=1;i<=pointset.cols();i++) {
    p.points.push_back(points_rp[NTuple(1,i)]);
    p.points.push_back(points_rp[NTuple(2,i)]);
    p.points.push_back(points_rp[NTuple(3,i)]);
  }
  nodemap[name] = p;
  return ArrayVector();
}

//!
//@Module GLSHOW Show a GL Assembly in a GL Window
//@@Section HANDLE
//@@Usage
//Shows a GL Assembly in a GL Window.  The syntax for its
//use is
//@[
//  glshow(name,scale)
//@]
//which shows the @|glassembly| named @|name| in a new GL
//window, with the scale set to @|scale|.  Roughly speaking
//@|scale| should represent the radial size of the object
//that you want to see in the window.
//@@Signature
//gfunction glshow GLShowFunction
//input name scale
//output none
//!
ArrayVector GLShowFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) return ArrayVector();
  QString name = arg[0].asString();
  double scale;
  if (arg.size() < 2) 
    scale = 1;
  else
    scale = arg[1].asDouble();
  if (!assemblymap.contains(name))
    throw Exception(QString("Assembly named ") + name + " could not be found");
  GLWidget *t = new GLWidget;
  t->setWindowIcon(QPixmap(":/images/freemat_figure_small_mod_64.png"));
  t->setWindowTitle(QString("GL Assembly %1").arg(name));
  t->scale = scale;
  t->name = name;
  t->show();
  return ArrayVector();
}


