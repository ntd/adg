include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src
AM_CFLAGS=			$(GLIB_CFLAGS)
LDADD=				$(GLIB_LIBS)


if HAVE_TEST_FRAMEWORK

# Helper library for ADG and CPML testing
check_LTLIBRARIES=		libadgtest.la
libadgtest_la_SOURCES=		adg-test.c \
				adg-test.h

endif
