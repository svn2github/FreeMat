#ifndef __ColorBar_hpp__
#define __ColorBar_hpp__

#include "QPWidget.hpp"
#include <vector>
#include <string>
#include "Point2D.hpp"
#include "Array.hpp"

namespace FreeMat {
  /**
   *  Widget to display a colormap
   */
  class ColorBar : public QPWidget {
    char m_colormap[3][256];
    double m_window;
    double m_level;
    uchar *picData;
    bool m_ishoriz;
    std::vector<std::string> m_labels;
    std::vector<Point2D> m_pos;
    int m_colorbar_width;
    int m_pref_width;
    int m_height;
  public:
    ColorBar(QWidget* parent);
    virtual ~ColorBar();
    /**
     * Set the colormap for the image
     */
    QSize sizeHint();
    QSize minimumSizeHint();
    QSizePolicy sizePolicy();
    void SetColormap(Array &dp);
    void SetBarHeight(int height);
    void DrawMe(QPainter& gc);
    void WindowLevel(double window, double level);
    void SetHorizontalFlag(bool ishoriz);
    void resizeEvent(QResizeEvent* e);
    void UpdateImage();
  };
}

#endif
