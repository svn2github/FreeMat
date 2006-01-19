  //!
  //@Module LEGEND Add Legend to Plot
  //@@Section PLOT
  //@@Usage
  //This command adds a legent to the current plot.  The general
  //syntax for its use is
  //@[
  //  legend(x,y,boxstyle,linestyle1,label1,linestyle2,label2,...)
  //@]
  //where @|x| and @|y| are the coordinates of the upper left 
  //corner of the legend box, @|boxstyle| is the style in which 
  //to draw the box around the legend (or empty quotes '' for no
  //box).  These mandatory parameters are followed by alternating
  //lists of linestyles and labels.  Each linestyle argument must
  //be a valid linestyle as taken by the @|plot| command.  The
  //label must be a string to place next to the linestyle.  This format
  //for the @|legend| command allows you to assemble a plot in various
  //ways (e.g., using the @|hold| command), and then add a legend with
  //the labels in the order you choose.  Of course, if you choose to
  //use @|plot| to automatically assign line colors, you will need to
  //know that colors are are assigned round-robin in the order:
  // 'r','g','b','k','c','m','y','r','g','b',etc...
  //@<
  //x = linspace(-1,1);
  //plot(x,cos(x*2*pi),'rx-',x,sin(x*4*pi),'go');
  //legend(0.25,0.60,'b-','rx-','First Harmonic','go','Second Harmonic');
  //mprintplot legend1
  //@>
  //which results in the following plot.
  //@figure legend1
  //!
