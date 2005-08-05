#include "ColorBar.hpp"
#include "Util.hpp"

namespace FreeMat {
  
  ColorBar::ColorBar(QWidget* parent) 
    : QPWidget(parent, "colorbar") {
      for (int i=0;i<256;i++) {
	m_colormap[0][i] = i;
	m_colormap[1][i] = i;
	m_colormap[2][i] = i;
      }
      picData = NULL;
      m_colorbar_width = 20;
      m_ishoriz = false;
      m_window = 1;
      m_level = 0.5;
      m_pref_width = 0;
      m_height = 300;
    }
  
  QSizePolicy ColorBar::sizePolicy() {
    return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  }

  void ColorBar::resizeEvent(QResizeEvent* e) {
    SetBarHeight(height());
  }
  
  void ColorBar::WindowLevel(double window, double level) {
    m_window = window;
    m_level = level;
    UpdateImage();
  }
  
  void ColorBar::SetColormap(Array &dp) {
    dp.promoteType(FM_DOUBLE);
    double *dpdat;
    dpdat = (double*) dp.getDataPointer();
    for (int j=0;j<3;j++)
      for (int i=0;i<256;i++)
	m_colormap[j][i] = (char)(255.0*(*dpdat++));
    UpdateImage();
  }

  void ColorBar::SetBarHeight(int height) {
    if (m_height != height) {
      m_height = height;
      UpdateImage();
    }
  }
  
  void ColorBar::UpdateImage() {
    delete picData;
    int picHeight = m_height;
    picData = new uchar[picHeight*m_colorbar_width*3];
    uchar *op = picData;
    for (int i=0;i<m_colorbar_width;i++) {
      for (int j=0;j<picHeight;j++) {
	int dv;
	int ndx;
	dv = j/(picHeight - 1.0)*255;
	if (!m_ishoriz)
	  ndx = 3*(i+m_colorbar_width*j);
	else
	  ndx = 3*(j+picHeight*i);
	op[ndx++] = m_colormap[0][dv];
	op[ndx++] = m_colormap[1][dv];
	op[ndx++] = m_colormap[2][dv];
      }
    }
    m_labels.clear();
    m_pos.clear();
    int numlabels = m_height/75;
    if (numlabels < 2) numlabels = 2;
    int maxwidth = 0;
    for (int i=0;i<numlabels;i++) {
    char buffer[1000];
    sprintf(buffer,"%f",m_level-m_window/2.0+i/(numlabels-1.0)*m_window);
    m_labels.push_back(buffer);
    m_pos.push_back(Point2D(m_colorbar_width+10,i/(numlabels-1.0)*m_height));
    std::cout << "label " << m_labels.back() << " at " << m_pos.back().x << "," << m_pos.back().y << "\n";
    Point2D dim(GetTextExtentNoGC(m_labels.back(),12));
    maxwidth = (maxwidth > dim.x) ? maxwidth : dim.x;
    }
    m_pref_width = maxwidth+m_colorbar_width;
    setMinimumSize(m_pref_width, 100);
    //    updateGeometry(); 
    //    update();
  }

  QSize ColorBar::sizeHint() {
    return QSize(m_pref_width, m_height);
  }

  QSize ColorBar::minimumSizeHint() {
    return QSize(m_pref_width, m_height);
  }
  
  ColorBar::~ColorBar() {
    delete picData;
  }
  
  void ColorBar::DrawMe(QPainter& gc) {
    // Draw the colorbar
    int offset = (height() - m_height)/2;
    BlitImage(gc, picData, m_colorbar_width, m_height, 0, offset);
    // Draw the labels
    gc.setPen(Qt::black);
    for (unsigned int i=0;i<m_labels.size();i++) {
      Point2D mpos(m_pos[i]);
      mpos.y += offset;
      if (i==0)
	DrawTextStringAligned(gc,m_labels[i],mpos,LR_LEFT,TB_TOP,0);
      else if (i==m_labels.size()-1)
	DrawTextStringAligned(gc,m_labels[i],mpos,LR_LEFT,TB_BOTTOM,0);
      else
	DrawTextStringAligned(gc,m_labels[i],mpos,LR_LEFT,TB_CENTER,0);
    }
  }
}
