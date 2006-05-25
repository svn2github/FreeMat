function retval = cond (a)
% Compute the (two-norm) condition number of a matrix. cond (a) is
% defined as norm (a) * norm (inv (a)), and is computed via a
% singular value decomposition.

% Inspired by cond.m (part of Octave) by 1997 John W. Eaton

%   M version contributor: M.W. Vogel 01-23-06



  if (nargin == 1)
    if (ndims (a) > 2)
      disp('cond: Only valid on 2-D objects')
    end

    [nr, nc] = size (a);
    if (nr == 0 | nc == 0)
      retval = 0.0;
    end
    if (any (any (isinf (a) | isnan (a))))
      disp('cond: argument must not contain Inf or NaN values');
    else
      sigma = svd (a);
      sigma_1 = sigma(1);
      sigma_n = sigma(length (sigma));
      if (sigma_1 == 0 | sigma_n == 0)
        retval = Inf;
      else
        retval = sigma_1 / sigma_n;
      end
    end
  else
    disp('cond (a)');
  end
