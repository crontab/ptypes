#!/bin/sh

#
# Build the documentation index file doc/ref.html
# Called automatically from ./mkdistr
#

cd doc

# outfile="/dev/stdout"
reffile="ref.html"
outfile="$reffile.tmp"

awkprog='
BEGIN {
	print "<table border=0>";
}
{
	printf "<tr><td><a href=\"%s\">%s</a></td><td>", $1, $2
	system("grep \"<title>\" " $1 " | sed \"s|<title> *PTypes: *||;s| *</title>||\"");
	printf "</td></tr>\n";
}
END {
	print "</table>";
}
'

sed '/%REF%/,$d' ref.tmpl.html > "$outfile"

grep '^<p> *<span class="def">' *.html \
	| sed 's/: *<p> *<span class="def"> */:/;s| *</span>.*||;s/[a-zA-Z_*&;]* //g;s/(.*//;s/:/|/' \
	| sort -u | sort -t '|' -k 2,2 \
	| awk -F '|' "$awkprog" \
	>> "$outfile"


sed '1,/%REF%/d' ref.tmpl.html >> "$outfile"

diff "$outfile" "$reffile" > /dev/null
if [ "$?" != "0" ] ; then
	cp "$outfile" "$reffile"
	echo "Reference updated"
fi

rm "$outfile"
