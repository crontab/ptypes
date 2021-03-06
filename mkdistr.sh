#!/bin/sh


version=`cat VERSION`
reldate=`cat RELEASE-DATE`
distr="ptypes-$version"

cxxhdr=`sed "s/%VERSION%/$version/;s/%RELEASE-DATE%/$reldate/" AuxFiles/CXX-HEADER`
shhdr=`sed "s/%VERSION%/$version/;s/%RELEASE-DATE%/$reldate/" AuxFiles/SH-HEADER`


rm -f $distr.tar.gz
rm -rf $distr

#
# public headers
#

mkdir -p $distr/include
for i in include/*.h ; do
	(echo "$cxxhdr" ; cat $i) > $distr/$i
done

#
# lib
#

mkdir -p $distr/lib

#
# bin
#

mkdir -p $distr/bin

#
# so
#

mkdir -p $distr/so

#
# src
#

mkdir -p $distr/src

for i in src/Makefile* ; do
	(echo "$shhdr" ; cat $i) > $distr/$i
done

for i in src/*.cxx ; do
	(echo "$cxxhdr" ; cat $i) > $distr/$i
done

cp src/*.s $distr/src/

#
# wshare
#

mkdir -p $distr/wshare

for i in wshare/Makefile* wshare/*.awk ; do
	(echo "$shhdr" ; cat $i) > $distr/$i
done

for i in wshare/*.cxx wshare/*.h ; do
	(echo "$cxxhdr" ; cat $i) > $distr/$i
done

#
# win32
#

mkdir -p $distr/win32

cp win32/*.sln win32/*.vcproj win32/*.rc win32/*.h $distr/win32

#
# library root
#

(echo "$shhdr" ; cat Makefile ) > $distr/Makefile
cp index.html $distr
cp LICENSE $distr

#
# html sources
#

echo "Building source file listings in HTML..."
./mkhtsrc
if [ "$?" != "0" ] ; then
	rm -rf $distr
	exit
fi

#
# documentation
#

echo "Building documentation index..."
./mkdocref

mkdir -p $distr/doc/include
cp doc/*.html doc/*.css doc/*.png $distr/doc
cp doc/include/*.html $distr/doc/include

#
# tar it and gz it
#

tar cf - $distr | gzip -c > $distr.tar.gz


#
# remove the temp directory
#

rm -rf $distr

