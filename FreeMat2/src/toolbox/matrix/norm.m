%!
%@Module NORM Norm Calculation
%@@Section ARRAY
%@@Usage
%Calculates the norm of a matrix.  There are two ways to
%use the @|norm| function.  The general syntax is
%@[
%   y = norm(A,p)
%@]
%where @|A| is the matrix to analyze, and @|p| is the
%type norm to compute.  The following choices of @|p|
%are supported
%\begin{itemize}
%  \item @|p = 1| returns the 1-norm, or the max column sum of A
%  \item @|p = 2| returns the 2-norm (largest singular value of A)
%  \item @|p = inf| returns the infinity norm, or the max row sum of A
%  \item @|p = 'fro'| returns the Frobenius-norm (vector Euclidean norm, or RMS value)
%\end{itemize}
%For a vector, the regular norm calculations are performed:
%\begin{itemize}
%  \item @|1 <= p < inf| returns @|sum(abs(A).^p)^(1/p)|
%  \item @|p| unspecified returns @|norm(A,2)|
%  \item @|p = inf| returns max(abs(A))
%  \item @|p = -inf| returns min(abs(A))
%\end{itemize}
%@@Examples
%Here are the various norms calculated for a sample matrix
%@<
%A = float(rand(3,4))
%norm(A,1)
%norm(A,2)
%norm(A,inf)
%norm(A,'fro')
%@>
%Next, we calculate some vector norms.
%@<
%A = float(rand(4,1))
%norm(A,1)
%norm(A,2)
%norm(A,7)
%norm(A,inf)
%norm(A,-inf)
%@>
%@@Tests
%@$"y=norm([1,0])","1","exact"
%@$"y=norm([4,3])","5","close"
%!
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = norm(A,p)
if (~isset('p'))
  p = 2;
end

if (min(size(A)) == 1)
  y = vector_norm(A,p);
else
  y = matrix_norm(A,p);
end

function y = vector_norm(A,p)
if (p<1 && p~=-inf) 
  error('p argument in vector norm must be between 1 and inf, or be -inf');
end
if (p == inf)
  y = max(abs(A));
elseif (p == -inf)
  y = min(abs(A));
elseif (p == 2)
  y = xnrm2(A);
else
  y = sum(abs(A).^p)^(1/p);
end

function y = matrix_norm(A,p)
switch p
  case 1
     y = max(sum(abs(A)));
  case 2
     s = svd(A);
     y = s(1);
  case inf
     y = max(sum(abs(A')));
  case 'fro'
     y = xnrm2(A);
  otherwise
     error('unrecognized norm p-argument');
end


