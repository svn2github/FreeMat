#!/usr/bin/perl -w

if (!open(INPUT,"<html/index.html")) {
    die "Can't open html/index.html for processing...\n";
}
# Skip the prefix
while (defined($data=<INPUT>)  && !($data =~ /Table of Child-Links/g)) {};
# Skip the header
while (defined($data=<INPUT>) && !($data =~ /<UL>/g)) {};
print $data;
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
	    print "<LI> <OBJECT type=\"text/sitemap\">\n";
	    print "       <param name=\"Name\" value=\"$name\"\>\n";
	    print "       <param name=\"Local\" value=\"html/$file\"\>\n";
	    print "     </OBJECT>\n";
	}
    } else {
	print $data;
    }
}
print "</BODY></HTML>\n";
close(INPUT);
#$linkstart = "<!--Table of Child-Links-->";
#$linkend = "<!--End of Table of Child-Links-->";
#$data =~ s/$linkstart([^$linkend]*)$linkend/($1)/gm;
#print $data;
