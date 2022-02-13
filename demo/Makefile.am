include $(top_srcdir)/build/Makefile.am.common

AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src \
				-DPKGDATADIR='"$(pkgdatadir)"' \
				-DSRCDIR='"$(abs_srcdir)"' \
				-DBUILDDIR='"$(abs_builddir)"'

# On Windows, do not spawn the command prompt window (shown by default)
# by setting a flag in the PE header of the executables
if OS_WINDOWS
AM_CFLAGS=			-mwindows
endif


bin_PROGRAMS=			adg-demo \
				cpml-demo

adg_demo_SOURCES=		adg-demo.c \
				demo.c \
				demo.h
adg_demo_CFLAGS=		$(CPML_CFLAGS) \
				$(ADG_CFLAGS)
adg_demo_LDADD=			$(CPML_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la \
				$(ADG_LIBS) \
				$(top_builddir)/src/adg/libadg-1.la

# The CPML demo program uses the ADG library only marginally for
# looking up the ui file (adg_find_file()) and for i18n.
cpml_demo_SOURCES=		cpml-demo.c \
				demo.c \
				demo.h
cpml_demo_CFLAGS=		$(CPML_CFLAGS) \
				$(ADG_CFLAGS)
cpml_demo_LDADD=		$(CPML_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la \
				$(ADG_LIBS) \
				$(top_builddir)/src/adg/libadg-1.la


if OS_WINDOWS
%.rc.o: adg.manifest
	$(AM_V_GEN)$(WINDRES) \
	    --define PACKAGE_NAME="$*" \
	    --define PACKAGE_VERSION="$(PACKAGE_VERSION)" \
	    --define PACKAGE_VERSIONS=`echo $(PACKAGE_VERSION) | tr . ,` \
	    $(srcdir)/adg.rc $@

adg_demo_LDADD+=		adg-demo.rc.o
cpml_demo_LDADD+=		cpml-demo.rc.o
endif



nodist_pkgdata_DATA=		adg-demo.ui
dist_pkgdata_DATA=		cpml-demo.ui

iconsdir=			$(pkgdatadir)/icons
dist_icons_DATA=		icons/adg-16.png \
				icons/adg-32.png \
				icons/adg-48.png \
				icons/adg-64.png \
				icons/adg-128.png \
				icons/adg-arc.png \
				icons/adg-browsing.png \
				icons/adg-curve.png \
				icons/adg-intersection.png \
				icons/adg-segment.png


EXTRA_DIST=			adg-demo.ui.in \
				adg.rc \
				adg.ico \
				adg.manifest


# Possibly remove files created by 'make coverage'
mostlyclean-local:
	-rm -f *.gcda *.gcno
