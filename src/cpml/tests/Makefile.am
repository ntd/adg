include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			-g \
				$(GLIB_CFLAGS) \
				$(CAIRO_CFLAGS) \
				-I$(top_srcdir)/src \
				-I$(top_builddir)/src
LDADD=				$(top_builddir)/src/cpml/libcpml-1.la \
				$(GLIB_LIBS)


# file groups
test_internals=			test-internal.c \
				test-internal.h

if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-segment$(EXEEXT)
test_segment_SOURCES=		test-segment.c \
				$(test_internals)

TEST_PROGS+=			test-primitive$(EXEEXT)
test_primitive_SOURCES=		test-primitive.c \
				$(test_internals)


endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
