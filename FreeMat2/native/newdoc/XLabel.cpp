//**
@Module XLABEL Plot X-axis Label Function
@@Usage
This command adds a label to the x-axis of the plot.  The general syntax
for its use is
@[
  xlabel('label')
@]
or in the alternate form
@[
  xlabel 'label'
@]
or simply
@[
  xlabel label
@]
Here @|label| is a string variable.
@@Example
Here is an example of a simple plot with a label on the $x$-axis.
@<
x = linspace(-1,1);
y = cos(2*pi*x);
plot(x,y,'r-');
xlabel('time');
mprintplot('xlabel1');
@>
which results in the following plot.
@figure xlabel1
**//
#include "Plot2D.hpp"
#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "Core.hpp"


namespace FreeMat {
  ArrayVector XLabelFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("xlabel function takes only a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("xlabel function takes only a single, string argument");
    Array t(arg[0]);
    Plot2D* f = GetCurrentPlot();
    f->SetXLabel(t.getContentsAsCString());
    return ArrayVector();
  }
}
