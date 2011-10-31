m4_define([adg_version],[0.6.3])
m4_define([adg_api_version],[1.0])
m4_define([adg_lt_version],[1:0:0])

m4_define([cpml_version],adg_version)
m4_define([cpml_api_version],adg_api_version)
m4_define([cpml_lt_version],[1:0:0])

m4_define([gtkdoc_prereq],[1.9])
m4_define([gobject_prereq],[2.10.1])
m4_define([cairo_prereq],[1.7.4])
m4_define([gtk_prereq],[2.12.0])
m4_define([pangocairo_prereq],[1.18.0])
m4_define([gi_prereq],[0.9.5])dnl Picked up from clutter


# Initialization

dnl autoconf 2.62 is required by GObject introspection:
dnl http://live.gnome.org/GObjectIntrospection/AutotoolsIntegration
AC_PREREQ([2.62])
AC_INIT([adg-1],adg_version,[http://dev.entidi.com/p/adg/],[adg],[http://adg.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
dnl automake 1.11 is required for multiline substitution (AM_SUBST_NOTMAKE)
AM_INIT_AUTOMAKE([1.11 gnits no-dist-gzip dist-bzip2 -Wall -Wno-portability])
AM_SILENT_RULES([yes])

AC_SUBST([ADG_API_PACKAGE],AC_PACKAGE_TARNAME-adg_api_version)


# Check for host information

AC_CANONICAL_HOST
case "${host}" in
  *-*-linux*)
    guessed_host=linux
    ;;
  *-*-freebsd*)
    guessed_host=freebsd
    ;;
  *-*-mingw*)
    guessed_host=mingw
    ;;
  *-*-solaris*)
    guessed_host=solaris
    ;;
  *-*-darwin*)
    guessed_host=darwin
    ;;
  *-*-beos*)
    guessed_host=beos
    ;;
  *-*-cygwin*)
    guessed_host=cygwin
    ;;
  *-*-minix)
    guessed_host=minix
    ;;
  *-*-aix*)
    guessed_host=aix
    ;;
  *)
    guessed_host=unknown
    ;;
esac
AM_CONDITIONAL([OS_UNIX],[test "${guessed_host}" != "mingw"])
AM_CONDITIONAL([OS_WINDOWS],[test "${guessed_host}" = "mingw" -o "${guessed_host}" = "cygwin"])


# Check for programs

AC_PROG_CC
AC_PROG_SED
PKG_PROG_PKG_CONFIG
AC_PATH_PROG([XSLTPROC],[xsltproc],[/usr/bin/xsltproc])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums],[/usr/bin/glib-mkenums])
AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal],[/usr/bin/glib-genmarshal])
AC_PATH_PROG([GTESTER],[gtester],[/usr/bin/gtester])
AC_PATH_PROG([GTESTER_REPORT],[gtester-report],[/usr/bin/gtester-report])
AC_PATH_PROG([GLADE3],[glade-3],[/usr/bin/glade-3])


# Check for libraries

AC_CHECK_LIB([m],[cos])


# Libtool initialization

AC_MSG_CHECKING([which libtool initialization strategy to adopt])
AC_MSG_RESULT([m4_ifset([LT_INIT],[LT-INIT],[AC-PROG-LIBTOOL])])
m4_ifset([LT_INIT],
         [LT_INIT([disable-static win32-dll])],
         [AC_DISABLE_STATIC
          AC_LIBTOOL_WIN32_DLL
          AC_PROG_LIBTOOL])


# I18n

AC_SUBST([GETTEXT_PACKAGE],[${ADG_API_PACKAGE}])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["${ADG_API_PACKAGE}"],
                   [Define to the domain used by gettext.])
ALL_LINGUAS="`grep -v '^#' "$srcdir/po/LINGUAS" | tr '\n' ' '`"
IT_PROG_INTLTOOL(0.40.1)
AM_GLIB_GNU_GETTEXT
GLIB_DEFINE_LOCALEDIR([LOCALEDIR])
AC_CONFIG_COMMANDS([default],
                   [case "${CONFIG_FILES}" in *po-properties/Makefile.in*)
        ${SED} -e "/POTFILES =/r po-properties/POTFILES" po-properties/Makefile.in > po-properties/Makefile
    esac],
                   [SED="${SED}"])


