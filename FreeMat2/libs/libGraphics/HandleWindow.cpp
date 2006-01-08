#include "HandleWindow.hpp"

  class BaseFigureQt : public BaseFigure, public QWidget {
  public:
    BaseFigureQt(unsigned ahandle);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    virtual void Show() {QWidget::show();};
  };

  void BaseFigureQt::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    hfig->resizeGL(width(),height());
  }

  void BaseFigureQt::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);
    QPainter pnt(this);
    QTRenderEngine gc(&pnt,0,0,width(),height());
    hfig->PaintMe(gc);
  }

  BaseFigureQt::BaseFigureQt(unsigned ahandle) :
    BaseFigure(ahandle), QWidget() {
    hfig->resizeGL(width(),height());
    char buffer[1000];
    sprintf(buffer,"Figure %d",Handle()+1);
    setWindowTitle(buffer);
  }

  class BaseFigureGL : public BaseFigure, public QGLWidget {
    float beginx, beginy;
    bool elevazim;
  public:
    BaseFigureGL(unsigned ahandle);
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    // Support dragging...
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    virtual void Show() {QWidget::show();};
  };

  BaseFigureGL::BaseFigureGL(unsigned ahandle) :
    BaseFigure(ahandle), QGLWidget() {
    hfig->resizeGL(width(),height());
    char buffer[1000];
    sprintf(buffer,"Figure %d",ahandle+1);
    setWindowTitle(buffer);
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
    qDebug("paint");
    GLRenderEngine gc(this,0,0,width(),height());
    hfig->PaintMe(gc);
  }

  void BaseFigureGL::resizeGL(int width, int height) {
    qDebug("size");
    hfig->resizeGL(width,height);
  }

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
  


HandleWindow::HandleWindow(unsigned ahandle) : QWidget() {
  handle = ahandle;
  hfig = new HandleFigure;
  child = null;
}

unsigned HandleWindow::Handle() {
  return handle;
}

HandleFigure* HandleWindow::HFig() {
  return hfig;
}

void HandleWindow::UpdateState() {
}
