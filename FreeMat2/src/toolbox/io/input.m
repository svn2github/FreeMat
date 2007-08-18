%!
%@Module INPUT Get Input From User
%@@Section IO
%@@Usage
%The @|input| function is used to obtain input from the user.  There are
%two syntaxes for its use.  The first is
%@[
%    r = input('prompt')
%@]
%in which case, the prompt is presented, and the user is allowed to enter
%an expression.  The expression is evaluated in the current workspace or
%context (so it can use any defined variables or functions), and returned
%for assignment to the variable (@|r| in this case).  In the second form
%of the @|input| function, the syntax is
%@[
%    r = input('prompt','s')
%@]
%in which case the text entered by the user is copied verbatim to the
%output.
%!

% Copyright (c) 2002-2006 Samit Basu

function y = input(prompt,stringflag)
  if (isset('stringflag') && ischar(stringflag) && (strcmp(stringflag,'s') | strcmp(stringflag,'S')))
    y = getline(prompt);
    y(end) = [];
  elseif isset('stringflag')
    error 'second argument to input must be the string ''s'' or ''S''.'
  else
    needval = 1;
    while (needval)
      a = getline(prompt);
      a(end) = [];
      needval = 0;
      if (~isempty(a))
        y = evalin('caller',a,'0;printf(''%s\n'',lasterr);needval=1;');
      else
        y = [];
      end
    end
  end
