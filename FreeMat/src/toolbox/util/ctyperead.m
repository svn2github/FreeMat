%!
%@Module CTYPEREAD Read a C Structure From File
%@@Section EXTERNAL
%@@Usage
%The @|ctyperead| function is a convenience function for
%reading a C structure from a file.  This is generally a
%very bad idea, as direct writing of C structures to files
%is notoriously unportable.  Consider yourself warned.  The
%syntax for this function is
%@[
%   a = ctyperead(fid,'typename')
%@]
%where @|'typename'| is a string containing the name of the
%C structure as defined using @|ctypedefine|, and @|fid|
%is the file handle returned by the @|fopen| command.  Note
%that this form will read a single structure from the file.
%If you want to read multiple structures into an array, 
%use the following form
%@[
%   a = ctyperead(fid,'typename',count)
%@]
%Note that the way this function works is by using @|ctypesize|
%to compute the size of the structure, reading that many bytes
%from the file, and then calling @|ctypethaw| on the resulting
%buffer.  A consequence of this behavior is that the byte-endian
%corrective behavior of FreeMat does not work.
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function a = ctyperead(fid,typename,count)
  if (nargin < 3), count = 1; end;
  if (nargin < 2), error('ctyperead requries at least a file id and a typename'); end;
  p = fread(fid,[1,ctypesize(typename,count)],'*uint8');
  a = ctypethaw(p,typename,count);

