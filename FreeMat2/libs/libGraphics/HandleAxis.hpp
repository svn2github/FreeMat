#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>

namespace FreeMat {

  //The HandleAxis class encapsulates a 3D axis object, and is
  //manipulated through the Set/Get interface.
  class HandleAxis {
  public:
    // These are all the properties of the axix
    vector<double> bkcolor, xcolor, ycolor, zcolor;
    string xlabel, ylabel, zlabel, title;
    string gridlinestyle;
    vector<string> zticklabel, xticklabel, yticklabel;
    vector<double> position;
    bool visible;
    vector<double> xlim, ylim, zlim, clim;
    vector<double> xtick, ytick, ztick;
    string xlimmode, ylimmode, zlimmode;
    string xtickmode, ytickmode, ztickmode;
    string xticklabelmode, yticklabelmode, zticklabelmode;
    string xdir, ydir, zdir;

    HandleAxis();
    ~HandleAxis();
    void RegisterProperties();  
  };

  void LoadHandleGraphicsFunctions(Context* context);
}

#endif
