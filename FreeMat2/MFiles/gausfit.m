
function [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing)
if (~exist('w')) 
  w = y*0+1; 
end
if (~exist('dcg')) 
  dcg = min(y(:)); 
end
ycor = y - dcg;
if (~exist('gaing')) 
  gaing = max(ycor); e
nd
ycor = ycor/gaing;
if (~exist('mug')) 
  mug = sum(ycor.*t)/sum(ycor); 
end
if (~exist('sigmag')) 
  sigmag = sqrt(abs(sum((ycor).*(t-mug).^2)/sum(ycor))); 
end
[xopt,err] = fitfun('gfitfun',[mug,sigmag,dcg,gaing],y,w,eps,t);
mu = xopt(1);
sigma = xopt(2);
dc = xopt(3);
gain = xopt(4);
yhat = gfitfun(xopt,t);


