include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src
AM_CFLAGS=			$(GTK2_CFLAGS)
AM_LDFLAGS=			$(GTK2_LIBS)
LDADD=				$(top_builddir)/src/cpml/libcpml-1.la \
				$(top_builddir)/src/adg/libadg-1.la \
				$(top_builddir)/src/adg-gtk/libadg-gtk-1.la


# file groups
test_internals=			test-internal.c \
				test-internal.h

if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-gtk-area$(EXEEXT)
test_gtk_area_SOURCES=		test-gtk-area.c \
				$(test_internals)

TEST_PROGS+=			test-gtk-layout$(EXEEXT)
test_gtk_layout_SOURCES=	test-gtk-layout.c \
				$(test_internals)

endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
