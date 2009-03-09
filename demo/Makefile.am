## Process this file with automake to produce Makefile.in.

AM_CFLAGS =		@GTK2_CFLAGS@ \
			-I$(top_srcdir)
AM_LDFLAGS =		@GTK2_LIBS@
AM_CPPFLAGS =		-DPKGDATADIR='"$(pkgdatadir)"' \
			-DSOURCEDIR='"$(abs_srcdir)"'
LDADD =			$(top_builddir)/adg/libadg.la

bin_PROGRAMS =		adg-demo \
			cpml-demo
noinst_PROGRAMS =	ldim

dist_pkgdata_DATA =	cpml-demo.ui
