include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src/tests \
				-I$(top_srcdir)/src
AM_CFLAGS=			$(ADG_CFLAGS)
LDADD=				$(top_builddir)/src/tests/libadgtest.la \
				$(top_builddir)/src/adg/libadg-1.la \
				$(top_builddir)/src/cpml/libcpml-1.la \
				$(ADG_LIBS)


TEST_PROGS+=			test-pair$(EXEEXT)
test_pair_SOURCES=		test-pair.c

TEST_PROGS+=			test-extents$(EXEEXT)
test_extents_SOURCES=		test-extents.c

TEST_PROGS+=			test-segment$(EXEEXT)
test_segment_SOURCES=		test-segment.c

TEST_PROGS+=			test-primitive$(EXEEXT)
test_primitive_SOURCES=		test-primitive.c

TEST_PROGS+=			test-arc$(EXEEXT)
test_arc_SOURCES=		test-arc.c

TEST_PROGS+=			test-curve$(EXEEXT)
test_curve_SOURCES=		test-curve.c

TEST_PROGS+=			test-gobject$(EXEEXT)
test_gobject_SOURCES=		test-gobject.c


# targets
check_PROGRAMS=			$(TEST_PROGS)


# Possibly remove files created on test coverage builds
mostlyclean-local:
	-rm -f *.gcda *gcno
