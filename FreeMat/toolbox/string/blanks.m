% BLANKS Create a blank string
% 
% Usage
% 
% 
%     str = blanks(n)
% 
% Create a string str containing n blank charaters.

function str = blanks(n)
  if ~isscalar(n)
    n = n(1);
    warning('Input should be a scalar number')
  elseif n < 0
    n = 0;
    warning('Input should be a positive number') 
  end
  str = repmat(' ',[1,n]);
