%!
%@Module GETFIELD Get Field Contents
%@@Section ELEMENTARY
%@@Usage
%Given a structure or structure array, returns the contents of the
%specified field.  The first version is for scalar structures, and
%has the following syntax
%@[
%   y = getfield(x,'fieldname')
%@]
%and is equivalent to @|y = x.fieldname| where @|x| is a scalar (1 x 1)
%structure.  If @|x| is not a scalar structure, then @|y| is the 
%first value, i.e., it is equivalent to @|y = x(1).fieldname|.  
%The second form allows you to specify a subindex into a
%structure array, and has the following syntax
%@[
%    y = getfield(x, {m,n}, 'fieldname')
%@]
%and is equivalent to @|y = x(m,n).fieldname|.  You can chain multiple
%references together using this syntax.
%!

% Copyright (c) 2002-2006 Samit Basu

function y = getfield(x,varargin)
if (isempty(varargin))
  error('Need at least one additional input to getfield');
end

% Check for the simple case of a single field index
field = varargin{1};
if (length(varargin)==1 && isa(field,'string'))
  y = x.(field);
  return;
end
% Loop through the references
y = x;
for i=1:length(varargin)
  ndx = varargin{i};
  if (isa(ndx,'cell'))
    y = y(ndx{:});
  elseif (isa(ndx,'string'))
    % Dereference the field - if a contents list is returned
    % handle appropriately using a deal call
    if (length(y) == 1)
      y = y.(ndx);
    else
      tmp = cell(1,length(y));
      [tmp{:}] = deal(y.(ndx));
      y = tmp{1};
    end
  else
    error('unrecognized indexing type');
  end
end

