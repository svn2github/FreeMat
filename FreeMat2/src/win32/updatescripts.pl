#!/bin/perl -w
# Build up the list of MFiles
$acount = @ARGV;
if ($acount == 0) { die("need version number");}

$a = `ls ../../MFiles/*.m`;
@ifiles = split(/^/,$a);
$mfiles = "";
$dellist = "";
foreach $script (@ifiles) {
    chop($script);
    $script =~ s/\//\\/g;
    $mfiles = $mfiles . "  File \"" . $script . "\"\n";
    $script =~ s/\.\.\\\.\.\\/\$INSTDIR\\/g;
    $dellist = $dellist . "  Delete \"" . $script . "\"\n";
}
# Build up the list of MDC files
$a = `ls ../../help/mdc/*.mdc`;
@ifiles = split(/^/,$a);
$mdcfiles = "";
foreach $script (@ifiles) {
    chop($script);
    $script =~ s/\//\\/g;
    $mdcfiles = $mdcfiles . "  File \"" . $script . "\"\n";
    $script =~ s/\.\.\\\.\.\\/\$INSTDIR\\/g;
    $script =~ s/mdc\\//;
    $dellist = $dellist . "  Delete \"" . $script . "\"\n";
}


# Read in the template script
open(INPUT,"<freemat_nsi.in") || die("can't open freemat_nsi.in for reading");
open(OUTPUT,">freemat.nsi") || die("can't open freemat.nsi for writing");
while ($nsi=<INPUT>) {
    $nsi =~ s/<VERSION_NUMBER>/\"$ARGV[0]\"/g;
    if ($nsi =~ m/MFILELIST/) {
	print OUTPUT $mfiles;
    } elsif ($nsi =~ m/MDCLIST/) {
	print OUTPUT $mdcfiles;
    } elsif ($nsi =~ m/DELLIST/) {
	print OUTPUT $dellist;
    } else {
	print OUTPUT $nsi;
    }
}
close INPUT;
close OUTPUT;

open(INPUT,"Script1_rc.in") || die("can't open Script1_rc.in for reading");
open(OUTPUT,">Script1.rc") || die("can't open Script1.rc for writing");
while ($nsi=<INPUT>) {
    $nsi =~ s/<VERSION_NUMBER>/$ARGV[0]/g;
    print OUTPUT $nsi;
}
close INPUT;
close OUTPUT;
