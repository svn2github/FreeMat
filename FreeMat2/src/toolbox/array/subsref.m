%!
%@Module SUBSREF Array Dereferencing
%@@Section ARRAY
%@@Usage
%This function can be used to index into basic array
%types (or structures).  It provides a functional interface
%to execute complex indexing expressions such as 
%@|a.b(3){5}| at run time (i.e. while executing a script or
%a function) without resorting to using @|eval|.  Note that
%this function should be overloaded for use with user defined
%classes, and that it cannot be overloaeded for base types.
%The basic syntax of the function is:
%@[
%   b = subsref(a,s)
%@]
%where @|s| is a structure array with two fields. The
%first field is
%\begin{itemize}
%\item @|type|  is a string containing either @|'()'| or
% @|'{}'| or @|'.'| depending on the form of the call.
%\item @|subs| is a cell array or string containing the
% the subscript information.
%\end{itemize}
%When multiple indexing experssions are combined together
%such as @|b = a(5).foo{:}|, the @|s| array should contain
%the following entries
%@[
%  s(1).type = '()'  s(1).subs = {5}
%  s(2).type = '.'   s(2).subs = 'foo'
%  s(3).type = '{}'  s(3).subs = ':'
%@]
%!
function b = subsref(a,s)
  for i=1:numel(s)
    switch (s(i).type)
    case '()'
      a = a(s(i).subs{:});
    case '{}'
      a = a{s(i).subs{:}};
    case '.'
      a = a.(s(i).subs);
    otherwise
      error('illegal indexing structure argument to subsref');
    end
  end
  b = a;


