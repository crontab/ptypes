#!/bin/sh

#
# Build html sources
# Called automatically from ./mkdistr
# Webcpp required
#

webcpp=`which webcpp`
if [ ! -x "$webcpp" ] ; then
	echo
	echo "**************************************************************"
	echo "*** ERROR: webcpp does not exist, can't build html sources ***"
	echo "**************************************************************"
	echo
	exit 2
fi

htsrc="doc"
mkdir -p "$htsrc/include"

htlist="include/*.h"
sedprg1='1,2d;s/font-weight: bold//;s/font-style: italic//'
sedprg2='s|<body>|<body bgcolor="#FFFFFF" leftmargin="40" marginwidth="40"> <p><a href="../htsrc.html">Index</a><hr noshade></p>|;s|</body>|<hr noshade></body>|'
sedprg3='s|ptpublic ||'
sedprg4='s|ptdecl ||'

webcpp_scheme="/usr/local/share/webcpp/ide-msvcpp.Scs2"
# webcpp_scheme="ide-msvcpp.Scs2"

for i in $htlist ; do
	src="$i"
	dest="$htsrc/$i.html"
	temp="$dest".tmp
	webcpp $src - -c="$webcpp_scheme" \
	| sed "$sedprg1;$sedprg2;$sedprg3;$sedprg4" > "$temp"
	diff "$dest" "$temp" > /dev/null
	if [ "$?" != "0" ] ; then
		cp "$temp" "$dest"
		echo "$src > $dest"
	fi
	rm "$temp"
done

