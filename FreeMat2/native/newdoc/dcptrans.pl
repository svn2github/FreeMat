#!/usr/bin/perl -w

undef $/;

sub outputMFile {
    my ($line, $clickres) = @_;
    my $result;
    $line =~ s/\@Module\s*(.*)/$1/gi;
    $line =~ s/\@\@(.*)/\n${\uc($1)}\n/gi;
    $line =~ s/\@\[//gi;
    $line =~ s/\@\]//gi;
    $line =~ s/\@\|([^\|]*)\|/$1/gi;
    $line =~ s/\@figure\s*(.*)/<<Figure $1 omitted>>/g;
    foreach $resulttext (@$clickres) {
	$line =~ s/\@<(.*?)\@>/\n$resulttext\n/sm;
    }
    return $line;
}

sub outputLaTeX {
    my ($line, $clickres) = @_;
    my $result;
    $line =~ s/\@Module\s*(.*)/\\subsection{$1}/gi;
    $line =~ s/\@\@(.*)/\n\\emph{$1}\n/gi;
    $line =~ s/\@\[/\\begin{verbatim}/gi;
    $line =~ s/\@\]/\\end{verbatim}/gi; 
    $line =~ s/\@\|([^\|]*)\|/\\verb|$1|/gi;
    $line =~ s/\@figure\s*(.*)/\\doplot{width=8cm}{$1}/g;
    foreach $resulttext (@$clickres) {
	$line =~ s/\@<(.*?)\@>/\\begin{verbatim}\n$resulttext\\end{verbatim}/sm;
    }
    return $line;
}

foreach $file (@ARGV) {
    print "Processing file $file\n";
    if (!open(INPUT,"<$file")) {
	print STDERR "Can't open input file $file\n"; 
	next; 
    } 
    # Read input file as one long record 
    $data=<INPUT>; 
    close INPUT; 
    @modules = ($data =~  (/\/\/\*\*\s*(.*?)\*\*\/\//gsm));
    foreach $module (@modules) {
	($module =~ /\@Module\s*(\w*)/gi);
	$modulename = lc($1);
	print "Module $modulename\n";
	$line = $module;
	@clicks=($line =~ m/\@<(.*?)\@>/gsm);
	$count = 1;
	@clickres = ();
	foreach $click (@clicks) {
	    print "Collecting click $count\n";
	    if (!open(OUTPUT,">tmpFMinput")) {
		die "Can't open tmpFMinput file...\n";
	    }
	    if ($count != 1) {
		print OUTPUT "load env.dat\n";
	    }
	    print OUTPUT $click;
	    print OUTPUT "save env.dat\n";
	    close OUTPUT;
	    $resulttext = `./FreeMat -e tmpFMinput`;
	    $resulttext =~ s/(--> save env.dat\s*\n)//g;
	    $resulttext =~ s/(--> load env.dat\s*\n)//g;
	    $resulttext =~ s/(-->\s*\n)//g;
	    $resulttext =~ s/(--> mprint).*\n//g;
	    @clickres = (@clickres,$resulttext);
	    $count++;
	}
	$outfile = $modulename . ".tex";
	if (!open(OUTPUT,">$outfile")) {
	    die "Can't open output file $file\n";
	}
	print OUTPUT &outputLaTeX($line,\@clickres);
	close OUTPUT;
	$outfile = $modulename . ".m";
	if (!open(OUTPUT,">$outfile")) {
	    die "Can't open output file $file\n";
	}
	print OUTPUT &outputMFile($line,\@clickres);
	close OUTPUT;
    }
}

