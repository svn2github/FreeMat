#!/usr/bin/perl -w
$filelist = `cd html && ls *.png *.html`;
$filelist =~ s/\n/\\\n/gm;
open(OUTPUT,">html/Makefile.am") || die "Can't open Makefile.am for writing...\n";
print OUTPUT "EXTRA_DIST = ";
print OUTPUT $filelist;
print OUTPUT "main.css\n";
print OUTPUT "\nAUTOMAKE_OPTIONS = foreign\n\n";
print OUTPUT "install-data-local:\n";
print OUTPUT "\tmkdir -p \$(prefix)/share/FreeMat/html\n";
print OUTPUT "\tfor fname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Installing manual html file \$\$fname\"; \\\n";
print OUTPUT "\t  \$(INSTALL) \$(srcdir)/\$\$fname \$(prefix)/share/FreeMat/html;\\\n";
print OUTPUT "\tdone;\n";
print OUTPUT "uninstall-local:\n";
print OUTPUT "\tfor fname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Uninstalling manual html file \$\$fname\"; \\\n";
print OUTPUT "\t  rm -f \$(prefix)/share/FreeMat/html/\$\$fname;\\\n";
print OUTPUT "\tdone;\n";

close OUTPUT;

$filelist = `cd mdc && ls *.mdc`;
chop $filelist;
$filelist =~ s/\n/\\\n/gm;
open(OUTPUT,">mdc/Makefile.am") || die "Can't open Makefile.am for writing...\n";
print OUTPUT "EXTRA_DIST = ";
print OUTPUT $filelist;
print OUTPUT "\n";
print OUTPUT "\nAUTOMAKE_OPTIONS = foreign\n\n";
print OUTPUT "install-data-local:\n";
print OUTPUT "\tmkdir -p \$(prefix)/share/FreeMat/MFiles\n";
print OUTPUT "\tfor fname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Installing text help file \$\$fname\"; \\\n";
print OUTPUT "\t  \$(INSTALL) \$(srcdir)/\$\$fname \$(prefix)/share/FreeMat/MFiles;\\\n";
print OUTPUT "\tdone;\n";
print OUTPUT "uninstall-local:\n";
print OUTPUT "\tfor fname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Uninstalling text help file \$\$fname\"; \\\n";
print OUTPUT "\t  rm -f \$(prefix)/share/FreeMat/MFiles/\$\$fname;\\\n";
print OUTPUT "\tdone;\n";
close OUTPUT;

$filelist = `cd ../MFiles && ls *.m`;
chop $filelist;
$filelist =~ s/\n/\\\n/gm;
open(OUTPUT,">../MFiles/Makefile.amc") || die "Can't open Makefile.am for writing in MFiles...\n";
print OUTPUT "EXTRA_DIST = ";
print OUTPUT $filelist;
print OUTPUT "\n";
print OUTPUT "\nAUTOMAKE_OPTIONS = foreign\n\n";
print OUTPUT "install-data-local:\n";
print OUTPUT "\tmkdir -p \$(prefix)/share/FreeMat/MFiles\n";
print OUTPUT "\tfor scriptname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Installing M file \$\$scriptname\"; \\\n";
print OUTPUT "\t  \$(INSTALL) \$(srcdir)/\$\$scriptname \$(prefix)/share/FreeMat/MFiles;\\\n";
print OUTPUT "\tdone;\n";
print OUTPUT "uninstall-local:\n";
print OUTPUT "\tfor scriptname in \$(EXTRA_DIST); do \\\n";
print OUTPUT "\t  echo \"Uninstalling M file \$\$scriptname\"; \\\n";
print OUTPUT "\t  rm -f \$(prefix)/share/FreeMat/MFiles/\$\$scriptname;\\\n";
print OUTPUT "\tdone;\n";
close OUTPUT;

$filelist = `ls html/*.html`;
open(OUTPUT,">freemat.hhp") || die "Can't open freemat.hhp for writing...\n";
print OUTPUT "[OPTIONS]\n";
print OUTPUT "Compatibility=1.1 or later\n";
print OUTPUT "Compiled file=freemat.chm\n";
print OUTPUT "Contents file=toc.hhc\n";
print OUTPUT "Default Window=Search\n";
print OUTPUT "Default topic=html\\index.html\n";
print OUTPUT "Display compile progress=No\n";
print OUTPUT "Full-text search=Yes\n";
print OUTPUT "Index file=freemat.hhk\n";
print OUTPUT "Language=0x409 English (United States)\n\n";
print OUTPUT "[WINDOWS]\n";
print OUTPUT "Search=,\"toc.hhc\",\"freemat.hhk\",\"html\\index.html\",,,,,,0x22520,,0x3006,,,,,,,,0\n\n";
print OUTPUT "[FILES]\n";
print OUTPUT $filelist;
print OUTPUT "\n[INFOTYPES]\n";
close OUTPUT


