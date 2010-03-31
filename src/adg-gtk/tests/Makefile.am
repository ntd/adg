include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			-g @ADG_GTK_CFLAGS@ \
				-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/adg/tests
AM_LDFLAGS=			@ADG_GTK_LIBS@
LDADD=				$(top_builddir)/src/adg/tests/test-internal.$(OBJEXT) \
				$(top_builddir)/src/adg-gtk/libadg-gtk-1.la


if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-widget
test_widget_SOURCES=		test-widget.c

endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
