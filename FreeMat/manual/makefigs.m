% Copyright (c) 2002, 2003 Samit Basu
%
% Permission is hereby granted, free of charge, to any person obtaining a 
% copy of this software and associated documentation files (the "Software"), 
% to deal in the Software without restriction, including without limitation 
% the rights to use, copy, modify, merge, publish, distribute, sublicense, 
% and/or sell copies of the Software, and to permit persons to whom the 
% Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included 
% in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
% OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
% THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
% DEALINGS IN THE SOFTWARE.
closeplot all
closeimage all
%% LOG plot
x = linspace(1,100);
plot(x,log(x));
grid('on');
xlabel('x');
ylabel('log(x)');
printplot logplot.eps
printplot logplot.jpg
%% EXP plot1
x = linspace(-1,1);
plot(x,exp(x));
grid('on');
xlabel('x');
ylabel('exp(x)');
printplot expplot1.eps
printplot expplot1.jpg
x = linspace(-1,1);
plot(exp(-i*x*2*pi));
grid('on');
xlabel('Real part of exp(-2*pi*i*x)');
ylabel('Imaginary part of exp(-2*pi*i*x)');
printplot expplot2.eps
printplot expplot2.jpg
%% Cos plot
x = linspace(0,1);
plot(x,cos(2*pi*x));
grid('on');
xlabel('x');
ylabel('cos(2*pi*x)');
printplot cosplot.eps
printplot cosplot.jpg
%% Sin plot
x = linspace(0,1);
plot(x,sin(2*pi*x));
grid('on');
xlabel('x');
ylabel('sin(2*pi*x)');
printplot sinplot.eps
printplot sinplot.jpg
%% Tan plot
x = linspace(-1,1);
plot(x,tan(x));
grid('on');
xlabel('x');
ylabel('tan(x)');
printplot tanplot.eps
printplot tanplot.jpg
%% Cosecant plot
t = linspace(-1,1,1000);
plot(t,csc(2*pi*t));
axis([-1,1,-10,10]);
grid('on');
xlabel('t');
ylabel('csc(2*pi*t)');
printplot cscplot.eps
printplot cscplot.jpg
%% Secant plot
t = linspace(-1,1,1000);
plot(t,sec(2*pi*t));
axis([-1,1,-10,10]);
grid('on');
xlabel('t');
ylabel('sec(2*pi*t)');
printplot secplot.eps
printplot secplot.jpg
%% Cotangent plot
t = linspace(-1,1);
plot(t,cot(t));
grid('on');
xlabel('t');
ylabel('cot(t)');
printplot cotplot.eps
printplot cotplot.jpg
%% Acos plot
t = linspace(-1,1);
plot(t,acos(t));
grid('on');
xlabel('t');
ylabel('acos(t)');
printplot acosplot.eps
printplot acosplot.jpg
%% Asin plot
t = linspace(-1,1);
plot(t,asin(t));
grid('on');
xlabel('t');
ylabel('asin(t)');
printplot asinplot.eps
printplot asinplot.jpg
%% Atan plot
t = linspace(-1,1);
plot(t,atan(t));
xlabel('t');
ylabel('atan(t)');
printplot atanplot.eps
printplot atanplot.jpg
%% Atan2 plot
x = linspace(-pi,pi);
sx = sin(x); cx = cos(x);
plot(x,atan(sx./cx),x,atan2(sx,cx));
axis tight
grid on
xlabel('x');
ylabel('atan2(sin(x),cos(x))');
printplot atan2plot.eps
printplot atan2plot.jpg
%% RANDN plot
t = linspace(-4,4);
plot(t,exp(-t.^2/2));
xlabel('x');
ylabel('PDF');
printplot gaus1.eps
printplot gaus1.jpg
%% FFT plot 1
t = linspace(0,2*pi,128);
x = cos(15*t);
y = fft(x);
plot(t,abs(y));
axis([0,2*pi,0,70]);
xlabel('Frequency');
ylabel('Spectrum');
printplot fft1.eps
printplot fft1.jpg
%% FFT plot 2
x = [];
x(1:10) = 1;
plot((0:255)/256*pi*2,real(fft(x,256)),'r-');
hold on
plot((0:9)/10*pi*2,real(fft(x)),'go');
axis tight
printplot fft2.eps
printplot fft2.jpg
%% Close the plot windows.
closeplot('all');
closeimage('all');
%% Image noise + bar
x = rand(512);
x((-64:63)+256,(-128:127)+256) = 1.0;
newimage
image(x);
printimage image1.eps
printimage image1.jpg
%% Colormap test
x = linspace(-1,1,512)'*ones(1,512);
y = x';
Z = exp(-(x.^2+y.^2)/0.3);
image(Z);
printimage colormap1.eps
printimage colormap1.jpg
colormap(copper);
image(Z);
printimage colormap2.eps
printimage colormap2.jpg
closeplot('all');
a = copper;
plot(a);
axis tight
xlabel('index');
ylabel('intensity');
printplot colormap3.eps
printplot colormap3.jpg
t = linspace(0,1,256);
A = [exp(-(t-1.0).^2/0.1);exp(-(t-0.5).^2/0.1);exp(-t.^2/0.1)]';
plot(A);
axis tight
xlabel('index');
ylabel('intensity');
printplot colormap4.eps
printplot colormap4.jpg
colormap(A);
printimage colormap5.eps
printimage colormap5.jpg
w = sqrt(sum(A'.^2));
sA = diag(1./w)*A;
plot(A);
axis tight
xlabel('index');
ylabel('intensity');
printplot colormap6.eps
printplot colormap6.jpg
colormap(sA);
printimage colormap7.eps
printimage colormap7.jpg
%% Printimage 
closeimage all
closeplot all
x = linspace(-1,1,512)'*ones(1,512);
y = x';
Z = exp(-(x.^2+y.^2)/0.3);
image(Z);
printimage printimage1.eps
printimage printimage1.jpg
%% Plot
closeimage all
closeplot all
x = linspace(-pi,pi);
y = cos(x);
plot(x,y);
printplot plot1.eps
printplot plot1.jpg
x = linspace(-pi,pi);
y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
plot(x,y);
printplot plot2.eps
printplot plot2.jpg
plot(x,y(:,1),'rx-',x,y(:,2),'b>-',x,y(:,3),'g*:');
printplot plot3.eps
printplot plot3.jpg
x = linspace(-pi,pi);
y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
plot(y(:,1),'r:',y(:,2),'b;',y(:,3),'g|');
printplot plot4.eps
printplot plot4.jpg
y = cos(2*x(:)) + i*cos(3*x(:));
plot(y);
printplot plot5.eps
printplot plot5.jpg
closeplot all
x = linspace(-1,1);
y = cos(2*pi*x);
plot(x,y,'r-');
xlabel('time');
printplot xlabel1.eps
printplot xlabel1.jpg
closeplot all
x = linspace(-1,1);
y = cos(2*pi*x);
plot(x,y,'r-');
ylabel('cost');
printplot ylabel1.eps
printplot ylabel1.jpg
closeplot all
x = linspace(-1,1);
y = cos(2*pi*x);
plot(x,y,'r-');
title('cost over time');
printplot title1.eps
printplot title1.jpg
x = linspace(-1,1);
y = cos(5*pi*x);
plot(x,y,'r-');
printplot printplot1.eps
printplot printplot1.jpg
%% Grid
closeplot all
closeimage all
x = linspace(-1,1);
y = cos(3*pi*x);
plot(x,y,'r-');
printplot grid1.eps
printplot grid1.jpg
grid on
printplot grid2.eps
printplot grid2.jpg
%% Axis
closeplot all
closeimage all
x = linspace(-pi,pi);
y = sqrt(2)*sin(3*x);
plot(x,y,'r-');
grid on
printplot axis1.eps
printplot axis1.jpg
axis tight
printplot axis2.eps
printplot axis2.jpg
a = axis;
a(1) = -pi/3;
a(2) = pi/3;
axis(a);
printplot axis3.eps
printplot axis3.jpg
axis auto
printplot axis4.eps
printplot axis4.jpg
%% Hold
closeplot all
closeimage all
x = linspace(-5,5,500);
t = exp(-x.^2);
y = t.*cos(2*pi*x*3);
plot(x,y);
printplot hold1.eps
printplot hold1.jpg
hold on
plot(x,t,'g-',x,-t,'b-')
printplot hold2.eps
printplot hold2.jpg