# Check for required packages

PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([GOBJECT],[gobject-2.0 >= ]gobject_prereq)
PKG_CHECK_MODULES([CAIRO],[cairo >= ]cairo_prereq)


# Check for optional packages

dnl PangoCairo
AC_ARG_ENABLE([pango],
              [AS_HELP_STRING([--enable-pango],
                              [use pango for rendering text @<:@default=check@:>@])],
              [],[enable_pango=check])
AS_IF([test "x${enable_pango}" != "xno"],
      [PKG_CHECK_MODULES([PANGO],[pangocairo >= ]pangocairo_prereq,
                         [enable_pango=yes
                          AC_DEFINE_UNQUOTED([PANGO_ENABLED],[1],
                                             [Defined if the pango support is enabled.])],
                         [AS_IF([test "x${enable_pango}" = "xyes"],
                                [AC_MSG_ERROR([${PANGO_PKG_ERRORS} and pango support requested])],
                                [enable_pango=no])])])
AM_CONDITIONAL([HAVE_PANGO],[test "x${enable_pango}" = "xyes"])

dnl GTK+2
AC_ARG_ENABLE([gtk2],
              [AS_HELP_STRING([--enable-gtk2],
                              [include GKT+2 specific widgets @<:@default=check@:>@])],
              [],[enable_gtk2=check])
AS_IF([test "x${enable_gtk2}" != "xno"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= ]gtk_prereq,
                         [enable_gtk2=yes
                          AC_DEFINE_UNQUOTED([GTK2_ENABLED],[1],
                                             [Defined if the GTK+2 support is enabled.])],
                         [AS_IF([test "x${enable_gtk2}" = "xyes"],
                                [AC_MSG_ERROR([${GTK2_PKG_ERRORS} and GTK+2 support requested])],
                                [enable_gtk2=no])])])
AM_CONDITIONAL([HAVE_GTK2],[test "x${enable_gtk2}" = "xyes"])

dnl Glade
AC_ARG_ENABLE([glade],
              [AS_HELP_STRING([--enable-glade],
                              [install glade catalog file @<:@default=check@:>@])],
              [],[enable_glade=check])
AS_IF([test "x${enable_glade}" != "xno"],
      [PKG_CHECK_MODULES([GLADE],[gladeui-1.0],
                         [enable_glade=yes],
                         [AS_IF([test "x${enable_glade}" = "xyes"],
                                [AC_MSG_ERROR([${GLADE_PKG_ERRORS} and glade support requested])],
                                [enable_glade=no])])])
AM_CONDITIONAL([HAVE_GLADE],[test "x${enable_glade}" = "xyes"])

dnl gtk-doc
GTK_DOC_CHECK(gtkdoc_prereq)

dnl GObject introspection
GOBJECT_INTROSPECTION_CHECK(gi_prereq)
AM_COND_IF([HAVE_INTROSPECTION],
           [enable_introspection=yes],
           [enable_introspection=no])

dnl GLib test framework
AC_ARG_ENABLE([test_framework],
              [AS_HELP_STRING([--enable-test-framework],
                              [enable GLib test framework @<:@default=check@:>@])],
              [],[enable_test_framework=check])
AS_IF([test "x${enable_test_framework}" != "xno"],
      [PKG_CHECK_EXISTS([glib-2.0 >= 2.16.0],
                        [enable_test_framework=yes],
                        [AS_IF([test "x${enable_test_framework}" = "xyes"],
                               [AC_MSG_ERROR([The test framework needs glib2-2.16.0 or later])],
                               [enable_test_framework=no])])])
AM_CONDITIONAL([HAVE_TEST_FRAMEWORK],[test "x${enable_test_framework}" = "xyes"])


# Check for compiler characteristics

AC_C_CONST


# Substitutions

