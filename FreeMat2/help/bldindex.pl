#!/usr/bin/perl -w

open(INPUT,"<html/index.html") || die "Can't open html/index.html for processing...\n";
open(OUTPUT,">html/manual.dcf") || die "Can't open manual.dcf...\n";
print OUTPUT "<assistantconfig version=\"3.2.0\">\n";
print OUTPUT " <DCF ref=\"index.html\" >\n";

# Skip the prefix
while (defined($data=<INPUT>)  && !($data =~ /Table of Child-Links/g)) {};
# Skip the header
while (defined($data=<INPUT>) && !($data =~ /^<UL>/g)) {};
#print OUTPUT "..";
#print OUTPUT $data;
# Scan through the file.. 
$depth = 0;
while (defined($data=<INPUT>) && !($data =~ /Table of Child-Links/g)) {
    print "Depth $depth\n";
    if ($data =~ /^<BR>/g) {
	# Does this line contain a <BR>? if so, discard
    } elsif ($data =~ /^<UL>/g) {
	# Does this line contain a <UL>? if so, indicate that nesting is increasing
	$opening = 1;
	$depth = $depth + 1;
    } else {
	if ($data =~ /^<LI>/g) {
	    chop ($data);
	    $nextline=<INPUT>;
	    $entry = $data . $nextline;
# Make sure this isn't a sub-page reference.
	    if (!($entry =~ /\#SECTION/g)) {
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
		print OUTPUT "<section ref=\"$file\" title=\"$name\"";
		if ($opening) {
		    print OUTPUT ">\n";
		} else {
		    print OUTPUT "/>\n";
		}
		$opening = 0;
	    }
	} else {
	    if ($data =~ /<\/UL>/g) {
		$data =~ s/<\/UL>/<\/section>/g;
		print OUTPUT "$data\n";
		$depth = $depth-1;
	    } 
	}
    }
}
print OUTPUT "  </DCF>\n";
print OUTPUT "</assistantconfig>\n";
close(INPUT);
close(OUTPUT);

