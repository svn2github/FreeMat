%!
%@Module HOLD Plot Hold Toggle Function
%@@Section HANDLE
%@@Usage
%Toggles the hold state on the currently active plot.  The
%general syntax for its use is
%@[
%   hold(state)
%@]
%where @|state| is either
%@[
%   hold('on')
%@]
%to turn hold on, or
%@[
%   hold('off')
%@]
%to turn hold off. If you specify no argument then
%@|hold| toggles the state of the hold:
%@[
%   hold
%@]
%You can also specify a particular axis to the hold command
%@[
%   hold(handle,...)
%@]
%where @|handle| is the handle for a particular axis.
%@@Function Internals
%The @|hold| function allows one to construct a plot sequence
%incrementally, instead of issuing all of the plots simultaneously
%using the @|plot| command.
%@@Example
%Here is an example of using both the @|hold| command and the
%multiple-argument @|plot| command to construct a plot composed
%of three sets of data.  The first is a plot of a modulated Gaussian.
%@<
%x = linspace(-5,5,500);
%t = exp(-x.^2);
%y = t.*cos(2*pi*x*3);
%plot(x,y);
%mprint hold1
%@>
%@figure hold1
%
%We now turn the hold state to @|'on'|, and add another plot
%sequence, this time composed of the top and bottom envelopes of
%the modulated Gaussian.  We add the two envelopes simultaneously
%using a single @|plot| command.  The fact that @|hold| is
%@|'on'| means that these two envelopes are added to (instead of
%replace) the current contents of the plot.
%@<
%plot(x,y);
%hold on
%plot(x,t,'g-',x,-t,'b-')
%mprint hold2
%@>
%@figure hold2
%!

% Copyright (c) 2002-2006 Samit Basu

function h = hold(varargin)
if (nargin == 0)
     hold_toggle(gca);
elseif (strcomp(varargin{1},'on'))
     hold_on(gca);
elseif (strcomp(varargin{1},'off'))
     hold_off(gca);
elseif (ishandle(varargin{1}))
   if (nargin == 1)
     hold_toggle(varargin{1})
   elseif (strcomp(varargin{2},'on'))
     hold_on(varargin{1});
   elseif (strcomp(varargin{2},'off'))
     hold_off(varargin{1});
   else
     error('Unrecognized form of hold');
   end
else
   error('Unrecognized arguments to hold');
end

function hold_off(handle)
  set(gca,'nextplot','replace');

function hold_on(handle)
  set(gcf,'nextplot','add');
  set(gca,'nextplot','add');

function hold_toggle(handle);
  if (strcomp(get(gca,'nextplot'),'replace'))
    set(gca,'nextplot','add');
  else
    set(gca,'nextplot','replace');
  end
