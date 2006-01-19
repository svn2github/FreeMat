%!
%@Module GAUSFIT Gaussian Curve Fit
%@@Section CURVEFIT
%@@Usage
%The @|gausfit| routine has the following syntax
%@[
%  [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing).
%@]
%where the required inputs are
%\begin{itemize}
%  \item @|t| - the values of the independant variable (e.g., time samples)
%  \item @|y| - the values of the dependant variable (e.g., f(t))
%\end{itemize}
%The following inputs are all optional, and default values are
%available for each of them.
%\begin{itemize}
%  \item @|w| - the weights to use in the fitting (set to ones if omitted)
%  \item @|mug| - initial estimate of the mean
%  \item @|sigmag| - initial estimate of the sigma (standard deviation)
%  \item @|dcg| - initial estimate of the DC value
%  \item @|gaing| - initial estimate of the gain
%\end{itemize}
%The fit is of the form @|yhat=gain*exp((t-mu).^2/(2*sigma^2))+dc|.
%The outputs are 
%\begin{itemize}
%  \item @|mu| - the mean of the fit
%  \item @|sigma| - the sigma of the fit
%  \item @|dc| - the dc term of the fit
%  \item @|gain| - the gain of the gaussian fit
%  \item @|yhat| - the output samples (the Gaussian fits)
%\end{itemize}
%Because the fit is nonlinear, a good initial guess is critical to
%convergence of the solution.  Thus, you can supply initial guesses
%for each of the parameters using the @|mug|, @|sigmag|, @|dcg|, 
%@|gaing| arguments.  Any arguments not supplied are estimated using 
%a simple algorithm. In particular, the DC value is estimated by 
%taking the minimum value  from the vector @|y|.  The gain is 
%estimated from the range of @|y|.  The mean and standard deviation 
%are estimated using the first and second order moments of @|y|.
%This function uses @|fitfun|.
%@@Example
%Suppose we want to fit a cycle of a cosine using a Gaussian shape.
%@<
%t = linspace(-pi,pi); 
%y = cos(t);
%[mu,sigma,dc,gain,yhat] = gausfit(t,y);
%plot(t,y,'rx',t,yhat,'g-');
%mprint gausfit1
%@>
%Which results in the following plot
%@figure gausfit1
%!
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


