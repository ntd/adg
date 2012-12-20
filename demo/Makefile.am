include $(top_srcdir)/build/Makefile.am.common

AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src \
				-DSRCDIR='"$(abs_srcdir)"' \
				-DBUILDDIR='"$(abs_builddir)"'
AM_CFLAGS=			$(GTK3_CFLAGS) $(GTK2_CFLAGS) \
				$(CAIRO_CFLAGS) \
				$(GOBJECT2_CFLAGS)
LDADD=				$(top_builddir)/src/adg/libadg-1.la


bin_PROGRAMS=			adg-demo \
				cpml-demo
adg_demo_SOURCES=		adg-demo.c

if HAVE_GTK3
cpml_demo_SOURCES=		cpml-demo.c
endif

if HAVE_GTK2
cpml_demo_SOURCES=		cpml-demo-gtk2.c
endif

if OS_WINDOWS
adgdatadir=			$(bindir)
else
adgdatadir=			$(pkgdatadir)
endif

nodist_adgdata_DATA=		adg-demo.ui \
				cpml-demo.ui
dist_adgdata_DATA=		adg-16.png \
				adg-32.png \
				adg-48.png \
				adg-64.png \
				adg-128.png


EXTRA_DIST=			cpml-demo.ui.in \
				adg-demo.ui.in
