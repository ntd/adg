include $(top_srcdir)/build/Makefile.am.common

SUBDIRS=			tests


# file groups
h_sources=			adg-gtk-area.h \
				adg-gtk-layout.h \
				adg-gtk-utils.h
built_h_sources=
private_h_sources=		adg-gtk-area-private.h \
				adg-gtk-internal.h \
				adg-gtk-layout-private.h
built_private_h_sources=	adg-gtk-marshal.h
c_sources=			adg-gtk-area.c \
				adg-gtk-layout.c \
				adg-gtk-utils.c
built_c_sources=		adg-gtk-marshal.c
template_sources=		adg-gtk-marshal.genmarshal

# targets
EXTRA_DIST=			$(template_sources)
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


mkenums_command=		$(MSG_GEN) ; \
				$(GLIB_MKENUMS) --template
genmarshal_command=		$(MSG_GEN) ; \
				$(GLIB_GENMARSHAL) --prefix=adg_gtk_marshal

adg-gtk-marshal.c: $(srcdir)/adg-gtk-marshal.genmarshal
	$(genmarshal_command) --body $(srcdir)/adg-gtk-marshal.genmarshal > $@

adg-gtk-marshal.h: $(srcdir)/adg-gtk-marshal.genmarshal
	$(genmarshal_command) --header $(srcdir)/adg-gtk-marshal.genmarshal > $@
