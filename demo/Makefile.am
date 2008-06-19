## Process this file with automake to produce Makefile.in.

AM_CFLAGS =		@GCONTAINER_CFLAGS@ \
			@GTK2_CFLAGS@ \
			-I$(top_srcdir)
AM_LDFLAGS =		@GCONTAINER_LIBS@ \
		       	@GTK2_LIBS@
LDADD =			$(top_builddir)/adg/libadg.la

bin_PROGRAMS =		adg-demo
noinst_PROGRAMS =	ldim
