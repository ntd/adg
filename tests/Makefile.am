AM_CFLAGS=		-g @ADG_CFLAGS@ \
			-I$(top_srcdir) \
			-I$(top_builddir)
AM_LDFLAGS=		@ADG_LIBS@
LDADD=			$(top_builddir)/adg/libadg.la


# file groups
if HAVE_TEST_FRAMEWORK
test_programs=		properties
endif


# targets
check_PROGRAMS=		$(test_programs)
TESTS=			$(test_programs)
