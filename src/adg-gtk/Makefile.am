include $(top_srcdir)/build/Makefile.am.common

SUBDIRS=			tests

AM_CFLAGS=			@ADG_GTK_CFLAGS@ \
				-I$(top_srcdir)/src
AM_LDFLAGS=			@ADG_GTK_LIBS@


# file groups
h_sources=			adg-widget.h
built_h_sources=
private_h_sources=		adg-widget-private.h
built_private_h_sources=
c_sources=			adg-widget.c
built_c_sources=


# targets
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources)
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

adggtk_includedir=		$(includedir)/adg-1/adg-gtk
adggtk_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libadg-gtk-1.la
libadg_gtk_1_la_LDFLAGS=
libadg_gtk_1_la_SOURCES=	$(h_sources) \
				$(private_h_sources) \
				$(c_sources)
nodist_libadg_gtk_1_la_SOURCES=	$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
libadg_gtk_1_la_LIBADD=		$(top_builddir)/src/adg/libadg-1.la
libadg_gtk_1_la_DEPENDENCIES=	$(top_builddir)/src/adg/libadg-1.la
