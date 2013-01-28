m4_define([adg_version],[0.6.6])
m4_define([adg_api_version],[1.0])
m4_define([adg_lt_version],[1:0:0])

m4_define([cpml_version],adg_version)
m4_define([cpml_api_version],adg_api_version)
m4_define([cpml_lt_version],[1:0:0])

m4_define([gtkdoc_prereq],[1.12])dnl Support introspection annotations
m4_define([gobject_prereq],[2.10.1])
m4_define([cairo_prereq],[1.7.4])
m4_define([gtk2_prereq],[2.12.0])
m4_define([gtk3_prereq],[3.0.0])
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
AM_INIT_AUTOMAKE([1.10 gnits no-dist-gzip dist-bzip2 -Wall -Wno-portability])

dnl m4 backward compatibility stuff
m4_ifdef([AM_SILENT_RULES],
         [AM_SILENT_RULES([yes])],
         [AC_SUBST([AM_V_GEN],[])])
m4_ifdef([AM_SUBST_NOTMAKE],[],
         [AC_DEFUN([AM_SUBST_NOTMAKE],[_AM_SUBST_NOTMAKE($@)])])
m4_ifdef([AM_COND_IF],[],
         [AC_DEFUN([AM_COND_IF],
                   [AS_IF([test -z "$$1_TRUE"],[$2],[$3])])])


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

dnl -win$ARCH will be appended to the Windows installer file name
AS_IF([test "x${host_cpu}" = "xx86_64"],[ARCH=64],[ARCH=32])
AC_SUBST([ARCH])
AM_SUBST_NOTMAKE([ARCH])


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
AC_PATH_PROG([MAKENSIS],[makensis],[/usr/bin/makensis])


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

AC_SUBST([ADG_API_PACKAGE],AC_PACKAGE_TARNAME-adg_api_version)
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

dnl GTK+ support
AC_ARG_WITH(gtk,
            [AS_HELP_STRING([--with-gtk@<:@=gtk2/gtk3@:>@],
                            [include GTK+ specific widgets @<:@default=check@:>@])],
            [],[with_gtk=check])
AS_CASE(["${with_gtk}"],
        [gtk2],      [gtk2_pkg=yes; gtk3_pkg=no],
        [gtk3],      [gtk2_pkg=no;  gtk3_pkg=yes],
        [check|yes], [gtk2_pkg=yes; gtk3_pkg=yes],
        [no],        [gtk2_pkg=no;  gtk3_pkg=no],
        [AC_MSG_ERROR([Invalid option passed to --with-gtk])])

# GTK+ autodetection, giving precedence to GTK+3
AS_IF([test "x${gtk3_pkg}" = "xyes"],
      [PKG_CHECK_MODULES([GTK3],[gtk+-3.0 >= ]gtk3_prereq,[gtk_pkg=gtk3],[:])])
AS_IF([test "x${gtk2_pkg}" = "xyes" -a "x${gtk_pkg}" = "x"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= ]gtk2_prereq,[gtk_pkg=gtk2],[:])])
AS_IF([test "x${gtk_pkg}" != "x"],[with_gtk=${gtk_pkg}])


# Handle "check" graceful degradation
AS_IF([test "x${with_gtk}" = "xcheck"],[with_gtk=no])

# Handle failures
AS_IF([test "x${with_gtk}" != "xno" -a "x${gtk_pkg}" = "x"],
      [AC_MSG_ERROR([GTK+ support explicitely required but no valid GTK+ found])])

AS_IF([test "x${with_gtk}" = "xgtk3"],
      [AC_DEFINE_UNQUOTED([GTK3_ENABLED],[1],
                          [Defined if the GTK+3 support is enabled.])])
AS_IF([test "x${with_gtk}" = "xgtk2"],
      [AC_DEFINE_UNQUOTED([GTK2_ENABLED],[1],
                          [Defined if the GTK+2 support is enabled.])])

AM_CONDITIONAL([HAVE_GTK], [test "x${with_gtk}" != "xno"])
AM_CONDITIONAL([HAVE_GTK2],[test "x${with_gtk}" = "xgtk2"])
AM_CONDITIONAL([HAVE_GTK3],[test "x${with_gtk}" = "xgtk3"])

dnl Glade catalog dir
AC_ARG_WITH(glade_catalogdir,
            [AS_HELP_STRING([--with-glade-catalogdir@<:@=DIR@:>@],
                            [where to install the glade catalogs @<:@default=check@:>@])],
            [],[with_glade_catalogdir=check])

# Both "check" and "yes" mean gladeui autodetection,
# but "yes" will fail on gladeui not found
AS_IF([test "x${with_glade_catalogdir}" = "xcheck" -o "x${with_glade_catalogdir}" = "xyes"],
      [glade_pkg=check])

