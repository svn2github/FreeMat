#include "ColorBar.hpp"
#include "Util.hpp"

namespace FreeMat {
  
  ColorBar::ColorBar(QWidget* parent, char orient) 
    : QPWidget(parent, "colorbar") {
      for (int i=0;i<256;i++) {
	m_colormap[0+i] = i;
	m_colormap[1*256+i] = i;
	m_colormap[2*256+i] = i;
      }
      picData = NULL;
      m_colorbar_width = 20;
      m_window = 1;
      m_level = 0.5;
      m_pref_width = 0;
      m_length = 300;
      m_orientation = orient;
    }
  
  QSizePolicy ColorBar::sizePolicy() {
    return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  }

  void ColorBar::resizeEvent(QResizeEvent* e) {
    if (IsEastWest())
      SetBarLength(height());
    else
      SetBarLength(width());      
  }
  
  void ColorBar::ChangeWinLev(double window, double level) {
    m_window = window;
    m_level = level;
    UpdateImage();
  }
  
  void ColorBar::ChangeColormap(char *map) {
    memcpy(m_colormap,map,3*256*sizeof(char));
    UpdateImage();
  }

  void ColorBar::SetBarLength(int length) {
    if (m_length != length) {
      m_length = length;
      UpdateImage();
    }
  }
  
  void ColorBar::UpdateImage() {
    delete picData;
    int picHeight = m_length;
    picData = new uchar[picHeight*m_colorbar_width*3];
    if (IsEastWest()) {
      uchar *op = picData;
      for (int i=0;i<m_colorbar_width;i++) {
	for (int j=0;j<picHeight;j++) {
	  int dv;
	  int ndx;
	  dv = (picHeight - 1 - j)/(picHeight - 1.0)*255;
	  ndx = 3*(i+m_colorbar_width*j);
	  op[ndx++] = m_colormap[0+dv];
	  op[ndx++] = m_colormap[1*256+dv];
	  op[ndx++] = m_colormap[2*256+dv];
	}
      }
      m_labels.clear();
      m_pos.clear();
      int numlabels = m_length/75;
      if (numlabels < 2) numlabels = 2;
      int maxwidth = 0;
      for (int i=0;i<numlabels;i++) {
	m_labels.push_back(TrimPrint(m_level-m_window/2.0+
				     (numlabels-1.0-i)/(numlabels-1.0)*m_window,
				     false));
	m_pos.push_back(Point2D(m_colorbar_width+10,
				i/(numlabels-1.0)*m_length));
	Point2D dim(GetTextExtentNoGC(m_labels.back(),12));
	maxwidth = (maxwidth > dim.x) ? maxwidth : dim.x;
      }
      m_pref_width = maxwidth+m_colorbar_width+10;
      setMinimumSize(m_pref_width, 100);
    } else {
      uchar *op = picData;
      for (int i=0;i<m_colorbar_width;i++) {
	for (int j=0;j<picHeight;j++) {
	  int dv;
	  int ndx;
	  dv = j/(picHeight - 1.0)*255;
	  ndx = 3*(j+picHeight*i);
	  op[ndx++] = m_colormap[0+dv];
	  op[ndx++] = m_colormap[1*256+dv];
	  op[ndx++] = m_colormap[2*256+dv];
	}
      }
      m_labels.clear();
      m_pos.clear();
      int numlabels = m_length/75;
      if (numlabels < 2) numlabels = 2;
      int maxwidth = 0;
      for (int i=0;i<numlabels;i++) {
	m_labels.push_back(TrimPrint(m_level-m_window/2.0+
				     i/(numlabels-1.0)*m_window,
				     false));
	m_pos.push_back(Point2D(i/(numlabels-1.0)*m_length,
				m_colorbar_width+10));
	Point2D dim(GetTextExtentNoGC(m_labels.back(),12));
	maxwidth = (maxwidth > dim.y) ? maxwidth : dim.y;
      }
      m_pref_width = maxwidth+m_colorbar_width+10;
      setMinimumSize(100, m_pref_width);
    }
    update();
  }

  QSize ColorBar::sizeHint() {
    if (IsEastWest())
      return QSize(m_pref_width, m_length);
    else
      return QSize(m_length, m_pref_width);
  }

  QSize ColorBar::minimumSizeHint() {
    if (IsEastWest())
      return QSize(m_pref_width, m_length);
    else
      return QSize(m_length, m_pref_width);
  }
  
  bool ColorBar::IsEastWest() {
    return ((m_orientation == 'e') || (m_orientation == 'w'));
  }

  ColorBar::~ColorBar() {
    delete picData;
  }
  
  void ColorBar::DrawMe(QPainter& gc) {
    if (IsEastWest()) {
      // Draw the colorbar
      int offset = (height() - m_length)/2;
      BlitImage(gc, picData, m_colorbar_width, m_length, 0, offset);
      // Draw the labels
      gc.setPen(Qt::black);
      SetFontSize(gc,12);
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
    } else {
      // Draw the colorbar
      int offset = (width() - m_length)/2;
      BlitImage(gc, picData, m_length, m_colorbar_width, offset, 0);
      // Draw the labels
      gc.setPen(Qt::black);
      SetFontSize(gc,12);
      for (unsigned int i=0;i<m_labels.size();i++) {
	Point2D mpos(m_pos[i]);
	mpos.x += offset;
	if (i==0)
	  DrawTextStringAligned(gc,m_labels[i],mpos,LR_LEFT,TB_TOP,0);
	else if (i==m_labels.size()-1)
	  DrawTextStringAligned(gc,m_labels[i],mpos,LR_RIGHT,TB_TOP,0);
	else
	  DrawTextStringAligned(gc,m_labels[i],mpos,LR_CENTER,TB_TOP,0);
      }
    }
  }
}
