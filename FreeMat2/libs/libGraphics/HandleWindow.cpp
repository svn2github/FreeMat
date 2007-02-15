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
#include "HandleAxis.hpp"
#include "HandleCommands.hpp"
#include <qgl.h>
#include "GLRenderEngine.hpp"
#include "QTRenderEngine.hpp"
#include <QStackedLayout>
#include "HandleCommands.hpp"
#include <QtGui>
#include <math.h>

class BaseFigureQt : public QWidget {
  HandleFigure *hfig;
  QImage backStore;
public:
  BaseFigureQt(QWidget *parent, HandleFigure *fig);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  //   QSize sizeHint() const;
  //   QSizePolicy sizePolicy() const;
};

// QSize BaseFigureQt::sizeHint() const {
//   HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
//   qDebug() << "Size hint " << (htv->Data()[0]) << "," << (htv->Data()[1]) << "\r\n";
//   return QSize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
//   //  return QSize(10000,10000);
// }

void BaseFigureQt::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  //  qDebug() << "resize " << width() << " " << height() << "\r\n";
  hfig->resizeGL(qMax(8,width()),
  		 qMax(8,height()));
  backStore = QImage(width(),height(),QImage::Format_RGB32);
}

static bool enableRepaint = false;

void EnableRepaint() {
  enableRepaint = true;
  qDebug() << "Enable repaint";
}

void DisableRepaint() {
  enableRepaint = false;
  qDebug() << "Disable repaint";
}

