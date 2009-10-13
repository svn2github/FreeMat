/*
 * Copyright (c) 2009 Samit Basu
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
#include <QtCore>
#include "Array.hpp"

//!
//@Module TIC Start Stopwatch Timer
//@@Section FREEMAT
//@@Usage
//Starts the stopwatch timer, which can be used to time tasks in FreeMat.
//The @|tic| takes no arguments, and returns no outputs.  You must use
//@|toc| to get the elapsed time.  The usage is
//@[
//  tic
//@]
//@@Example
//Here is an example of timing the solution of a large matrix equation.
//@<
//A = rand(100);
//b = rand(100,1);
//tic; c = A\b; toc
//@>
//@@Signature
//function tic TicFunction
//inputs none
//outputs none
//!
  
static QTime ticvalue;

ArrayVector TicFunction(int nargout, const ArrayVector& arg) {
  ticvalue.start();
  return ArrayVector();
}

//!
//@Module CLOCK Get Current Time
//@@Section FreeMat
//@@Usage
//Returns the current date and time as a vector.  The syntax for its use is
//@[
//   y = clock
//@]
//where @|y| has the following format:
//@[
//   y = [year month day hour minute seconds]
//@]
//@@Example
//Here is the time that this manual was last built:
//@<
//clock
//@>
//@@Signature
//function clock ClockFunction
//inputs none
//outputs y
//!
ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
  QDateTime ctime(QDateTime::currentDateTime());
  Array retvec(Double,NTuple(1,6));
  BasicArray<double> &dp(retvec.real<double>());
  dp[1] = ctime.date().year();
  dp[2] = ctime.date().month();
  dp[3] = ctime.date().day();
  dp[4] = ctime.time().hour();
  dp[5] = ctime.time().minute();
  dp[6] = ctime.time().second() + ctime.time().msec()/1.0e3;
  return ArrayVector(retvec);
}

//!
//@Module CLOCKTOTIME Convert Clock Vector to Epoch Time
//@@Section FreeMat
//@@Usage
//Given the output of the @|clock| command, this function computes
//the epoch time, i.e, the time in seconds since January 1,1970 
//at 00:00:00 UTC.  This function is most useful for calculating elapsed
//times using the clock, and should be accurate to less than a millisecond
//(although the true accuracy depends on accuracy of the argument vector). 
//The usage for @|clocktotime| is
//@[
//   y = clocktotime(x)
//@]
//where @|x| must be in the form of the output of @|clock|, that is
//@[
//   x = [year month day hour minute seconds]
//@]
//@@Example
//Here is an example of using @|clocktotime| to time a delay of 1 second
//@<
//x = clock
//sleep(1)
//y = clock
//clocktotime(y) - clocktotime(x)
//@>
//@@Signature
//function clocktotime ClockToTimeFunction
//inputs x
//outputs y
//!
ArrayVector ClockToTimeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  Array targ(arg[0].toClass(Double));
  if (targ.length() != 6)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  const BasicArray<double> &dp(targ.constReal<double>());
  QDate t_date;
  t_date.setDate(int(dp[1]),int(dp[2]),int(dp[3]));
  QTime t_time;
  t_time.setHMS(int(dp[4]),int(dp[5]),int(dp[6]));
  QDateTime t_datetime(t_date,t_time);
  double retval = t_datetime.toTime_t() + (dp[6] - int(dp[6]));
  return ArrayVector(Array(retval));
}
  

//!
//@Module TOC Stop Stopwatch Timer
//@@Section FREEMAT
//@@Usage
//Stop the stopwatch timer, which can be used to time tasks in FreeMat.
//The @|toc| function takes no arguments, and returns no outputs.  You must use
//@|toc| to get the elapsed time.  The usage is
//@[
//  toc
//@]
//@@Example
//Here is an example of timing the solution of a large matrix equation.
//@<
//A = rand(100);
//b = rand(100,1);
//tic; c = A\b; toc
//@>
//@@Signature
//function toc TocFunction
//inputs none
//outputs y
//!
ArrayVector TocFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(ticvalue.elapsed()/1.0e3));
}
