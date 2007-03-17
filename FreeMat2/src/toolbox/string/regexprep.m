%!
%@Module REGEXPREP Regular Expression Replacement Function
%@@Section STRING
%@@Usage
%Replaces regular expressions in the provided string.  The syntax for its
%use is 
%@[
%  outstring = regexprep(instring,pattern,replacement,modes)
%@]
%Here @|instring| is the string to be operated on.  And @|pattern| is a regular
%expression of the type accepted by @|regexp|.  For each match, the contents
%of the matched string are replaced with the replacement text.  Tokens in the
%regular expression can be used in the replacement text using @|$N| where @|N|
%is the number of the token to use.  You can also specify the same @|mode| 
%flags that are used by @|regexp|.
%!
function outputs = regexprep(instring,pattern,replacement,varargin)
if (iscellstr(instring))
  outputs = {};
  for i=1:numel(instring)
     outputs = [outputs,regexprep_helper(instring{i},pattern,replacement,varargin{:})];
  end;
  outputs = reshape(outpus,size(instring));
elseif (isstr(instring))
  outputs = regexprep_helper(instring,pattern,replacement,varargin{:});
else
  error('input instring must be either a string or a cell array of strings');
end

function outputs = regexprep_helper(instring,pattern,replacement,varargin)
if (iscellstr(pattern) && iscellstr(replacement))
  if (numel(pattern) ~= numel(replacement))
    error('when pattern and replacement are both cell-arrays of strings, they must both be the same size');
  end
  outputs = instring;
  for i=1:numel(pattern)
    outputs = regexprepdriver(outputs,pattern{i},replacement{i},varargin{:});
  end;
elseif (isstr(pattern) && (iscellstr(replacement) || isstr(replacement)))
  outputs = regexprepdriver(instring,pattern,replacement,varargin{:});
elseif (iscellstr(pattern) && isstr(replacement))
  outputs = instring;
  for i=1:numel(pattern)
    outputs = regexprepdriver(outputs,pattern{i},replacement,varargin{:});
  end;
else
  error('pattern and replacement must both be either cell-arrays of strings or strings');
end

