m4_define([adg_version],[0.9.1])
m4_define([cpml_version],adg_version)

dnl How to handle {adg,cmpl}_lt_version (current:revision:age):
dnl - ABI unmodified (e.g. refactoring):      current:revision+1:age
dnl - Backward compatible ABI (e.g. new API): current+1:0:age+1
dnl - Backward incompatible ABI:              current+1:0:0
m4_define([adg_lt_version],[7:0:0])
m4_define([cpml_lt_version],[4:0:0])

m4_define([gtkdoc_prereq],    [1.12]  )dnl Support introspection annotations
m4_define([gobject_prereq],   [2.14.0])dnl Required by GRegex
m4_define([cairo_prereq],     [1.7.4] )dnl Required by cairo_scaled_font_text_to_glyphs()
m4_define([gtk2_prereq],      [2.18.0])dnl Required by gtk_widget_get_allocation()
m4_define([gtk3_prereq],      [3.0.0] )dnl First stable release
m4_define([pangocairo_prereq],[1.10.0])dnl Cairo support in Pango
m4_define([gi_prereq],        [1.0.0] )dnl First stable release


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

AC_DEFUN([ADG_DEPENDENCY],
         [EXTRADIR="$EXTRADIR --extra-dir=$($PKG_CONFIG --variable=prefix $1)/share/gtk-doc/html/$2"])

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
  *-*-minix*)
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
AC_ARG_VAR([TAR])
AC_PATH_PROG([TAR],[tar],[/usr/bin/tar])
AC_ARG_VAR([BZIP2])
AC_PATH_PROG([BZIP2],[bzip2],[/usr/bin/bzip2])


# Check for coverage build

dnl Coverage with gcov is available only for GNU gcc
AS_IF([test "x$GNU" != "xyes"],
      [AC_ARG_ENABLE([gcov],
                     [AS_HELP_STRING([--enable-gcov],
                                     [enable coverage build @<:@default=no@:>@])],
                     [],[enable_gcov=no])])
AS_IF([test "x$enable_gcov" = "xyes"],
      [AC_ARG_VAR([GCOV])
       AC_CHECK_TOOL([GCOV],[gcov],[/usr/bin/gcov])
       CFLAGS="$CFLAGS --coverage"
       LDFLAGS="$LDFLAGS --coverage"])
AM_CONDITIONAL([ENABLE_GCOV],[test "x$enable_gcov" = "xyes"])


# Check for libraries

AC_CHECK_LIB([m],[cos])

dnl On Windows, GLib 2.16 is required by _adg_dgettext() for the
dnl g_win32_get_package_installation_directory_of_module() API.
dnl On other platforms, it just needs to be in sync with GObject.
AM_COND_IF([OS_WINDOWS],
           [AC_PATH_PROG([MAKENSIS],[makensis],[/usr/bin/makensis])
            AC_PATH_PROG([WINE],[wine])
            AC_ARG_VAR([WINDRES],[Command for Windows resources manipulation])
            AC_CHECK_TOOL([WINDRES],[windres])
            glib_prereq=2.16.0],
           [glib_prereq=]gobject_prereq)


# Libtool initialization

AC_MSG_CHECKING([which libtool initialization strategy to adopt])
AC_MSG_RESULT([m4_ifset([LT_INIT],[LT-INIT],[AC-PROG-LIBTOOL])])
m4_ifset([LT_INIT],
         [LT_INIT([disable-static win32-dll])],
         [AC_DISABLE_STATIC
          AC_LIBTOOL_WIN32_DLL
          AC_PROG_LIBTOOL])


# I18n

dnl The following macro is required in order to be able to use
dnl autopoint (formerly called gettextize) from autoreconf.
dnl The argument is a trade-off between modern and supported.
AM_GNU_GETTEXT_VERSION([0.17])

AM_GNU_GETTEXT([external])
AC_SUBST([ADG_API_PACKAGE],AC_PACKAGE_TARNAME)
AC_SUBST([GETTEXT_PACKAGE],[${ADG_API_PACKAGE}])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["${ADG_API_PACKAGE}"],
                   [Define to the domain used by gettext.])


# Check for required packages

PKG_CHECK_MODULES([GLIB],[glib-2.0 >= ]${glib_prereq},
                  [glib_postfix=" (glib-$($PKG_CONFIG --modversion glib-2.0))"])
