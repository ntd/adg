dnl Process this file with autoconf to produce a configure script.

AC_INIT([adg], [0.2.1],
        [ntd@users.sourceforge.net],
        [adg])
AC_CONFIG_SRCDIR(README)
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_DISABLE_STATIC


##################################################
# Check for programs.
##################################################

AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG


##################################################
# I18n.
##################################################

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.15])


##################################################
# Check for gtk-doc.
##################################################

GTK_DOC_CHECK(1.4)


##################################################
# Check for packages.
##################################################

PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= 2.8.0])
PKG_CHECK_MODULES([GCONTAINER],[gcontainer >= 1.99.0],,
                  AC_MSG_ERROR([gcontainer 1.99.0 or above not found: you can download the latest version from http://sourceforge.net/projects/gcontainer/]))


##################################################
# Final step.
##################################################

AC_CONFIG_FILES([Makefile
                 adg/Makefile
		 demo/Makefile
                 docs/Makefile
                 docs/version.xml
                 po/Makefile.in])
AC_OUTPUT
