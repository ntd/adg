m4_define([adg_version],[0.5.5])
m4_define([adg_api_version],[1.0])

m4_define([gtkdoc_prereq],[1.9])
m4_define([gobject_prereq],[2.4.0])
m4_define([cairo_prereq],[1.7.4])
m4_define([gtk_prereq],[2.12.0])


# Initialization

AC_PREREQ([2.59])
AC_INIT([Automatic Drawing Generation],
        [adg_version],[http://dev.entidi.com/p/adg/],
        [adg],[http://adg.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.6])


# Check for programs

AC_PROG_CC
AC_DISABLE_STATIC
LT_INIT
PKG_PROG_PKG_CONFIG
AC_PATH_PROG([XSLTPROC],[xsltproc],[/usr/bin/xsltproc])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums],[/usr/bin/glib-mkenums])
AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal],[/usr/bin/glib-genmarshal])


# Check for functions

dnl Alloca is required by cpml, so it should be removed
dnl in case the adg and cpml libraries will be splitted
AC_FUNC_ALLOCA


# I18n

AC_SUBST([GETTEXT_PACKAGE],[adg-adg_api_version])
AC_DEFINE([GETTEXT_PACKAGE],[["]AC_PACKAGE_TARNAME-adg_api_version["]],
                   [Define to the domain used by gettext.])
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT
GLIB_DEFINE_LOCALEDIR([LOCALEDIR])
AC_OUTPUT_COMMANDS([case "$CONFIG_FILES" in *po-properties/Makefile.in*)
        sed -e "/POTFILES =/r po-properties/POTFILES" po-properties/Makefile.in > po-properties/Makefile
      esac])


# Check for required packages

GTK_DOC_CHECK([gtkdoc_prereq])
PKG_CHECK_MODULES([GOBJECT2],[gobject-2.0 >= gobject_prereq])
PKG_CHECK_MODULES([CAIRO],[cairo >= cairo_prereq])


# Check for optional GTK+2 support

AC_ARG_ENABLE([gtk2],
              [AS_HELP_STRING([--enable-gtk2],
                              [include GKT+2 specific widgets @<:@default=check@:>@])],
              [],[enable_gtk2=check])
ADG_REQUIRES=''
AS_IF([test "x$enable_gtk2" != "xno"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= gtk_prereq],
                         [enable_gtk2=yes
                          ADG_OPTIONAL_REQUIRES=',gtk+-2.0 >= gtk_prereq'],
                         [AS_IF([test "x$enable_gtk2" = "xyes"],
                                [AC_MSG_ERROR([$GTK2_PKG_ERRORS but GTK+2 support requested])],
                                [enable_gtk2=no])])])
AM_CONDITIONAL([HAVE_GTK2],[test "x$enable_gtk2" = "xyes"])
AC_SUBST([ADG_OPTIONAL_REQUIRES])


# Final step

SHAVE_INIT([],[enable])
AC_CONFIG_FILES([shave
                 shave-libtool
                 cpml.pc
                 adg.pc
                 Makefile
                 cpml/Makefile
                 adg/Makefile
                 demo/Makefile
                 docs/Makefile
                 docs/adg/Makefile
                 docs/adg/bookinfo.xml
                 docs/cpml/Makefile
                 docs/cpml/bookinfo.xml
                 po/Makefile.in
                 po-properties/Makefile.in])
AC_OUTPUT
