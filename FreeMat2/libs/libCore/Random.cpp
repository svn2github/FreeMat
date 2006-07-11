/*
 * Copyright (c) 2002-2006 Samit Basu
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

#include "Core.hpp"
#include "Exception.hpp"
#include "Malloc.hpp"
#include <math.h>
#include <stdio.h>
#include "RanLib.hpp"

static bool initialized = false;

//!
//@Module SEED Seed the Random Number Generator
//@@Section RANDOM
//@@Usage
//Seeds the random number generator using the given integer seeds.  
//Changing the seed allows you to choose which pseudo-random
//sequence is generated.  The seed takes two @|uint32| values:
//@[
//  seed(s,t)
//@]
//where @|s| and @|t| are the seed values.
//@@Example
//Here's an example of how the seed value can be used to reproduce
//a specific random number sequence.
//@<
//seed(32,41);
//rand(1,5)
//seed(32,41);
//rand(1,5)
//@>
//!
ArrayVector SeedFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("Seed function requires a two integer arguments");
  Array tmp1(arg[0]);
  uint32 seedval1;
  Array tmp2(arg[1]);
  uint32 seedval2;
  seedval1 = tmp1.getContentsAsIntegerScalar();
  seedval2 = tmp2.getContentsAsIntegerScalar();
  setall(seedval1,seedval2);
  init_genrand(seedval1);
  initialized = true;
  return ArrayVector();
}

//!
//@Module RANDBETA Beta Deviate Random Number Generator
//@@Section RANDOM
//@@Usage
//Creates an array of beta random deviates based on the supplied
//two parameters. The general syntax for @|randbeta| is 
//@[
//   y = randbeta(alpha, beta)
//@]
//where @|alpha| and @|beta| are the two parameters of the 
//random deviate.  There are three forms for calling @|randbeta|.
//The first uses two vectors @|alpha| and @|beta| of the same
//size, in which case the output @|y| is the same size as both
//inputs, and each deviate uses the corresponding values of @|alpha|
//and @|beta| from the arguments.  In the other forms, either
//@|alpha| or @|beta| are scalars.
//@@Function Internals
//The probability density function (PDF) of a beta random variable
//is
//\[
//f(x) = x^(a-1) * (1-x)^(b-1) / B(a,b)
//\]
//for @|x| between 0 and 1.  The function @|B(a,b)| is defined so
//that the integral of @|f(x)| is 1.
//@@Example
//Here is a plot of the PDF of a beta random variable with @|a=3|,
//@|b=7|.
//@<
//a = 3; b = 7;
//x = (0:100)/100; t = x.^(a-1).*(1-x).^(b-1); 
//t = t/(sum(t)*.01);
//plot(x,t);
//mprint betapdf
//@>
//which is plotted as
//@figure betapdf
//If we generate a few random deviates with these values,
//we see they are distributed around the peak of roughly
//@|0.25|.
//@<
//randbeta(3*ones(1,5),7*ones(1,5))
//@>
//!
ArrayVector RandBetaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbeta requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randbeta requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_FLOAT);
  arg2.promoteType(FM_FLOAT);
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) 
    dp[i] = genbet(p1[i*arg1_advance],p2[i*arg2_advance]);
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDI Uniformly Distributed Integer
//@@Section RANDOM
//@@Usage
//Generates an array of uniformly distributed integers between
//the two supplied limits.  The general syntax for @|randi| is
//@[
//   y = randi(low,high)
//@]
//where @|low| and @|high| are arrays of integers.  Scalars
//can be used for one of the arguments.  The output @|y| is
//a uniformly distributed pseudo-random number between @|low|
//and @|high| (inclusive).
//@@Example
//Here is an example of a set of random integers between 
//zero and 5:
//@<
//randi(zeros(1,6),5*ones(1,6))
//@>
//!
ArrayVector RandIFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randi requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randbeta requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_INT32);
  arg2.promoteType(FM_INT32);
  int32 *dp;
  dp = (int32 *) Malloc(sizeof(int32)*outDims.getElementCount());
  int32 *p1;
  p1 = (int32*) arg1.getDataPointer();
  int32 *p2;
  p2 = (int32*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) 
    dp[i] = ignuin(p1[i*arg1_advance],p2[i*arg2_advance]);
  return singleArrayVector(Array(FM_INT32,outDims,dp));
}
  
//!
//@Module RANDCHI Generate Chi-Square Random Variable
//@@Section RANDOM
//@@Usage
//Generates a vector of chi-square random variables with the
//given number of degrees of freedom.  The general syntax for
//its use is 
//@[
//   y = randchi(n)
//@]
//where @|n| is an array containing the degrees of freedom for
//each generated random variable.
//@@Function Internals
//A chi-square random variable is essentially distributed as
//the squared Euclidean norm of a vector of standard Gaussian random 
//variables.  The number of degrees of freedom is generally the
//number of elements in the vector.  In general, the PDF of
//a chi-square random variable is
//\[
// f(x) = \frac{x^{r/2-1}e^{-x/2}}{\Gamma(r/2)2^{r/2}}
//\]
//@@Example
//First, a plot of the PDF for a family of chi-square random variables
//@<
//f = zeros(7,100);
//x = (1:100)/10;
//for n=1:7;t=x.^(n/2-1).*exp(-x/2);f(n,:)=10*t/sum(t);end
//plot(x,f');
//mprint chipdf
//@>
//The PDF is below:
//@figure chipdf
//Here is an example of using @|randchi| and @|randn| to compute
//some chi-square random variables with four degrees of freedom.
//@<
//randchi(4*ones(1,6))
//sum(randn(4,6).^2)
//@>
//!
ArrayVector RandChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randchi requires exactly one parameter (the vector of degrees of freedom)");
  Array arg1(arg[0]);
  if (arg1.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  arg1.promoteType(FM_FLOAT);
  // Output dimension is the larger of the two
  Dimensions outDims(arg1.getDimensions());
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    if (p1[i] <= 0)
      throw Exception("argument to randchi must be positive");
    dp[i] = genchi(p1[i]);
  }
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDEXP Generate Exponential Random Variable
//@@Section RANDOM
//@@Usage
//Generates a vector of exponential random variables with
//the specified parameter.  The general syntax for its use is
//@[
//   y = randexp(lambda)
//@]
//where @|lambda| is a vector containing the parameters for
//the generated random variables.
//@@Function Internals
//The exponential random variable is usually associated with
//the waiting time between events in a Poisson random process.
//The PDF of an exponential random variable is:
//\[
//   f(x) = \lambda e^{-\lambda x}
//\]
//@@Example
//Here is an example of using the @|randexp| function to generate
//some exponentially distributed random variables
//@<
//randexp(ones(1,6))
//@>
//!
ArrayVector RandExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randexp requires exactly one parameter (the vector of means)");
  Array arg1(arg[0]);
  if (arg1.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  arg1.promoteType(FM_FLOAT);
  // Output dimension is the larger of the two
  Dimensions outDims(arg1.getDimensions());
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) 
    dp[i] = genexp(p1[i]);
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDP Generate Poisson Random Variable
//@@Section RANDOM
//@@Usage
//Generates a vector Poisson random variables with the given
//parameters.  The general syntax for its use is
//@[
//   y = randp(nu),
//@]
//where @|nu| is an array containing the rate parameters
//for the generated random variables.  
//@@Function Internals
//A Poisson random variable is generally defined by taking the
//limit of a binomial distribution as the sample size becomes
//large, with the expected number of successes being fixed (so
//that the probability of success decreases as @|1/N|).  
//The Poisson distribution is given by
//\[
//  P_{\nu}(n) = \frac{\nu^n e^{-nu}}{n!}.
//\]
//@@Example
//Here is an exmaple of using @|randp| to generate some Poisson
//random variables, and also using @|randbin| to do the same
//using @|N=1000| trials to approximate the Poisson result.
//@<
//randp(33*ones(1,10))
//randbin(1000*ones(1,10),33/1000*ones(1,10))
//@>
//!
ArrayVector RandPoissonFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randp requires exactly one parameter (the vector of means)");
  Array arg1(arg[0]);
  if (arg1.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  arg1.promoteType(FM_FLOAT);
  // Output dimension is the larger of the two
  Dimensions outDims(arg1.getDimensions());
  int32 *dp;
  dp = (int32 *) Malloc(sizeof(int32)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) 
    dp[i] = ignpoi(p1[i]);
  return singleArrayVector(Array(FM_INT32,outDims,dp));
}

//!
//@Module RANDBIN Generate Binomial Random Variables
//@@Section RANDOM
//@@Usage
//Generates random variables with a binomial distribution.
//The general syntax for its use is
//@[
//   y = randbin(N,p)
//@]
//where @|N| is a vector representing the number of Bernoulli
//trials, and @|p| is the success probability associated with each
//trial.
//@@Function Internals
//A Binomial random variable describes the number of successful
//outcomes from @|N| Bernoulli trials, with the probability of
//success in each trial being @|p|.  The probability distribution
//is
//\[
//   P(n) = \frac{N!}{n!(N-n)!}p^n(1-p)^{N-n}
//\]
//@@Example
//Here we generate @|10| binomial random variables, corresponding
//to @|N=100| trials, each with probability @|p=0.1|, using
//both @|randbin| and then again using @|rand| (to simulate the trials):
//@<
//randbin(100,.1*ones(1,10))
//sum(rand(100,10)<0.1)
//@>
//!
ArrayVector RandBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbin requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randbin requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_UINT32);
  arg2.promoteType(FM_FLOAT);
  uint32 *dp;
  dp = (uint32 *) Malloc(sizeof(uint32)*outDims.getElementCount());
  uint32 *p1;
  p1 = (uint32*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    dp[i] = ignbin(p1[i*arg1_advance],p2[i*arg2_advance]);
  }
  return singleArrayVector(Array(FM_UINT32,outDims,dp));
}

//!
//@Module RANDNBIN Generate Negative Binomial Random Variables
//@@Section RANDOM
//@@Usage
//Generates random variables with a negative binomial distribution.
//The general syntax for its use is
//@[
//   y = randnbin(r,p)
//@]
//where @|r| is a vector of integers representing the number of
//successes, and @|p| is the probability of success.
//@@Function Internals
//A negative binomial random variable describes the number of failures
//@|x| that occur in @|x+r| bernoulli trials, with a success on the 
//@|x+r| trial.  The pdf is given by
//\[
//  P_{r,p}(x)=\left(\begin{matrix}x+r-1\\r-1\end{matrix}\right)p^r(1-p)^x.
//\]
//@@Example
//Here we generate some negative binomial random variables:
//@<
//randnbin(3*ones(1,4),.01)
//randnbin(6*ones(1,4),.01)
//@>
//!
ArrayVector RandNBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnbin requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randnbin requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_UINT32);
  arg2.promoteType(FM_FLOAT);
  uint32 *dp;
  dp = (uint32 *) Malloc(sizeof(uint32)*outDims.getElementCount());
  uint32 *p1;
  p1 = (uint32*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    dp[i] = ignnbn(p1[i*arg1_advance],p2[i*arg2_advance]);
  }
  return singleArrayVector(Array(FM_UINT32,outDims,dp));
}

//!
//@Module RANDF Generate F-Distributed Random Variable
//@@Section RANDOM
//@@Usage
//Generates random variables with an F-distribution.  The general
//syntax for its use is
//@[
//   y = randf(n,m)
//@]
//where @|n| and @|m| are vectors of the number of degrees of freedom
//in the numerator and denominator of the chi-square random variables
//whose ratio defines the statistic.
//@@Function Internals
//The statistic @|F_{n,m}| is defined as the ratio of two chi-square
//random variables:
//\[
//  F_{n,m} = \frac{\chi_n^2/n}{\chi_m^2/m}
//\]
//The PDF is given by
//\[
//  f_{n,m} = \frac{m^{m/2}n^{n/2}x^{n/2-1}}{(m+nx)^{(n+m)/2}B(n/2,m/2)},
//\]
//where @|B(a,b)| is the beta function.
//@@Example
//Here we use @|randf| to generate some F-distributed random variables,
//and then again using the @|randchi| function:
//@<
//randf(5*ones(1,9),7)
//randchi(5*ones(1,9))./randchi(7*ones(1,9))
//@>
//!
ArrayVector RandFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randf requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randf requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_FLOAT);
  arg2.promoteType(FM_FLOAT);
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    if ((p1[i*arg1_advance] <= 0) || (p2[i*arg2_advance]) <= 0)
      throw Exception("randf requires positive arguments");
    dp[i] = genf(p1[i*arg1_advance],p2[i*arg2_advance]);
  }
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDGAMMA Generate Gamma-Distributed Random Variable
//@@Section RANDOM
//@@Usage
//Generates random variables with a gamma distribution.  The general
//syntax for its use is
//@[
//   y = randgamma(a,r),
//@]
//where @|a| and @|r| are vectors describing the parameters of the
//gamma distribution.  Roughly speaking, if @|a| is the mean time between
//changes of a Poisson random process, and we wait for the @|r| change,
//the resulting wait time is Gamma distributed with parameters @|a| 
//and @|r|.
//@@Function Internals
//The Gamma distribution arises in Poisson random processes.  It represents
//the waiting time to the occurance of the @|r|-th event in a process with
//mean time @|a| between events.  The probability distribution of a Gamma
//random variable is
//\[
//   P(x) = \frac{a^r x^{r-1} e^{-ax}}{\Gamma(r)}.
//\]
//Note also that for integer values of @|r| that a Gamma random variable
//is effectively the sum of @|r| exponential random variables with parameter
//@|a|.
//@@Example
//Here we use the @|randgamma| function to generate Gamma-distributed
//random variables, and then generate them again using the @|randexp|
//function.
//@<
//randgamma(1,15*ones(1,9))
//sum(randexp(ones(15,9)))
//@>
//!
ArrayVector RandGammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randgamma requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randgamma requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_FLOAT);
  arg2.promoteType(FM_FLOAT);
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    dp[i] = gengam(p1[i*arg1_advance],p2[i*arg2_advance]);
  }
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDMULTI Generate Multinomial-distributed Random Variables
//@@Section RANDOM
//@@Usage
//This function generates samples from a multinomial distribution
//given the probability of each outcome.  The general syntax for
//its use is
//@[
//   y = randmulti(N,pvec)
//@]
//where @|N| is the number of experiments to perform, and @|pvec|
//is the vector of probabilities describing the distribution of
//outcomes.
//@@Function Internals
//A multinomial distribution describes the number of times each
//of @|m| possible outcomes occurs out of @|N| trials, where each
//outcome has a probability @|p_i|.  More generally, suppose that
//the probability of a Bernoulli random variable @|X_i| is @|p_i|,
//and that 
//\[
//   \sum_{i=1}^{m} p_i = 1.
//\]
//Then the probability that @|X_i| occurs @|x_i| times is
//\[
//   P_N(x_1,x_2,\ldots,x_n) = \frac{N!}{x_1!\cdots x_n!} p_1^{x_1}\cdots p_n^{x_n}.
//\]
//@@Example
//Suppose an experiment has three possible outcomes, say heads,
//tails and edge, with probabilities @|0.4999|, @|0.4999| and
//@|0.0002|, respectively.  Then if we perform ten thousand coin
//flips we get
//@<
//randmulti(10000,[0.4999,0.4999,0.0002])
//@>
//!
ArrayVector RandMultiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randmulti requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  int N = arg1.getContentsAsIntegerScalar();
  if (N<0) 
    throw Exception("number of events to generate for randmulti must be a nonnegative integer");
  arg2.promoteType(FM_FLOAT);
  // Verify the correctness of the probability argument
  float *dp;
  dp = (float*) arg2.getDataPointer();
  float Psum = 0.0;
  int i;
  for (i=0;i<arg2.getLength();i++) {
    if ((dp[i] < 0) || (dp[i] > 1)) 
      throw Exception("probabiliy vector argument to randmulti must have all elements between 0 and 1");
    Psum += dp[i];
  }
  if (Psum > 1.00000F)
    throw Exception("sum of probabilities (effectively) greater than 1");
  Dimensions outDims;
  outDims = arg2.getDimensions();
  int32 *ip = (int32*) Malloc(sizeof(int32)*arg2.getLength());
  genmul(N,dp,arg2.getLength(),(long int*) ip);
  return singleArrayVector(Array(FM_INT32,outDims,ip));
}
  
//!
//@Module RANDNCHI Generate Noncentral Chi-Square Random Variable
//@@Section RANDOM
//@@Usage
//Generates a vector of non-central chi-square random variables
//with the given number of degrees of freedom and the given
//non-centrality parameters.  The general syntax for its use is
//@[
//   y = randnchi(n,mu)
//@]
//where @|n| is an array containing the degrees of freedom for
//each generated random variable (with each element of @|n| >= 1),
//and @|mu| is the non-centrality shift (must be positive).
//@@Function Internals
//A non-central chi-square random variable is the sum of a chisquare
//deviate with @|n-1| degrees of freedom plus the square of a normal
//deviate with mean @|mu| and standard deviation 1.
//@@Examples
//Here is an example of a non-central chi-square random variable:
//@<
//randnchi(5*ones(1,9),0.3)
//@>
//!
ArrayVector RandNChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnchi requires two parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  if (arg1.isEmpty() || arg2.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  // Check the logic to see if one or both are scalar values
  if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
    throw Exception("randnchi requires either one of the two arguments to be a scalar, or both arguments to be the same size");
  int arg1_advance;
  int arg2_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  // Output dimension is the larger of the two
  Dimensions outDims;
  if (arg1.getLength() > arg2.getLength()) {
    outDims = arg1.getDimensions();
  } else {
    outDims = arg2.getDimensions();
  }
  arg1.promoteType(FM_FLOAT);
  arg2.promoteType(FM_FLOAT);
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    if (p1[i*arg1_advance] <= 1.0)
      throw Exception("degrees of freedom argument must be > 1.0");
    if (p2[i*arg2_advance] < 0.0)
      throw Exception("noncentrality parameter must be positive");
    dp[i] = gennch(p1[i*arg1_advance],p2[i*arg2_advance]);
  }
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

//!
//@Module RANDNF Generate Noncentral F-Distribution Random Variable
//@@Section RANDOM
//@@Usage
//Generates a vector of non-central F-distributed random variables
//with the specified parameters.  The general syntax for its use is
//@[
//   y = randnf(n,m,c)
//@]
//where @|n| is the number of degrees of freedom in the numerator,
//and @|m| is the number of degrees of freedom in the denominator.
//The vector @|c| determines the non-centrality shift of the numerator.
//@@Function Internals
//A non-central F-distributed random variable is the ratio of a
//non-central chi-square random variable and a central chi-square random
//variable, i.e.,
//\[
//   F_{n,m,c} = \frac{\chi_{n,c}^2/n}{\chi_m^2/m}.
//\]
//@@Example
//Here we use the @|randf| to generate some non-central F-distributed
//random variables:
//@<
//randnf(5*ones(1,9),7,1.34)
//@>
//!
ArrayVector RandNFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("randnf requires three parameter arguments");
  Array arg1(arg[0]);
  Array arg2(arg[1]);
  Array arg3(arg[2]);
  if (arg1.isEmpty() || arg2.isEmpty() || arg3.isEmpty()) 
    return singleArrayVector(Array::emptyConstructor());
  int arg1_advance;
  int arg2_advance;
  int arg3_advance;
  arg1_advance = (arg1.isScalar()) ? 0 : 1;
  arg2_advance = (arg2.isScalar()) ? 0 : 1;
  arg3_advance = (arg3.isScalar()) ? 0 : 1;
  if (arg1_advance && arg2_advance && (!arg1.getDimensions().equals(arg2.getDimensions())))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg1_advance && arg3_advance && (!arg1.getDimensions().equals(arg3.getDimensions())))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg2_advance && arg3_advance && (!arg2.getDimensions().equals(arg3.getDimensions())))
    throw Exception("vector arguments to randnf must be the same size");
  // Output dimension is the larger of the two
  Dimensions outDims;
  if ((arg1.getLength() > arg2.getLength()) && (arg1.getLength() > arg3.getLength()))
    outDims = arg1.getDimensions();
  else if ((arg2.getLength() > arg1.getLength()) && (arg2.getLength() > arg3.getLength()))
    outDims = arg2.getDimensions();
  else if ((arg3.getLength() > arg1.getLength()) && (arg3.getLength() > arg2.getLength()))
    outDims = arg3.getDimensions();
  arg1.promoteType(FM_FLOAT);
  arg2.promoteType(FM_FLOAT);
  arg3.promoteType(FM_FLOAT);
  float *dp;
  dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
  float *p1;
  p1 = (float*) arg1.getDataPointer();
  float *p2;
  p2 = (float*) arg2.getDataPointer();
  float *p3;
  p3 = (float*) arg3.getDataPointer();
  int i;
  for (i=0;i<outDims.getElementCount();i++) {
    if (p1[i*arg1_advance] <= 1.0)
      throw Exception("numerator degrees of freedom argument must be > 1.0");
    if (p2[i*arg2_advance] <= 0.0)
      throw Exception("denominator degrees of freedom must be positive");
    if (p3[i*arg2_advance] < 0.0)
      throw Exception("noncentrality parameter must be non-negative");
    dp[i] = gennf(p1[i*arg1_advance],p2[i*arg2_advance],p3[i*arg3_advance]);
  }
  return singleArrayVector(Array(FM_FLOAT,outDims,dp));
}

void InitializeRandGen() {
  unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
  init_by_array(init, length);
  initialized = true;
}

ArrayVector RandStateControl(const ArrayVector& arg) {
  char* key = arg[0].getContentsAsCString();
  if (!((strcmp(key,"state") == 0) ||
	(strcmp(key,"STATE") == 0)))
    throw Exception("expecting string 'state' as first argument");
  if (arg.size() == 1) {
    uint32 *mt = (uint32*) Malloc(sizeof(uint32)*625);
    GetRandStateVect(mt);
    return singleArrayVector(Array(FM_UINT32,Dimensions(625,1),mt));
  } else {
    Array statevec(arg[1]);
    if ((statevec.isScalar()) && (statevec.getContentsAsIntegerScalar() == 0)) {
      InitializeRandGen();
      return ArrayVector();
    } else {
      statevec.promoteType(FM_UINT32);
      if (statevec.getLength() != 625)
	throw Exception("illegal state vector - must be of length 625");
      SetRandStateVect((uint32*)statevec.getDataPointer());
      return ArrayVector();
    }
  }
}

//!
//@Module RANDN Gaussian (Normal) Random Number Generator
//@@Section RANDOM
//@@Usage
//Creates an array of pseudo-random numbers of the specified size.
//The numbers are normally distributed with zero mean and a unit
//standard deviation (i.e., @|mu = 0, sigma = 1|). 
// Two seperate syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//  y = randn(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//random numbers.  The type of @|y| is @|double|, a 64-bit floating
//point array.  To get arrays of other types, use the typecast 
//functions.
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//  y = randn([d1,d2,...,dn]).
//@]
//This syntax is more convenient for calling @|randn| using a 
//variable for the argument.
//
//Finally, @|randn| supports two additional forms that allow
//you to manipulate the state of the random number generator.
//The first retrieves the state
//@[
//  y = randn('state')
//@]
//which is a 625 length integer vector.  The second form sets
//the state
//@[
//  randn('state',y)
//@]
//or alternately, you can reset the random number generator with
//@[
//  randn('state',0)
//@]
//@@Function Internals
//Recall that the
//probability density function (PDF) of a normal random variable is
//\[
//f(x) = \frac{1}{\sqrt{2\pi \sigma^2}} e^{\frac{-(x-\mu)^2}{2\sigma^2}}.
//\]
//The Gaussian random numbers are generated from pairs of uniform random numbers using a transformation technique. 
//@@Example
//The following example demonstrates an example of using the first form of the @|randn| function.
//@<
//randn(2,2,2)
//@>
//The second example demonstrates the second form of the @|randn| function.
//@<
//randn([2,2,2])
//@>
//In the next example, we create a large array of 10000  normally distributed pseudo-random numbers.  We then shift the mean to 10, and the variance to 5.  We then numerically calculate the mean and variance using @|mean| and @|var|, respectively.
//@<
//x = 10+sqrt(5)*randn(1,10000);
//mean(x)
//var(x)
//@>
//Now, we use the state manipulation functions of @|randn| to exactly reproduce 
//a random sequence.  Note that unlike using @|seed|, we can exactly control where
//the random number generator starts by saving the state.
//@<
//randn('state',0)    % restores us to startup conditions
//a = randn(1,3)      % random sequence 1
//b = randn('state'); % capture the state vector
//c = randn(1,3)      % random sequence 2  
//randn('state',b);   % restart the random generator so...
//c = randn(1,3)      % we get random sequence 2 again
//@>
//!
ArrayVector RandnFunction(int nargout, const ArrayVector& arg) {
  if (!initialized) 
    InitializeRandGen();
  int i;
  Array t;
  Dimensions dims;
  int32 *dp;
  if (arg.size() == 0)
    dims.makeScalar();
  else {
    if (arg[0].isString())
      return RandStateControl(arg);
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (i=0;i<arg.size();i++)
      allScalars &= arg[i].isScalar();
    if (allScalars) {
      t = arg[0];
      if (arg.size() == 1) {
	// If all scalars and only one argument - we want a square zero matrix
	dims[0] = t.getContentsAsIntegerScalar();
	dims[1] = dims[0];
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=0;i<arg.size();i++) {
	  t = arg[i];
	  dims[i] = t.getContentsAsIntegerScalar();
	}
      }
    } else {
      if (arg.size() > 1)
	throw Exception("Arguments to randn function must be either all scalars or a single vector");
      t = arg[0];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims[i] = dp[i];
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims[i] >= 0);
    if (!allPositive)
      throw Exception("Randn function requires positive arguments");
  }
  double *qp;
  qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
  int len;
  int j;
  len = dims.getElementCount();
  for (j=0;j<len;j+=2) {
    double x1, x2, w, y1, y2;
    do {
      x1 = 2.0*genrand_res53()-1.0;
      x2 = 2.0*genrand_res53()-1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    qp[j] = y1;
    if (j<len-1)
      qp[j+1] = y2;
  }
  return singleArrayVector(Array(FM_DOUBLE,dims,qp));
}

//!
//@Module RAND Uniform Random Number Generator
//@@Section RANDOM
//@@Usage
//Creates an array of pseudo-random numbers of the specified size.
//The numbers are uniformly distributed on @|[0,1)|.  
//Two seperate syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//  y = rand(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//random numbers.  The type of @|y| is @|double|, a 64-bit floating
//point array.  To get arrays of other types, use the typecast 
//functions.
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//  y = rand([d1,d2,...,dn]).
//@]
//This syntax is more convenient for calling @|rand| using a 
//variable for the argument.
//
//Finally, @|rand| supports two additional forms that allow
//you to manipulate the state of the random number generator.
//The first retrieves the state
//@[
//  y = rand('state')
//@]
//which is a 625 length integer vector.  The second form sets
//the state
//@[
//  rand('state',y)
//@]
//or alternately, you can reset the random number generator with
//@[
//  rand('state',0)
//@]
//@@Example
//The following example demonstrates an example of using the first form of the @|rand| function.
//@<
//rand(2,2,2)
//@>
//The second example demonstrates the second form of the @|rand| function.
//@<
//rand([2,2,2])
//@>
//The third example computes the mean and variance of a large number of uniform random numbers.  Recall that the mean should be @|1/2|, and the variance should be @|1/12 ~ 0.083|.
//@<
//x = rand(1,10000);
//mean(x)
//var(x)
//@>
//Now, we use the state manipulation functions of @|rand| to exactly reproduce 
//a random sequence.  Note that unlike using @|seed|, we can exactly control where
//the random number generator starts by saving the state.
//@<
//rand('state',0)    % restores us to startup conditions
//a = rand(1,3)      % random sequence 1
//b = rand('state'); % capture the state vector
//c = rand(1,3)      % random sequence 2  
//rand('state',b);   % restart the random generator so...
//c = rand(1,3)      % we get random sequence 2 again
//@>
//!
ArrayVector RandFunction(int nargout, const ArrayVector& arg) {
  if (!initialized)
    InitializeRandGen();
  int i;
  Array t;
  Dimensions dims;
  int32 *dp;
  if (arg.size() == 0)
    dims.makeScalar();
  else {
    // Check for state assignment
    if (arg[0].isString())
      return RandStateControl(arg);
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (i=0;i<arg.size();i++)
      allScalars &= arg[i].isScalar();
    if (allScalars) {
      t = arg[0];
      if (arg.size() == 1) {
	// If all scalars and only one argument - we want a square zero matrix
	dims[0] = t.getContentsAsIntegerScalar();
	dims[1] = dims[0];
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=0;i<arg.size();i++) {
	  t = arg[i];
	  dims[i] = t.getContentsAsIntegerScalar();
	}	  
      }
    } else {
      if (arg.size() > 1)
	throw Exception("Arguments to rand function must be either all scalars or a single vector");
      t = arg[0];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims[i] = dp[i];
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims[i] >= 0);
    if (!allPositive)
      throw Exception("Rand function requires posItive arguments");
  }
  double *qp;
  qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
  int len;
  int j;
  len = dims.getElementCount();
  for (j=0;j<len;j++)
    qp[j] = genrand_res53();
  return singleArrayVector(Array(FM_DOUBLE,dims,qp));
}
