#!/usr/bin/perl -w

open(INPUT,"<html/node1.html") || die "Can't open html/node1.html for processing...\n";
open(OUTPUT,">html/manual.dcf") || die "Can't open manual.dcf...\n";
print OUTPUT "<assistantconfig version=\"3.2.0\">\n";
print OUTPUT " <DCF ref=\"index.html\" >\n";
print OUTPUT " <section title=\"FreeMat Reference Manual\">\n";
# Skip the prefix
while (defined($data=<INPUT>)  && !($data =~ /Table of Contents/g)) {};
# Skip the header
while (defined($data=<INPUT>) && !($data =~ /^<UL>/g)) {};
#print OUTPUT "..";
#print OUTPUT $data;
# Scan through the file.. 
while (defined($data=<INPUT>) && !($data =~ /Table of Contents/g)) {
    if ($data =~ /^<BR>/g) {
	# Does this line contain a <BR>? if so, discard
    } elsif ($data =~ /^<UL>/g) {
	# Does this line contain a <UL>? if so, indicate that nesting is increasing
	if ($dataavail) {
	    print OUTPUT "<section ref=\"$file\" title=\"$name\">\n";
	    $dataavail = 0;
	}
    } else {
	if ($data =~ /^<LI>/g) {
	    if ($dataavail) {
		print OUTPUT "<section ref=\"$file\" title=\"$name\"/>\n";
		$dataavail = 0;
	    }
	    chop ($data);
	    $nextline=<INPUT>;
	    $entry = $data . $nextline;
# Make sure this isn't a sub-page reference.
#	    if (!($entry =~ /\#SECTION/g)) 
	    {
# Get the name of the entry
		($entry =~ />([^<]*)<\/A>/g);
		$name = $1;
# Get the value of the entry
#	    ($entry =~ /<A([^E])*EHREF=\"([^\"]*)\">/g);
		$pst = $entry;
		($pst =~ /HREF=\"([^\"]*)\"/g);
		$file = $1;
#	    print OUTPUT "name = $name ";
#	    print OUTPUT "file = $file ";
		($name =~ /^(\w*)/g);
#		$keyword = lc($1);
#	    print OUTPUT "name2 = $name ";
#	    print OUTPUT "keyword = $keyword\n";
		$dataavail = 1;
	    }
	} else {
	    if ($data =~ /<\/UL>/g) {
		if ($dataavail) {
		    print OUTPUT "<section ref=\"$file\" title=\"$name\"/>\n";
		    $dataavail = 0;
		}
		$data =~ s/<\/UL>/<\/section>/g;
		print OUTPUT "$data\n";
		$dataavail = 0;
	    } 
	}
    }
}
print OUTPUT "  </DCF>\n";
print OUTPUT "</assistantconfig>\n";
close(INPUT);
close(OUTPUT);

