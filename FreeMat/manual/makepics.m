% Core.cpp
x = linspace(-pi,pi);
y = cos(x);
plot(x,y);
xlabel('x');
ylabel('y');
grid('on');
title('Plot of Cosine Function');
print('jpeg','cos_pic1.jpg');
print('eps','cos_pic1.eps');
