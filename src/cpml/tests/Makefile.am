include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/tests
AM_CFLAGS=			$(CAIRO_CFLAGS) \
				$(GOBJECT_CFLAGS)
LDADD=				$(top_builddir)/src/cpml/libcpml-1.la


if HAVE_TEST_FRAMEWORK

LDADD+=				$(top_builddir)/src/tests/libadgtest.la

TEST_PROGS+=			test-pair$(EXEEXT)
test_pair_SOURCES=		test-pair.c

TEST_PROGS+=			test-extents$(EXEEXT)
test_extents_SOURCES=		test-extents.c

TEST_PROGS+=			test-segment$(EXEEXT)
test_segment_SOURCES=		test-segment.c

TEST_PROGS+=			test-primitive$(EXEEXT)
test_primitive_SOURCES=		test-primitive.c

TEST_PROGS+=			test-gobject$(EXEEXT)
test_primitive_SOURCES=		test-gobject.c

endif


LDADD+=				$(CAIRO_LIBS) \
				$(GOBJECT_LIBS)


# targets
check_PROGRAMS=			$(TEST_PROGS)


# Possibly remove files created on test coverage builds
mostlyclean-local:
	-rm -f *.gcda *gcno
