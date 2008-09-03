%!
%@Module BLANKS Create a blank string
%@@Section String
%@@Usage
%@[
%    str = blanks(n)
%@]
%Create a string @|str| containing @|n| blank charaters.
%@@Example
%A simple example:
%@<
%fprintf(['x0123456789y\n','x',blanks(10),'y\n'])
%@>
%@@Tests
%@$"str=['x',blanks(5),'y']","'x     y'","exact"
%!

function str = blanks(n)
  if ~isscalar(n)
    n = n(1);
    warning('Input should be a scalar number')
  elseif n < 0
    n = 0;
    warning('Input should be a positive number') 
  end
  str = repmat(' ',[1,n]);
