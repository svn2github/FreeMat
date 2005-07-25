function drawtextlabels(this)
  setforegroundcolor([0,0,0]);
  for i=1:length(textLabels)
    [xc,yc] = mappoint(this,textLabels(i,1),textLabels(i,2));
    drawtextstringaligned(textLabels{i},[xc;yc],'left','bottom');
  end
  
