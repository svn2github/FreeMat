
function p = polyfit(x,y,n)
x = x(:);
y = y(:);
if (size(x,1) ~= size(y,1))
  error('size mismatch on arguments to polyfit - they should be the same size');
end
if (n < 0 | n > 200)
  error('invalid polynomial order in polyfit - must be between [1,200] inclusive');
end
A = zeros(size(x,1),n+1);
xp = x*0.0f+1.0f;
for i=(n+1):-1:1
  A(:,i) = xp;
  xp = xp.*x;
end
p = (A\y)';
