m4_define([adg_version],[0.5.6])
m4_define([adg_api_version],[1.0])

m4_define([gtkdoc_prereq],[1.9])
m4_define([gobject_prereq],[2.4.0])
m4_define([cairo_prereq],[1.7.4])
m4_define([gtk_prereq],[2.12.0])


# Initialization

dnl autoconf 2.60 implements datarootdir, required by ADG
dnl to help overcoming the hardcoded glade catalogdir
AC_PREREQ([2.60])
AC_INIT([Automatic Drawing Generation],adg_version,
        [http://dev.entidi.com/p/adg/],[adg],[http://adg.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([build])
AM_INIT_AUTOMAKE([1.6 gnits no-dist-gzip dist-bzip2 -Wall])


# Check for programs

AC_PROG_CC
AC_PROG_SED
PKG_PROG_PKG_CONFIG
AC_PATH_PROG([XSLTPROC],[xsltproc],[/usr/bin/xsltproc])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums],[/usr/bin/glib-mkenums])
AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal],[/usr/bin/glib-genmarshal])
AC_PATH_PROG([GTESTER],[gtester],[/usr/bin/gtester])
AC_PATH_PROG([GTESTER_REPORT],[gtester-report],[/usr/bin/gtester-report])

AC_MSG_CHECKING([which libtool initialization strategy to adopt])
AC_MSG_RESULT([m4_ifset([LT_INIT],[LT-INIT],[AC-PROG-LIBTOOL])])
m4_ifset([LT_INIT],
         [LT_INIT([disable-static])],
         [AC_DISABLE_STATIC
          AC_PROG_LIBTOOL])


# I18n

AC_SUBST([GETTEXT_PACKAGE],adg-adg_api_version)
AC_DEFINE([GETTEXT_PACKAGE],"AC_PACKAGE_TARNAME-adg_api_version",
	  [Define to the domain used by gettext.])
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT
GLIB_DEFINE_LOCALEDIR([LOCALEDIR])
AC_OUTPUT_COMMANDS([case "${CONFIG_FILES}" in *po-properties/Makefile.in*)
        ${SED} -e "/POTFILES =/r po-properties/POTFILES" po-properties/Makefile.in > po-properties/Makefile
      esac])


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
                         [enable_gtk2=yes],
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

CPML_REQUIRES='cairo >= cairo_prereq'
CPML_CFLAGS="${CAIRO_CFLAGS}"
CPML_LIBS="${CAIRO_LIBS}"
ADG_REQUIRES='gobject-2.0 >= gobject_prereq'
ADG_CFLAGS="${GOBJECT2_CFLAGS} ${CPML_CFLAGS}"
ADG_LIBS="${GOBJECT2_LIBS} ${CPML_LIBS}"
AS_IF([test "x${enable_gtk2}" = "xyes"],
      [ADG_GTK_REQUIRES='gtk+-2.0 >= gtk_prereq'
       ADG_GTK_CFLAGS="${GTK2_CFLAGS} ${CPML_CFLAGS}"
       ADG_GTK_LIBS="${GTK2_LIBS} ${CPML_LIBS}"
       AC_SUBST([ADG_GTK_REQUIRES])
       AC_SUBST([ADG_GTK_CFLAGS])
       AC_SUBST([ADG_GTK_LIBS])])
AC_SUBST([CPML_REQUIRES])
AC_SUBST([CPML_CFLAGS])
AC_SUBST([CPML_LIBS])
AC_SUBST([ADG_REQUIRES])
AC_SUBST([ADG_CFLAGS])
AC_SUBST([ADG_LIBS])

SHAVE_INIT([build],[enable])
AC_CONFIG_FILES([build/shave
                 build/shave-libtool
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
                 demo/adg-demo.ui
                 po/Makefile.in
                 po-properties/Makefile.in
                 docs/Makefile
                 docs/cpml/Makefile
                 docs/cpml/bookinfo.xml
                 docs/adg/Makefile
                 docs/adg/bookinfo.xml])
AC_OUTPUT


# Report

AC_MSG_NOTICE([generating report

ADG canvas adg_version will be built with the following options:
----------------------------------------------------------
                  API compatibility: adg_api_version
                CPML library to use: internal (cpml-adg_version)
           GTK+2 additional widgets: ${enable_gtk2}
             Install glade catalogs: ${enable_glade}
                Build API reference: ${enable_gtk_doc}
             Test framework support: ${enable_test_framework}
])
