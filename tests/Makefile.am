include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=		-g @ADG_CFLAGS@ \
			-I$(top_srcdir) \
			-I$(top_builddir)
AM_LDFLAGS=		@ADG_LIBS@
LDADD=			$(top_builddir)/adg/libadg.la


# file groups
test_programs=
test_internals=		test-internal.c \
			test-internal.h

if HAVE_TEST_FRAMEWORK

test_programs+=		test-model
test_model_SOURCES=	test-model.c \
			$(test_internals)

test_programs+=		test-edges
test_edges_SOURCES=	test-edges.c \
			$(test_internals)

test_programs+=		test-entity
test_entity_SOURCES=	test-entity.c \
			$(test_internals)

endif


# targets
check_PROGRAMS=		$(test_programs)
TESTS=			$(test_programs)
