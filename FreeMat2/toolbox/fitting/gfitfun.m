function y = gfitfun(x,times)
y = x(4)*exp(-((times-x(1)).^2)/(2*x(2)^2)) + x(3);
