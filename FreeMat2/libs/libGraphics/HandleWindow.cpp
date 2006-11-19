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
#include "HandleWindow.hpp"
#include <qgl.h>
#include "GLRenderEngine.hpp"
#include "QTRenderEngine.hpp"
#include <QStackedLayout>
#include "HandleCommands.hpp"
#include <QtGui>

class BaseFigureQt : public QWidget {
  HandleFigure *hfig;
public:
  BaseFigureQt(QWidget *parent, HandleFigure *fig);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
};

QSize BaseFigureQt::sizeHint() const {
  HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
  return QSize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
}

QSizePolicy BaseFigureQt::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void BaseFigureQt::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  hfig->resizeGL(qMax(8,width()),
  		 qMax(8,height()));
}

void BaseFigureQt::paintEvent(QPaintEvent *e) {
  QPainter pnt(this);
  QTRenderEngine gc(&pnt,0,0,width(),height());
  hfig->PaintMe(gc);
  QWidget::paintEvent(e);
}

BaseFigureQt::BaseFigureQt(QWidget *parent, HandleFigure *fig) : 
  QWidget(parent) {
  hfig = fig;
  //  hfig->resizeGL(width(),height());
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
  
HandleWindow::HandleWindow(unsigned ahandle) : QMainWindow() {
  initialized = false;
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  handle = ahandle;
  hfig = new HandleFigure(this);
  char buffer[1000];
  sprintf(buffer,"Figure %d",ahandle+1);
  setWindowTitle(buffer);
  qtchild = new BaseFigureQt(NULL,hfig);
  //   if (QGLFormat::hasOpenGL())
  //     glchild = new BaseFigureGL(NULL,hfig);
  //  layout = new QStackedWidget(this);
  //  QHBoxLayout *box = new QHBoxLayout(this);
  //  box->setMargin(0);
  //  setLayout(box);
  //   layout = new QTabWidget;
  //   layout->addTab(qtchild,"QT");
  //   layout->addTab(glchild,"GL");
  //  layout->addWidget(qtchild);
  //   if (QGLFormat::hasOpenGL())
  //     layout->addWidget(glchild);
  //  layout->show();
  //  box->addWidget(layout);
  createActions();
  createMenus();
  createToolBars();
  setCentralWidget(qtchild);
  // resize(600,400);
  initialized = true;
}

void HandleWindow::zoom() {
}

void HandleWindow::pan() {
}

void HandleWindow::save() {
}

void HandleWindow::copy() {
}

void HandleWindow::createActions() {
  zoomAct = new QAction(QIcon(":/images/zoomin.png"),"&Zoom",this);
  connect(zoomAct,SIGNAL(triggered()),this,SLOT(zoom()));
  panAct = new QAction(QIcon(":/images/pan.png"),"&Pan",this);
  connect(panAct,SIGNAL(triggered()),this,SLOT(pan()));
  saveAct = new QAction(QIcon(":/images/save.png"),"&Save",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
}

void HandleWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(zoomAct);
  editMenu->addAction(panAct);
}

void HandleWindow::createToolBars() {
  toolBar = addToolBar("Tools");
  toolBar->setObjectName("ToolsToolBar");
  toolBar->addAction(saveAct);
  toolBar->addAction(copyAct);
  toolBar->addAction(zoomAct);
  toolBar->addAction(panAct);
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
  if (!initialized) return;
  //  HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
  //  qtchild->resize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  qtchild->updateGeometry();
  adjustSize();
  //   if (hfig->StringCheck("renderer","opengl") && (QGLFormat::hasOpenGL())) {
  //     if (layout->currentWidget() != glchild) {
  //       layout->setCurrentWidget(glchild);
  //       glchild->show();
  //       glchild->updateGeometry();
  //       repaint();
  //       glchild->updateGL();
  //       update();
  //       UpdateState();
  //     }
  //     glchild->updateGL();
  //     update();
  //   } else if (hfig->StringCheck("renderer","painters")) {
  //     if (layout->currentWidget() != qtchild) {
  //       if (QGLFormat::hasOpenGL())
  // 	glchild->setGeometry(0,0,1,1);
  //  layout->setCurrentWidget(qtchild);
  //     }
  update();
//   }
}
