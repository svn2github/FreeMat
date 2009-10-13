%!
%@Module CAT Concatenation of Arrays
%@@Section CLASS
%@@Usage
%This function concatenates arrays in a given
%dimension.  The syntax for its use is
%@[
%   cat (DIM, A, B)
%   cat (DIM, A, B, C ...)
%@]
%to return the concatenation along the dimension @|DIM| of all arguments. 
%@|cat(1, A, B, C)| is the same as @|[A; B; C]| or @|vertcat(A, B, C)|.
%@|cat(2, A, B, C)| is the same as @|[A, B, C]| or @|horzcat(A, B, C)|.
%@@Tests
%@$exact#y1=cat(1,x1,x2)
%@$exact#y1=cat(2,x1,x2)
%!

function ret = cat(varargin)
    if nargin < 2
        disp('Invalid use of cat.  Correct usage is:')
        help cat
        retall
    end
    if nargin == 2
        ret = varargin{2};
        return
    else
        dim = varargin{1};
        if ~isscalar(dim)
            error('Dimension must be a scalar.')
        end
        if dim < 1
            error('Invalid dimension argument.')
        elseif dim == 1
            ret = vertcat(varargin{2:end});
        elseif dim == 2
            ret = horzcat(varargin{2:end});
        else % dim>=3
            for jj = 2:nargin
                if ~isempty(varargin{jj})
                    break
                end
            end
            ret = varargin{jj};
            if isempty(ret)
                % All input arrays are empty
                return
            end
            for ii = jj+1:nargin
                if isempty(varargin{ii})
                    continue
                end
                ret = hi_dim_cat2(varargin{1},ret,varargin{ii});
            end
        end
    end

function C = hi_dim_cat2(dim, A, B) 
    sizA = size(A);
    sizB = size(B);
    ndimA = numel(sizA);
    ndimB = numel(sizB);
    ndimC = max([dim, ndimA, ndimB]);
    sizA2 = ones(1,ndimC);
    sizB2 = ones(1,ndimC);
    sizA2(1:ndimA) = sizA;
    sizB2(1:ndimB) = sizB;
    
    % All size elements of A and B must be the same 
    % except can be possibly different at "dim" location
    if sizA2([1:dim-1,dim+1:ndimC]) ~= sizB2([1:dim-1,dim+1:ndimC])
        strdim = num2str(dim);
        strdims = num2str([1:dim-1,dim+1:ndimC]);
        error(['Concatenating in dimension ', strdim, ' requires size matching in all dimension(s) ', strdims ,'.'])
    else
        sizC = sizA2;
        sizC(dim) = sizA2(dim)+sizB2(dim);
        C = zeros(sizC);
        dA = cell(ndimC,1);
        for ii = 1:ndimC
            dA{ii} = 1:sizA2(ii);
        end
        dB = dA;
        dB{dim} = 1:sizB2(dim);
        dAC = dA;
        dBC = dB;
        dBC{dim} = sizA2(dim) + dBC{dim};
        C(dAC{:})  = A(dA{:});
        C(dBC{:})  = B(dB{:});
    end

