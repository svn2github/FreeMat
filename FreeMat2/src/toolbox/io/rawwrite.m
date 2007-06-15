%!
%@Module RAWWRITE Write N-dimensional Array From File
%@@Section IO
%@@Usage
%The syntax for @|rawwrite| is
%@[
%   function rawwrite(fname,x,byteorder)
%@]
%where @|fname| is the name of the file to write to, and the
%(numeric) array @|x| is writen to the file in its native
%type (e.g. if @|x| is of type @|int16|, then it will be written
%to the file as 16-bit signed integers.  If @|byteorder| is
%left unspecified, the file is assumed to be
%of the same byte-order as the machine @|FreeMat| is running on.
%If you wish to force a particular byte order, specify the @|byteorder|
%argument as
%\begin{itemize}
%   \item @|'le','ieee-le','little-endian','littleEndian','little'|
%   \item @|'be','ieee-be','big-endian','bigEndian','big'|
%\end{itemize}
%!

% Contributed by Bruno DeMan

function rawwrite(fname,x,byteorder)
  try 
    if (isset('byteorder'))
      fp = fopen(fname,'wb',byteorder);
    else
      fp = fopen(fname,'wb');
    end
    fwrite(fp,x);
    fclose(fp);
  catch
    error(['Unable to write file ' fname ' in function rawwrite']);
  end

