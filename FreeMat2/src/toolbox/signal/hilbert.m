%!
%@Module HILBERT Hilbert Transform
%@@Section TRANSFORMS
%@@Usage
%The @|hilbert| function computes the hilbert transform of the argument
%vector or matrix.  The FreeMat @|hilbert| function is compatible with
%the one from the MATLAB API.  This means that the output of the
%@|hilbert| function is the sum of the original function and an
%imaginary signal containing the Hilbert transform of it.  There are
%two syntaxes for the hilbert function.  The first is
%@[
%  y = hilbert(x)
%@]
%where @|x| is real vector or matrix.  If @|x| is a matrix, then he
%Hilbert transform is computed along the columns of @|x|.  
%!