#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/configure.ac && test -d $srcdir/adg && test -f $srcdir/adg/adg.h) ||
{
        echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
        echo " top-level adg directory"
        exit 1
}

pushd $srcdir
autoreconf -isf
gtkdocize --flavour no-tmpl || exit 1
popd
