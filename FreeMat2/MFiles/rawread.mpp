//!
//@Module RAWREAD Read N-dimensional Array From File
//@@Section IO
//@@Usage
//The syntax for @|rawread| is
//@[
//   function x = rawread(fname,size,precision,byteorder)
//@]
//where @|fname| is the name of the file to read from, 
//and @|size| is an n-dimensional vector that stores the
//size of the array in each dimension.  The argument @|precision|
//is the type of the data to read in:
//\begin{itemize}
//  \item 'uint8','uchar','unsigned char' for unsigned, 8-bit integers
//  \item 'int8','char','integer*1' for signed, 8-bit integers
//  \item 'uint16','unsigned short' for unsigned, 16-bit  integers
//  \item 'int16','short','integer*2' for  signed, 16-bit integers
//  \item 'uint32','unsigned int' for unsigned, 32-bit integers
//  \item 'int32','int','integer*4' for signed, 32-bit integers
//  \item 'single','float32','float','real*4' for 32-bit floating point
//  \item 'double','float64','real*8' for 64-bit floating point
//  \item 'complex','complex*8' for  64-bit complex floating point (32 bits 
//         for the real and imaginary part).
//  \item 'dcomplex','complex*16' for 128-bit complex floating point (64
//         bits for the real and imaginary part).
//\end{itemize}
//As a special feature, one of the size elements can be 'inf', 
//in which case, the largest possible array is read in.
//If @|byteorder| is left unspecified, the file is assumed to be
//of the same byte-order as the machine @|FreeMat| is running on.
//If you wish to force a particular byte order, specify the @|byteorder|
//argument as
//\begin{itemize}
//   \item @|'le','ieee-le','little-endian','littleEndian','little'|
//   \item @|'be','ieee-be','big-endian','bigEndian','big'|
//\end{itemize}
function x = rawread(fname,size,precision,byteorder)
  try 
    if (exist('byteorder'))
      fp = fopen(fname,'rb',byteorder);
    else
      fp = fopen(fname,'rb');
    end
    x = fread(fp,size,precision);
    fclose(fp);
  catch
    error(['Unable to read file ' fname ' in function rawread']);
  end

