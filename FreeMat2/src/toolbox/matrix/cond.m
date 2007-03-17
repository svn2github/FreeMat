%!
%@Module COND Condition Number of a Matrix
%@@Section ARRAY
%@@Usage
%Calculates the condition number of a matrix.  To compute the
%2-norm condition number of a matrix (ratio of largest to smallest
%singular values), use the syntax
%@[
%   y = cond(A)
%@]
%where A is a matrix.  If you want to compute the condition number
%in a different norm (e.g., the 1-norm), use the second syntax
%@[
%   y = cond(A,p)
%@]
%where @|p| is the norm to use when computing the condition number.
%The following choices of @|p| are supported
%\begin{itemize}
%  \item @|p = 1| returns the 1-norm, or the max column sum of A
%  \item @|p = 2| returns the 2-norm (largest singular value of A)
%  \item @|p = inf| returns the infinity norm, or the max row sum of A
%  \item @|p = 'fro'| returns the Frobenius-norm (vector Euclidean norm, or RMS value)
%\end{itemize}
%@@Function Internals
%The condition number is defined as
%\[
%  \frac{\|A\|_p}{\|A^{-1}\|_p}
%\]
%This equation is precisely how the condition number is computed for
%the case @|p ~= 2|.  For the @|p=2| case, the condition number can 
%be computed much more efficiently using the ratio of the largest and
%smallest singular values.
%@@Example
%The condition number of this matrix is large
%@<
%A = [1,1;0,1e-15]
%cond(A)
%cond(A,1)
%@>
%You can also (for the case @|p=1| use @|rcond| to calculate an estimate
%of the condition number
%@<
%1/rcond(A)
%@>
%!
function y = cond(A,p)
    if (nargin < 2)
        p = 2;
    end
    if (nargin < 1)
        error('expect at least 1 input to cond function');
    end
    if (p ~= 2)
        y = norm(A,p)*norm(inv(A),p);
    else
        s = svd(A);
        y = s(1)/s(end);
    end
    