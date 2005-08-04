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
    }
  
  void ColorBar::resizeEvent(QResizeEvent* e) {
    UpdateImage();
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
  
  void ColorBar::UpdateImage() {
    delete picData;
    int picHeight = height();
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
    int numlabels = height()/75;
    int maxwidth = 0;
    //    QPainter p(this);
    for (int i;i<numlabels;i++) {
    char buffer[1000];
    sprintf(buffer,"%f",m_level-m_window/2.0+i/(numlabels-1)*m_window);
    m_labels.push_back(buffer);
    m_pos.push_back(Point2D(m_colorbar_width+10,i/(numlabels-1.0)*height()));
    //    Point2D dim(GetTextExtent(p,m_labels.back()));
    //    maxwidth = (maxwidth > dim.x) ? maxwidth : dim.x;
    }
    //    setMinimumSize(maxwidth+m_colorbar_width+10,height());
  }
  
  ColorBar::~ColorBar() {
    delete picData;
  }
  
  void ColorBar::DrawMe(QPainter& gc) {
    // Draw the colorbar
    BlitImage(gc, picData, m_colorbar_width, height(), 0, 0);
    // Draw the labels
    for (unsigned int i=0;i<m_labels.size();i++) 
      DrawTextStringAligned(gc,m_labels[i],m_pos[i],LR_LEFT,TB_CENTER,0);
  }
}
