include $(top_srcdir)/build/Makefile.am.common

SUBDIRS=			tests


# file groups
h_sources=			adg-gtk-utils.h \
				adg-gtk-area.h
built_h_sources=
private_h_sources=		adg-gtk-internal.h \
				adg-gtk-area-private.h
built_private_h_sources=
c_sources=			adg-gtk-utils.c \
				adg-gtk-area.c
built_c_sources=


# targets
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources)
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

adg_gtk_includedir=		$(includedir)/adg-1/adg-gtk
adg_gtk_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libadg-gtk-1.la
libadg_gtk_1_la_SOURCES=	$(h_sources) \
				$(private_h_sources) \
				$(c_sources)
nodist_libadg_gtk_1_la_SOURCES=	$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
libadg_gtk_1_la_CFLAGS=		$(GTK2_CFLAGS) \
				-I$(top_srcdir)/src
libadg_gtk_1_la_LDFLAGS=	-no-undefined
libadg_gtk_1_la_LIBADD=		$(GTK2_LIBS) \
				$(top_builddir)/src/adg/libadg-1.la
