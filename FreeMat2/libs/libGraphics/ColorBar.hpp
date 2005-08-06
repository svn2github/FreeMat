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
    Q_OBJECT
  public slots:
    void ChangeWinLev(double win, double lev);
    void ChangeColormap(char* map);
  private:
    char m_colormap[3*256];
    double m_window;
    double m_level;
    uchar *picData;
    char m_orientation;
    std::vector<std::string> m_labels;
    std::vector<Point2D> m_pos;
    int m_colorbar_width;
    int m_pref_width;
    int m_length;
  public:
    ColorBar(QWidget* parent, char orient);
    virtual ~ColorBar();
    /**
     * Set the colormap for the image
     */
    QSize sizeHint();
    QSize minimumSizeHint();
    QSizePolicy sizePolicy();
    void SetBarLength(int length);
    void DrawMe(QPainter& gc);
    void SetHorizontalFlag(bool ishoriz);
    void resizeEvent(QResizeEvent* e);
    void UpdateImage();
    bool IsEastWest();
  };
}

#endif
