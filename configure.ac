dnl Process this file with autoconf to produce a configure script.

AC_INIT([ADG],[0.4.3],[ntd at entidi.it],[adg],[http://adg.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE


# Check for programs

AC_PROG_CC
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG


# Check for functions

dnl Alloca is required by cpml, so it should be removed
dnl in case the adg and cpml libraries will be splitted
AC_FUNC_ALLOCA


# I18n

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.15])


# Check required packages

GTK_DOC_CHECK([1.9])
PKG_CHECK_MODULES([GOBJECT2],[gobject-2.0 >= 2.4.0])
PKG_CHECK_MODULES([CAIRO],[cairo >= 1.7.4])


# Check for optional GTK+2 support

AC_ARG_ENABLE([gtk2],
              [AS_HELP_STRING([--enable-gtk2],
                              [include GKT+2 specific widgets @<:@default=check@:>@])],
              [],[enable_gtk2=check])
AS_IF([test "x$enable_gtk2" != "xno"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= 2.12.0],
                         [enable_gtk2=yes],
                         [AS_IF([test "x$enable_gtk2" = "xyes"],
                                [AC_MSG_ERROR([$GTK2_PKG_ERRORS but GTK+2 support requested])],
                                [enable_gtk2=no])])])
AM_CONDITIONAL([HAVE_GTK2],[test "x$enable_gtk2" = "xyes"])


# Final step

SHAVE_INIT([],[enable])
AC_CONFIG_FILES([shave
                 shave-libtool
                 Makefile
                 cpml/Makefile
                 adg/Makefile
		 demo/Makefile
                 docs/Makefile
                 docs/adg/Makefile
                 docs/adg/bookinfo.xml
                 docs/cpml/Makefile
                 docs/cpml/bookinfo.xml
                 po/Makefile.in])
AC_OUTPUT
