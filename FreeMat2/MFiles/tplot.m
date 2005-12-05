function h = tplot(varargin)
  % Check for the single argument case
  h = [];
  if (nargin == 1)
    Y = varargin{1};
    if (isvec(Y)) Y = Y(:); end;
    if (isreal(Y))
      n = 1:size(Y,1);
      for i=1:size(Y,2)
	h = [h,tplotvector(n,Y(:,i))];
      end
    else
      for i=1:size(Y,2)
	h = [h,tplotvector(real(Y(:,i)),imag(Y(:,i)))];
      end      
    end
  end
  if (nargin == 2)
    X = varargin{1};
    Y = varargin{2};
    if (isvec(X) & ~isvec(Y))
      X = matchmat(Y,X);
    elseif (~isvec(X) & isvec(Y))
      Y = matchmat(X,Y);
    end
    if (isvec(X)), X = X(:); end;
    if (isvec(Y)), Y = Y(:); end;
    for i=1:size(Y,2)
      h = [h,tplotvector(X(:,i),Y(:,i))];
    end
  end
    
function x = matchmat(a,b)
if (length(b) == size(a,1))
  x = repmat(b(:),[1,size(a,2)]);
elseif (length(b) == size(a,2))
  x = repmat(b(:)',[size(a,1),1]);
else
  error('plot(X,Y) where one argument is a vector requires the other argument to have a matching dimension');
end

function p = isvec(x)
p = (ndims(x) == 2) & ((size(x,1) == numel(x)) | (size(x,2) == numel(x)));

function k = tplotvector(x,y)
  a = gca;
  set(a,'color','w');
  ndx = length(get(a,'children'))+1;
  % Get the colororder
  colororder = get(a,'colororder');
  % select the row using a modulo
  ndxmod = uint32(mod(ndx-1,size(colororder,1))+1);
  k = line('xdata',x,'ydata',y,'parent',gca,'color',colororder(ndxmod,:));
  set(gca,'children',[get(gca,'children'),k]);
  if (strcmp(get(a,'xlimmode'),'auto'))
    xlim = get(a,'xlim');
    xlim(1) = min(xlim(1),min(x));
    xlim(2) = max(xlim(2),max(x));
    set(a,'xlim',xlim);
  end
  if (strcmp(get(a,'ylimmode'),'auto'))
    ylim = get(a,'ylim');
    ylim(1) = min(ylim(1),min(y));
    ylim(2) = max(ylim(2),max(y));
    set(a,'ylim',ylim);
  end

    
