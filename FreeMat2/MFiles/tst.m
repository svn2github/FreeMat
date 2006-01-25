t = linspace(-pi,pi); 
y = cos(t);
plot(t,y,'rx-');
hold on
plot(t,cos(t-pi/4),'go-');
plot(t,-sin(t+pi/4),'k:');
plot(t,cos(t-pi/8),'y*');
plot(t,-sin(t+pi/8),'sb');
legend('Power','phase','sinusoid','longer label','location','s');

