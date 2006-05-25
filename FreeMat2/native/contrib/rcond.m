function retval = rcond (a)
%RCOND reciprocal condition estimator.
%   RCOND(X) is an estimate for the reciprocal of the
%   condition of X in the 1-norm obtained by the 'cond'
%   condition estimator. If X is well conditioned, RCOND(X)
%   is near 1.0. If X is badly conditioned, RCOND(X) is
%   near EPS.

%   M version contributor: M.W. Vogel 01-23-06

  if (nargin == 1)
    retval = 1 / cond(a);
  else
    disp('cond (a)');
  end