DLLDIR="$(${PKG_CONFIG} --variable=prefix gobject-2.0)/bin"
CPML_REQUIRES='cairo >= cairo_prereq'
AM_COND_IF([HAVE_PANGO],
      [ADG_REQUIRES='pangocairo >= pangocairo_prereq'
       ADG_H_ADDITIONAL='
#include <pango/pango.h>
#include "adg/adg-text.h"
#include "adg/adg-pango-style.h"
'],
      [ADG_REQUIRES='gobject-2.0 >= gobject_prereq'
       ADG_H_ADDITIONAL=''])
AM_COND_IF([HAVE_GTK2],
           [ADG_REQUIRES='gtk+-2.0 >= gtk_prereq'
            ADG_H_ADDITIONAL="${ADG_H_ADDITIONAL}
#include <gtk/gtk.h>
#include \"adg/adg-gtk-utils.h\"
#include \"adg/adg-gtk-area.h\"
#include \"adg/adg-gtk-layout.h\"
"
            ADG_CANVAS_H_ADDITIONAL='
void            adg_canvas_set_paper            (AdgCanvas      *canvas,
                                                 const gchar    *paper_name,
                                                 GtkPageOrientation orientation);
void            adg_canvas_set_page_setup       (AdgCanvas      *canvas,
                                                 GtkPageSetup   *page_setup);
'],
           [ADG_CANVAS_H_ADDITIONAL=''])
AM_COND_IF([HAVE_GLADE],
           [gladeroot=`${PKG_CONFIG} --variable=datarootdir gladeui-1.0`
            AS_IF([test "x${gladeroot}" = "x"],
                  [GLADE_CATALOGDIR="\${datarootdir}/glade3/catalogs}"],
                  [gladecatalog=`${PKG_CONFIG} --variable=catalogdir gladeui-1.0`
                   GLADE_CATALOGDIR=`echo "${gladecatalog}" | ${SED} -e "s|^${gladeroot}|"'${datarootdir}|'`])
            report_glade_catalogdir="
                  Glade catalog dir: ${GLADE_CATALOGDIR}"],
           [GLADE_CATALOGDIR=''
            report_glade_catalogdir=''])

AC_SUBST([CPML_API_VERSION],cpml_api_version)
AC_SUBST([CPML_LT_VERSION],cpml_lt_version)
AC_SUBST([CPML_REQUIRES])
AC_SUBST([ADG_API_VERSION],adg_api_version)
AC_SUBST([ADG_LT_VERSION],adg_lt_version)
AC_SUBST([ADG_REQUIRES])
AC_SUBST([ADG_H_ADDITIONAL])
AC_SUBST([ADG_CANVAS_H_ADDITIONAL])
AC_SUBST([DLLDIR])
AC_SUBST([GLADE_CATALOGDIR])

AM_SUBST_NOTMAKE([CPML_REQUIRES])
AM_SUBST_NOTMAKE([ADG_REQUIRES])
AM_SUBST_NOTMAKE([ADG_H_ADDITIONAL])
AM_SUBST_NOTMAKE([ADG_CANVAS_H_ADDITIONAL])
AM_SUBST_NOTMAKE([DLLDIR])


# Generation

AC_CONFIG_FILES([build/adg.nsi
                 Makefile
                 src/Makefile
                 src/cpml/cpml-1.pc
                 src/cpml/Makefile
                 src/cpml/tests/Makefile
                 src/adg/adg-1.pc
                 src/adg.h
                 src/adg/Makefile
                 src/adg/adg-canvas.h
                 src/adg/tests/Makefile
                 demo/Makefile
                 demo/cpml-demo.ui
                 demo/adg-demo.ui
                 po/Makefile.in
                 po-properties/Makefile.in
                 docs/Makefile
                 docs/cpml/Makefile
                 docs/cpml/bookinfo.xml
                 docs/adg/Makefile
                 docs/adg/bookinfo.xml])
AC_CONFIG_FILES([demo/adg-glade],[chmod +x demo/adg-glade])
AC_OUTPUT


# Report

AC_MSG_NOTICE([generating report

AC_PACKAGE_NAME adg_version will be built with the following options:
----------------------------------------------------------
                  API compatibility: adg_api_version
                CPML library to use: internal (cpml-adg_version)
         Build pango based entities: ${enable_pango}
                      GTK+2 support: ${enable_gtk2}
             Install glade catalogs: ${enable_glade}${report_glade_catalogdir}
                Build API reference: ${enable_gtk_doc}
             GObject instrospection: ${enable_introspection}
             Test framework support: ${enable_test_framework}
])
