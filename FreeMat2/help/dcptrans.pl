#!/usr/bin/perl -w

undef $/;

sub outputMFile {
    my ($line, $clickres) = @_;
    my $result;
    $line =~ s/\@Module\s*(.*)/$1/gi;
    $line =~ s/\@\@Section(.*)//gi;
    $line =~ s/\@\@Function(.*)//gi;
    $line =~ s/\@\@Example(.*)//gsm;
    $line =~ s/\@\@(.*)/\n${\uc($1)}\n/gi;
    $line =~ s/\@\[//gi;
    $line =~ s/\@\]//gi;
    $line =~ s/\@\{/\n\$\n ***/gi;
    $line =~ s/\@\}/\$\n/gi;
    $line =~ s/\@\|([^\|]*)\|/$1/gi;
    $line =~ s/\@figure\s*(.*)/<<Figure $1 omitted>>/g;
    $line =~ s/\\begin\{itemize\}//g;
    $line =~ s/\\end\{itemize\}//g;
    $line =~ s/\\item/\n\*/g;
    $line =~ s/\\\[/\n\$/g;
    $line =~ s/\\\]/\$\n/g;
    foreach $resulttext (@$clickres) {
	$line =~ s/\@<(.*?)\@>/\n\$\n$resulttext\$\n/sm;
    }
    return $line;
}

sub outputLaTeX {
    my ($line, $clickres) = @_;
    my $result;
    $line =~ s/\@Module\s*(.*)/\\subsection{$1}/gi;
    $line =~ s/\@\@Section(.*)//gi;
    $line =~ s/\@\@(.*)/\n\\subsubsection{$1}\n/gi;
    $line =~ s/\@\[/\n\\begin{verbatim}/gi;
    $line =~ s/\@\]/\\end{verbatim}\n/gi; 
    $line =~ s/\@\{/\n\\begin{verbatim}/gi;
    $line =~ s/\@\}/\\end{verbatim}\n/gi;
    $line =~ s/\@\|([^\|]*)\|/\\verb|$1|/gi;
    $line =~ s/\@figure\s*(.*)/\n\n\\doplot{width=8cm}{$1}\n/g;
    foreach $resulttext (@$clickres) {
	$line =~ s/\@<(.*?)\@>/\n\\begin{verbatim}\n$resulttext\\end{verbatim}\n/sm;
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
    @modules = ($data =~  (/\/\/\!(.*?)\/\/\!/gsm));
    foreach $module (@modules) {
	($module =~ /\@Module\s*(\w*)/gi);
        $modulename = lc($1);
	print "Module $modulename\n";
	($module =~ /\@\@Section\s*(\w*)/gi);
	$sectionname = uc($1);
	print "Section $sectionname\n";
	if (!open(OUTPUT,">>$sectionname.tex")) {
	    die "Can't open $sectionname for output...\n";
	}
	print OUTPUT "\\input{";
	print OUTPUT $modulename;
	print OUTPUT "}\n";
	close OUTPUT;
        $line = $module;
        $line =~ s/^\s*\/\///gsm;
        @functs=($line =~ m/\@\{(.*?)\@\}/gsm);
        foreach $funct (@functs) {
	    $funct =~ /^\s*([\w.]*)/g;
            $funcname = $1;
	    $funct =~ s/^\s*([\w.]*)//g;
            print "Writing function $funcname\n";
            if (!open(OUTPUT,">$funcname")) {
               die "Can't open $funcname for output...\n";
            }
            print OUTPUT $funct;
            close OUTPUT;
	}
	@clicks=($line =~ m/\@<(.*?)\@>/gsm);
	$count = 1;
	@clickres = ();
	print "Collecting clicks ";
	foreach $click (@clicks) {
	    print "...$count";
	    if (!open(OUTPUT,">tmpFMinput")) {
		die "Can't open tmpFMinput file...\n";
	    }
	    $dir = `pwd`;
	    chop($dir);
	    print OUTPUT "mkhsetpath('$dir/../../MFiles');\n";
	    if ($count != 1) {
		print OUTPUT "load env.dat\n";
	    }
	    print OUTPUT $click;
	    print OUTPUT "save env.dat\nquit\n";
	    close OUTPUT;
	    $resulttext = `../../build/src/x11/FreeMat -e <tmpFMinput`;
	    $resulttext =~ s/^ Free.*\n//g;
	    $resulttext =~ s/^ Copy.*\n//g;
	    $resulttext =~ s/(--> mkhsetpath.*\n)//g;
	    $resulttext =~ s/(--> save env.dat\s*\n)//g;
	    $resulttext =~ s/(--> load env.dat\s*\n)//g;
	    $resulttext =~ s/(--> quit\n)//g;
	    $resulttext =~ s/(-->\s*\n)//g;
	    $resulttext =~ s/(--> mprint).*\n//g;
	    @clickres = (@clickres,$resulttext);
	    $count++;
	}
	print "\n";
	$outfile = $modulename . ".tex";
	if (!open(OUTPUT,">$outfile")) {
	    die "Can't open output file $file\n";
	}
	print OUTPUT &outputLaTeX($line,\@clickres);
	close OUTPUT;
	$outfile = $modulename . ".mdc";
	if (!open(OUTPUT,">$outfile")) {
	    die "Can't open output file $file\n";
	}
	print OUTPUT &outputMFile($line,\@clickres);
	close OUTPUT;
    }
}

