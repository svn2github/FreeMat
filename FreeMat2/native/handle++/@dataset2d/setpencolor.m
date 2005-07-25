function setpencolor(this,usestyle)
  if (usestyle)
    penstyle = utilitymaplinestyletotype(this.line);
  else
    penstyle = 'solid';
  end
  
  setlinestyle(penstyle);
  setforegroundcolor(utilitymapcolorspectocolor(this.color));
  
  if (usestyle && (this.line == ' '))
    setlinestyle('none');
  end
  
    
