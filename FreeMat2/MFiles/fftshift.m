%!
%@Module FFTSHIFT Shift FFT Output
%@@Section TRANSFORMS
%@@Usage
%The @|fftshift| function shifts the DC component (zero-frequency)
%of the output from an FFT to the center of the array.  For vectors
%this means swapping the two halves of the vector.  For matrices,
%the first and third quadrants are swapped.  So on for N-dimensional
%arrays.  The syntax for its use is
%@[
%     y = fftshift(x).
%@]
%Alternately, you can specify that only one dimension be shifted
%@[
%     y = fftshift(x,dim).
%@]
%!
function y = fftshift(x,dim)
if (nargin > 1)
  if (numel(dim) ~= 1 | dim ~= floor(dim(1)) | dim < 1)
    error('dim must be a positive scalar integer');
  end
  shiftvec = zeros(1,dim);
  shiftvec(dim) = floor(size(x,dim)/2);
else
  shiftvec = floor(size(x)/2);
end
y = circshift(x,shiftvec);

