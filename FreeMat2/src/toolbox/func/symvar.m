%!
%@Module SYMVAR Find Symbolic Variables in an Expression
%@@Section FUNCTION
%@@Usage
%Finds the symbolic variables in an expression.  The syntax for its
%use is 
%@[
%  syms = symvar(expr)
%@]
%where @|expr| is a string containing an expression, such as
%@|'x^2 + cos(t+alpha)'|.  The result is a cell array of strings
%containing the non-function identifiers in the expression.  Because
%they are usually not used as identifiers in expressions, the strings
% @|'pi','inf','nan','eps','i','j'| are ignored.
%@@Example
%Here are some simple examples:
%@<
%symvar('x^2+sqrt(x)')  % sqrt is eliminated as a function
%symvar('pi+3')         % No identifiers here
%symvar('x + t*alpha')  % x, t and alpha
%@>
%@@Tests
%@$"y=symvar('x^2+sqrt(x)')","{'x'}","exact"
%@$"y=symvar('pi+3')","{}","exact"
%@$"y=symvar('x+t*alpha-cos(pi)')","{'x','t','alpha'}","exact"
%!
function syms = symvar(expr)
  ignore = {'pi','inf','nan','eps','i','j'};
  tsyms = regexp(expr,'(\b[a-zA-Z]\w*\b)(?!\s*\()','tokens');
  tsyms = unique([tsyms{:}]);
  syms = {};
  for i = 1:numel(tsyms)
    if (~any(strcmp(tsyms{i},ignore)))
      syms = [syms,{tsyms{i}}];
    end
  end
  
  