include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			@GTK2_CFLAGS@ \
				-I$(top_srcdir)/src \
				-I$(top_builddir)/src

if OS_WINDOWS

## On windows, ADG_DATADIR is handled as a path relative
## to the binary executable
adgdatadir=			$(bindir)
AM_CPPFLAGS=			-DADG_DATADIR='""'

else # ! OS_WINDOWS

adgdatadir=			$(pkgdatadir)
AM_CPPFLAGS=			-DADG_DATADIR='"$(adgdatadir)"'

endif


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
nodist_adgdata_DATA=		adg-demo.ui \
				cpml-demo.ui
dist_adgdata_DATA=		adg-64.png
endif

EXTRA_DIST=			cpml-demo.ui.in \
				adg-demo.ui.in
