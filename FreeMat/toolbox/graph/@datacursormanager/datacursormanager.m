function c = datacursormanager( a )
  if (nargin == 0)
    c.c = [];
    c = class(c,'datacursormanager');
  elseif isa(a,'datacursormanager')
    c = a;
  else
    c.c = a;
    c = class(c,'datacursormanager');
  end

  p = hpoint;
  % Convert p to a fractional coordinate
  s = get(gcf,'figsize'); s = s(:);
  p = p(:)./s(:);
  %p(2) = 1-p(2);
  % Get the list ofchildren
  children = get(gcf,'children');
  % Check us agains each child
  hit=0;
  for i=1:numel(children)
      position = get(children(i),'position')
      if (hitTest(position,p))
          xlims = get(children(i),'xlim');
          ylims = get(children(i),'ylim');
		  xdir = get(children(i),'xdir');
		  ydir = get(children(i),'ydir');
		  if strcmp(xdir,'reverse')
			p(1)=position(3)-p(1);
		  end
		  if ~strcmp(ydir,'reverse')
			p(2)=position(4)-p(2);
		  end
          tpos(1) = xlims(1) + (p(1)-position(1))/position(3)*(xlims(2)-xlims(1));
          tpos(2) = ylims(1) + (p(2)-position(2))/position(4)*(ylims(2)-ylims(1));
          %tpos(1) = xlims(1) + (p(1)-position(1))*(xlims(2)-xlims(1));
          %tpos(2) = ylims(1) + (p(2)-position(2))*(ylims(2)-ylims(1));
		  
		  
		  hg = get( children(i),'children' );
		  for i=1:length(hg)
			if strcmp(get(hg(i),'type'),'image')
			end
			if strcmp(get(hg(i),'type'),'line')
			end
			if strcmp(get(hg(i),'type'),'surface')
			end
		  end
	%[xlims(1)+p(1)*(xlims(2)-xlims(1)) ylims(1)+p(2)*(ylims(2)-ylims(1))]
          ht=htext('string','blah','position',[tpos(1) tpos(2)], ...
          'edgecolor',[0 0 0],'backgroundcolor',[1 1 1]);
          p
		  t = tpos
          hit=1;
      end
  end
  if hit==1
      return
  end
  t = [nan,nan];

function b = hitTest(rect,p)
    if ((p(1) >= rect(1)) && (p(1) <= (rect(1)+rect(3))) && (p(2) >= rect(2)) && (p(2) <= (rect(2)+rect(4))))
        b = 1;
    else
        b = 0;
    end