# gladeui autodetection, giving precedence to gladeui-2.0 if possible
AS_IF([test "x${glade_pkg}" = "xcheck" -a "x${with_gtk}" != "xgtk2"],
      [PKG_CHECK_MODULES([GLADEUI2],[gladeui-2.0],[glade_pkg=gladeui-2.0],[:])])
AS_IF([test "x${glade_pkg}" = "xcheck"],
      [PKG_CHECK_MODULES([GLADEUI1],[gladeui-1.0],[glade_pkg=gladeui-1.0],[:])])
AS_IF([test "x${glade_pkg}" != "x" -a "x${glade_pkg}" != "xcheck"],
      [NTD_UNEXPAND([${glade_pkg}],
                    [catalogdir],
                    [with_glade_catalogdir="${NTD_UNEXPANDED}"],
                    [AC_MSG_ERROR([${glade_pkg} found but pkg-config file invalid])])])

# Handle "yes" failure
AS_IF([test "x${glade_pkg}" = "xcheck" -a "x${with_glade_catalogdir}" = "xyes"],
      [AC_MSG_ERROR([Glade support explicitely required but no valid gladeui found])])

# Handle "check" graceful degradation
AS_IF([test "x${glade_pkg}" = "xcheck" -a "x${with_glade_catalogdir}" = "xcheck"],
      [with_glade_catalogdir=no])

AM_CONDITIONAL([HAVE_GLADE],[test "x${with_glade_catalogdir}" != "xno"])
AM_COND_IF([HAVE_GLADE],
           [enable_glade=yes
            report_glade="
                  Glade catalog dir: ${with_glade_catalogdir}"],
           [enable_glade=no
            report_glade=""])
AC_SUBST([GLADE_CATALOGDIR],${with_glade_catalogdir})

dnl gtk-doc
GTK_DOC_CHECK(gtkdoc_prereq)

dnl GObject introspection
GOBJECT_INTROSPECTION_CHECK(gi_prereq)
AC_ARG_WITH(girdir,
            [AS_HELP_STRING([--with-girdir=DIR],
                            [where the gir files are installed @<:@default=check@:>@])],
            [],[with_girdir=check])
AS_IF([test "x${with_girdir}" = "xcheck"],
      [NTD_UNEXPAND([gobject-introspection-1.0],
                    [girdir],
                    [with_girdir="${NTD_UNEXPANDED}"])])
AC_SUBST([GIRDIR],${with_girdir})
AC_ARG_WITH(typelibdir,
            [AS_HELP_STRING([--with-typelibdir=DIR],
                            [where the typelib files are installed @<:@default=check@:>@])],
            [],[with_typelibdir=check])
AS_IF([test "x${with_typelibdir}" = "xcheck"],
      [NTD_UNEXPAND([gobject-introspection-1.0],
                    [typelibdir],
                    [with_typelibdir="${NTD_UNEXPANDED}"])])
AC_SUBST([TYPELIBDIR],${with_typelibdir})
AM_COND_IF([HAVE_INTROSPECTION],
           [enable_introspection='yes'
            report_introspection="
         Where to install gir files: ${with_girdir}
 Where to install the typelib files: ${with_typelibdir}"],
           [enable_introspection='no'
            report_introspection=""])

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


# Additional substitutions

USRDIR="$($PKG_CONFIG --variable=prefix gobject-2.0)"
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
AM_COND_IF([HAVE_GTK3],[ADG_REQUIRES='gtk+-3.0 >= gtk3_prereq'])
AM_COND_IF([HAVE_GTK2],[ADG_REQUIRES='gtk+-2.0 >= gtk2_prereq'])
AM_COND_IF([HAVE_GTK],
           [ADG_H_ADDITIONAL="${ADG_H_ADDITIONAL}
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

AC_SUBST([CPML_API_VERSION],cpml_api_version)
AC_SUBST([CPML_LT_VERSION],cpml_lt_version)
AC_SUBST([CPML_REQUIRES])
AC_SUBST([ADG_API_VERSION],adg_api_version)
AC_SUBST([ADG_LT_VERSION],adg_lt_version)
AC_SUBST([ADG_REQUIRES])
AC_SUBST([ADG_H_ADDITIONAL])
AC_SUBST([ADG_CANVAS_H_ADDITIONAL])
AC_SUBST([USRDIR])

AM_SUBST_NOTMAKE([CPML_REQUIRES])
AM_SUBST_NOTMAKE([ADG_REQUIRES])
AM_SUBST_NOTMAKE([ADG_H_ADDITIONAL])
AM_SUBST_NOTMAKE([ADG_CANVAS_H_ADDITIONAL])
AM_SUBST_NOTMAKE([USRDIR])


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
                       GTK+ support: ${with_gtk}
             Install glade catalogs: ${enable_glade}${report_glade}
                Build API reference: ${enable_gtk_doc}
             GObject instrospection: ${enable_introspection}${report_introspection}
             Test framework support: ${enable_test_framework}
])
