%!
%@Module DET Determinant of a Matrix
%@@Section ARRAY
%@@Usage
%Calculates the determinant of a matrix.  Note that for all but
%very small problems, the determinant is not particularly useful.
%The condition number @|cond| gives a more reasonable estimate as
%to the suitability of a matrix for inversion than comparing @|det(A)|
%to zero.  In any case, the syntax for its use is
%@[
%  y = det(A)
%@]
%where A is a square matrix.
%@@Function Internals
%The determinant is calculated via the @|LU| decomposition.  Note that
%the determinant of a product of matrices is the product of the 
%determinants.  Then, we have that 
%\[
%  L U = P A
%\]
%where @|L| is lower triangular with 1s on the main diagonal, @|U| is
%upper triangular, and @|P| is a row-permutation matrix.  Taking the
%determinant of both sides yields
%\[
% |L U| = |L| |U| = |U| = |P A| = |P| |A|
%\]
%where we have used the fact that the determinant of @|L| is 1.  The
%determinant of @|P| (which is a row exchange matrix) is either 1 or 
%-1.
%@@Example
%Here we assemble a random matrix and compute its determinant
%@<
%A = rand(5);
%det(A)
%@>
%Then, we exchange two rows of @|A| to demonstrate how the determinant
%changes sign (but the magnitude is the same)
%@<
%B = A([2,1,3,4,5],:);
%det(B)
%@>
%@@Tests
%@{  test_det1.m
%% Test the determinant calculation (bug 1584651)
%function test_val = test_det1
%A = [1 2
%       3 4];
%test_val = (det(A) == -2)
%@}
%!
function y = det(A)
    if (~isnumeric(A))
        error('Argument to det must be numeric');
    end
    if (ndims(A) ~= 2)
        error('argument to det must be 2D matrix');
    end
    if (size(A,1) ~= size(A,2))
        error('argument to det must be square');
    end
    N = size(A,1);
    [L,U,P] = lu(A);
    o = (1:N)';
    p = P*o;
    exchange_count = det_exchange_count(p);
    y = prod(diag(U));
    if (mod(exchange_count,2) ~= 0)
        y = -y;
    end
    
function exchange_count = det_exchange_count(pivot)
    exchange_count = 0;
    for i=1:numel(pivot)
        if (pivot(i) ~= i)
            k = find(pivot == i);
            p = pivot(i);
            pivot(i) = i;
            pivot(k) = p;
            exchange_count = exchange_count + 1;
        end
    end
