
function z = conv(x,y)
% is x a column vector
xiscol = size(x,1) > size(x,2);
yiscol = size(y,1) > size(y,2);
if (xiscol | yiscol)
  z = conv2(x(:),y(:));
else
  z = conv2(x(:).',y(:).');
end

