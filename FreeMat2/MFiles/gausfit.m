%
% Function [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing)
%
% Fits a Gaussian shape to the given vector using a nonlinear
% Levenburg-Marquadt algorithm (fitfun).  The inputs are 
%    t - the values of the independant variable (e.g., time samples)
%    y - the values of the dependant variable (e.g., f(t))
%    w - the weights to use in the fitting (set to ones if omitted)
%
% The fit is of the form:
%    yhat = gain*exp((t-mu).^2/(2*sigma^2)) + dc
%
% Because the fit is nonlinear, a good initial guess is critical to
% convergence of the solution.  Thus, you can supply initial guesses
% for each of the parameters using the mug, sigmag, dcg, gaing arguments.
% Any arguments not supplied are estimated using a simple algorithm.
% In particular, the DC value is estimated by taking the minimum value
% from the vector y.  The gain is estimated from the range of y.  
% The mean and standard deviation are estimated using the first and 
% second order moments of y.
%
% Example:
% 
% Suppose we want to fit a cycle of a cosine using a Gaussian shape.
% t = linspace(-pi,pi); y = cos(t);
% [mu,sigma,dc,gain,yhat] = gausfit(t,y)
function [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing)
if (~exist('w')) w = y*0+1; end
if (~exist('dcg')) dcg = min(y(:)); end
ycor = y - dcg;
if (~exist('gaing')) gaing = max(ycor); end
ycor = ycor/gaing;
if (~exist('mug')) mug = sum(ycor.*t)/sum(ycor); end
if (~exist('sigmag')) sigmag = sqrt(abs(sum((ycor).*(t-mug).^2)/sum(ycor))); end
[xopt,err] = fitfun('gfitfun',[mug,sigmag,dcg,gaing],y,w,eps,t);
mu = xopt(1);
sigma = xopt(2);
dc = xopt(3);
gain = xopt(4);
yhat = gfitfun(xopt,t);


