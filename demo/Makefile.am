include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			@GTK2_CFLAGS@ \
				-I$(top_srcdir)/src \
				-I$(top_builddir)/src
AM_CPPFLAGS=			-DPKGDATADIR='"$(pkgdatadir)"'


if HAVE_GTK2
bin_PROGRAMS=			adg-demo \
				cpml-demo
adg_demo_SOURCES=		adg-demo.c \
				demo.c \
				demo.h
adg_demo_LDADD=			$(top_builddir)/src/adg-gtk/libadg-gtk-1.la
cpml_demo_SOURCES=		cpml-demo.c \
				demo.c \
				demo.h
cpml_demo_LDADD=		$(GTK2_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la
nodist_pkgdata_DATA=		adg-demo.ui
dist_pkgdata_DATA=		cpml-demo.ui \
				adg-64.png
endif

EXTRA_DIST=			adg-demo.ui.in
