#include "HandleWindow.hpp"
#include <qgl.h>
#include "GLRenderEngine.hpp"
#include "QTRenderEngine.hpp"
#include <QStackedLayout>
#include "HandleCommands.hpp"
#include <QtGui>

namespace FreeMat {

class BaseFigureQt : public QWidget {
  HandleFigure *hfig;
 public:
  BaseFigureQt(QWidget *parent, HandleFigure *fig);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
};

void BaseFigureQt::resizeEvent(QResizeEvent *e) {
  //qDebug("Qtsize");
  QWidget::resizeEvent(e);
  hfig->resizeGL(width(),height());
}

void BaseFigureQt::paintEvent(QPaintEvent *e) {
  //  qDebug("Qtpaint");
  QWidget::paintEvent(e);
  QPainter pnt(this);
  QTRenderEngine gc(&pnt,0,0,width(),height());
  hfig->PaintMe(gc);
}

BaseFigureQt::BaseFigureQt(QWidget *parent, HandleFigure *fig) : 
  QWidget(parent) {
  hfig = fig;
  hfig->resizeGL(width(),height());
 }

class BaseFigureGL : public QGLWidget {
  HandleFigure *hfig;
 public:
  BaseFigureGL(QWidget *parent, HandleFigure *fig);
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int width, int height);
  // Support dragging...
  //   void mousePressEvent(QMouseEvent* e);
  //   void mouseMoveEvent(QMouseEvent* e);
  //   void mouseReleaseEvent(QMouseEvent* e);
  //  virtual void Show() {QWidget::show();};
};

  BaseFigureGL::BaseFigureGL(QWidget *parent, HandleFigure *fig) : 
    QGLWidget(parent) {
    hfig = fig;
    hfig->resizeGL(width(),height());
  }
  
  void BaseFigureGL::initializeGL() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
  }
  
  void BaseFigureGL::paintGL() {
    //    qDebug("GLpaint");
    GLRenderEngine gc(this,0,0,width(),height());
    hfig->PaintMe(gc);
  }

  void BaseFigureGL::resizeGL(int width, int height) {
    //    qDebug("GLsize");
    hfig->resizeGL(width,height);
  }

#if 0
  void BaseFigureGL::mousePressEvent(QMouseEvent* e) {
    if (e->button() & Qt::LeftButton)
      elevazim = true;
    else
      elevazim = false;
    beginx = e->x();
    beginy = e->y();
  }

void BaseFigureGL::mouseMoveEvent(QMouseEvent* e) {
  if (elevazim) {
    elev -= (e->y() - beginy);
    azim += (e->x() - beginx);
    elev = (elev + 360) % 360;
    azim = (azim + 360) % 360;
  } else {
    arot += (e->y() - beginy);
    arot = (arot + 360) % 360;
  }
  beginx = e->x();
  beginy = e->y();
  //    updateGL();
}

void BaseFigureGL::mouseReleaseEvent(QMouseEvent* e) {
}
#endif

  void HandleWindow::closeEvent(QCloseEvent* e) {
    NotifyFigureClosed(handle);
  }
  
  HandleWindow::HandleWindow(unsigned ahandle) : QWidget() {
  setWindowIcon(QPixmap(":/images/freemat-2.xpm"));
  handle = ahandle;
  hfig = new HandleFigure(this);
  char buffer[1000];
  sprintf(buffer,"Figure %d",ahandle+1);
  setWindowTitle(buffer);
  qtchild = new BaseFigureQt(NULL,hfig);
  glchild = new BaseFigureGL(NULL,hfig);
  layout = new QStackedWidget(this);
  QHBoxLayout *box = new QHBoxLayout(this);
  box->setMargin(0);
  setLayout(box);
  //   layout = new QTabWidget;
  //   layout->addTab(qtchild,"QT");
  //   layout->addTab(glchild,"GL");
  layout->addWidget(qtchild);
  layout->addWidget(glchild);
  layout->show();
  box->addWidget(layout);
  resize(600,400);
}

unsigned HandleWindow::Handle() {
  return handle;
}

HandleFigure* HandleWindow::HFig() {
  return hfig;
}

  void HandleWindow::GetClick(int &x, int &y) {
    // Set the cross cursor
    QApplication::setOverrideCursor(Qt::CrossCursor);
    // Run the event loop
    m_loop.exec();
    x = click_x;
    y = click_y;
    QApplication::restoreOverrideCursor();
  }

  void HandleWindow::mousePressEvent(QMouseEvent* e) {
    click_x = e->x();
    click_y = e->y();
    m_loop.exit();
  }

  void HandleWindow::UpdateState() {
    HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
    resize(htv->Data()[0],htv->Data()[1]);
    if (hfig->StringCheck("renderer","opengl")) {
      if (layout->currentWidget() != glchild) {
	layout->setCurrentWidget(glchild);
	glchild->show();
	glchild->updateGeometry();
	//      layout->removeWidget(qtchild);
	//      qtchild->hide();
	//	qDebug("GLactive");
	//     glchild->show();
	repaint();
	glchild->updateGL();
	update();
	UpdateState();
      }
      glchild->updateGL();
      update();
    } else if (hfig->StringCheck("renderer","painters")) {
      if (layout->currentWidget() != qtchild) {
	glchild->setGeometry(0,0,1,1);
	//      glchild->makeCurrent();
	//glchild->resizeGL(1,1);
	//       glClearColor(1,1,1,1);
	//       glClearDepth(1.0f);
	//       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//       glchild->swapBuffers();
	//       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	layout->setCurrentWidget(qtchild);
	//      qtchild->show();
	//      glchild->hide();
	//     qtchild->show();
	qDebug("QTactive");
	//      glchild->updateGL();
      }
      update();
    }
  }
}
