function writeindex(&p)
  secnames = fieldnames(p.sectables);
  for n=1:numel(secnames)
    writesectiontable(secnames{n},p.sectables.(secnames{n}),p);
  end
  fp = fopen(sprintf('%s/help2/html/index.html',p.sourcepath),'w');
  fprintf(fp,['<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n']);
  fprintf(fp,'\n');
  fprintf(fp,'<HTML>\n');
  fprintf(fp,'<HEAD>\n');
  fprintf(fp,'<TITLE>%s Documentation</TITLE>\n',verstring);
  fprintf(fp,'</HEAD>\n');
  fprintf(fp,'<BODY>\n');
  fprintf(fp,'<H1>%s Documentation</H1>\n',verstring);
  fprintf(fp,'<P>\n');
  fprintf(fp,'<H2> Documentation Sections </H2>\n');
  fprintf(fp,'<UL>\n');
  for n=1:numel(secnames)
    fprintf(fp,'<LI> <A HREF=sec_%s.html> %s </A> </LI>\n',secnames{n}, ...
            p.section_descriptors.(secnames{n}));
  end
  fprintf(fp,'</UL>\n');
  fprintf(fp,'</BODY>\n');
  fprintf(fp,'</HTML>\n');
  fclose(fp);
%  
%  // Build the module list
%  QFile file2(sourcepath+"/help/html/modules.txt");
%  if (!file2.open(QFile::WriteOnly | QIODevice::Text))
%    Halt("Unable to open "+sourcepath+"/help/html/modules.txt for output");
%  // Loop over the sections
%  QStringList modulenames;
%  foreach (QString secname, sections) {
%    // Get the modules for this section
%    QList<QStringList> moduledlist(sectables.values(secname));
%    for (unsigned k=0;k<moduledlist.size();k++)
%      modulenames << moduledlist[k][0].toLower() + " (" + secname + ")";
%  }
%  modulenames.sort();
%  QTextStream f2(&file2);
%  for (unsigned k=0;k<modulenames.size();k++) {
%    f2 << modulenames[k] << endl;
%  }
%  // Build the section index
%  QFile file3(sourcepath+"/help/html/sectable.txt");
%  if (!file3.open(QFile::WriteOnly | QIODevice::Text))
%    Halt("Unable to open "+sourcepath+"/help/html/sectable.txt for output");
%  QTextStream f3(&file3);
%  foreach (QString secname, sectionOrdered) {
%    QString secdesc(section_descriptors.value(secname));
%    QList<QStringList> modules(sectables.values(secname));
%    QStringList moduleList;
%    for (unsigned m=0;m<modules.size();m++)
%      moduleList << modules[m][1];
%    moduleList.sort();
%    f3 << secdesc << "\n";
%    for (unsigned m=0;m<moduleList.size();m++) {
%      f3 << "+" << " (" << secname << ")" << moduleList[m] << "\n";
%    }
%  }
%
  
function writesectiontable(secname,modules,&p)
  fp = fopen(sprintf('%s/help2/html/sec_%s.html',p.sourcepath,secname), ...
             'w');
  fprintf(fp,['<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\' ...
              'n']);
  fprintf(fp,'\n');
  fprintf(fp,'<HTML>\n');
  fprintf(fp,'<HEAD>\n');
  fprintf(fp,'<TITLE>%s</TITLE>\n',p.section_descriptors.(secname));
  fprintf(fp,'</HEAD>\n');
  fprintf(fp,'<BODY>\n');
  fprintf(fp,'<H2>%s</H2>\n',p.section_descriptors.(secname));
  fprintf(fp,'<P>\n');
  fprintf(fp,'<A HREF=index.html> Main Index </A>\n');
  fprintf(fp,'<P>\n');
  fprintf(fp,'<UL>\n');
  for n=1:size(modules,1)
    fprintf(fp,'<LI> <A HREF=%s_%s.html> %s </A> %s </LI>\n',secname, ...
            modules{n,1},modules{n,1},modules{n,2});
  end
  fprintf(fp,'</UL>\n');
  fprintf(fp,'</BODY>\n');
  fprintf(fp,'</HTML>\n');
  fclose(fp);

  
  
