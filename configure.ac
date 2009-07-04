dnl Process this file with autoconf to produce a configure script.

AC_INIT([adg], [0.4.3],
        [ntd at entidi.it],
        [adg])
AC_CONFIG_SRCDIR([adg/adg.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR(m4)
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


# Check for gtk-doc

GTK_DOC_CHECK(1.9)


# Check for packages

PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= 2.12.0])
PKG_CHECK_MODULES([CAIRO],[cairo >= 1.7.4])


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
                 docs/adg/version.xml
                 docs/cpml/Makefile
                 docs/cpml/version.xml
                 po/Makefile.in])
AC_OUTPUT
