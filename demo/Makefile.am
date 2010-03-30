include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=		@ADG_CFLAGS@ \
			-I$(top_srcdir)/src \
			-I$(top_builddir)/src
AM_LDFLAGS=		@ADG_LIBS@
AM_CPPFLAGS=		-DPKGDATADIR='"$(pkgdatadir)"' \
			-DSOURCEDIR='"$(abs_top_srcdir)/demo"'
LDADD=			$(top_builddir)/src/adg/libadg.la


if HAVE_GTK2

bin_PROGRAMS=		adg-demo \
			cpml-demo
adg_demo_SOURCES=	adg-demo.c \
			demo.c \
			demo.h
cpml_demo_SOURCES=	cpml-demo.c \
			demo.c \
			demo.h

dist_pkgdata_DATA=	adg-demo.ui \
			cpml-demo.ui

endif
