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

#include "Exception.hpp"
#include <math.h>
#include <stdio.h>
#include "RanLib.hpp"
#include "Operators.hpp"
#include "Utils.hpp"

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
//where @|s| and @|t| are the seed values.  Note that due to limitations
//in @|ranlib|, the values of @|s,t| must be between @|0 <= s,t <= 2^30|.
//@@Example
//Here's an example of how the seed value can be used to reproduce
//a specific random number sequence.
//@<
//seed(32,41);
//rand(1,5)
//seed(32,41);
//rand(1,5)
//@>
//@@Tests
//@{ test_seed1.m
//function test_val = test_seed1
//seed(32,41);
//  a1 = rand(1,5);
//  seed(32,41);
//  a2 = rand(1,5);
//  test_val = issame(a1,a2);
//@}
//@@Signature
//function seed SeedFunction jitsafe
//inputs s t
//outputs none
//!
ArrayVector SeedFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("Seed function requires a two integer arguments");
  uint32 seedval1;
  uint32 seedval2;
  seedval1 = (uint32) arg[0].asInteger();
  seedval2 = (uint32) arg[1].asInteger();
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
//@@Signature
//function randbeta RandBetaFunction jitsafe
//inputs alpha beta
//outputs y
//!

struct OpRandBeta {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,float>(genbet(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai, const T& br, const T& bi, T& cr, T& ci) {
    throw Exception("randbeta is not supported for complex arguments");
  }
};

ArrayVector RandBetaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbeta requires two parameter arguments");
  return DotOp<OpRandBeta>(arg[0],arg[1]);
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
//@@Signature
//function randi RandIFunction jitsafe
//inputs low high
//outputs y
//!
struct OpRandI {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignuin(long(v1),long(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randi is not supported for complex arguments");
  }
};

ArrayVector RandIFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randi requires two parameter arguments");
  return DotOp<OpRandI>(arg[0],arg[1]);
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
//@@Signature
//function randchi RandChiFunction jitsafe
//inputs n
//outputs y
//!
struct OpRandChi {
  template <typename T>
  static inline T func(const T& v1) {
    if (v1 <= 0) throw Exception("argument to randchi must be positive");
    return CastConvert<T,float>(genchi(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randchi is not supported for complex arguments");
  }
};

ArrayVector RandChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randchi requires exactly one parameter (the vector of degrees of freedom)");
  return UnaryOp<OpRandChi>(arg[0]);
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
//@@Signature
//function randexp RandExpFunction jitsafe
//inputs lambda
//outputs y
//!
struct OpRandExp {
  template <typename T>
  static inline T func(const T& v1) {
    return CastConvert<T,float>(genexp(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randexp is not supported for complex arguments");
  }
};

ArrayVector RandExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randexp requires exactly one parameter (the vector of means)");
  return UnaryOp<OpRandExp>(arg[0]);
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
//@@Signature
//function randp RandPoissonFunction jitsafe
//inputs n
//outputs y
//!
struct OpRandPoisson {
  template <typename T>
  static inline T func(const T& v1) {
    return CastConvert<T,int32>(ignpoi(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randp is not supported for complex arguments");
  }
};

ArrayVector RandPoissonFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randp requires exactly one parameter (the vector of means)");
  return UnaryOp<OpRandPoisson>(arg[0]);
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
//@@Signature
//function randbin RandBinFunction jitsafe
//inputs N p
//outputs y
//!
struct OpRandBin {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignbin(uint32(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randbin is not supported for complex arguments");
  }
};

ArrayVector RandBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbin requires two parameter arguments");
  return DotOp<OpRandBin>(arg[0],arg[1]);
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
//  P_{r,p}(x)=\left(\begin{matrix} x+r-1 \\ r-1 \end{matrix}\right)p^r(1-p)^x.
//\]
//@@Example
//Here we generate some negative binomial random variables:
//@<
//randnbin(3*ones(1,4),.01)
//randnbin(6*ones(1,4),.01)
//@>
//@@Signature
//function randnbin RandNBinFunction jitsafe
//inputs r p
//outputs y
//!
struct OpRandNBin {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignnbn(uint32(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randnbin is not supported for complex arguments");
  }
};

ArrayVector RandNBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnbin requires two parameter arguments");
  return DotOp<OpRandNBin>(arg[0],arg[1]);
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
//@@Signature
//function randf RandFFunction jitsafe
//inputs n m
//outputs y
//!
struct OpRandF {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if ((v1 <= 0) || (v2 <= 0)) throw Exception("randf requires positive arguments");
    return CastConvert<T,float>(genf(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randf is not supported for complex arguments");
  }  
};

ArrayVector RandFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randf requires two parameter arguments");
  return DotOp<OpRandF>(arg[0],arg[1]);
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
//@@Signature
//function randgamma RandGammaFunction jitsafe
//inputs a r
//outputs y
//!
struct OpRandGamma {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,float>(gengam(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randgamma is not supported for complex arguments");
  }  
};

ArrayVector RandGammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randgamma requires two parameter arguments");
  return DotOp<OpRandGamma>(arg[0],arg[1]);
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
//@@Signature
//function randmulti RandMultiFunction jitsafe
//inputs N pvec
//outputs y
//!
ArrayVector RandMultiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randmulti requires two parameter arguments");
  if (arg[0].isEmpty() || arg[1].isEmpty())
    return ArrayVector(EmptyConstructor());
  int N = arg[0].asInteger();
  if (N<0) 
    throw Exception("number of events to generate for randmulti must be a nonnegative integer");
  Array arg2 = arg[1].toClass(Float);
  if (arg2.isSparse()) throw Exception("randmulti does not work with sparse arguments");
  BasicArray<float> &dp(arg2.real<float>());
  float Psum = 0;
  for (index_t i=1;i<=arg2.length();i++) {
    if ((dp[i] < 0) || (dp[i] > 1))
      throw Exception("probability vector argument to randmulti must have all elements between 0 and 1");
    Psum += dp[i];
  }
  for (index_t i=1;i<=arg2.length();i++) 
    dp[i] /= Psum;
  NTuple outDims(arg2.dimensions());
  BasicArray<int32> rp(outDims);
  genmul(N,dp.data(),int(arg2.length()),(long*)rp.data());
  return ArrayVector(Array(rp).toClass(Double));
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
//@@Signature
//function randnchi RandNChiFunction jitsafe
//inputs n mu
//outputs y
//!
struct OpRandNChi {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (v1 <= 1) throw Exception("degrees of freedom argument must be > 1");
    if (v2 < 0) throw Exception("noncentrality parameter must be positive");
    return CastConvert<T,float>(gennch(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randnchi is not supported for complex arguments");
  }  
};

ArrayVector RandNChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnchi requires two parameter arguments");
  return DotOp<OpRandNChi>(arg[0],arg[1]);
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
//@@Signature
//function randnf RandNFFunction jitsafe
//inputs n m c
//outputs y
//!
ArrayVector RandNFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("randnf requires three parameter arguments");
  Array arg1(arg[0].asDenseArray().toClass(Float));
  Array arg2(arg[1].asDenseArray().toClass(Float));
  Array arg3(arg[2].asDenseArray().toClass(Float));
  if (arg1.isEmpty() || arg2.isEmpty() || arg3.isEmpty()) 
    return ArrayVector(EmptyConstructor());
  int arg1_advance = (arg1.isScalar()) ? 0 : 1;
  int arg2_advance = (arg2.isScalar()) ? 0 : 1;
  int arg3_advance = (arg3.isScalar()) ? 0 : 1;
  if (arg1_advance && arg2_advance && (arg1.dimensions() != arg2.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg1_advance && arg3_advance && (arg1.dimensions() != arg3.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg2_advance && arg3_advance && (arg2.dimensions() != arg3.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  // Output dimension is the larger of the two
  NTuple outDims;
  if ((arg1.length() > arg2.length()) && (arg1.length() > arg3.length()))
    outDims = arg1.dimensions();
  else if ((arg2.length() > arg1.length()) && (arg2.length() > arg3.length()))
    outDims = arg2.dimensions();
  else if ((arg3.length() > arg1.length()) && (arg3.length() > arg2.length()))
    outDims = arg3.dimensions();
  if (arg1.isScalar()) arg1 = Uniform(outDims,arg1.constRealScalar<float>());
  if (arg2.isScalar()) arg2 = Uniform(outDims,arg2.constRealScalar<float>());
  if (arg3.isScalar()) arg3 = Uniform(outDims,arg3.constRealScalar<float>());
  BasicArray<float> dp(outDims);
  const BasicArray<float> &p1(arg1.constReal<float>());
  const BasicArray<float> &p2(arg2.constReal<float>());
  const BasicArray<float> &p3(arg3.constReal<float>());
  for (index_t i=1;i<=dp.length();i++) {
    if (p1[i] <= 1.0) throw Exception("numerator degrees of freedom must be > 1.0");
    if (p2[i] <= 0.0) throw Exception("denominator degrees of freedom must be positive");
    if (p3[i] < 0.0) throw Exception("noncentrality parameter must be non-negative");
    dp[i] = gennf(p1[i],p2[i],p3[i]);
  }
  return ArrayVector(Array(dp));
}

static void InitializeRandGen() {
  unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
  init_by_array(init, length);
  initialized = true;
}

static ArrayVector RandStateControl(const ArrayVector& arg) {
  QString key = arg[0].asString().toUpper();
  if (!(key=="STATE"))
    throw Exception("expecting string 'state' as first argument");
  if (arg.size() == 1) {
    BasicArray<uint32> mp(NTuple(625,1));
    GetRandStateVect(mp.data());
    return ArrayVector(Array(mp).toClass(Double));
  } else {
    Array statevec(arg[1]);
    if ((statevec.isScalar()) && (statevec.asInteger() == 0)) {
      InitializeRandGen();
      return ArrayVector();
    } else {
      statevec = statevec.toClass(UInt32);
      if (statevec.length() != 625)
	throw Exception("illegal state vector - must be of length 625");
      SetRandStateVect(statevec.real<uint32>().data());
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
//@@Signature
//function randn RandnFunction jitsafe
//inputs varargin
//outputs y
//!
ArrayVector RandnFunction(int nargout, const ArrayVector& arg) {
  if (!initialized) 
    InitializeRandGen();
  if ((arg.size() > 0) && (arg[0].isString()))
    return RandStateControl(arg);
  NTuple dim(ArrayVectorAsDimensions(arg));
  BasicArray<double> qp(dim);
  index_t len = dim.count();
  for (index_t j=1;j<=len;j+=2) {
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
    if (j<=(len-1))
      qp[j+1] = y2;
  }
  return ArrayVector(Array(qp));
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
//@@Signature
//function rand RandFunction jitsafe
//inputs varargin
//outputs y
//!
ArrayVector RandFunction(int nargout, const ArrayVector& arg) {
  if (!initialized)
    InitializeRandGen();
  if ((arg.size() > 0) && (arg[0].isString()))
    return RandStateControl(arg);
  NTuple dim(ArrayVectorAsDimensions(arg));
  BasicArray<double> qp(dim);
  for (index_t j=1;j<=qp.length();j++) 
    qp[j] = genrand_res53();
  return ArrayVector(Array(qp));
}
