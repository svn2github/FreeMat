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

#include "ScalarImage.hpp"
#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"

namespace FreeMat {

#define MAX_IMAGES 50

  ScalarImage* images[MAX_IMAGES];
  int currentImage;

  void CloseImageHelper(int fig) {
    if (fig == -1) return;
    if (images[fig] == NULL) return;
    CloseXWindow(images[fig]);
    images[fig] = NULL;
    if (currentImage == fig)
      currentImage = -1;
  }
    
  void InitializeImageSubsystem() {
    currentImage = -1;
    for (int i=0;i<MAX_IMAGES;i++) 
      images[i] = NULL;
  }
  
  void NewImage() {
    // First search for an unused image number
    int imageNum = 0;
    bool imageFree = false;
    while ((imageNum < MAX_IMAGES) && !imageFree) {
      imageFree = (images[imageNum] == NULL);
      if (!imageFree) imageNum++;
    }
    if (!imageFree) {
      throw Exception("No more image handles available!  Close some images...");
    }
    images[imageNum] = new ScalarImage(imageNum);
    images[imageNum]->Show();
    images[imageNum]->Raise();
    currentImage = imageNum;
  }

  void SelectImage(int imagenum) {
    if (images[imagenum] == NULL) {
      images[imagenum] = new ScalarImage(imagenum);
    }
    images[imagenum]->Show();
    images[imagenum]->Raise();
    currentImage = imagenum;
  } 

  ScalarImage* GetCurrentImage() {
    if (currentImage == -1)
      NewImage();
    images[currentImage]->Raise();
    return images[currentImage];
  }

  void NotifyImageClose(int figNum) {
    images[figNum] = NULL;
    if (currentImage == figNum)
      currentImage = -1;
  }

  //!
  //@Module NEWIMAGE New Image Window Function
  //@@Usage
  //Creates a new window for the display of images using the @|image| commands.
  //The general syntax for its use is
  //@[
  //   y = newimage
  //@]
  //where @|y| is the handle (or image number) of the newly created window.  
  //Image handles are sequential, starting with 1, unless one is closed, in 
  //which case the smallest unused handle is returned.
  //!
  ArrayVector NewImageFunction(int nargout,const ArrayVector& arg) {  
    NewImage();
    ArrayVector retval;
    retval.push_back(Array::int32Constructor(currentImage+1));
    return retval;
  }

