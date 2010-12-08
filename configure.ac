m4_define([adg_version],[0.6.1])
m4_define([adg_api_version],[1.0])

m4_define([gtkdoc_prereq],[1.9])
m4_define([gobject_prereq],[2.10.1])
m4_define([cairo_prereq],[1.7.4])
m4_define([gtk_prereq],[2.12.0])


# Initialization

dnl autoconf 2.60 implements datarootdir, required by ADG
dnl to help overcoming the hardcoded glade catalogdir
AC_PREREQ([2.60])
AC_INIT([adg-1.0],adg_version,[http://dev.entidi.com/p/adg/],[adg],[http://adg.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AM_INIT_AUTOMAKE([1.6 gnits no-dist-gzip dist-bzip2 -Wall])
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
AM_CONDITIONAL([OS_WINDOWS],[test "${guessed_host}" == "mingw" -o "${guessed_host}" == "cygwin"])


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

GTK_DOC_CHECK(gtkdoc_prereq)
PKG_CHECK_MODULES([GOBJECT2],[gobject-2.0 >= ]gobject_prereq)
PKG_CHECK_MODULES([CAIRO],[cairo >= ]cairo_prereq)


# Check for optional GTK+2 support

AC_ARG_ENABLE([gtk2],
              [AS_HELP_STRING([--enable-gtk2],
                              [include GKT+2 specific widgets @<:@default=check@:>@])],
              [],[enable_gtk2=check])
AS_IF([test "x${enable_gtk2}" != "xno"],
      [PKG_CHECK_MODULES([GTK2],[gtk+-2.0 >= ]gtk_prereq,
                         [enable_gtk2=yes
                          DLLDIR=`${PKG_CONFIG} --variable=prefix gtk+-2.0`/bin
                          AC_SUBST([DLLDIR])],
                         [AS_IF([test "x${enable_gtk2}" = "xyes"],
                                [AC_MSG_ERROR([${GTK2_PKG_ERRORS} and GTK+2 support requested])],
                                [enable_gtk2=no])])])
AM_CONDITIONAL([HAVE_GTK2],[test "x${enable_gtk2}" = "xyes"])


# Check for optional glade support

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
AS_IF([test "x${enable_glade}" == "xyes"],
      [AC_MSG_CHECKING([for the glade catalog path])
       gladeroot=`${PKG_CONFIG} --variable=datarootdir gladeui-1.0`
       AS_IF([test "x${gladeroot}" == "x"],
             [GLADE_CATALOGDIR="\${datarootdir}/glade3/catalogs}"],
             [gladecatalog=`${PKG_CONFIG} --variable=catalogdir gladeui-1.0`
              GLADE_CATALOGDIR=`echo "${gladecatalog}" | ${SED} -e "s|^${gladeroot}|\$\{datarootdir\}|" -`])
       AC_SUBST([GLADE_CATALOGDIR])
       AC_MSG_RESULT(${GLADE_CATALOGDIR})])



# Check for optional GLib test framework

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


# Final step

AC_SUBST([CPML_REQUIRES],'cairo >= cairo_prereq')
AC_SUBST([ADG_REQUIRES],'gobject-2.0 >= gobject_prereq')
AC_SUBST([ADG_GTK_REQUIRES],'gtk+-2.0 >= gtk_prereq')

SHAVE_INIT([build],[enable])
AC_CONFIG_FILES([build/shave
                 build/shave-libtool
                 build/adg.nsi
                 Makefile
                 src/Makefile
                 src/cpml-1.pc
                 src/cpml/Makefile
                 src/adg-1.pc
                 src/adg/Makefile
                 src/adg/tests/Makefile
                 src/adg-gtk-1.pc
                 src/adg-gtk/Makefile
                 src/adg-gtk/tests/Makefile
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
           GTK+2 additional widgets: ${enable_gtk2}
             Install glade catalogs: ${enable_glade}
                Build API reference: ${enable_gtk_doc}
             Test framework support: ${enable_test_framework}
])