void BaseFigureQt::paintEvent(QPaintEvent *e) {
  if (enableRepaint && hfig->ParentWindow()->isDirty()) {
    // enableRepaint is true, and the background is dirty - update
    // the backing store, and then redraw it.
    QPainter pnt(&backStore);
    QTRenderEngine gc(&pnt,0,0,width(),height());
    hfig->PaintMe(gc);
    hfig->ParentWindow()->markClean();
  }
  QPainter pnt(this);
  pnt.drawImage(0,0,backStore);
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
  
bool HandleWindow::event(QEvent* e) {
  if (e->type() == QEvent::WindowActivate) {
    NotifyFigureActive(handle);
    //    qDebug() << "focus event " << handle << "\n";
  }
  return QWidget::event(e);
}

HandleWindow::HandleWindow(unsigned ahandle) : QMainWindow() {
  initialized = false;
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  handle = ahandle;
  hfig = new HandleFigure(this);
  char buffer[1000];
  sprintf(buffer,"Figure %d",ahandle+1);
  setWindowTitle(buffer);
  qtchild = new BaseFigureQt(this,hfig);
  band = NULL;
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
  resize(600,400);
  initialized = true;
  mode = normal_mode;
}


// Useful tools:
//
// Zoom in - fix for images
// Zoom out - fix for images
// Pan - fix for images
// Rotate - fix for imagesqw
// Annotate
// Save
// Copy
// Examine (values)
//

void HandleWindow::zoom(bool active) {
  panAct->setChecked(false);
  rotateAct->setChecked(false);
  camRotateAct->setChecked(false);
  if (active)
    mode = zoom_mode;
  else
    mode = normal_mode;
}

void HandleWindow::pan(bool active) {
  zoomAct->setChecked(false);
  rotateAct->setChecked(false);
  camRotateAct->setChecked(false);
  if (active)
    mode = pan_mode;
  else
    mode = normal_mode;
}

void HandleWindow::rotate(bool active) {
  zoomAct->setChecked(false);
  panAct->setChecked(false);
  camRotateAct->setChecked(false);
  if (active)
    mode = rotate_mode;
  else
    mode = normal_mode;
}

void HandleWindow::camRotate(bool active) {
  zoomAct->setChecked(false);
  panAct->setChecked(false);
  rotateAct->setChecked(false);
  if (active)
    mode = cam_rotate_mode;
  else
    mode = normal_mode;
}

void HandleWindow::save() {
  QString fn = QFileDialog::getSaveFileName();
  HPrintFunction(0,singleArrayVector(Array::stringConstructor(fn.toStdString())));
}

void HandleWindow::copy() {
  HCopyFunction(0,ArrayVector());
}

void HandleWindow::createActions() {
  zoomAct = new QAction(QIcon(":/images/zoomin.png"),"&Zoom",this);
  zoomAct->setCheckable(true);
  zoomAct->setToolTip("Left click to zoom in by 2, left click and drag to zoom a region, right click to zoom out by 2");
  connect(zoomAct,SIGNAL(triggered(bool)),this,SLOT(zoom(bool)));
  panAct = new QAction(QIcon(":/images/pan.png"),"&Pan",this);
  connect(panAct,SIGNAL(triggered(bool)),this,SLOT(pan(bool)));
  panAct->setCheckable(true);
  rotateAct = new QAction(QIcon(":/images/rotate.png"),"&Rotate",this);
  connect(rotateAct,SIGNAL(triggered(bool)),this,SLOT(rotate(bool)));
  rotateAct->setCheckable(true);
  camRotateAct = new QAction(QIcon(":/images/cam_rotate.png"),"&Camera Rotate",this);
  connect(camRotateAct,SIGNAL(triggered(bool)),this,SLOT(camRotate(bool)));
  camRotateAct->setCheckable(true);
  saveAct = new QAction(QIcon(":/images/save.png"),"&Save",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
  closeAct = new QAction(QIcon(":/images/quit.png"),"&Close",this);
  connect(closeAct,SIGNAL(triggered()),this,SLOT(close()));
}

void HandleWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(closeAct);
  editMenu = menuBar()->addMenu("&Tools");
  editMenu->addAction(copyAct);
  editMenu->addAction(zoomAct);
  editMenu->addAction(panAct);
  editMenu->addAction(rotateAct);
  editMenu->addAction(camRotateAct);
}

void HandleWindow::createToolBars() {
  toolBar = addToolBar("Tools");
  toolBar->setObjectName("ToolsToolBar");
  toolBar->addAction(saveAct);
  toolBar->addAction(closeAct);
  toolBar->addAction(copyAct);
  toolBar->addAction(zoomAct);
  toolBar->addAction(panAct);
  toolBar->addAction(rotateAct);
  toolBar->addAction(camRotateAct);
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
  int save_mode = mode;
  mode = click_mode;
  m_loop.exec();
  x = click_x;
  y = click_y;
  QApplication::restoreOverrideCursor();
  mode = save_mode;
}


HandleAxis* GetContainingAxis(HandleFigure *fig, int x, int y) {
  //  qDebug() << "Click " << x << "," << y;
  HPHandles *cp = (HPHandles*) fig->LookupProperty("children");
  std::vector<unsigned> children(cp->Data());
  //  qDebug() << "Click " << x << "," << y;
  for (int i=0;i<children.size();i++) {
    HandleObject* hp = LookupHandleObject(children[i]);
    if (hp->IsType("axes")) {
      // Get the axis extents
      std::vector<double> position(((HandleAxis*) hp)->GetPropertyVectorAsPixels("position"));
      //      qDebug() << "Axis: " << position[0] << "," << position[1] << "," << position[2] << "," << position[3];
      if ((x >= position[0]) && (x < (position[0]+position[2])) &&
	  (y >= position[1]) && (y < (position[1]+position[3])))
	return (HandleAxis*)hp;
    }
  }
  return NULL;
}

void HandleWindow::mousePressEvent(QMouseEvent* e) {
  try {
    if (mode == click_mode) {
      click_x = e->x();
      click_y = e->y();
      m_loop.exit();
    }
    if ((mode == zoom_mode) && (e->button() == Qt::LeftButton))  {
      origin = e->pos();
      if (!band)
	band = new QRubberBand(QRubberBand::Rectangle, this);
      band->setGeometry(QRect(origin,QSize()));
      band->show();
      zoom_active = true;
    } else
      zoom_active = false;
    if (mode == pan_mode) {
      origin = e->pos();
      QRect plot_region(qtchild->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(e->x()),remapY(e->y()));
      if (h) {
	HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	pan_xrange = (hp->Data()[1] - hp->Data()[0]);
	pan_xmean = (hp->Data()[1] + hp->Data()[0])/2;
	hp = (HPTwoVector*) h->LookupProperty("ylim");
	pan_yrange = (hp->Data()[1] - hp->Data()[0]);
	pan_ymean = (hp->Data()[1] + hp->Data()[0])/2;
	pan_active = true;
      } else {
	pan_active = false;
      }
    }
    if ((mode == rotate_mode) || (mode == cam_rotate_mode)) {
      origin = e->pos();
      QRect plot_region(qtchild->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(e->x()),remapY(e->y()));
      if (h) {
	rotate_active = true;
	rotate_camera = ((HPThreeVector*) h->LookupProperty("cameraposition"))->Data();
	rotate_up = ((HPThreeVector*) h->LookupProperty("cameraupvector"))->Data();
	rotate_target = ((HPThreeVector*) h->LookupProperty("cameratarget"))->Data();
	rotate_forward = rotate_target;
	rotate_forward[0] -= rotate_camera[0];
	rotate_forward[1] -= rotate_camera[1];
	rotate_forward[2] -= rotate_camera[2];
	rotate_source_cam_dist = sqrt(rotate_forward[0]*rotate_forward[0] + 
				      rotate_forward[1]*rotate_forward[1] + 
				      rotate_forward[2]*rotate_forward[2]);
	rotate_forward[0] /= rotate_source_cam_dist;
	rotate_forward[1] /= rotate_source_cam_dist;
	rotate_forward[2] /= rotate_source_cam_dist;
	rotate_right = rotate_up;
	rotate_right[0] = (rotate_forward[1] * rotate_up[2]) - (rotate_forward[2] * rotate_up[1]);
	rotate_right[1] = (rotate_forward[2] * rotate_up[0]) - (rotate_forward[0] * rotate_up[2]);
	rotate_right[2] = (rotate_forward[0] * rotate_up[1]) - (rotate_forward[1] * rotate_up[0]);
      }
    }
  } catch (Exception &e) {
  }
}

void HandleWindow::mouseMoveEvent(QMouseEvent* e) {
  try {
    if ((mode == zoom_mode) && zoom_active)
      band->setGeometry(QRect(origin, e->pos()).normalized());
    if ((mode == pan_mode) && pan_active) {
      QPoint dest(e->pos());
      QRect plot_region(qtchild->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	std::vector<double> position(h->GetPropertyVectorAsPixels("position"));
	double delx, dely;
	if (h->StringCheck("xdir","reverse"))
	  delx = (e->x() - origin.x())/position[2];
	else
	  delx = -(e->x() - origin.x())/position[2];
	if (h->StringCheck("ydir","reverse"))
	  dely = -(e->y() - origin.y())/position[3];
	else
	  dely = (e->y() - origin.y())/position[3];
	h->SetTwoVectorDefault("xlim",pan_xmean+pan_xrange*delx-pan_xrange/2,
			       pan_xmean+pan_xrange*delx+pan_xrange/2);
	h->SetConstrainedStringDefault("xlimmode","manual");
	h->SetTwoVectorDefault("ylim",pan_ymean+pan_yrange*dely-pan_yrange/2,
			       pan_ymean+pan_yrange*dely+pan_yrange/2);
	h->SetConstrainedStringDefault("ylimmode","manual");
	h->UpdateState();
	hfig->Repaint();
      }
    }
    if ((mode == rotate_mode) && rotate_active) {
      QPoint dest(e->pos());
      QRect plot_region(qtchild->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	double az = (e->x() - origin.x())/180.0*M_PI;
	double el = (e->y() - origin.y())/180.0*M_PI;
	// The delx means we rotate the camera 
	vector<double> camera_position(rotate_target);
	camera_position[0] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[0] + 
						      -cos(el)*cos(az)*rotate_forward[0] + 
						      sin(el)*rotate_up[0]);
	camera_position[1] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[1] + 
						      -cos(el)*cos(az)*rotate_forward[1] + 
						      sin(el)*rotate_up[1]);
	camera_position[2] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[2] + 
						      -cos(el)*cos(az)*rotate_forward[2] + 
						      sin(el)*rotate_up[2]);
	vector<double> camera_up(rotate_target);
	camera_up[0] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[0] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[0] + 
			sin(el+M_PI/2.0)*rotate_up[0]);
	camera_up[1] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[1] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[1] + 
			sin(el+M_PI/2.0)*rotate_up[1]);
	camera_up[2] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[2] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[2] + 
			sin(el+M_PI/2.0)*rotate_up[2]);
	h->SetThreeVectorDefault("cameraposition",camera_position[0],
				 camera_position[1],camera_position[2]);
	h->SetConstrainedStringDefault("camerapositionmode","manual");
	h->SetThreeVectorDefault("cameraupvector",camera_up[0],
				 camera_up[1],camera_up[2]);
	h->SetConstrainedStringDefault("cameraupvectormode","manual");
	h->UpdateState();
	hfig->Repaint();
      }
    }
    if ((mode == cam_rotate_mode) && rotate_active) {
      QPoint dest(e->pos());
      QRect plot_region(qtchild->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	double el = (e->y() - origin.y())/180.0*M_PI;
	vector<double> camera_up(rotate_target);
	camera_up[0] = cos(el)*rotate_up[0] - sin(el)*rotate_right[0];
	camera_up[1] = cos(el)*rotate_up[1] - sin(el)*rotate_right[1];
	camera_up[2] = cos(el)*rotate_up[2] - sin(el)*rotate_right[2];
	h->SetThreeVectorDefault("cameraupvector",camera_up[0],
				 camera_up[1],camera_up[2]);
	h->SetConstrainedStringDefault("cameraupvectormode","manual");
	h->UpdateState();
	hfig->Repaint();
      }
    }
  } catch (Exception &e) {
  }
}

