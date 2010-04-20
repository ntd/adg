include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			-g @ADG_GTK_CFLAGS@ \
				-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/adg/tests
LDADD=				@ADG_GTK_LIBS@ \
				$(top_builddir)/src/adg/tests/test-internal.$(OBJEXT) \
				$(top_builddir)/src/adg-gtk/libadg-gtk-1.la


if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-gtk-area
test_gtk_area_SOURCES=		test-gtk-area.c

endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
