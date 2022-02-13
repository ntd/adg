# ntd.m4 - Collection of autoconf snippets.
#
# Copyright (C) 2012  Nicola Fontana <ntd@entidi.it>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.

dnl NTD_UNEXPAND(PACKAGE,VARIABLE,[ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
dnl Get the unexpanded value VARIABLE in PACKAGE by using pkg-config.
dnl The result is returned in the NTD_UNEXPANDED shell variable.
AC_DEFUN([NTD_UNEXPAND],[
NTD_UNEXPANDED="$(${PKG_CONFIG} \
 --define-variable=prefix='${prefix}' \
 --define-variable=exec_prefix='${exec_prefix}' \
 --define-variable=libdir='${libdir}' \
 --define-variable=bindir='${bindir}' \
 --define-variable=datarootdir='${datarootdir}' \
 --define-variable=datadir='${datadir}' \
 --define-variable=includedir='${includedir}' \
 --variable=[$2] [$1])"

if test "x$NTD_UNEXPANDED" != "x"; then
	m4_default([$3], [:])
m4_ifvaln([$4], [else
	$4])dnl
fi])
