function h = newplot
fig = gcf;
fg_mode = get(fig,'nextplot');
if (strcomp(fg_mode,'replace'))
     clf;
end
ax = gca;
ax_mode = get(ax,'nextplot');
if (strcomp(ax_mode,'replace'))
     cla;
end
h = gca;
return;