ADG_DEPENDENCY([glib-2.0],[glib])
PKG_CHECK_MODULES([GOBJECT],[gobject-2.0 >= ]gobject_prereq)
ADG_DEPENDENCY([gobject-2.0],[gobject])
PKG_CHECK_MODULES([CAIRO],[cairo >= ]cairo_prereq)
ADG_DEPENDENCY([cairo],[cairo])


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
                                             [Defined if the pango support is enabled.])
                          ADG_DEPENDENCY([pangocairo],[pango])
                          pango_postfix=" (pangocairo-$($PKG_CONFIG --modversion pangocairo))"],
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
      [PKG_CHECK_MODULES([GTK3],[gtk+-3.0 >= ]gtk3_prereq,
                         [gtk_pkg=gtk3
                          ADG_DEPENDENCY([gtk+-3.0],[gtk3])],
                         [:])])
AS_IF([test "x${gtk2_pkg}" = "xyes" -a "x${gtk_pkg}" = "x"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= ]gtk2_prereq,
                         [gtk_pkg=gtk2
                          ADG_DEPENDENCY([gtk+-2.0],[gtk2])],
                         [:])])
AS_IF([test "x${gtk_pkg}" != "x"],[with_gtk=${gtk_pkg}])

# Handle "check" graceful degradation
AS_IF([test "x${with_gtk}" = "xcheck"],[with_gtk=no])

# Handle failures
AS_IF([test "x${with_gtk}" != "xno" -a "x${gtk_pkg}" = "x"],
      [AC_MSG_ERROR([GTK+ support explicitely required but no valid GTK+ found])])

AS_IF([test "x${with_gtk}" = "xgtk3"],
      [gtk_postfix=" (gtk+-$($PKG_CONFIG --modversion gtk+-3.0))"
       AC_DEFINE_UNQUOTED([GTK3_ENABLED],[1],
                          [Defined if the GTK+3 support is enabled.])])
