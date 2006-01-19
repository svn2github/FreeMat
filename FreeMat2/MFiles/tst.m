t = linspace(-pi,pi); 
y = cos(t);
[mu,sigma,dc,gain,yhat] = gausfit(t,y);
z = exp(-t.^2)
[mu,sigma,dc,gain,zhat] = gausfit(t,z);
