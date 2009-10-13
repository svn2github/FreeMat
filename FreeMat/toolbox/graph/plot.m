% PLOT PLOT Plot Function
% 
% Usage
% 
% This is the basic plot command for FreeMat.  The general syntax for its
% use is
% 
%   plot(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
% 
% where the <data> arguments can have various forms, and the
% linespec arguments are optional.  We start with the
% <data> term, which can take on one of multiple forms:
%   -  Vector Matrix Case -- In this case the argument data is a pair
%     of variables.  A set of x coordinates in a numeric vector, and a 
%     set of y coordinates in the columns of the second, numeric matrix.
%     x must have as many elements as y has columns (unless y
%     is a vector, in which case only the number of elements must match).  Each
%     column of y is plotted sequentially against the common vector x.
% 
%   -  Unpaired Matrix Case -- In this case the argument data is a 
%     single numeric matrix y that constitutes the y-values
%     of the plot.  An x vector is synthesized as x = 1:length(y),
%     and each column of y is plotted sequentially against this common x
%     axis.
% 
%   -  Complex Matrix Case -- Here the argument data is a complex
%     matrix, in which case, the real part of each column is plotted against
%     the imaginary part of each column.  All columns receive the same line
%     styles.
% 
% Multiple data arguments in a single plot command are treated as a \emph{sequence}, meaning
% that all of the plots are overlapped on the same set of axes.
% The linespec is a string used to change the characteristics of the line.  In general,
% the linespec is composed of three optional parts, the colorspec, the 
% symbolspec and the linestylespec in any order.  Each of these specifications
% is a single character that determines the corresponding characteristic.  First, the 
% colorspec:
%   -  'b' - Color Blue
% 
%   -  'g' - Color Green
% 
%   -  'r' - Color Red
% 
%   -  'c' - Color Cyan
% 
%   -  'm' - Color Magenta
% 
%   -  'y' - Color Yellow
% 
%   -  'k' - Color Black
% 
% The symbolspec specifies the (optional) symbol to be drawn at each data point:
%   -  '.' - Dot symbol
% 
%   -  'o' - Circle symbol
% 
%   -  'x' - Times symbol
% 
%   -  '+' - Plus symbol
% 
%   -  '*' - Asterisk symbol
% 
%   -  's' - Square symbol
% 
%   -  'd' - Diamond symbol
% 
%   -  'v' - Downward-pointing triangle symbol
% 
%   -  '^' - Upward-pointing triangle symbol
% 
%   -  '<' - Left-pointing triangle symbol
% 
%   -  '>' - Right-pointing triangle symbol
% 
% The linestylespec specifies the (optional) line style to use for each data series:
%   -  '-' - Solid line style
% 
%   -  ':' - Dotted line style
% 
%   -  '-.' - Dot-Dash-Dot-Dash line style
% 
%   -  '--' - Dashed line style
% 
% For sequences of plots, the linespec is recycled with color order determined
% by the properties of the current axes.  You can also use the properties
% argument to specify handle properties that will be inherited by all of the plots
% generated during this event.  Finally, you can also specify the handle for the
% axes that are the target of the plot operation.
% 
%   handle = plot(handle,...)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = plot(varargin)
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
   % search for the propertyname/value pairs
   propstart = 0;
   if (nargin > 2)
      propstart = nargin-1;
      while ((propstart >= 1) && isa(varargin{propstart},'char') && ...
         pvalid('line',varargin{propstart}))
         propstart = propstart - 2;
      end
      propstart = propstart + 2;
   end
   propset = {};
   if ((propstart > 0) && (propstart < nargin))
	propset = varargin(propstart:end);
	varargin(propstart:end) = [];
   end
   h = [];
   while (~isempty(varargin))
      cs = ''; ms = ''; ps = '';
      if (length(varargin) == 1)
         h = [h,plot_single(varargin{1},handle,propset)];
         varargin(1) = [];
      elseif (islinespec(varargin{2},cs,ms,ps))
         h = [h,plot_single(varargin{1},handle,completeprops(cs,ms,ps,propset))];
         varargin(1:2) = [];
      elseif (length(varargin) ==2)
         h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
         varargin(1:2) = [];
      elseif (islinespec(varargin{3},cs,ms,ps))
         h = [h,plot_double(varargin{1},varargin{2},handle,...
         completeprops(cs,ms,ps,propset))];
         varargin(1:3) = [];
      else
         h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
         varargin(1:2) = [];
      end
   end
   axes(saveca);
   if (nargout > 0)
     ohandle = h;
   end
   
function h = plot_single(Y,handle,lineprops)
   h = [];
   if (isvector(Y)) Y = Y(:); end;
   if (isreal(Y))
      n = 1:size(Y,1);
      for i=1:size(Y,2)
         h = [h,tplotvector(handle,n,Y(:,i),lineprops)];
      end
   else
      for i=1:size(Y,2)
         h = [h,tplotvector(handle,real(Y(:,i)),imag(Y(:,i)),lineprops)];
      end      
   end
   
function h = plot_double(X,Y,handle,lineprops)
   h = [];
   [X,Y]=matchmat(X,Y);
   if (isvector(X)) X = X(:); end;
   if (isvector(Y)) Y = Y(:); end;
   for i=1:size(Y,2)
      h = [h,tplotvector(handle,X(:,i),Y(:,i),lineprops)];
   end
   
function [a,b] = matchmat(a,b)
   if (isvector(a) && ~isvector(b))
      if (length(a)==size(b,1))
         a = repmat(a(:),[1,size(b,2)]);
         return
      else
         if (length(a) == size(b,2))
            b=b';
            a = repmat(a(:)',[size(b,2),1])';
            return
         else
            error('plot(X,Y) dimensions do not match');
         end
      end
   end
      
   if (~isvector(a) && isvector(b))
      if (length(b) == size(a,1))
         b = repmat(b(:),[1,size(a,2)]);
      else
         if (length(b) == size(a,2))
            a=a';
            b = repmat(b(:)',[size(a,2),1])';
         else
            error('plot(X,Y) where one argument is a vector requires the other argument to have a matching dimension');
         end
      end
   end
   
   
function k = tplotvector(handle,x,y,lineprops)
   ndx = length(get(handle,'children'))+1;
   % Get the colororder
   colororder = get(handle,'colororder');
   % select the row using a modulo
   ndxmod = round(mod(ndx-1,size(colororder,1))+1);
   if (~any(strcmp(lineprops,'color')))
     lineprops = [lineprops,{'markeredgecolor',colororder(ndxmod,:),'markerfacecolor',colororder(ndxmod,:)}];
   end
   k = hline('xdata',x,'ydata',y,'color',colororder(ndxmod,:),lineprops{:});
   
