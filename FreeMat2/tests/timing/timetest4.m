%>> timetest4
%Run time for case 1 is 0.010479 ok
%Run time for case 2 is 0.043684 
%Run time for case 3 is 0.055329
%Run time for case 4 is 0.082830
%Run time for case 5 is 0.784426 ok
%Run time for case 6 is 0.124472 
%Run time for case 7 is 0.738984
%Run time for case 8 is 0.047794
%Run time for case 9 is 0.115748 ok
%Run time for case 10 is 0.051994
%Run time for case 11 is 0.224738
%Run time for case 12 is 0.083184
%Run time for case 13 is 0.055793
%
% FreeMat 2.0
%--> timetest4
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
% Run time for case 1 is 0.002028
% Run time for case 2 is 2.191146
% Run time for case 3 is 2.220203
% Run time for case 4 is 2.640038
% Run time for case 5 is 2.701126
% Run time for case 6 is 2.207211
% Run time for case 7 is 0.000087
% Run time for case 8 is 2.197576
% Run time for case 9 is 2.196498
%
% FreeMat
%--> timetest4
% Run time for case 1 is 0.016014
% Run time for case 2 is 0.257976
% Run time for case 3 is 0.387730
% Run time for case 4 is 0.768118
% Run time for case 5 is 1.156680
% Run time for case 6 is 0.340649
% Run time for case 7 is 2.346052
% Run time for case 8 is 0.275053
%
% FreeMat 2.1 - ForLoopHelper and literals pre-instancing.
%--> timetest4
% Run time for case 1 is 0.001104
% Run time for case 2 is 0.272548
% Run time for case 3 is 0.388887
% Run time for case 4 is 0.596402
% Run time for case 5 is 0.809294
% Run time for case 6 is 0.336881
% Run time for case 7 is 2.396140
% Run time for case 8 is 0.090501
% Run time for case 9 is 0.124992
% Run time for case 10 is 0.286669
% Run time for case 11 is 0.706027
%
% Octave (test 7 hangs)
% 
%octave:2> timetest4
%Run time for case 1 is 0.020229
%Run time for case 2 is 0.156994
%Run time for case 3 is 0.203542
%Run time for case 4 is 0.245667
%Run time for case 5 is 0.157356
%Run time for case 6 is 0.203923
%Run time for case 7 is 0.000379
%Run time for case 8 is 0.155819
%Run time for case 9 is 0.128124
%octave:3>
% Looking at test #10, I don't see why this is at 268 ms.  A simple
% look at demo(17) indicates that the lookup time for a variable
% should be no more than 30ms, and there are copies being made.
% Commenting out the call to rhsExpression changes the runtime to
% 
% Here is the current summary of performance:
%--> timetest4(1)
% Run time for case 1 is 0.016765 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.278605 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.359015 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.558223 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.802652 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.305044 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.290032 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.089313 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.120596 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.270645 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.631564 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.552238 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.165995 vs 0.055793 , 12.100000 objects copied
%
% Case 13 is interesting.  No data copies are made, but it is still 3X too slow.
% One possible culprit is the mutex.  Commenting out the QMutexLocker yields:
% Run time for case 1 is 0.016167 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.251691 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.413307 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.658801 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.781440 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.273061 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.171303 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.083725 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.106028 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.214963 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.711335 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.657318 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.141301 vs 0.055793 , 12.100000 objects copied
%
% Demo18 does a simple j=i assignment inside a loop, via the Context interface.
% its run time is 65 ms.
% Demo19 uses the scope interface (bypassing Context).  Its run time is 55 msec.
%
% One way to get this type of performance is to have the parser annotate the 
% variables at parse time. But I still need to understand the 2X penalty for
% case 13 between what I think should happen and what appears to be happening.
%
% Demo20 uses the context interface, but includes the ArrayVector routing.
% This has a penalty, and brings the total up to about 92 msec.
%
% Special casing the lookup of identifiers in Interpreter::expression() yields the
% following run times:
%--> timetest4
% Run time for case 1 is 0.016563 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.258637 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.276692 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.476320 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.803687 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.224013 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.152764 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.073595 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.110448 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.219278 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.503066 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.489793 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.089882 vs 0.055793 , 12.100000 objects copied
%-->
%This remaining ~30ms penalty could be due to the trip through Context instead of Scope.
%to try to eliminate this penalty, I kept track of the top and bottom scopes
%in local variables in Context (to avoid the repeated calls to back() and
%front() on the scope stack).  
%
%%After this change, the times are:
%--> timetest4
% Run time for case 1 is 0.016548 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.259969 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.276733 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.483276 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.825061 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.227777 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.204037 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.085292 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.105501 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.256824 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.501826 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.488088 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.095693 vs 0.055793 , 12.100000 objects copied
%
% Demo21 measures the time associated with an Array->Array copy.  It clocks
% in at about 3ms for a copy.  Not too bad.  So what is the discrepancy between
% the expression "a=b" and Demo18 (65 ms vs 95 ms)?  
%
%
% Demo19 vs Demo20 - Demo19 runs in about 56 msec, and Demo20 runs in about
% 120 msec.  The only difference between the two is in the Context interface
% (which is used in Demo20, and not in Demo19).  I had concluded that
% removing the mutex in Context improved the performance.    Indeed, the
% mutex locks cost about 30 msec, so without them Demo20 runs in 96 msec
% instead of 56 msec.
%
% Caching the top and bottom of the scopestack in Context improves the situation slightly 
% (to about 90 msec) for Demo20.  The real difference (extra 30 msec) is the
% call to ArrayVector.  The current setup is:
%
%   demo(18) = 57 msec
%   demo(19) = 53 msec
%   demo(20) = 88 msec
%
% So the use of the context interface itself is no big penalty.  There is a 31msec
% penalty because of the ArrayVector call.  That's not very good.
%
% Here is the current state of the test:
%
%--> timetest4
% Run time for case 1 is 0.016045 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.254365 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.273559 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.473558 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.792444 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.222127 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.180805 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.071660 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.102978 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.208291 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.493852 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.089337 vs 0.048534 , 13.100000 objects copied
% Run time for case 13 is 0.471501 vs 0.083184 , 100012.100000 objects copied
% Run time for case 14 is 0.089109 vs 0.055793 , 12.100000 objects copied
%-->
%
% So the entitlement of case 14 is 57msec.  The 89 comes from where? Not from
% a call to ArrayVector (no vectors are constructed in this case).  This
% overhead likely comes from the tree class.  Consider the oprofile results from
% running copytest (which exercises case 14 a number of times)
%
% 263445   12.6617  Scope::lookupVariable(std::string const&)
% 217239   10.4410  Interpreter::block(tree)
% 191219    9.1904  Scope::insertVariable(std::string const&, Array const&)
% 179948    8.6487  tree::child(unsigned int)
% 164621    7.9120  tree::tree(tree const&)
% 133411    6.4120  tree_node::getCopy()
% 116538    5.6011  FreeTreeNode(tree_node*)
% 111053    5.3374  Interpreter::assignmentStatement(tree, bool)
% 85213     4.0955  Dimensions::getElementCount() const
% 78114     3.7543  Interpreter::expression(tree)
% 49462     2.3773  void ForLoopHelper<int>(tree, Class, int const*, int, std::string, Interpreter*)
% 42439     2.0397  tree::~tree()
% 41091     1.9749  Array::operator=(Array const&)
% 35906     1.7257  Scope::isVariableGlobal(std::string const&)
% 35523     1.7073  Interpreter::statement(tree)
% 33888     1.6287  Context::insertVariable(std::string const&, Array const&)
% 32467     1.5604  Interpreter::statementType(tree, bool)
% 28373     1.3637  Array::isEmpty() const
% 27757     1.3341  Array::getReadWriteDataPointer()
% 26224     1.2604  Context::lookupVariable(std::string const&)
% 19825     0.9528  Scope::isVariablePersistent(std::string const&)
% 17796     0.8553  Array::ensureSingleOwner()
% 16311     0.7839  Data::getCopy()
% 15046     0.7231  Array::isScalar() const
% 14037     0.6746  Array::int32RangeConstructor(int, int, int, bool)
% 13592     0.6533  Array::~Array()
% 13111     0.6301  Dimensions::isScalar() const
% 12829     0.6166  Data::deleteCopy()
%
%
% The stuff in this call which is not present in Demo(18)?  Lets run Demo(18) and see:
% 
% 13482    24.2709  Scope::lookupVariable(std::string const&)
% 10018    18.0349  Scope::isVariableGlobal(std::string const&)
% 8398     15.1185  HDemoFunction(int, std::vector<Array, std::allocator<Array> > const&, Interpreter*)
% 7520     13.5378  Scope::insertVariable(std::string const&, Array const&)
% 3761      6.7707  Context::insertVariable(std::string const&, Array const&)
% 3741      6.7347  Array::operator=(Array const&)
% 2941      5.2945  Data::getCopy()
% 2881      5.1865  Scope::isVariablePersistent(std::string const&)
% 2038      3.6689  Context::lookupVariable(std::string const&)
% 507       0.9127  Data::deleteCopy()
%
% Tree related routines take up about: 8.6+7.9+6.4+5.6+2.0 = 30% of the run time
% I infer that tree.numchildren() (which is called in Interpreter::block(tree)) is also
% a problem, contributing about 10.4 percent.  This means that 40% of the run time
% is spent in tree related stuff (at least in this case)!  Totally unacceptable.
%
% For now, I have disabled reference counting for trees, and added const& reference
% passing for most methods in Interpreter
%
% 
%
% --> timetest4
% Run time for case 1 is 0.014018 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.255559 vs 0.044388 , 100011.000000 objects copied
% Run time for case 3 is 0.278972 vs 0.055616 , 100011.100000 objects copied
% Run time for case 4 is 0.503169 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.882688 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.224979 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.414330 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.053312 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.091693 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.207056 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.524458 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.072698 vs 0.048534 , 13.100000 objects copied
% Run time for case 13 is 0.507487 vs 0.083184 , 100012.100000 objects copied
% Run time for case 14 is 0.075657 vs 0.055793 , 12.100000 objects copied
%
% This is pretty good.  I consider case 14 to be done.  OK - looking at the profile
% output 
%
%26911    19.5473  Scope::lookupVariable(std::string const&)
%18613    13.5199  Scope::insertVariable(std::string const&, Array const&)
%12231     8.8842  Interpreter::block(tree const&)
%10412     7.5630  Interpreter::assignmentStatement(tree const&, bool)
%8455      6.1415  Dimensions::getElementCount() const
%6766      4.9146  Scope::isVariableGlobal(std::string const&)
%4852      3.5243  Context::insertVariable(std::string const&, Array const&)
%
% suggests more cycles hiding in Interpreter::block.  A slight tweak (changed the
% tests for steptrap).
% yeilds 62.8 msec. woohoo!
%
% On to the next test of interest.
%
%--> timetest4
% Run time for case 1 is 0.013820 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.230351 vs 0.044388 , 100011.000000 objects copied
% Run time for case 3 is 0.242330 vs 0.055616 , 100011.100000 objects copied
% Run time for case 4 is 0.442080 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.757253 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.200452 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.187634 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.049926 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.084510 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.200774 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.459391 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.064036 vs 0.048534 , 13.100000 objects copied
% Run time for case 13 is 0.447284 vs 0.083184 , 100012.100000 objects copied
% Run time for case 14 is 0.064714 vs 0.055793 , 12.100000 objects copied
%
% Cases 12 and 14 are as good as its going to get.  On to case 13.  To clarify
% the behavior, I added a new case (15):
%
%  G{15} = 'm = 3; n = 0; for i=1:100000; n = m+1; end;';
%
% --> timetest4
% Run time for case 1 is 0.013520 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.228841 vs 0.044388 , 100011.000000 objects copied
% Run time for case 3 is 0.245186 vs 0.055616 , 100011.100000 objects copied
% Run time for case 4 is 0.445338 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.744549 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.201953 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.149101 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.049095 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.081966 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.184903 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.459388 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.063991 vs 0.048534 , 13.100000 objects copied
% Run time for case 13 is 0.442942 vs 0.083184 , 100012.100000 objects copied
% Run time for case 14 is 0.064326 vs 0.055793 , 12.100000 objects copied
% Run time for case 15 is 0.443490 vs 0.083837 , 100013.100000 objects copied
% Run time for case 16 is 0.455521 vs 0.295581 , 100015.100000 objects copied
% Run time for case 17 is 0.623559 vs 0.323746 , 200016.100000 objects copied
%
% So what happens with "n=m+1"?
%
% There is a copy that occurs, but that copy seems to be special cased.
% Case 16 is not a special case.  So lets look at the oprofile output.
%
%samples  %        symbol name
%164300   10.6389  Dimensions::getElementCount() const
%129833    8.4071  Add(Array, Array)
%54018     3.4978  Interpreter::block(tree const&)
%50936     3.2983  Interpreter::expression(tree const&)
%45203     2.9270  Array::isEmpty() const
%38694     2.5056  Array::getDimensions() const
%36900     2.3894  std::vector<std::string, std::allocator<std::string> >::vector(std::vector<std::string, std
%::allocator<std::string> > const&)
%35072     2.2710  void ForLoopHelper<int>(tree const&, Class, int const*, int, std::string, Interpreter*)
%34291     2.2204  Dimensions::isScalar() const
%32317     2.0926  Array::getLength() const
%28668     1.8563  void addfullreal<int>(int, int*, int const*, int, int const*, int)
%27381     1.7730  VectorCheck(Array&, Array&, bool, std::string)
%26402     1.7096  Array::isScalar() const
%26086     1.6892  Array::getDataPointer() const
%22826     1.4781  Array::promoteType(Class)
%22541     1.4596  std::vector<std::string, std::allocator<std::string> >::begin() const
%
%If we cache the number of elements in an array, we can introduce a 10% improvement.  The
%target is to get 30% out.  Possible places we can get the 30% from:
%
%  Dimensions::getElementCount() - 10.6%
%  Add(Array, Array)             -  8.4%
%  Array::isEmpty()              -  2.9%
%  Dimensions::isScalar()        -  2.2%
%  Array::getLength()            -  2.1%
%  Array::isScalar()             -  1.7%
%  Interpreter::block()          -  2.6% (treeVector iterator for loop)
%
% This yeilds: 30%
%octave:1> timetest4(0)
%Run time for case 1 is 0.022592
%Run time for case 2 is 0.162862
%Run time for case 3 is 0.208480
%Run time for case 4 is 0.239034
%Run time for case 5 is 0.161546
%Run time for case 6 is 0.207451
%Run time for case 7 is 0.127532
%Run time for case 8 is 0.159229
%Run time for case 9 is 0.131591
%Run time for case 10 is 0.126735
%Run time for case 11 is 0.719359
%Run time for case 12 is 0.210571
%Run time for case 13 is 0.243342
%Run time for case 14 is 0.207737
%Run time for case 15 is 0.244181
%Run time for case 16 is 0.599329
%Run time for case 17 is 0.619348
%Run time for case 18 is 0.443014
%
%
% Here's the current performance (with the update assignmentStatement
% performance tweak) (note these times are on a D410, not the dual P4.
%
%--> timetest4
% Run time for case 1 is 0.024412 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.151345 vs 0.044388 , 11.100000 objects copied
% Run time for case 3 is 0.082593 vs 0.055616 , 11.200000 objects copied
% Run time for case 4 is 0.315397 vs 0.083225 , 100011.100000 objects copied
% Run time for case 5 is 0.509582 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.151175 vs 0.126407 , 100014.000000 objects copied
% Run time for case 7 is 1.186230 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.061244 vs 0.044205 , 11.300000 objects copied
% Run time for case 9 is 0.059322 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.127254 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.326329 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.191671 vs 0.048534 , 100013.100000 objects copied
% Run time for case 13 is 0.313710 vs 0.083184 , 100012.200000 objects copied
% Run time for case 14 is 0.072043 vs 0.055793 , 12.200000 objects copied
%
% Case 13 and case 12 are interesting.   Here a copy can be avoided
%
% I needed to update the test.  Here's the latest numbers:
%--> timetest4
% Run time for case 1 is 0.026345 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.145332 vs 0.044388 , 11.100000 objects copied
% Run time for case 3 is 0.132110 vs 0.055616 , 11.200000 objects copied
% Run time for case 4 is 0.338322 vs 0.083225 , 100011.100000 objects copied
% Run time for case 5 is 0.510757 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.155246 vs 0.126407 , 100014.000000 objects copied
% Run time for case 7 is 0.247094 vs 0.756268 , 100009.200000 objects copied
% Run time for case 8 is 0.061240 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.059460 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.128537 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.322873 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.190352 vs 0.048534 , 100013.100000 objects copied
% Run time for case 13 is 0.314675 vs 0.083184 , 100012.200000 objects copied
% Run time for case 14 is 0.071883 vs 0.055793 , 12.200000 objects copied
% Run time for case 15 is 0.314853 vs 0.083837 , 100013.100000 objects copied
% Run time for case 16 is 0.322346 vs 0.295581 , 100015.100000 objects copied
% Run time for case 17 is 0.432502 vs 0.323746 , 200016.100000 objects copied
% Run time for case 18 is 0.347644 vs 0.233305 , 100016.200000 objects copied
%
% The next cases of interest are: 4, 12, 13, 15.
%
%  4: i = i + 1;
% 12: m = m;
% 13: m = m + 1;
% 15: n = m + 1;
%
% These expressions are all scalar expressions.  Why does 15 take so
% long? Can the copies be eliminated?  In three of these cases, the 
% expression is an "+1".  
%
%
function timetest4(countflag)
  runcount = 10;
  if (~exist('countflag')) countflag = 1; end
  G{1} = 'for i=1:100000; end;';
  G{2} = 'for i=1:100000; i = 2; end;';
  G{3} = 'm = 2; for i=1:100000; i = m; end;';
  G{4} = 'for i=1:100000; i = i + 1; end;';
  G{5} = 'for i=1:100000; i = [1,1]; end;';
  G{6} = 'm = [1,1]; for i=1:100000; i = m; end;';
%  G{7} = 'for i=1:100000; clear i; end;';
  G{7} = 'for i=1:100000;  i; end;';
  G{8} = 'for i=1:100000; m = 2; end;';
  G{9} = 'for i=1:100000; 2; end;';
  G{10} = 'm = [1,1]; for i=1:100000; m; end;';
  G{11} = 'm = [1,1]; for i=1:100000; n = m + m; end;';
  G{12} = 'm = [1,1]; for i=1:100000; m = m; end;';
  G{13} = 'm = 0; for i=1:100000; m = m + 1; end;';
  G{14} = 'm = 3; n = 0; for i=1:100000; n = m; end;';
  G{15} = 'm = 3; n = 0; for i=1:100000; n = m+1; end;';
  G{16} = 'm = [1,1]; n = 0; for i=1:100000; n = m+1; end;';
  G{17} = 'm = zeros(2,2,2); n = 0; for i=1:100000; n = m+1; end;';
  G{18} = 'm = 3; n = 0; g = [1,1]; for i=1:100000; n = g; n = m+1; end;';
  mperf = [0.010592 0.044388 0.055616 0.083225 0.778065 0.126407 0.756268 0.044205 0.119176 0.051550 0.231276 0.048534 0.083184 0.055793 0.083837 0.295581 0.323746 0.233305];
  for k = 1:numel(G)
    time = 0;
    ogen = 0;
    for j = 1:runcount
      if (countflag), ocount = demo(16); end;
      tic;
        eval(G{k});
        time = time + toc;
        if (countflag), ogen = ogen + demo(16) - ocount; end;
      end
      time = time/runcount;
      if (countflag), ogen = ogen/runcount; end;
      if (countflag)
        disp(sprintf('Run time for case %d is %f vs %f , %f objects copied',k,time,mperf(k),ogen));
      else
        disp(sprintf('Run time for case %d is %f',k,time));
      end
    end
    
      
      
      
  
