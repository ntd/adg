include $(top_srcdir)/build/Makefile.am.common

AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src
AM_CFLAGS=			$(CAIRO_CFLAGS) \
				$(GOBJECT2_CFLAGS)
AM_LDFLAGS=			$(CAIRO_LIBS) \
				$(GOBJECT2_LIBS)
LDADD=				$(top_builddir)/src/cpml/libcpml-1.la \
				$(top_builddir)/src/adg/libadg-1.la

if OS_WINDOWS

## On windows, ADG_DATADIR is handled as a path relative
## to the binary executable
adgdatadir=			$(bindir)
AM_CPPFLAGS+=			-DADG_DATADIR='""'

else # ! OS_WINDOWS

adgdatadir=			$(pkgdatadir)
AM_CPPFLAGS+=			-DADG_DATADIR='"$(adgdatadir)"'

endif


if HAVE_GTK2

AM_CFLAGS+=			$(GTK2_CFLAGS)
AM_LDFLAGS+=			$(GTK2_LIBS)

bin_PROGRAMS=			adg-demo \
				cpml-demo
adg_demo_SOURCES=		adg-demo.c \
				demo.c \
				demo.h
adg_demo_LDADD=			$(LDADD) \
				$(top_builddir)/src/adg/libadg-1.la
cpml_demo_SOURCES=		cpml-demo.c \
				demo.c \
				demo.h
nodist_adgdata_DATA=		adg-demo.ui \
				cpml-demo.ui
dist_adgdata_DATA=		adg-64.png
endif

EXTRA_DIST=			cpml-demo.ui.in \
				adg-demo.ui.in
