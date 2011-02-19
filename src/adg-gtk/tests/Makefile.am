include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			-g \
				$(GTK2_CFLAGS) \
				-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/adg/tests \
				-I$(top_builddir)/src
LDADD=				$(top_builddir)/src/adg/tests/test-internal.$(OBJEXT) \
				$(top_builddir)/src/adg-gtk/libadg-gtk-1.la


if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-gtk-area$(EXEEXT)
test_gtk_area_SOURCES=		test-gtk-area.c

TEST_PROGS+=			test-gtk-layout$(EXEEXT)
test_gtk_layout_SOURCES=	test-gtk-layout.c

endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