AS_IF([test "x${with_gtk}" = "xgtk2"],
      [gtk_postfix=" (gtk+-$($PKG_CONFIG --modversion gtk+-2.0))"
       AC_DEFINE_UNQUOTED([GTK2_ENABLED],[1],
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
AS_IF([test "x$enable_gtk_doc" = "xyes"],
      [gtkdoc_postfix=" (gtk-doc-$($PKG_CONFIG --modversion gtk-doc))"])


dnl GObject introspection
GOBJECT_INTROSPECTION_CHECK(gi_prereq)

dnl If introspection is used, GObject support in cairo must be enabled.
dnl This in turn means cairo-gobject library must be present. That
dnl library has been introduced in cairo 1.10.0.
AS_IF([test "x$enable_introspection" = "xyes"],
      [PKG_CHECK_MODULES([CAIRO_GOBJECT],[cairo-gobject],
                         [AC_DEFINE_UNQUOTED([CAIRO_GOBJECT_ENABLED],[1],
                                             [Defined if GObject support in cairo is present.])],
                         [AC_MSG_ERROR([GObject introspection requires the cairo-gobject library])])])

dnl Introspection girdir
AC_ARG_WITH(girdir,
            [AS_HELP_STRING([--with-girdir@<:@=DIR@:>@],
                            [where to install the .gir files @<:@default=check@:>@])],
            [],[with_girdir=check])
AS_IF([test "x$with_girdir" = "xcheck"],
      [with_girdir="$INTROSPECTION_GIRDIR"])
AC_SUBST([ADG_GIRDIR],${with_girdir})

dnl Introspection typelibdir
AC_ARG_WITH(typelibdir,
            [AS_HELP_STRING([--with-typelibdir@<:@=DIR@:>@],
                            [where to install the .typelib files @<:@default=check@:>@])],
            [],[with_typelibdir=check])
AS_IF([test "x$with_typelibdir" = "xcheck"],
      [with_typelibdir="$INTROSPECTION_TYPELIBDIR"])
AC_SUBST([ADG_TYPELIBDIR],${with_typelibdir})

dnl Introspection report
AS_IF([test "x$enable_introspection" = "xyes"],
      [report_introspection="
         Where to install gir files: $with_girdir
 Where to install the typelib files: $with_typelibdir"],
      [report_introspection=""])


dnl GLib test framework
AC_ARG_ENABLE([test_framework],
              [AS_HELP_STRING([--enable-test-framework],
                              [enable GLib test framework @<:@default=check@:>@])],
              [],[enable_test_framework=check])
AS_IF([test "x$enable_gcov" = "xyes"], [enable_test_framework=yes])
AS_IF([test "x${enable_test_framework}" != "xno"],
      [PKG_CHECK_EXISTS([glib-2.0 >= 2.16.0],
                        [enable_test_framework=yes],
                        [AS_IF([test "x${enable_test_framework}" = "xyes"],
                               [AC_MSG_ERROR([The test framework needs glib2-2.16.0 or later])],
                               [enable_test_framework=no])])])
AM_CONDITIONAL([HAVE_TEST_FRAMEWORK],[test "x${enable_test_framework}" = "xyes"])


# Check for compiler characteristics

AC_C_CONST


# Windows substitutions for NSis installer

dnl Get the HOSTPREFIX from gobject-2.0.pc: all the dependencies *must* be
dnl installed under the same prefix.
HOSTPREFIX="$($PKG_CONFIG --variable=prefix gobject-2.0)"
AC_SUBST([HOSTPREFIX])
AM_SUBST_NOTMAKE([HOSTPREFIX])

dnl -win$PACKAGE_ARCH will be appended to the installer file name
AS_IF([test "x${host_cpu}" = "xx86_64"],[PACKAGE_ARCH=64],[PACKAGE_ARCH=32])
AC_SUBST([PACKAGE_ARCH])


# Additional substitutions

dnl The GObject dependency can be made optional by skipping
dnl the building of the GObject wrappers.
CPML_REQUIRES='cairo >= cairo_prereq gobject-2.0 >= gobject_prereq'
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
#include <gtk/gtk.h>

void            adg_canvas_set_paper            (AdgCanvas      *canvas,
                                                 const gchar    *paper_name,
                                                 GtkPageOrientation orientation);
void            adg_canvas_set_page_setup       (AdgCanvas      *canvas,
                                                 GtkPageSetup   *page_setup);
GtkPageSetup *  adg_canvas_get_page_setup       (AdgCanvas      *canvas);
'],
           [ADG_CANVAS_H_ADDITIONAL=''])

AC_SUBST([CPML_LT_VERSION],cpml_lt_version)
AC_SUBST([CPML_REQUIRES])
AC_SUBST([ADG_LT_VERSION],adg_lt_version)
AC_SUBST([ADG_REQUIRES])
AC_SUBST([ADG_H_ADDITIONAL])
AC_SUBST([ADG_CANVAS_H_ADDITIONAL])

AM_SUBST_NOTMAKE([CPML_REQUIRES])
AM_SUBST_NOTMAKE([ADG_REQUIRES])
AM_SUBST_NOTMAKE([ADG_H_ADDITIONAL])
AM_SUBST_NOTMAKE([ADG_CANVAS_H_ADDITIONAL])

dnl CPML compiler flags and library dependencies
CPML_CFLAGS="$CAIRO_CFLAGS $GOBJECT_CFLAGS"
CPML_LIBS="$CAIRO_LIBS $GOBJECT_LIBS"
AC_SUBST([CPML_CFLAGS])
AC_SUBST([CPML_LIBS])

dnl ADG compiler flags and library dependencies
ADG_CFLAGS="$CAIRO_GOBJECT_CFLAGS"
ADG_LIBS="$CAIRO_GOBJECT_LIBS"
AM_COND_IF([HAVE_PANGO],
	   [ADG_CFLAGS="$PANGO_CFLAGS $ADG_CFLAGS"
	    ADG_LIBS="$PANGO_LIBS $ADG_LIBS"])
AM_COND_IF([HAVE_GTK2],
	   [ADG_CFLAGS="$GTK2_CFLAGS $ADG_CFLAGS"
	    ADG_LIBS="$GTK2_LIBS $ADG_LIBS"])
AM_COND_IF([HAVE_GTK3],
	   [ADG_CFLAGS="$GTK3_CFLAGS $ADG_CFLAGS"
	    ADG_LIBS="$GTK3_LIBS $ADG_LIBS"])
AC_SUBST([ADG_CFLAGS])
AC_SUBST([ADG_LIBS])

AC_SUBST([EXTRADIR])


# Generation

AC_CONFIG_FILES([build/Custom.nsh
                 Makefile
                 src/Makefile
                 src/tests/Makefile
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
                CPML library to use: internal (cpml-adg_version)
         Build pango based entities: ${enable_pango}${pango_postfix}
                       GTK+ support: ${with_gtk}${gtk_postfix}
             Install glade catalogs: ${enable_glade}${report_glade}
                Build API reference: ${enable_gtk_doc}${gtkdoc_postfix}
             GObject instrospection: ${enable_introspection}${report_introspection}
                Test coverage build: ${enable_gcov}
             Test framework support: ${enable_test_framework}${glib_postfix}
])
