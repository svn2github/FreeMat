%  GAUSFIT Gaussian Curve Fit
%  
%  
%  USAGE
%  
%  The gausfit routine has the following syntax
%  
%  /   [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing).
%  
%  where the required inputs are
%  
%    
%  * t - the values of the independant variable (e.g., time samples)
%    
%  * y - the values of the dependant variable (e.g., f(t))
%  
%  The following inputs are all optional, and default values are
%  available for each of them.
%  
%    
%  * w - the weights to use in the fitting (set to ones if omitted)
%    
%  * mug - initial estimate of the mean
%    
%  * sigmag - initial estimate of the sigma (standard deviation)
%    
%  * dcg - initial estimate of the DC value
%    
%  * gaing - initial estimate of the gain
%  
%  The fit is of the form yhat=gain*exp((t-mu).^2/(2*sigma^2))+dc.
%  The outputs are 
%  
%    
%  * mu - the mean of the fit
%    
%  * sigma - the sigma of the fit
%    
%  * dc - the dc term of the fit
%    
%  * gain - the gain of the gaussian fit
%    
%  * yhat - the output samples (the Gaussian fits)
%  
%  Because the fit is nonlinear, a good initial guess is critical to
%  convergence of the solution.  Thus, you can supply initial guesses
%  for each of the parameters using the mug, sigmag, dcg, 
%  gaing arguments.  Any arguments not supplied are estimated using 
%  a simple algorithm. In particular, the DC value is estimated by 
%  taking the minimum value  from the vector y.  The gain is 
%  estimated from the range of y.  The mean and standard deviation 
%  are estimated using the first and second order moments of y.
%  This function uses fitfun.
%  
function [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing)
if (~isset('w')) 
  w = y*0+1; 
end
if (~isset('dcg')) 
  dcg = min(y(:)); 
end
ycor = y - dcg;
if (~isset('gaing')) 
  gaing = max(ycor); 
end
ycor = ycor/gaing;
if (~isset('mug')) 
  mug = sum(ycor.*t)/sum(ycor); 
end
if (~isset('sigmag')) 
  sigmag = sqrt(abs(sum((ycor).*(t-mug).^2)/sum(ycor))); 
end
[xopt,err] = fitfun('gfitfun',[mug,sigmag,dcg,gaing],y,w,eps,t);
mu = xopt(1);
sigma = xopt(2);
dc = xopt(3);
gain = xopt(4);
yhat = gfitfun(xopt,t);


