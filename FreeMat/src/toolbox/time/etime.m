%!
%@Module ETIME Elapsed Time Function
%@@Section FREEMAT
%@@Usage
%The @|etime| calculates the elapsed time between two @|clock| vectors
%@|x1| and @|x2|.  The syntax for its use is
%@[
%   y = etime(x1,x2)
%@]
%where @|x1| and @|x2| are in the @|clock| output format
%@[
%   x = [year month day hour minute seconds]
%@]
%@@Example
%Here we use @|etime| as a substitute for @|tic| and @|toc|
%@<
%x1 = clock;
%sleep(1);
%x2 = clock;
%etime(x2,x1);
%@>
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = etime(x1,x2)
  if (~exist('x1') | ~exist('x2'))
     error 'etime expects two arguments'
  end
  try
    y1 = clocktotime(x1);
    y2 = clocktotime(x2);
  catch
     error 'invalid input vectors to etime'
  end
  y = y1 - y2;
