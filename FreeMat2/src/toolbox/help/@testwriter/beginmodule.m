function beginmodule(&p,sourcepath,modname,moddesc,secname,section_descriptors)
    p.moddesc = moddesc;
    p.secname = lower(secname);
    p.modulename = lower(modname);
    p.section_descriptors = section_descriptors;
    p.sourcepath = sourcepath;
    p.verbatim = 0;
    p.ignore = 1;
    p.num = 1;
