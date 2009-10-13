% CONTOUR CONTOUR Contour Plot Function
% 
% Usage
% 
% This command generates contour plots.  There are several syntaxes for
% the command.  The simplest is
% 
%   contour(Z)
% 
% which generates a contour plot of the data in matrix Z, and will
% automatically select the contour levels.  The x,y coordinates of the
% contour default to 1:n and 1:m, where n is the number of
% columns and m is the number of rows in the Z matrix.  Alternately,
% you can specify a scalar n
% 
%   contour(Z,n)
% 
% which indicates that you want n contour levels.  For more control,
% you can provide a vector v containing the levels to contour.  If you
% want to generate a contour for a particular level, you must pass a
% vector [t,t] where t is the level you want to contour.  If you
% have data that lies on a particular X,Y grid, you can pass either
% vectors x,y or matrices X,Y to the contour function via
% 
%   contour(X,Y,Z)
%   contour(X,Y,Z,n)
%   contour(X,Y,Z,v)
% 
% Each form of contour can optionally take a line spec to indicate the
% color and linestyle of the contours to draw:
% 
%   contour(...,linespec)
% 
% or any of the other forms of contour.  Furthermore, you can supply an
% axis to target the contour plot to (so that it does not get added to
% the current axis, which is the default):
% 
%   contour(axis_handle,...)
% 
% Finally, the contour command returns a handle to the newly returned
% contour plot. 
% 
%   handle = contour(...)
% 
% To place labels on the contour plot, use the clabel function.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = contour(varargin)
  % Check for an axes handle 
    if (nargin>=2)
      if (isnumeric(varargin{1}) && (length(varargin{1})==1) && ...
          ishandle(varargin{1},'axes'))
        handle = varargin{1}(1);
        varargin(1) = [];
        nargin = nargin - 1;
      else
        handle = newplot;
      end
    else
      handle = newplot;
    end
    saveca = gca;
    axes(handle);
    % Have to decrypt the mode...
    % strip off the linespec, if provided
    color = []; marker = []; linetype = [];
    linespec_given = islinespec(varargin{end},color,marker,linetype);
    if (linespec_given)
      varargin(end) = [];
      nargin = nargin - 1;
    end
    if (nargin == 1)
      h = hcontour('zdata',varargin{1});
    elseif (nargin == 2)
      if (numel(varargin{2}) == 1)
        zdata = varargin{1};
        zmin = min(zdata(:));
        zmax = max(zdata(:));
        h = hcontour('zdata',varargin{1},'levellist',...
                     linspace(zmin,zmax,varargin{2}));
      else
        h = hcontour('zdata',varargin{1},'levellist',varargin{2});
      end
    elseif (nargin == 3)
      xdata = varargin{1};
      ydata = varargin{2};
      zdata = varargin{3};
      h = hcontour('xdata',xdata,'ydata',ydata,'zdata',zdata);
    elseif (nargin == 4)
      xdata = varargin{1};
      ydata = varargin{2};
      zdata = varargin{3};
      if (numel(varargin{4}) == 1)
        zmin = min(zdata(:));
        zmax = max(zdata(:));
        h = hcontour('xdata',xdata,'ydata',ydata,'zdata',zdata,'levellist',...
                     linspace(zmin,zmax,varargin{4}));
      else
        h = hcontour('xdata',xdata,'ydata',ydata,'zdata',zdata, ...
                     'levellist',varargin{4}); 
      end
    end
    if (linespec_given)
      set(h,'linestyle',linetype,'linecolor',color);
    end
    axes(saveca);
    if (nargout > 0)
      ohandle = h;
    end
    
function b = islinespec(t,&colorspec,&markerspec,&linespec)
   % try to parse a string out as a linespec
   % a linespec consists of three parts:
   %   a colorspec - y,m,c,r,g,b,w,k
   %   a markerspec - +o*.xs,square,s,diamond,d,^v><
   %   a linespec - -,--,:,-.
   if (~isa(t,'char'))
      b = 0;
      return;
   end
   giveup = 0;
   colorspec = 'none';
   markerspec = 'none';
   linespec = 'none';
   orig_t = t;
   while (~giveup && length(t)>0)
      giveup = 1;
      if (matchit(t,colorset))
         colorspec = parseit(t,colorset);
         giveup = 0;
      end;
      if (matchit(t,markerset))
         markerspec = parseit(t,markerset);
         giveup = 0;
      end
      if (matchit(t,styleset))
         linespec = parseit(t,styleset);
         giveup = 0;
      end
   end
   if (giveup)
      b = 0;
   else
      b = 1;
   end
   
function b = matchit(t,dictionary)
   b = any(stcmp(dictionary,t));
   
function b = parseit(&t,dictionary)
   n = stcmp(dictionary,t);
   b = dictionary{min(find(n))};
   t(1:length(b)) = [];
   
function c = colorset
   c = {'y','m','c','r','g','b','w','k'};
   
function c = styleset
   c = {'--',':','-.','-'};
   
function c = markerset
   c = {'+','o','*','.','x','square','s','diamond','d','^','v','>','<'};
   
function b = stcmp(source,pattern)
   b = zeros(size(source),'logical');
   for i=1:numel(source)
      b(i) = strncmp(source{i},pattern,length(source{i}));
   end
