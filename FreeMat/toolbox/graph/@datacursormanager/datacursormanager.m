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
    s = get(gcf,'figsize'); s = s(1:2)
    p1 = p(:) ./ s(:);

    % Get the list of children
    children = get(gcf,'children');
    % Check us agains each child
    hit=0;
    for i=1:numel(children)
        position = get(children(i),'position')
        if (hitTest(position,p1))
            
            if strcmp( get(gca,'PlotBoxAspectRatioMode'), 'manual') || strcmp(get(gca,'DataAspectRatioMode'),'manual')
                ar = get(children(i),'PlotBoxAspectRatio')
                axis_pos = s.*position(1:2);
                axis_size = s.*position(3:4);
                axis_ar = axis_size(2)/axis_size(1);
                plot_ar = ar(2)/ar(1);            

                if plot_ar >= axis_ar
                    plot_pos(2) = axis_pos(2);
                    plot_size(2) = axis_size(2);
                    plot_size(1) = plot_size(2) / plot_ar;
                    plot_pos(1) = axis_pos(1)+(axis_size(1)-plot_size(1))/2.;
                else
                    plot_pos(1) = axis_pos(1);
                    plot_size(1) = axis_size(1);
                    plot_size(2) = plot_size(1) * plot_ar;
                    plot_pos(2) = axis_pos(2)+(axis_size(2)-plot_size(2))/2.;
                end
            end
            
            
            xlims = get(children(i),'xlim');
            ylims = get(children(i),'ylim');
            xdir = get(children(i),'xdir');
            ydir = get(children(i),'ydir');
            if strcmp(xdir,'reverse')
                tpos(1) = xlims(2) + (p(1) - plot_pos(1))/plot_size(1)*(xlims(1)-xlims(2));
            else
                tpos(1) = xlims(1) + (p(1) - plot_pos(1))/plot_size(1)*(xlims(2)-xlims(1));
            end
            if ~strcmp(ydir,'reverse')
                tpos(2) = ylims(2) + (p(2) - plot_pos(2))/plot_size(2)*(ylims(1)-ylims(2));
            else
                tpos(2) = ylims(1) + (p(2) - plot_pos(2))/plot_size(2)*(ylims(2)-ylims(1));               
            end
            tpos = tpos
            
            hg = get( children(i),'children' );
            for i=1:length(hg)
                if strcmp(get(hg(i),'type'),'image')
                    c = get( hg,'cdata');
                    v = c(round(tpos(2)),round(tpos(1)));
                end
                if strcmp(get(hg(i),'type'),'line')
                    v = 0;
                end
                if strcmp(get(hg(i),'type'),'surface')
                    v = 0;
                end
            end
            %[xlims(1)+p(1)*(xlims(2)-xlims(1)) ylims(1)+p(2)*(ylims(2)-ylims(1))]
            ht=htext('string','','position',[tpos(1) tpos(2)],'boundingbox',[20 20 20 20] ...
            'edgecolor',[0 0 0],'backgroundcolor',[0 0 0]);
            dx = 0.03*(xlims(2)-xlims(1));
            dy = 0.03*(ylims(2)-ylims(1));
            
            ht=htext('string',sprintf('x: %.2g, y: %g\n val: %g',tpos(1),tpos(2),v),'position',[tpos(1)+dx tpos(2)-dy], ...
            'edgecolor',[0 0 0],'backgroundcolor',[1 1 1]);
            %p
            %t = tpos
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