  //!
  //@Module USEIMAGE Use Image Window Function
  //@@Usage
  //Changes the active image window to the specified handle (or image number).  
  //The general syntax for its use is 
  //@[
  //  useimage(handle)
  //@]
  //where @|handle| is the handle to use.  If the image window corresponding to
  //@|x| does not already exist, a new window with this handle number is 
  //created.
  //@@Example
  //In this example, we create two image windows, and then use the @|useimage|
  //command to activate the first window.
  //@<
  //newimage
  //newimage
  //useimage(1)
  //@>  
  //!
  ArrayVector UseImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("useimage function takes a single, integer argument");
    Array t(arg[0]);
    int fignum = t.getContentsAsIntegerScalar();
    if ((fignum<=0) || (fignum>MAX_GFX))
      throw Exception("image number is out of range - it must be between 1 and 50");
    SelectImage(fignum-1);
    return ArrayVector();
  }

  //!
  //@Module CLOSEIMAGE Close Image Window
  //@@Usage
  //Closes an image window, either the currently active window, a 
  //window with a specific handle, or all image windows.  The general
  //syntax for its use is
  //@[
  //   closeimage(handle)
  //@]
  //in which case the image window with the speicified @|handle| is
  //closed.  Alternately, issuing the command with no argument
  //@[
  //   closeimage
  //@]
  //is equivalent to closing the currently active image window.  Finally
  //the command
  //@[
  //   closeimage('all')
  //@]
  //closes all image windows currently open.
  //!
  ArrayVector CloseImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 1)
      throw Exception("closeimage takes at most one argument - either the string 'all' to close all images, or a scalar integer indicating which image is to be closed.");
    int action;
    if (arg.size() == 0) 
      action = 0;
    else {
      Array t(arg[0]);
      if (t.isString()) {
	char *allflag = t.getContentsAsCString();
	if (strcmp(allflag,"all") == 0) 
	  action = -1;
	else
	  throw Exception("string argument to closeimage function must be 'all'");
      } else {
	int handle = t.getContentsAsIntegerScalar();
	if (handle < 1)
	  throw Exception("Invalid image number argument to closeimage function");
	action = handle;
      }
    }
    if (action == 0) {
      if (currentImage != -1) 
	CloseImageHelper(currentImage);
    } else if (action == -1) {
      for (int i=0;i<MAX_IMAGES;i++)
	CloseImageHelper(i);
    } else {
      if ((action < MAX_IMAGES) && (action >= 1))
	CloseImageHelper(action-1);
    }
    FlushWindowEvents();
    return ArrayVector();
  }

  //!
  //@Module COLORMAP Image Colormap Function
  //@@Usage
  //Changes the colormap used for the display of the indexed (scalar) images in
  //the currently active image window.  The generic syntax for its use is
  //@[
  //  colormap(map)
  //@]
  //where @|map| is a 768 element array (usually organized as @|3 \times 256|),
  //which defines the RGB (Red Green Blue) coordinates for each color in the
  //colormap.
  //@@Function Internals
  //Assuming that the contents of the colormap function argument @|c| are 
  //labeled as:
  //\[
  //  c = \begin{bmatrix}
  //    r_1 & g_1 & b_1 \			\
  //    r_1 & g_2 & b_2 \			\
  //    r_1 & g_3 & b_3 \			\
  //    \vdots & \vdots & \vdots 
  //      \end{bmatrix} 
  //\]
  //then these columns for the RGB coordinates of pixel in the mapped image.
  //Assume that the image occupies the range $[a,b]$.  Then the RGB color 
  //of each pixel depends on the value $x$ via the following integer
  //\[
  //  k = 1 + \lfloor 256 \frac{x-a}{b-a} \rfloor,
  //\]
  //so that a pixel corresponding to image value $x$ will receive RGB color 
  //$[r_k,g_k,b_k]$.
  //Colormaps are generally used to pseudo color images to enhance 
  //visibility of features, etc.
  //@@Examples
  //We start by creating a smoothly varying image of a 2D Gaussian pulse.
  //@<
  //x = linspace(-1,1,512)'*ones(1,512);
  //y = x';
  //Z = exp(-(x.^2+y.^2)/0.3);
  //image(Z);
  //mprintimage('colormap1');
  //@>
  //which we display with the default (grayscale) colormap here.
  //@figure colormap1
  //
  //Next we switch to the @|copper| colormap, and redisplay the image.
  //@<
  //colormap(copper);
  //image(Z);
  //mprintimage('colormap2');
  //@>
  //which results in the following image.
  //@figure colormap2
  //
  //If we capture the output of the @|copper| command and plot it, we obtain
  //the following result:
  //@<
  //a = copper;
  //plot(a);
  //mprintplot('colormap3');
  //@>
  //@figure colormap3
  //
  //Note that in the output that each of the color components are linear functions
  //of the index, with the ratio between the red, blue and green components remaining
  //constant as a function of index.  The result is an intensity map with a copper
  //tint.  We can similarly construct a colormap of our own by defining the 
  //three components seperately.  For example, suppose we take three gaussian
  //curves, one for each color, centered on different parts of the index space:
  //@<
  //t = linspace(0,1,256);
  //A = [exp(-(t-1.0).^2/0.1);exp(-(t-0.5).^2/0.1);exp(-t.^2/0.1)]';
  //plot(A);
  //mprintplot('colormap4');
  //@>
  //@figure colormap4
  //
  //The resulting image has dark bands in it near the color transitions.
  //@<
  //image(Z);
  //colormap(A);
  //mprintimage('colormap5');
  //@>
  //@figure colormap5
  //
  //These dark bands are a result of the nonuniform color intensity, which 
  //we can correct for by renormalizing each color to have the same norm.
  //@<
  //w = sqrt(sum(A'.^2));
  //sA = diag(1./w)*A;
  //plot(A);
  //mprintplot('colormap6');
  //@>
  //@figure colormap6
  //
  //The resulting image has no more dark bands.
  //@<
  //image(Z);
  //colormap(A);
  //mprintimage('colormap7');
  //@>
  //@figure colormap7
  //!
  ArrayVector ColormapFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("colormap takes a single argument");
    Array t(arg[0]);
    t.promoteType(FM_DOUBLE);
    if (t.getLength() != 256*3)
      throw Exception("Colormap argument must have exactly 768 elements");
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetColormap(t);
    return ArrayVector();
  }

  //!
  //@Module SIZEIMAGE Set Size of an Image Window
  //@@Usage
  //The @|sizeimage| function changes the size of the currently
  //selected image window.  The general syntax for its use is
  //@[
  //   sizeimage(width,height)
  //@]
  //where @|width| and @|height| are the dimensions of the image
  //window.
  //!
  ArrayVector SizeImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("sizeimage function takes two arguments: height and width");
    Array w(arg[1]);
    Array h(arg[0]);
    int width;
    int height;
    width = w.getContentsAsIntegerScalar();
    height = h.getContentsAsIntegerScalar();
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetSize(width,height);
    return ArrayVector();
  }

  //!
  //@Module WINLEV Image Window-Level Function
  //@@Usage
  //Adjusts the data range used to map the current image to the current
  //colormap.  The general syntax for its use is
  //@[
  //  winlev(window,level)
  //@]
  //where @|window| is the new window, and @|level| is the new level, or
  //@[
  //  winlev
  //@]
  //in which case it returns a vector containing the current window
  //and level for the active image.
  //@@Function Internals
  //FreeMat deals with scalar images on the range of @|[0,1]|, and must
  //therefor map an arbitrary image @|x| to this range before it can
  //be displayed.  By default, the @|image| command chooses 
  //\[
  //  \mathrm{window} = \max x - \min x,
  //\]
  //and
  //\[
  //  \mathrm{level} = \frac{\mathrm{window}}{2}
  //\]
  //This ensures that the entire range of image values in @|x| are 
  //mapped to the screen.  With the @|winlev| function, you can change
  //the range of values mapped.  In general, before display, a pixel @|x|
  //is mapped to @|[0,1]| via:
  //\[
  //   \max\left(0,\min\left(1,\frac{x - \mathrm{level}}{\mathrm{window}}
  //   \right)\right)
  //\]
  //@@Examples
  //The window level function is fairly easy to demonstrate.  Consider
  //the following image, which is a Gaussian pulse image that is very 
  //narrow:
  //@<
  //t = linspace(-1,1,256);
  //xmat = ones(256,1)*t; ymat = xmat';
  //A = exp(-(xmat.^2 + ymat.^2)*100);
  //image(A);
  //mprintimage('winlev1');
  //@>
  //The data range of @|A| is @|[0,1]|, as we can verify numerically:
  //@<
  //min(A(:))
  //max(A(:))
  //@>
  //To see the tail behavior, we use the @|winlev| command to force FreeMat
  //to map a smaller range of @|A| to the colormap.
  //@<
  //image(A);
  //winlev(1e-4,0.5e-4)
  //mprintimage('winlev2');
  //@>
  //The result is a look at more of the tail behavior of @|A|.
  //We can also use the winlev function to find out what the
  //window and level are once set, as in the following example.
  //@<
  //image(A);
  //winlev(1e-4,0.5e-4)
  //winlev
  //@>
  //!
  ArrayVector WinLevFunction(int nargout, const ArrayVector& arg) {
    ScalarImage *f;
    if (arg.size() == 0) {
      f = GetCurrentImage();
      double *dp = (double*) Malloc(sizeof(double)*2);
      dp[0] = f->GetCurrentWindow();
      dp[1] = f->GetCurrentLevel();
      Dimensions dim(2);
      dim[0] = 1;
      dim[1] = 2;
      Array ret(FM_DOUBLE,dim, dp);
      ArrayVector retvec;
      retvec.push_back(ret);
      return retvec;
    } else if (arg.size() != 2)
      throw Exception("Winlev function takes two, real arguments.");
    Array win(arg[0]);
    Array lev(arg[1]);
    double window, level;
    window = win.getContentsAsDoubleScalar();
    if (window == 0)
      throw Exception("zero is not a valid choice for the window");
    level = lev.getContentsAsDoubleScalar();
    f = GetCurrentImage();
    f->WindowLevel(window,level);
    return ArrayVector();
  }

  //!
  //@Module IMAGE Image Display Function
  //@@Usage
  //Displays a scalar (grayscale or colormapped) image on the 
  //currently active image window.  The general syntax for its use is
  //@[
  //  image(x,zoom)
  //@]
  //where @|x| is a 2-Dimensional numeric matrix containing the
  //image to be displayed.  The data is autoscaled by default to occupy
  //the entire colormap or grayscale-space.  The image must be real valued,
  //but can be non-square.  The second argument @|zoom| is optional, and
  //specifies the initial zoom of the image.  For more details of how the
  //zoom works, see the @|zoom| command.
  //@@Example
  //In this example, we create an image that is @|512 x 512| pixels
  //square, and set the background to a noise pattern.  We set the central
  //@|128 x 256| pixels to be white.
  //@<
  //x = rand(512);
  //x((-64:63)+256,(-128:127)+256) = 1.0;
  //newimage
  //image(x)
  //mprintimage image1
  //@>
  //
  //The resulting image looks like:
  //@figure image1
  //!
  ArrayVector ImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("image function requires at least one argument");
    if (arg.size() > 2)
      throw Exception("image function takes at most two arguments (image and zoom factor)");
    Array img;
    img = arg[0];
    if (img.isEmpty())
      throw Exception("argument image is empty");
    if (!img.is2D())
      throw Exception("argument to image function must be 2D");
    if (img.isReferenceType())
      throw Exception("argument to image function must be a data type");
    if (img.isComplex())
      throw Exception("argument to image function must be real");
    img.promoteType(FM_DOUBLE);
    ScalarImage *f;
    f = GetCurrentImage();
    double zoomfact = 1.0f;
    if (arg.size() == 2) {
      Array z(arg[1]);
      zoomfact = z.getContentsAsDoubleScalar();
    }
    f->SetImageArray(img,zoomfact);
    return ArrayVector();
  }
  
  //!
  //@Module PRINTIMAGE Print an Image To A File
  //@@Usage
  //This function ``prints'' the currently active image to a file.  The 
  //generic syntax for its use is
  //@[
  //  printimage(filename)
  //@]
  //or, alternately,
  //@[
  //  printimage filename
  //@]
  //where @|filename| is the (string) filename of the destined file.  The current
  //image is then saved to the output file using a format that is determined
  //by the extension of the filename.  The exact output formats may vary on
  //different platforms, but generally speaking, the following extensions
  //should be supported cross-platform:
  //\begin{itemize}
  //\item @|jpg|, @|jpeg|  --  JPEG file 
  //\item @|ps|, @|eps| -- Encapsulated Postscript file 
  //\item @|png| -- Portable Net Graphics file
  //\end{itemize}
  //Note that only the image is printed, not the window displaying
  //the image.  If you want something like that (essentially a window-capture)
  //use a seperate utility or your operating system's built in screen
  //capture ability.
  //@@Example
  //Here is a simple example of how the figures in this manual are generated.
  //@<
  //x = linspace(-1,1,512)'*ones(1,512);
  //y = x';
  //Z = exp(-(x.^2+y.^2)/0.3);
  //image(Z);
  //printimage printimage1.eps
  //printimage printimage1.jpg
  //mprintimage printimg1
  //@>
  //which creates two images @|printimage1.eps|, which is an Encapsulated
  //Postscript file, and @|printimage1.jpg| which is a JPEG file.
  //
  //@figure printimg1
  //!
  ArrayVector PrintImageFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("printimage function takes a single, string argument");
    if (!(arg[0].isString()))
      throw Exception("printimage function takes a single, string argument");
    Array t(arg[0]);
    ScalarImage* f = GetCurrentImage();
    f->PrintMe(t.getContentsAsCString());
    return ArrayVector();
  }

  //!
  //@Module ZOOM Image Zoom Function
  //@@Usage
  //This function changes the zoom factor associated with the currently active
  //image.  The generic syntax for its use is
  //@[
  //  zoom(x)
  //@]
  //where @|x| is the zoom factor to be used.  The exact behavior of the zoom
  //factor is as follows:
  //\begin{itemize}
  //\item @|x>0| The image is zoomed by a factor @|x| in both directions.
  //\item @|x=0| The image on display is zoomed to fit the size of the image window, but
  //  the aspect ratio of the image is not changed.  (see the Examples section for
  //more details).  This is the default zoom level for images displayed with the
  //@|image| command.
  //\item @|x<0| The image on display is zoomed to fit the size of the image window, with
  //  the zoom factor in the row and column directions chosen to fill the entire window.
  //  The aspect ratio of the image is not preserved.  The exact value of @|x| is
  //  irrelevant.
  //\end{itemize}
  //@@Example
  //To demonstrate the use of the @|zoom| function, we create a rectangular image 
  //of a Gaussian pulse.  We start with a display of the image using the @|image|
  //command, and a zoom of 1.
  //@<
  //x = linspace(-1,1,300)'*ones(1,600);
  //y = ones(300,1)*linspace(-1,1,600);
  //Z = exp(-(x.^2+y.^2)/0.3);
  //image(Z);
  //zoom(1.0);
  //mprintimage zoom1
  //@>
  //@figure zoom1
  //
  //At this point, resizing the window accomplishes nothing, as with a zoom factor 
  //greater than zero, the size of the image is fixed.
  //
  //If we change the zoom to another factor larger than 1, we enlarge the image by
  //the specified factor (or shrink it, for zoom factors @|0 < x < 1|.  Here is the
  //same image zoomed out to 60%
  //@<
  //image(Z);
  //zoom(0.6);
  //mprintimage zoom3
  //@>
  //@figure zoom3
  //
  //Similarly, we can enlarge it to 130%
  //@<
  //image(Z)
  //zoom(1.3);
  //mprintimage zoom4
  //@>
  //@figure zoom4
  //
  //The ``free'' zoom of @|x = 0| results in the image being zoomed to fit the window
  //without changing the aspect ratio.  The image is zoomed as much as possible in
  //one direction.
  //@<
  //image(Z);
  //zoom(0);
  //sizeimage(200,400);
  //mprintimage zoom5
  //@>
  //@figure zoom5
  //
  //The case of a negative zoom @|x < 0| results in the image being scaled arbitrarily.
  //This allows the image aspect ratio to be changed, as in the following example.
  //@<
  //image(Z);
  //zoom(-1);
  //sizeimage(200,400);
  //mprintimage zoom6
  //@>
  //@figure zoom6
  //!
  ArrayVector ZoomFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Zoom function takes a single, real argument.");
    Array fact(arg[0]);
    fact.promoteType(FM_FLOAT);
    ScalarImage *f;
    f = GetCurrentImage();
    f->Zoom(fact.getContentsAsDoubleScalar());
    return ArrayVector();
  }
  
  //!
  //@Module POINT Image Point Information Function
  //@@Usage
  //Returns information about the currently displayed image based on a use
  //supplied mouse-click.  The general syntax for its use is
  //@[
  //   y = point
  //@]
  //The returned vector @|y| has three elements: 
  //\[
  //  y= [r,c,v]
  //\]
  //where @|r,c| are the row and column coordinates of the scalar image selected
  //by the user, and @|v| is the value of the scalar image at that point.  Image
  //zoom is automatically compensated for, so that @|r,c| are the coordinates into
  //the original matrix.  They will generally be fractional to account for the
  //exact location of the mouse click.
  //!
  ArrayVector PointFunction(int nargout, const ArrayVector& arg) {
    ScalarImage *f;
    f = GetCurrentImage();
    ArrayVector retval;
    retval.push_back(f->GetPoint());
    return retval;
  }
}
