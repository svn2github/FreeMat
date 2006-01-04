function h = plot(varargin)
  % Check for an axes handle
  if (nargin>=2)
    if (isnumeric(varargin{1}) & (length(varargin{1})==1) & ...
    ishandle(varargin{1},'axes'))
       handle = varargin{1}(1);
       varargin(1) = [];
       nargin = nargin - 1;
    else   
       handle = newplot;
    end
  end
  % search for the propertyname/value pairs
  propstart = 0;
  if (nargin > 2)
    propstart = nargin-1;
    while ((propstart >= 1) & isa(varargin{propstart},'string') & ...
    pvalid('line',varargin{propstart}))
      propstart = propstart - 2;
    end
    propstart = propstart + 2;
  end
  propset = {};
  if ((propstart > 0) & (propstart < nargin))
     propset = varargin(propstart:end);
  end
  h = [];
  while (~isempty(varargin))
    cs = ''; ms = ''; ps = '';
    if (nargin == 1)
      h = [h,plot_single(varargin{1},handle,propset)];
      varargin(1) = [];
    elseif (islinespec(varargin{2},cs,ms,ps))
      h = [h,plot_single(varargin{1},handle,CompleteProps(cs,ms,ps,propset))];
      varargin(1:2) = [];
    elseif (nargin ==2)
      h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
      varargin(1:2) = [];
    elseif (islinespec(varargin{3},cs,ms,ps))
      h = [h,plot_double(varargin{1},varargin{2},handle,...
           CompleteProps(cs,ms,ps,propset))];
      varargin(1:3) = [];
    else
      h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
      varargin(1:2) = [];
    end
end
    
function h = plot_single(Y,handle,lineprops)
    h = [];
    if (isvec(Y)) Y = Y(:); end;
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
    if (isvec(X) & ~isvec(Y))
      X = matchmat(Y,X);
    elseif (~isvec(X) & isvec(Y))
      Y = matchmat(X,Y);
    end
    if (isvec(X)), X = X(:); end;
    if (isvec(Y)), Y = Y(:); end;
    for i=1:size(Y,2)
      h = [h,tplotvector(handle,X(:,i),Y(:,i),lineprops)];
    end

function x = matchmat(a,b)
if (length(b) == size(a,1))
  x = repmat(b(:),[1,size(a,2)]);
elseif (length(b) == size(a,2))
  x = repmat(b(:)',[size(a,1),1]);
else
  error('plot(X,Y) where one argument is a vector requires the other argument to have a matching dimension');
end

function q = CompleteProps(cs,ms,ps,p)
q = {'color',cs,'marker',ms,'linestyle',ps,'markeredgecolor',cs,'markerfacecolor',cs,p{:}};

function p = isvec(x)
p = (ndims(x) == 2) & ((size(x,1) == numel(x)) | (size(x,2) == numel(x)));

function k = tplotvector(handle,x,y,lineprops)
  set(handle,'color','w');
  ndx = length(get(handle,'children'))+1;
  % Get the colororder
  colororder = get(handle,'colororder');
  % select the row using a modulo
  ndxmod = uint32(mod(ndx-1,size(colororder,1))+1);
  k = line('xdata',x,'ydata',y,'color',colororder(ndxmod,:),lineprops{:});

function b = islinespec(t,&colorspec,&markerspec,&linespec)
% try to parse a string out as a linespec
% a linespec consists of three parts:
%   a colorspec - y,m,c,r,g,b,w,k
%   a markerspec - +o*.xs,square,s,diamond,d,^v><
%   a linespec - -,--,:,-.
if (~isa(t,'string'))
  b = 0;
  return;
end
giveup = 0;
colorspec = '';
markerspec = '';
linespec = '';
orig_t = t;
while (~giveup & length(t)>0)
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
