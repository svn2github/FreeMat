#!/usr/bin/perl -w

undef $/;

foreach $file (@ARGV) {
    print "Processing file $file\n";
    if (!open(INPUT,"<$file")) {
	print STDERR "Can't open input file $file\n"; 
	next; 
    } 
    # Read input file as one long record 
    $data=<INPUT>; 
    close INPUT; 
    # Strip the comments
    $data =~ s/%.*\n//g;
    $data =~ s/\\subsection{([^}]*)}/\@Module $1/g;
    $data =~ s/\\subsubsection{([^}]*)}/\@\@$1/g;
    $data =~ s/\\verb\|([^\|]*)\|/\@\|$1\|/g;
    @clicks = ($data =~ /\\begin{verbatim}(.*?)\\end{verbatim}/gsm);
    foreach $click (@clicks) {
	if (($click =~ /(-->)/g))  {
	    $click =~ s/^(?!-->).*\n//mg;
	    $click =~ s/(-->)\s*(.*)/$2/mg;
	    $data =~ s/\\begin{verbatim}(.*?)\\end{verbatim}/\@<\n$click\@>/s;
	} else {
	    $data =~ s/\\begin{verbatim}(.*?)\\end{verbatim}/\@\[$click\@\]/s;
	}
    }
    $outfile = $file . ".dcp";
    if (!open(OUTPUT,">$outfile")) {
	die "Can't open output file $file\n";
    }
    print OUTPUT $data;
    close OUTPUT;
}

