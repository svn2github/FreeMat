#include <qgl.h>
#include <QMouseEvent>
#include <qapplication.h>

class GLWidget : public QGLWidget {
 public:
  GLWidget(QWidget *parent=0, char *name=0) {
    rtri = 25;
    rquad = 32;
    xmin = -6;
    xmax = 6;
    ymin = -6;
    ymax = 6;
    zmin = -6;
    zmax = 6;
  }
  virtual ~GLWidget() {
  }
 protected:
  int elev, azim;
  GLfloat rtri, rquad;
  GLfloat xmin, xmax, ymin, ymax, zmin, zmax;
  GLfloat beginx, beginy;

  void mousePressEvent(QMouseEvent* e) {
    beginx = e->x();
    beginy = e->y();
  }

  void mouseMoveEvent(QMouseEvent* e) {
    elev -= (e->y() - beginy);
    azim += (e->x() - beginx);
    elev = (elev + 360) % 360;
    azim = (azim + 360) % 360;
    beginx = e->x();
    beginy = e->y();
    updateGL();
  }

  void mouseReleaseEvent(QMouseEvent* e) {
  }

  virtual void initializeGL() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  }
  virtual void resizeGL(int width, int height) {
    height = height?height:1;
    glViewport( 20, 20, (GLint)width-40, (GLint)height-40 );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    GLfloat scale = 2.0;
    glOrtho(scale*xmin, scale*xmax, scale*ymin, 
	    scale*ymax, scale*zmin, scale*zmax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
  virtual void paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(elev,0,1,0);
    glRotatef(azim,1,0,0);
    
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f,0.0f);
    GLfloat xmean, ymean, zmean;
    xmean = (xmin+xmax)/2;
    ymean = (ymin+ymax)/2;
    zmean = (zmin+zmax)/2;
    glVertex3f(xmin,ymean,zmean);
    glVertex3f(xmax,ymean,zmean);
    glVertex3f(xmean,ymin,zmean);
    glVertex3f(xmean,ymax,zmean);
    glVertex3f(xmean,ymean,zmin);
    glVertex3f(xmean,ymean,zmax);
    glEnd();

    // Draw the box
    glEnable(GL_CULL_FACE);
    glBegin(GL_QUADS);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f( xmin, ymin, zmin);
    glVertex3f( xmax, ymin, zmin);
    glVertex3f( xmax, ymax, zmin);
    glVertex3f( xmin, ymax, zmin);
    
    glVertex3f( xmin, ymin, zmax);
    glVertex3f( xmin, ymax, zmax);
    glVertex3f( xmax, ymax, zmax);
    glVertex3f( xmax, ymin, zmax);

    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymin, zmax);

    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmin);

    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmin);

    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);

    glEnd();
    glDisable(GL_CULL_FACE);

    glPushMatrix();

    glTranslatef(-1.5f,0.0f,-2.0f);
    glRotatef(rtri,0.0f,1.0f,0.0f);
    glBegin(GL_TRIANGLES);  
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f); 
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f); 
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f); 
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);  
    glEnd();  

    glPopMatrix();

    glPushMatrix();

    glTranslatef(1.5f,0.0f,0.0f);
    glRotatef(rquad,1.0f,0.0f,0.0f);
    
    glColor3f(0.5f,0.5f,1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glColor3f(1.0f,0.5f,0.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);
    glColor3f(1.0f,0.0f,0.0f);    
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glColor3f(1.0f,1.0f,0.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);   
    glColor3f(1.0f,0.0f,1.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f); 
    glEnd();   

    glPopMatrix();

  }
};

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  GLWidget *w = new GLWidget;
  //  a.setMainWidget(w);
  w->show();
  return a.exec();
}
