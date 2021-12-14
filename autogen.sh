#!/bin/sh
#
# Run this to generate the configuration scripts after a fresh
# repository clone/checkout.
#
# USAGE: ./autogen.sh [--verbose]
#
# where --verbose shows stdout and stderr of every command (by
# default they are redirect to /dev/null).
#
# This script does *not* call configure (as usually done in other
# projects) because this would prevent VPATH builds.

arg1=$1

step() {
    printf "$1... "

    if test "$arg1" = --verbose; then
	eval $2
    else
	eval $2 >/dev/null 2>&1
    fi
    result=$?

    if test "$result" = "0"; then
	printf "\033[32mok\033[0m\n"
    else
	printf "\033[31mfailed\033[0m\n  ** \"$2\" returned $result\n"
	exit $result
    fi
}


srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

step	"Checking for top-level adg directory" \
	"test -f '$srcdir/configure.ac' -a -f '$srcdir/src/adg.h.in'"

cd "$srcdir"

step	"Creating dummy ChangeLog, if needed" \
	"test -f './ChangeLog' || touch './ChangeLog'"

# autoreconf interaction with libtool has been broken for ages:
# explicitely calling libtoolize seems to avoid some problem
step	"Calling libtoolize" \
	"libtoolize --automake"

# GNU gettext seems to have timestamp problems with git:
# https://bugzilla.gnome.org/show_bug.cgi?id=661128
step	"Making adg.pot look older" \
	"touch -t 200001010000 po/adg.pot"

step	"Regenerating autotools files" \
	"autoreconf -isf -Wall"


printf "Now run \033[1m./configure\033[0m to customize your building\n"
