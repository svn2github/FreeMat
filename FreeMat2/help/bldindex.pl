#!/usr/bin/perl -w

undef $/;

if (!open(INPUT,"<main.tex")) {
    die "Can't open main.tex...\n";
}
$data=<INPUT>;
close INPUT;
# Collect the chapter names
@chapts=($data =~ m/\\chapter\{([^\}]*)\}/gsm);
# Write the chapter names to the main index
if (!open(OUTPUT,">mtex/topics.mdc")) {
    die "Can't open mtex/topics.mdc for writing...\n";
}
foreach $chapt (@chapts) {
    ($chapt =~ /^(\w*)/g);
    $modname = uc($1);
    print OUTPUT "$modname -- $chapt\n";
}
close OUTPUT;
# For each chapter, write out a TOC file
foreach $chapt (@chapts) {
    ($chapt =~ /^(\w*)/g);
    $modname = uc($1);
    print "$modname -- $chapt\n";
    print "***************\n";
    @sects=($data =~ m/\\chapter\{$chapt\}(.*)(\\chapt|\\end)/gsm);
    foreach $sect (@sects) {
	print $sect;
    }
}
