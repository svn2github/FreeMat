
function t = dec2bin(x,n)
  x = x(:); 
  if (~exist('n') && max(x) > 0)
    n = ceil(log2(max(x)));
  elseif (~exist('n'))
    t = zeros(size(x));
    return;
  elseif (max(x) == 0)
    t = zeros(size(x));
    return;
  end
  t = string(int2bin(x,n)+'0');
