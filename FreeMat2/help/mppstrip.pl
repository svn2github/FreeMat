#!/usr/bin/perl -w

undef $/;

foreach $file (@ARGV) {
    print "Processing file $file\n";
    if (!open(INPUT,"<$file")) {
	print STDERR "Can't open input file $file\n"; 
	next; 
    } 
    # Calculate the output name
    $file =~ s/mpp/m/g;
    print $file;
    print "\n";
    # Read input file as one long record 
    $data=<INPUT>; 
    close INPUT; 
    $data =~  s/\/\/\!(.*?)\/\/\!//gsm;
    # write out the initial .m file
    print "Writing file $file\n";
    if (!open(OUTPUT,">$file")) {
	die "Can't open $file for output...\n";
    }
    print OUTPUT $data;
    close OUTPUT;
}

