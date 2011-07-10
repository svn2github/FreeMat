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

#include "Interpreter.hpp"
#include "Array.hpp"

//!
//@Module JITCONTROL Control the Just In Time Compiler
//@@Section FREEMAT
//@@Usage
//The @|jitcontrol| functionality in FreeMat allows you to control
//the use of the Just In Time (JIT) compiler.  Starting in FreeMat
//version 4, the JIT compiler is enabled by default on all platforms
//where it is successfully built.  The JIT compiler should significantly
//improve the performance of loop intensive, scalar code.  As development
//progresses, more and more functionality will be enabled under the JIT.
//In the mean time (if you use the GUI version of FreeMat) you can use
//the JIT chat window to get information on why your code was JIT compiled
//(or not).
//@@Tests
//@{ do_jit_test.m
//function test_val = do_jit_test(test_name)
//printf('Running %s without JIT...',test_name);
//jitcontrol off
//tic; A1 = feval(test_name); nojit = toc;
//jitcontrol on
//% Run it twice to remove the compile time
//tic; A2 = feval(test_name); wjit = toc;
//tic; A2 = feval(test_name); wjit = toc;
//printf('Speedup is %g...',nojit/wjit);
//test_val = issame(A1,A2);
//if (test_val)
//  printf('PASS\n');
//else
//  printf('FAIL\n');
//end
//@}
//@{ jit_test001.m
//function A = jit_test001
//  A = zeros(1,100000);
//  for i=1:100000; A(i) = i; end;
//@}
//@{ jit_test002.m
//function A = jit_test002
//  A = zeros(512);
//  for i=1:512;
//    for j=1:512;
//       A(i,j) = i-j;
//    end
//  end
//@}
//@{ jit_test003.m
//function A = jit_test003
//  A = zeros(512);
//  for i=1:512
//    for j=1:512
//      k = i-j;
//      if ((k < 5) && (k>-5))
//         A(i,j) = k;
//      end
//    end
//  end
//@}
//@{ jit_test004.m
//function A = jit_test004
//  A = zeros(512);
//  for i=1:512
//    for j=1:512
//      k = i-j;
//      if ((k < 5) && (k>-5))
//         A(i,j) = k;
//      else
//         A(i,j) = i+j;
//      end
//    end
//  end
//@}
//@{ jit_test005.m
//function A = jit_test005
//  A = zeros(1,1000);
//  for i=1:1000
//    for m=1:100
//      if (i > 50)
//        k = 55;
//      elseif (i > 40)
//        k = 45;
//      elseif (i > 30)
//        k = 35;
//      else 
//        k = 15;
//      end
//    end
//    A(i) = k;
//  end
//@}
//@{ jit_test006.m
//function A = jit_test006
//  A = zeros(1000);
//  m = 0;
//  for i=1:1000;
//    for j=1:1000;
//      A(i,j) = i-j;
//      m = i;
//    end
//  end
//  A = m;
//@}
//@{ jit_test007.m
//function A = jit_test007
//  B = ones(1,100000);
//  j = 0;
//  for i=1:100000
//    j = j + B(1,i);
//  end
//  A = j;
//@}
//@{ jit_test008.m
//function A = jit_test008
//  A = zeros(1,10000);
//  for i=1:10010;
//    A(i) = i;
//  end
//@}
//@{ jit_test009.m
//function A = jit_test009
//  B = ones(1,10000);
//  A = 0;
//  for k=1:10000
//    A = A + B(k);
//  end
//@}
//@{ jit_test010.m
//function A = jit_test010
//  B = 1:100000;
//  A = zeros(1,100000);
//  for i=1:100000
//    A(i) = B(i);
//  end
//@}
//@{ jit_test011.m
//function A = jit_test011
//  A = zeros(1,100000);
//  B = 5;
//  for i=1:100000
//    C = B + i;
//    A(i) = C;
//  end
//@}
//@{ jit_test012.m
//function A = jit_test012
//  A = zeros(400);
//  for i=1:500
//    for j=1:500;
//      A(i,j) = i-j;
//    end
//  end
//@}
//@{ jit_test013.m
//function A = jit_test013
//  A = zeros(512);
//  for i=1:512
//    for j=1:512
//      A(i,j) = abs(i-j);
//    end
//  end
//@}
//@{ jit_test014.m
//function A = jit_test014
//  N = 500;
//  A = zeros(N,N);
//  for i=1:N;
//    for j=1:N;
//      A(j,i) = abs(i-j);
//    end
//  end
//@}
//@{ jit_test015.m
//function A = jit_test015
//  A = 0;
//  for i=0:200000
//    A = A + abs(sin(i/200000*pi));
//  end
//@}
//@{ jit_test016.m
//function A = jit_test016
//  A = 0;
//  for i=0:200000
//    A = A + abs(cos(i/200000*pi));
//  end
//@}
//@{ jit_test017.m
//function A = jit_test017
//  A = 0;
//  for j=1:100
//    B = j;
//    for i=1:10000
//      A = A + B;
//    end
//  end
//@}
//@{ jit_test018.m
//function A = jit_test018
//  A = 0;
//  for i=1.2:4000.2
//    A = A + i;
//  end
//@}
//@{ jit_test019.m
//function A = jit_test019
//  A = 150000;
//  C = 0;
//  for b=1:A
//    C = C + b;
//  end
//  A = C;
//@}
//@{ jit_test020.m
//function a = jit_test020
//  a = zeros(1,10000);
//  for i=1:10000;
//    a(i) = sec(i/10000);
//  end
//@}
//@{ jit_test021.m
//function A = jit_test021
//  A = zeros(100,100);
//  for i=1:(100*100)
//    A(i) = i;
//  end
//@}
//@{ jit_test022.m
//function A = jit_test022
//  A = 1:1000000;
//  B = 0;
//  for i=1:1000000;
//    B = B + A(i);
//  end
//  A = B;
//@}
//@{ jit_test023.m
//function A = jit_test023
//  A = zeros(1,100000);
//  for i=1:100000;
//    A(i) = i/5.0;
//  end
//@}
//@{ jit_test024.m
//function A = jit_test024
//  A = zeros(1,10000);
//  for j=1:10
//    for i=1:10000;
//      A(i) = tjit_sum(A(i),i);
//     end;
//  end;
//
//function y = tjit_sum(a,b)
//  y = a + tjit_double(b);
//
//function y = tjit_double(a)
//  y = a*2;
//@}
//@{ jit_test025.m
//function A = jit_test025
//  B = [3,4,5];
//  C = [1;2;3];
//  A = zeros(1,10000);
//  for i=1:10000
//    A(i) = B*C;
//  end
//@}
//@{ jit_test026.m
//function A = jit_test026
//  B = [3,4;5,2];
//  C = [2;5];
//  A = zeros(2,10000);
//  for i=1:10000
//    C(2,1) = i;
//    A(1:2,i) = B*C + 1;
//  end
//@}
//@{ jit_test027.m
//function A = jit_test027
//  A = zeros(1,10000);
//  B = [1,2,3];
//  C = [4,5,6];
//  for i=1:9000
//    A(1,i+(0:5)) = [B,C];
//  end
//@}
//@{ jit_test028.m
//function A = jit_test028
//  A = zeros(10000,1);
//  B = [1;2;3];
//  C = [4;5;6];
//  for i=1:9000
//    A(i+(0:5),1) = [B;C];
//  end
//@}
//@{ jit_test029.m
//function A = jit_test029
//  A = zeros(10000,1);
//  for i=1:10000
//    A(i,1) = 5*(i - 4);
//  end
//@}
//@{ jit_test030.m
//function A = jit_test030
//  P = [1,2,3,4,5];
//  B = [6,5,3,4,5];
//  A = [0,0,0,0,0];
//  for i=1:10000
//    C = P.*B;
//    D = P./B;
//    E = P.\B;
//    A = A + C + D - E;
//  end
//@}
//@{ jit_test031.m
//function A = jit_test031
//  A = zeros(2,10000);
//  B = [3,4;5,1];
//  C = [1;0];
//  for i=1:10000
//    A(1:2,i) = B \ [1;i];
//  end
//@}
//@{ jit_test032.m
//function A = jit_test032
//  A = zeros(10000,2);
//  B = [3,4;5,1];
//  for i=1:10000
//    A(i,1:2) = [1,i] / B;
//  end
//@}
//@{ jit_test033.m
//function A = jit_test033
//  A = zeros(100000,1);
//  for i=1:90000
//   A(i+(1:2:50),1) = A(i+(1:2:50),1) + 6;
//  end
//@}
//@{ jit_test034.m
//function A = jit_test034
//  A = zeros(1,100000);
//  for i=1:100000
//    A(i) = ((i > 500) && (i < 10000)) || ((i >= 20000) && (i <= 30000)) || (i == 50000);
//  end
//@}
//@{ jit_test035.m
//function A = jit_test035
//  A = zeros(2,100000);
//  C = [5,6];
//  for i=1:100000
//    C(2) = i;
//    A(1:2,i) = C';
//  end
//@}
//@{ jit_test036.m
//function A = jit_test036
//  A = zeros(2,100000);
//  B = zeros(5,100000);
//  for i=1:10000
//     for j=1:5
//        B(j,i) = j+i;
//     end;
//     A(1,i) = min(B(1:5,i));
//     A(2,i) = max(B(1:5,i));
//  end
//@}
//@{ jit_test037.m
//function A = jit_test037
//  A = zeros(5,100000);
//  for i=1:10000
//    for j=1:5
//      A(j,i) = (j-2.5)/2.5*0.87;
//    end;
//    A(1:5,i) = cos(A(1:5,i));
//  end
//@}
//@{ test_jit.m
//function success = test_jit
//  tests = dir('jit_test*.m');
//  success = true;
//  for i=1:numel(tests)
//    [direc,fname] = fileparts(tests(i).name);
//    success = success && do_jit_test(fname);
//  end
//@}
//@@Signature
//sfunction jitcontrol JITControlFunction
//inputs flag
//outputs flag
//!
ArrayVector JITControlFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->JITControl())
      return ArrayVector(Array(QString("on")));
    else
      return ArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("jitcontrol function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setJITControl(true);
    else if (txt == "OFF")
      eval->setJITControl(false);
    else
      throw Exception("jitcontrol function argument needs to be 'on/off'");
  }
  return ArrayVector();
}

