#!/usr/bin/perl -w

open(INPUT,"<html/index.html") || die "Can't open html/index.html for processing...\n";
open(OUTPUT,">toc_body.hhc") || die "Can't open output toc_body.hhc...\n";
open(OUTPUT2,">index_body.hhk") || die "Can't open output index_body.hhc...\n";

# Skip the prefix
while (defined($data=<INPUT>)  && !($data =~ /Table of Child-Links/g)) {};
# Skip the header
while (defined($data=<INPUT>) && !($data =~ /<UL>/g)) {};
print OUTPUT $data;
# Scan through the file..  
while (defined($data=<INPUT>) && !($data =~ /Table of Child-Links/g)) {
# Does this line start with a "<LI>?
# If so, get the next line and aggregate.
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
	    print OUTPUT "<LI> <OBJECT type=\"text/sitemap\">\n";
	    print OUTPUT "       <param name=\"Name\" value=\"$name\"\>\n";
	    print OUTPUT "       <param name=\"Local\" value=\"html/$file\"\>\n";
	    print OUTPUT "     </OBJECT>\n";
	    ($name =~ /^(\w*)/g);
	    $keyword = lc($1);
	    print OUTPUT2 "<LI> <OBJECT type=\"text/sitemape\">\n";
	    print OUTPUT2 "       <param name=\"Name\" value=\"$keyword\">\n";
	    print OUTPUT2 "       <param name=\"Name\" value=\"$name\"\>\n";
	    print OUTPUT2 "       <param name=\"Local\" value=\"html/$file\"\>\n";
	    print OUTPUT2 "     </OBJECT>\n";
	}
    } else {
	print OUTPUT $data;
    }
}
print OUTPUT "</BODY></HTML>\n";
close(INPUT);
close(OUTPUT);
print OUTPUT2 "</UL>\n</BODY></HTML>\n";

#$linkstart = "<!--Table of Child-Links-->";
#$linkend = "<!--End of Table of Child-Links-->";
#$data =~ s/$linkstart([^$linkend]*)$linkend/($1)/gm;
#print $data;
