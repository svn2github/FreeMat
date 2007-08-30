%!
%@Module CSTRUCTWRITE Write a C Structure To File
%@@Section EXTERNAL
%@@Usage 
%The @|cstructwrite| function is a convenience function for
%writing a C structure to a file.  This is generally a very
%bad idea, as writing of C structures to files is notoriously
%unportable.  Consider yourself warned.  The syntax for this
%function is 
%@[
%  cstructwrite(fid,a,'typename')
%@]
%where @|a| is the FreeMat structure to write, @|'typename'| is
%a string containing the name of the C structure to use when
%writing the structure to the file (previously defined using
%@|cstructdefine|), and @|fid| is the file handle returned
%by @|fopen|.
%!
function cstructwrite(fid,a,typename)
  if (nargin < 3), error('cstructwrite requires 3 args, the fid, variable, and typename'); end;
  fwrite(fid,cstructfreeze(a,typename));

