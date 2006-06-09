%!
%@Module IFFTSHIFT Inverse Shift FFT Output
%@@Section TRANSFORMS
%@@Usage
%The @|ifftshift| function shifts the DC component (zero-frequency)
%of the output from the center of the array back to the first position
%and iseffectively the inverse of @|fftshift|.  For vectors
%this means swapping the two halves of the vector.  For matrices,
%the first and third quadrants are swapped.  So on for N-dimensional
%arrays.  The syntax for its use is
%@[
%     y = ifftshift(x).
%@]
%Alternately, you can specify that only one dimension be shifted
%@[
%     y = ifftshift(x,dim).
%@]
%!

% Copyright (c) 2002-2006 Samit Basu

function y = ifftshift(x,dim)
if (nargin > 1)
  if (numel(dim) ~= 1 | dim ~= floor(dim(1)) | dim < 1)
    error('dim must be a positive scalar integer');
  end
  shiftvec = zeros(1,dim);
  shiftvec(dim) = floor(size(x,dim)/2);
else
  shiftvec = floor(size(x)/2);
end
y = circshift(x,-shiftvec);

