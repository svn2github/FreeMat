
function y = polyval(p,x)
y = p(1);
n = prod(size(p));
for i = 2:n
  y = y.*x + p(i);
end
