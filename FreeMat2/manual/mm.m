x = linspace(-pi,pi);
y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
plot(y(:,1),'r:',y(:,2),'b;',y(:,3),'g|');
printplot plot4.eps
printplot plot4.jpg

