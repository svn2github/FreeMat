%!
%@Module DOT Dot Product Function
%@@Section ELEMENTARY
%@@Usage
%Computes the scalar dot product of its two arguments.  The general
%syntax for its use is
%@[
%  y = dot(x,z)
%@]
%where @|x| and @|z| are numerical vectors of the same length.  If 
%@|x| and @|z| are multi-dimensional arrays of the same size, then
%the dot product is taken along the first non-singleton dimension.
%You can also specify the dimension to take the dot product along using
%the alternate form
%@[
%  y = dot(x,z,dim)
%@]
%where @|dim| specifies the dimension to take the dot product along.
%@@Tests
%@$"y=dot([1,2,1],[1;2;1])","6","exact"
%@$"y=dot([1,2;3,2],[2,1;4,5])","[14,12]","exact"
%@$"y=dot([1,2;3,2],[2,1;4,5],2)","[4;22]","exact"
%@$"y=dot([1,2;3,2],[2,1;4,5],3)","[2,2;12,10]","exact"
%!
function y = dot(x,z,dim)
  if (isvector(x) && isvector(z))
    if (numel(x) ~= numel(z))
      error('dot requires its arguments to be equal length');
    end
    y = x(:)'*z(:);
  else
    try
      q = x.*z;
    catch
      error('dot requires multi-dimensional arguments to be the same size and numeric');
    end
    if (~exist('dim'))
      y = sum(q);
    else
      y = sum(q,dim);
    end
  end
  
    