int HandleWindow::remapX(int x) {
  QRect plot_region(qtchild->geometry());
  return x - plot_region.x();
}

int HandleWindow::remapY(int y) {
  QRect plot_region(qtchild->geometry());
  return (plot_region.height()-y+plot_region.y());
}

void HandleWindow::mouseReleaseEvent(QMouseEvent * e) {
  try {
    if (mode == pan_mode)
      pan_active = false;
    if (mode == zoom_mode) {
      if (zoom_active) {
	band->hide();
	QRect rect(band->geometry().normalized());
	if ((rect.width()*rect.height()) < 20) {
	  // Treat as a click
	  int click_x, click_y;
	  click_x = e->x();
	  click_y = e->y();
	  HandleAxis *h = GetContainingAxis(hfig,remapX(click_x),remapY(click_y));
	  if (h) {
	    HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	    double range = (hp->Data()[1] - hp->Data()[0])/2;
	    double mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("xlim",mean-range/2,mean+range/2);
	    h->SetConstrainedStringDefault("xlimmode","manual");
	    hp = (HPTwoVector*) h->LookupProperty("ylim");
	    range = (hp->Data()[1] - hp->Data()[0])/2;
	    mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("ylim",mean-range/2,mean+range/2);
	    h->SetConstrainedStringDefault("ylimmode","manual");
	    h->UpdateState();
	    hfig->Repaint();
	  }
	} else {       
	  QRect plot_region(qtchild->geometry());
	  HandleAxis *h = GetContainingAxis(hfig,remapX(rect.x()),remapY(rect.y()));
	  if (h) {
	    std::vector<double> position(h->GetPropertyVectorAsPixels("position"));
	    double xminfrac = (remapX(rect.x()) - position[0])/position[2];
	    double xmaxfrac = (remapX(rect.x()+rect.width()) - position[0])/position[2];
	    double yminfrac = (remapY(rect.y()+rect.height()) - position[1])/position[3];
	    double ymaxfrac = (remapY(rect.y()) - position[1])/position[3];
// 	    qDebug() << "xrange " << xminfrac << "," << xmaxfrac;
// 	    qDebug() << "yrange " << yminfrac << "," << ymaxfrac;
	    xminfrac = qMax(0.,qMin(1.,xminfrac));
	    xmaxfrac = qMax(0.,qMin(1.,xmaxfrac));
	    yminfrac = qMax(0.,qMin(1.,yminfrac));
	    ymaxfrac = qMax(0.,qMin(1.,ymaxfrac));
	    if (h->StringCheck("ydir","reverse")) {
	      double y1 = 1-yminfrac;
	      double y2 = 1-ymaxfrac;
	      yminfrac = y2;
	      ymaxfrac = y1;
	    }
	    if (h->StringCheck("xdir","reverse")) {
	      double x1 = 1-xminfrac;
	      double x2 = 1-xmaxfrac;
	      xminfrac = x2;
	      xmaxfrac = x1;
	    }
	    HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	    double range = (hp->Data()[1] - hp->Data()[0]);
	    double mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("xlim",mean-range/2+xminfrac*range,mean-range/2+xmaxfrac*range);
	    h->SetConstrainedStringDefault("xlimmode","manual");
	    hp = (HPTwoVector*) h->LookupProperty("ylim");
	    range = (hp->Data()[1] - hp->Data()[0]);
	    mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("ylim",mean-range/2+yminfrac*range,mean-range/2+ymaxfrac*range);
	    h->SetConstrainedStringDefault("ylimmode","manual");
	    h->UpdateState();
	    hfig->Repaint();
	  }
	}
      } else {
	// Treat as a click
	int click_x, click_y;
	click_x = e->x();
	click_y = e->y();
	QRect plot_region(qtchild->geometry());
	HandleAxis *h = GetContainingAxis(hfig,remapX(click_x),remapY(click_y));
	if (h) {
	  HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	  double range = (hp->Data()[1] - hp->Data()[0])*2;
	  double mean = (hp->Data()[1] + hp->Data()[0])/2;
	  h->SetTwoVectorDefault("xlim",mean-range/2,mean+range/2);
	  h->SetConstrainedStringDefault("xlimmode","manual");
	  hp = (HPTwoVector*) h->LookupProperty("ylim");
	  range = (hp->Data()[1] - hp->Data()[0])*2;
	  mean = (hp->Data()[1] + hp->Data()[0])/2;
	  h->SetTwoVectorDefault("ylim",mean-range/2,mean+range/2);
	  h->SetConstrainedStringDefault("ylimmode","manual");
	  h->UpdateState();
	  hfig->Repaint();
	}
      }
    }
  } catch (Exception &e) {
  }
}

void HandleWindow::UpdateState() {
  if (!initialized) return;
    dirty = true;
  //  HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
  //  qtchild->resize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  //  qtchild->updateGeometry();
  //  adjustSize();
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
