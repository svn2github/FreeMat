/*
 * Copyright (c) 2009 Eugene Ingerman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "Printf.hpp"
#include "Print.hpp"
#include "HandleList.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"
#include <QTextCodec>
#include <QFile>
#include <QTextStream>

extern HandleList<FilePtr*> fileHandles;

class PrintfStream{
public:
  virtual PrintfStream& operator <<( const char* data ) = 0;
};

class PrintfFileStream : public PrintfStream{
private:
  QFile* fp;
  QTextStream ts;
public:
  PrintfFileStream( QFile* fp_ ) : fp( fp_ ) { ts.setDevice( fp ); };
  virtual PrintfFileStream& operator <<( const char* data ){
    ts << data;
    return *this;
  };
  ~PrintfFileStream() { fp->flush(); };
};

class PrintfStringStream : public PrintfStream{
private:
  QString* str;
public:
  PrintfStringStream( QString* str_ ) : str(str_){};
  virtual PrintfStringStream& operator <<( const char* data ){
    (*str) += QString( data );
    return *this;
  };
};


bool isEscape(char *dp) {
  return ((dp[0] == '\\') &&
	  ((dp[1] == 'n') ||
	   (dp[1] == 't') ||
	   (dp[1] == 'r') ||
	   (dp[1] == '\\')));
}

static int flagChar(char c) {
  return ((c == '#') ||  (c == '0') || (c == '-') ||  
	  (c == ' ') ||  (c == '+'));
}

static int convspec(char c) {
  return ((c == 'd') || (c == 'i') || (c == 'o') || 
	  (c == 'u') || (c == 'x') || (c == 'X') ||
	  (c == 'e') || (c == 'E') || (c == 'f') || 
	  (c == 'F') || (c == 'g') || (c == 'G') ||
	  (c == 'a') || (c == 'A') || (c == 'c') || 
	  (c == 's'));
}

static char* validateFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && convspec(*cp)) 
    return cp+1;
  else
    return 0;
}

QString ConvertEscapeSequences(const QString &src) {
  QString dest;
  int i=0;
  while (i < src.size()) {
    if ((src[i] != '\\') || (i == (src.size()-1))) {
      dest += src[i];
    } else {
      if (src[i+1] == 'n') {
	dest += '\n';
	i++;
      } else if (src[i+1] == 't') {
	dest += '\t';
	i++;
      } else if (src[i+1] == 'r') {
	dest += '\r';
	i++;
      } else {
	dest += src[i+1];
	i++;
      }
    }
    i++;
  }
  return dest;
}

class PrintfDataServer{
private:
  const ArrayVector arg;
  int vec_ind;
  int elem_ind;
  bool hasMoreData;
  void IncDataPtr(void){
    if( ++elem_ind >= arg[ vec_ind ].length() ){
      if( ++vec_ind < arg.size() ){
	elem_ind = 0;
      }
      else{
	hasMoreData = false;
      }
    }
  };

public:
  PrintfDataServer( const ArrayVector& arg_ ):arg(arg_),vec_ind(1),elem_ind(0){
    //vec_ind starts with 1, because zeroth argument is format string
    hasMoreData = (arg.size() > 1); //( (arg.size() > 1) && (arg[1].getLength() > 0));
  };

  void GetNextAsDouble(double& data){
    if( !hasMoreData )
      throw Exception("Error: no more data");
    if( arg[vec_ind].isEmpty()) {
      data = 0;
    } else {
      if( arg[ vec_ind ].isScalar() )
	data = arg[ vec_ind ].asDouble();
      else
	data = arg[ vec_ind ].get( elem_ind+1 ).asDouble();
    }
    IncDataPtr();
  };

  void GetNextAsString(std::string& str){
    if( !hasMoreData )
      throw Exception("Error: no more data");
    Array d(arg[ vec_ind ]);
    if (!d.isEmpty()) {
      if( d.isString() ){
	QString s = d.asString();
	QChar* data = s.data();
	while( elem_ind < s.length() ){
	  str.push_back((data + elem_ind++)->toAscii());
	}
      }else{
	str.push_back(d.get(elem_ind+1).asInteger());
      }
    }
    IncDataPtr();
  };
  bool HasMoreData(void){ return hasMoreData; };
};

//Common routine used by sprintf,printf,fprintf.  They all
//take the same inputs, and output either to a string, the
//console or a file.  For output to a console or a file, 
//we want escape-translation on.  For output to a string, we
//want escape-translation off.  So this common routine prints
//the contents to a string, which is then processed by each 
//subroutine.
void PrintfHelperFunction(int nargout, const ArrayVector& arg, PrintfStream& output, QByteArray& errmsg, Array& ret, bool convEscape = false ) 
{
  Array format(arg[0]);
  QString frmt = format.asString();

  std::vector<char> buff( frmt.length()+1 ); //vectors are contiguous in memory. we'll use it instead of char[]
  strncpy(&buff[0], frmt.toStdString().c_str(), frmt.length()+1 );

  // Search for the start of a format subspec
  char *dp = &buff[0];
  char *np;
  char sv;
  int nprn = 0;
  int noutput = 0;
  // Buffer to hold each sprintf result
#define BUFSIZE 65534
  char nbuff[BUFSIZE+1];
  memset( nbuff, 0, BUFSIZE+1 );

  // Buffer to hold the output
  //output.clear();
  errmsg.clear();

  PrintfDataServer ps( arg );

  //do while there is still data to output or format string to save
  while( (*dp) || ps.HasMoreData() ) {
    if ( !(*dp) && ps.HasMoreData() ) //still have arguments, need to rewind format.
      dp = &buff[0];

    np = dp;
    int nbuf_ind = 0;
    //copy string upto formatting character and do escape conversion in the process
    while ((*dp) && (*dp != '%') && nbuf_ind < BUFSIZE ){ 
      if (convEscape && isEscape(dp)) {
	switch (*(dp+1)) {
	case '\\':
	  *(nbuff+nbuf_ind) = '\\';
	  break;
	case 'n':
	  *(nbuff+nbuf_ind) = '\n';
	  break;
	case 't':
	  *(nbuff+nbuf_ind) = '\t';
	  break;
	case 'r':
	  *(nbuff+nbuf_ind) = '\r';
	  break;
	}
	dp += 2; ++nbuf_ind;
      } else
	*(nbuff+nbuf_ind++) = *(dp++);
    }

    // Print out the formatless segment
    *(nbuff+nbuf_ind) = 0;
    nprn = nbuf_ind; noutput += nbuf_ind;
    output << nbuff;    


    // Process the format spec
    if (*dp == '%' && *(dp+1)) {
      np = validateFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + QString(dp));
      else {
	if (*(np-1) == '%') {
	  nprn = snprintf(nbuff,BUFSIZE,"%%"); nbuff[std::min(nprn+1,BUFSIZE-1)]='\0'; noutput += nbuf_ind;
	  output << nbuff;
	  sv=0;
	} else 
	  if( *(np-1) == 's') {
	    std::string str;
	    ps.GetNextAsString( str );
	    const char* pStr = str.c_str();
	    sv = *np;
	    *np = 0;
	    nprn = snprintf(nbuff,BUFSIZE,dp,pStr); nbuff[std::min(nprn+1,BUFSIZE-1)]='\0'; noutput += nbuf_ind;
	    output << nbuff;
	  } else{
	    sv = *np;
	    *np = 0;

	    double data;
	    ps.GetNextAsDouble( data );

	    switch (*(np-1)) 
	      {
	      case 'd':
	      case 'i':
	      case 'o':
	      case 'u':
	      case 'x':
	      case 'X':
	      case 'c':
		nprn = snprintf(nbuff,BUFSIZE,dp,(int32)data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		output << nbuff;
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
		nprn = snprintf(nbuff,BUFSIZE,dp,data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		output << nbuff;
		break;
	      }
	  }
	if( sv )
	  *np = sv;
	dp = np;
      }
    }
  }
  ret = Array( static_cast<double>(noutput) );
}

//!
//@Module SPRINTF Formated String Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a string.  The general syntax for its use is
//@[
//  y = sprintf(format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|sprintf| command is not vectorized!  Each
//variable must be a scalar.  The returned value @|y| contains the
//string that would normally have been printed. For
//more details on the format string, see @|printf|.  
//@@Examples
//Here is an example of a loop that generates a sequence of files based on
//a template name, and stores them in a cell array.
//@<
//l = {}; for i = 1:5; s = sprintf('file_%d.dat',i); l(i) = {s}; end;
//l
//@>
//@@Tests
//@$exact#y1=sprintf('hello %d',5)
//@$exact#y1=sprintf('%d aa %s',5,'bcd')
//@$exact#y1=sprintf('%d %%aa %s %f',5,'bcd',5)
//@$exact#y1=sprintf('%d aa ',[5 6; 7 8])
//@@Signature
//function sprintf SprintfFunction
//inputs varargin
//outputs string
//!
ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("sprintf requires at least one (string) argument");
  if (!arg[0].isString())
    throw Exception("sprintf format argument must be a string");

  QString outf;
  PrintfStringStream textstream( &outf );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;

  ret << Array( outf );
  ret << output;
  return ret;
}

//!
//@Module PRINTF Formated Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to the output.  The general syntax for its use is
//@[
//  printf(format,a1,a2,...)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|printf| command is not vectorized!  Each
//variable must be a scalar.
//
//It is important to point out that the @|printf| function does not
//add a newline (or carriage return) to the output by default.  That
//can lead to some confusing behavior if you do not know what to expect.
//For example, the command @|printf('Hello')| does not appear to
//produce any output.  In fact, it does produce the text, but it then
//gets overwritten by the prompt.  To see the text, you need 
//@|printf('Hello\n')|.  This seems odd, but allows you to assemble a
//line using multiple @|printf| commands, including the @|'\n'| when
//you are done with the line.  You can also use the @|'\r'| character
//as an explicit carriage return (with no line feed).  This allows you
//to write to the same line many times (to show a progress string, for
//example).
//
//@@Format of the format string:
//
//The  format  string  is a character string, beginning and ending in its
//initial shift state, if any.  The format string is composed of zero  or
//more   directives:  ordinary  characters  (not  %),  which  are  copied
//unchanged to the output stream; and conversion specifications, each  of
//which results in fetching zero or more subsequent arguments.  Each 
//conversion specification is introduced by the character %, and ends with a
//conversion  specifier.  In between there may be (in this order) zero or
//more flags, an optional minimum field width, and an optional precision.
//
//The  arguments must correspond properly (after type promotion) with the
//conversion specifier, and are used in the order given.
//
//@@The flag characters:
//The character @|%| is followed by zero or more of the following flags:
//\begin{itemize}
//  \item @|\#|   The  value  should be converted to an ``alternate form''.  For @|o| conversions, the first character of the output  string  is  made  zero (by prefixing a @|0| if it was not zero already).  For @|x| and @|X| conversions, a nonzero result has the string @|'0x'| (or @|'0X'| for  @|X|  conversions) prepended to it.  For @|a, A, e, E, f, F, g,| and @|G|  conversions, the result will always  contain  a  decimal  point,  even  if  no digits follow it (normally, a decimal point appears  in the results of those conversions only if  a  digit  follows).  For @|g| and @|G| conversions, trailing zeros are not removed from the  result as they would otherwise be.  For other  conversions,  the  result is undefined.
//  \item @|0|   The value should be zero padded.  For @|d, i, o, u, x, X, a, A, e, E, f, F, g,| and @|G| conversions, the converted value is padded  on the  left  with  zeros rather than blanks.  If the @|0| and @|-| flags  both appear, the @|0| flag is ignored.  If  a  precision  is  given  with  a numeric conversion @|(d, i, o, u, x, and X)|, the @|0| flag is  ignored.  For other conversions, the behavior is undefined.
//  \item @|-|   The converted value is to be left adjusted on the  field  boundary.  (The default is right justification.) Except for @|n| conversions, the converted value is padded on the right  with  blanks, rather than on the left with blanks or zeros.  A @|-| overrides a @|0| if both are given.
//  \item @|' '| (a space) A blank should be left before a  positive  number  (or empty string) produced by a signed conversion.
//  \item @|+| A  sign  (@|+| or @|-|) always be placed before a number produced by a signed conversion.  By default a sign is used only for  negative numbers. A @|+| overrides a space if both are used.
//\end{itemize}
//@@The field width:
//An  optional decimal digit string (with nonzero first digit) specifying a 
//minimum field width.  If the converted  value  has  fewer  characters than 
//the  field  width,  it will be padded with spaces on the left (or right, 
//if the left-adjustment flag has been given).  A  negative  field width is 
//taken as a @|'-'| flag followed by a positive field  width. In no case does 
//a non-existent or small field width cause truncation of a field; if the 
//result of a conversion is wider than the  field  width, the field is 
//expanded to contain the conversion result.
//
//@@The precision:
//
//An  optional  precision,  in the form of a period (@|'.'|)  followed by an optional decimal digit string.  If the precision is given as just @|'.'|, or the precision is negative, the precision is  taken  to  be zero.   This  gives the minimum number of digits to appear for @|d, i, o, u, x|, and @|X| conversions, the number of digits to appear after the radix character  for  @|a, A, e, E, f|, and @|F| conversions, the maximum number of significant digits for @|g| and @|G| conversions, or the  maximum  number  of  characters to be printed from a string for s conversions.
//
//@@The conversion specifier:
//
//A character that specifies the type of conversion to be  applied.   The
//conversion specifiers and their meanings are:
//\begin{itemize}
//\item @|d,i|   The  int  argument is converted to signed decimal notation.  The  precision, if any, gives the minimum number of digits that  must   appear;  if  the  converted  value  requires fewer digits, it is    padded on the left with zeros. The default precision is @|1|.  When @|0|  is printed with an explicit precision @|0|, the output is empty.
//\item @|o,u,x,X|   The unsigned int argument is converted to  unsigned  octal  (@|o|),  unsigned  decimal  (@|u|),  or unsigned hexadecimal (@|x| and @|X|) notation.  The letters @|abcdef| are used for @|x| conversions;  the  letters @|ABCDEF| are used for @|X| conversions.  The precision, if any,  gives the minimum number of digits that must appear; if the converted  value  requires  fewer  digits, it is padded on the left  with zeros. The default precision is @|1|.  When @|0| is printed  with  an explicit precision @|0|, the output is empty.
//\item @|e,E|    The  double  argument  is  rounded  and  converted  in the style  @|[-]d.ddde dd| where there is one digit before  the  decimal-point  character and the number of digits after it is equal to the precision; if the precision is missing, it is taken as  @|6|;  if  the    precision  is  zero,  no  decimal-point character appears.  An @|E|  conversion uses the letter @|E| (rather than @|e|)  to  introduce  the  exponent.   The exponent always contains at least two digits; if  the value is zero, the exponent is @|00|.
//\item @|f,F|   The double argument is rounded and converted to decimal notation  in  the  style  @|[-]ddd.ddd|, where the number of digits after the decimal-point character is equal to the precision specification.  If  the precision is missing, it is taken as @|6|; if the precision  is explicitly zero, no decimal-point character  appears.   If  a   decimal point appears, at least one digit appears before it.
//\item @|g,G|   The double argument is converted in style @|f| or @|e| (or @|F| or @|E|  for  @|G|  conversions).  The precision specifies the number of significant digits.  If the precision is missing, @|6| digits  are  given;  if  the  precision is zero, it is treated as @|1|.  Style e is used   if the exponent from its conversion is less than @|-4|  or  greater than or equal to the precision.  Trailing zeros are removed from  the fractional part of the result; a decimal point appears  only  if it is followed by at least one digit.
//\item @|c| The int argument is  converted  to  an  unsigned  char, and  the resulting character is written.
//\item @|s| The string argument is printed.
//\item @|%|   A @|'%'| is written. No argument is converted. The complete conversion specification is @|'%%'|.
//\end{itemize}
//@@Example
//Here are some examples of the use of @|printf| with various arguments.  First we print out an integer and double value.
//@<
//printf('intvalue is %d, floatvalue is %f\n',3,1.53);
//@>
//Next, we print out a string value.
//@<
//printf('string value is %s\n','hello');
//@>
//Now, we print out an integer using 12 digits, zeros up front.
//@<
//printf('integer padded is %012d\n',32);
//@>
//Print out a double precision value with a sign, a total of 18 characters (zero prepended if necessary), a decimal point, and 12 digit precision.
//@<
//printf('float value is %+018.12f\n',pi);
//@>
//@@Signature
//sfunction printf PrintfFunction
//inputs varargin
//outpus none
//!
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("printf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("printf format argument must be a string");
  
  QString outf;
  PrintfStringStream textstream( &outf );
  
  QByteArray errmsg;
  Array output;
  
  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;
  
  eval->outputMessage( outf );
  return ArrayVector();
}

//!
//@Module FPRINTF Formated File Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a file.  The general syntax for its use is
//@[
//  fprintf(fp,format,a1,a2,...).
//@]
//or, 
//@[
//  fprintf(format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|ai| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|fprintf| command is not vectorized!  Each
//variable must be a scalar.  The value @|fp| is the file handle.  If @|fp| is omitted,
//file handle @|1| is assumed, and the behavior of @|fprintf| is effectively equivalent to @|printf|. For
//more details on the format string, see @|printf|.
//@@Examples
//A number of examples are present in the Examples section of the @|printf| command.
//@@Signature
//sfunction fprintf FprintfFunction
//inputs varargin
//outputs varargout
//!
ArrayVector FprintfFunction(int nargout, const ArrayVector& arg, 
			    Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("fprintf requires at least one (string) argument");
  ArrayVector argCopy(arg);
  int handle = 1;
  if (arg.size() > 1) {
    Array tmp(arg[0]);
    if (tmp.isScalar()) {
      handle = tmp.asInteger();
      argCopy.pop_front();
    }
    else {
      handle=1;
    }
  }
  if (handle == 1)
    return PrintfFunction(nargout,argCopy,eval);
  Array format(argCopy[0]);
  if (!format.isString())
    throw Exception("fprintf format argument must be a string");
  ArrayVector argcopy(arg);
  argcopy.pop_front();

  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));

  PrintfFileStream textstream( fptr->fp );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, argcopy, textstream, errmsg, output, true );
  return ArrayVector() << output;
}

//!
//@Module DISP Display a Variable or Expression
//@@Section IO
//@@Usage
//Displays the result of a set of expressions.  The @|disp| function
//takes a variable number of arguments, each of which is an expression
//to output:
//@[
//  disp(expr1,expr2,...,exprn)
//@]
//This is functionally equivalent to evaluating each of the expressions
//without a semicolon after each.
//@@Example
//Here are some simple examples of using @|disp|.
//@<
//a = 32;
//b = 1:4;
//disp(a,b,pi)
//@>
//@@Signature
//sfunction disp DispFunction
//inputs varargin
//outputs none
//!
ArrayVector DispFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  for (int i=0;i<arg.size();i++) 
    PrintArrayClassic(arg[i],eval->getPrintLimit(),eval);
  return ArrayVector();
} 

