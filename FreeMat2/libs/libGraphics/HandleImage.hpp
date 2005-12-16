#ifndef __HandleImage_hpp__
#define __HandleImage_hpp__

#include "HandleObject.hpp"
#include <qimage.h>

namespace FreeMat {
  class HandleImage : public HandleObject {
  protected:
    QImage img;
    void PrepImageRGBNoAlphaMap(const double *dp, int rows, int cols,
				std::vector<double> &alpha);
    double* RGBExpandImage(const double *dp, int rows, int cols);
    std::vector<double> GetAlphaMap(int rows, int cols);
  public:
    HandleImage();
    virtual ~HandleImage();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine& gc);
    std::vector<double> GetLimits();
    void UpdateCAlphaData();
  };
}

#endif
