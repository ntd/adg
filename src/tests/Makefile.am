include $(top_srcdir)/autotools/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/adg \
				-I$(top_builddir)/src/adg
AM_CFLAGS=			$(ADG_CFLAGS) \
				$(CPML_CFLAGS) \
				$(CAIRO_CFLAGS) \
				$(GOBJECT_CFLAGS)
LDADD=				$(ADG_LIBS) \
				$(top_builddir)/src/adg/libadg-1.la \
				$(CPML_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la \
				$(CAIRO_LIBS) \
				$(GOBJECT_LIBS)


# Helper library for ADG and CPML testing
check_LTLIBRARIES=		libadgtest.la
libadgtest_la_SOURCES=		adg-test.c \
				adg-test.h
