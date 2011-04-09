#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

if test ! -f $srcdir/configure.ac -o ! -f $srcdir/src/adg.h.in; then
    echo "**Error**: '$srcdir' does not look like the top-level adg directory"
    exit 1
fi

cd $srcdir
glib-gettextize -f || exit $?
intltoolize -f || exit $?

# autoreconf interaction with libtool has been broken for ages:
# explicitely calling libtoolize avoid some problems
libtoolize --automake || exit $?

autoreconf -is -Wall || exit $?

./configure "$@" && echo "Now type 'make' to compile $PROJECT."
