// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef __Plot3D_hpp__
#define __Plot3D_hpp__

#include "DataSet2D.hpp"
#include "Axis.hpp"
#include "XPWidget.hpp"

namespace FreeMat {

  typedef std::vector<DataSet3D> DataSet3DVector;
  
  class Plot3D: public XPWidget {
  public:
    /**
     * Default constructor.
     */
    Plot3D();
    /**
     * Default destructor.
     */
    ~Plot3D();
    void OnMouseDown(int x, int y);
    void OnMouseUp(int x, int y);
    void OnDrag(int x, int y);
    void OnDraw(GraphicsContext &gc);
  private:
    DataSet3DVector data;
    /**
     * A characteristic length in the construction of the plot.
     */
    int space;
    float quat[4];
    int beginx;
    int beginy;
    Axis *xAxis;
    Axis *yAxis;
    Axis *zAxis;
  };
}

#endif  
