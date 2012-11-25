#!/bin/sh

RPMBUILDDIR=`pwd`/RPMBUILD/
PACKAGE_NAME=oss_core
REVISION_NUMBER=`git describe --always  HEAD^ | sed -e 's/^v[0-9.]\+-//g' -e 's/-/./g'`
BUILD_NUMBER=`echo ${REVISION_NUMBER:(-8)}`
VERSION_NUMBER=`cat VERSION`
COMPRESS="gzip -v --fast "

rm -rf "$RPMBUILDDIR"

echo "%_topdir     $RPMBUILDDIR" > ~/.rpmmacros
mkdir -p "$RPMBUILDDIR/BUILD"
mkdir -p "$RPMBUILDDIR/SOURCES"
mkdir -p "$RPMBUILDDIR/RPMS"
mkdir -p "$RPMBUILDDIR/SRPMS"
mkdir -p "$RPMBUILDDIR/SPECS"

sudo yum install -y `grep '^BuildRequires:' $PACKAGE_NAME.spec.in | \
	    sed \
	        -e 's/BuildRequires://' \
	        -e 's/,/\n/g' | \
	    awk '{print $$1}' | \
	    sort -u`

echo ""
echo "Creating source archives for $PACKAGE_NAME-$VERSION_NUMBER-$BUILD_NUMBER.  This will take a while."
echo ""

git archive --format tar -o $PACKAGE_NAME-$VERSION_NUMBER.tar --prefix $PACKAGE_NAME-$VERSION_NUMBER/ HEAD 
$COMPRESS $PACKAGE_NAME-$VERSION_NUMBER.tar
mv -f $PACKAGE_NAME-$VERSION_NUMBER.tar.* $RPMBUILDDIR/SOURCES/
cp --remove-destination $PACKAGE_NAME.spec.in $RPMBUILDDIR/SPECS/$PACKAGE_NAME.spec

sed -i "s:@VERSION@:$VERSION_NUMBER:g" $RPMBUILDDIR/SPECS/$PACKAGE_NAME.spec
sed -i "s:@PACKAGE_REVISION@:$BUILD_NUMBER:g" $RPMBUILDDIR/SPECS/$PACKAGE_NAME.spec
sed -i "s:@PACKAGE@:$PACKAGE_NAME:g" $RPMBUILDDIR/SPECS/$PACKAGE_NAME.spec

rpmbuild -ba $RPMBUILDDIR/SPECS/$PACKAGE_NAME.spec
