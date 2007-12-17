%!
%@Module IMAGESC Image Display Function
%@@Section HANDLE
%@@Usage
%The @|imagesc| command has the following general syntax
%@[
%  handle = imagesc(x,y,C,clim)
%@]
%where @|x| is a two vector containing the @|x| coordinates
%of the first and last pixels along a column, and @|y| is a
%two vector containing the @|y| coordinates of the first and
%last pixels along a row.  The matrix @|C| constitutes the
%image data.  It must either be a scalar matrix, in which case
%the image is colormapped using the  @|colormap| for the current
%figure.  If the matrix is @|M x N x 3|, then @|C| is intepreted
%as RGB data, and the image is not colormapped.  The @|clim|
%argument is a pairs [low high] that specifies scaling.  You can 
%also omit the @|x| and @|y|, 
%@[
%  handle = imagesc(C, clim)
%@]
%in which case they default to @|x = [1,size(C,2)]| and 
%@|y = [1,size(C,1)]|.  Finally, you can use the @|image| function
%with only formal arguments
%@[
%  handle = imagesc(properties...)
%@]
%
%@@Example
%In this example, we create an image that is @|512 x 512| pixels
%square, and set the background to a noise pattern.  We set the central
%@|128 x 256| pixels to be white.
%@<
%x = rand(512);
%x((-64:63)+256,(-128:127)+256) = 1.0;
%figure
%imagesc(x,[0 .5])
%colormap(gray)
%mprint image1
%@>
%
%!

% Copyright (c) 2002-2007 Samit Basu, Eugene Ingerman
% Licensed under the GPL

function ohandle = imagesc(varargin)
  	ax = newplot;
	if (length(varargin) == 0), return; end
	lim=[];
	if( (nargin==2) || (nargin==4) )
		if ( (length( varargin{end} ) == 2) && varargin{end}(1)<=varargin{end}(2) ) %last element - limits
			lim=varargin{end};
			varargin{end}=[];
			nargin=nargin-1;
		end
	end
	
	switch(nargin)
		case 1
			handle = image(varargin{1},'cdatamapping','scaled');
		case 3
			handle = image(varargin{:},'cdatamapping','scaled');
		otherwise
			error('Unrecognized form of image command');
	end
	
	if ~isempty(lim),
	  clim(lim);
	end

	if (nargout > 0)
		ohandle = handle;
	end