//!
//@Module DBAUTO Control Dbauto Functionality
//@@Section DEBUG
//@@Usage
//The dbauto functionality in FreeMat allows you to debug your
//FreeMat programs.  When @|dbauto| is @|on|, then any error
//that occurs while the program is running causes FreeMat to 
//stop execution at that point and return you to the command line
//(just as if you had placed a @|keyboard| command there).  You can
//then examine variables, modify them, and resume execution using
//@|return|.  Alternately, you can exit out of all running routines
//via a @|retall| statement.  Note that errors that occur inside of
//@|try|/@|catch| blocks do not (by design) cause auto breakpoints.  The
//@|dbauto| function toggles the dbauto state of FreeMat.  The
//syntax for its use is
//@[
//   dbauto(state)
//@]
//where @|state| is either
//@[
//   dbauto('on')
//@]
//to activate dbauto, or
//@[
//   dbauto('off')
//@]
//to deactivate dbauto.  Alternately, you can use FreeMat's string-syntax
//equivalence and enter
//@[
//   dbauto on
//@]
//or 
//@[
//   dbauto off
//@]
//to turn dbauto on or off (respectively).  Entering @|dbauto| with no arguments
//returns the current state (either 'on' or 'off').
//@@Signature
//sfunction dbauto DbAutoFunction
//inputs flag
//outputs flag
//!
ArrayVector DbAutoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->AutoStop()) 
      return ArrayVector(Array(QString("on")));
    else 
      return ArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("dbauto function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setAutoStop(true);
    else if (txt == "OFF")
      eval->setAutoStop(false);
    else
      throw Exception("dbauto function argument needs to be 'on/off'");
  }
  return ArrayVector();
}


