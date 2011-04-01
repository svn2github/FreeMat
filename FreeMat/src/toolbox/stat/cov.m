%!
%@Module COV Covariance Matrix
%@@Section ELEMENTARY
%@@Usage
%Computes the covariance of a matrix or a vector.  The general
%syntax for its use is
%@[
%  y = cov(x)
%@]
%where @|x| is a matrix or a vector.  If @|x| is a vector then
%@|cov| returns the variance of @|x|.  If @|x| is a matrix then
%@|cov| returns the covariance matrix of the columns of @|x|.
%You can also call @|cov| with two arguments to compute the 
%matrix of cross correlations.  The syntax for this mode is
%@[
%  y = cov(x,z)
%@]
%where @|x| and @|z| are matrices of the same size.  Finally,
%you can provide a normalization flag @|d| that is either @|0|
%or @|1|, which changes the normalization factor from @|L-1| (for @|d=0|) to 
%@|L| (for @|d=1|) where @|L| is the number of rows in the matrix @|x|.  In
%this case, the syntaxes are
%@[
%  y = cov(x,z,d)
%@]
%for the two-argument case, and
%@[
%  y = cov(x,d)
%@]
%for the one-argument case.
%@@Example
%The following demonstrates some uses of the @|cov| function
%@<
%A = [5,1,3;3,2,1;0,3,1]
%B = [4,-2,0;1,5,2;-2,0,1];
%@>
%We start with the covariance matrix for @|A|
%@<
%cov(A)
%@>
%and again with the (biased) normalization
%@<
%cov(A,1)
%@>
%Here we compute the cross covariance between @|A| and @|B|
%@<
%cov(A,B)
%@>
%and again with biased normalization
%@<
%cov(A,B,1)
%@>
%!
function y = cov(x,z,d)
  if (nargin == 1) && isvector(x)
    y = var(x);
  elseif (nargin == 1)
    y = cov_matrix(x,size(x,1));
  elseif (nargin == 2 && all(size(x)==size(z)))
    y = cov([x(:),z(:)]);
  elseif (nargin == 2 && (isscalar(z)))
    if (z == 1)
      y = cov_matrix(x,size(x,1));
    elseif (z == 0)
      y = cov_matrix(x,max(1,size(x,1)-1));
    end
  elseif (nargin == 3 && all(size(x)==size(z)) && isscalar(d))
    y = cov([x(:),z(:)],d);
  else
    error('Unrecognized form for call to cov');
  end
end

function y = cov_matrix(x,norm_factor)
  x = x - repmat(mean(x),[size(x,1),1]);
  y = (x'*x)/norm_factor;
end