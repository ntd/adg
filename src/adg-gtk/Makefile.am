include $(top_srcdir)/build/Makefile.am.common


SUBDIRS=			tests
AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src
AM_CFLAGS=			$(GTK2_CFLAGS)
AM_LDFLAGS=			$(GTK2_LIBS)
LDADD=				$(top_builddir)/src/cpml/libadg-1.la


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
EXTRA_DIST=			adg-gtk-introspection.h \
				$(template_sources)
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


## GObject introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS=
INTROSPECTION_SCANNER_ARGS=	--add-include-path=$(top_builddir)/src/cpml \
				--add-include-path=$(top_builddir)/src/adg
INTROSPECTION_COMPILER_ARGS=	--includedir=$(top_builddir)/src/cpml \
				--includedir=$(top_builddir)/src/adg

if HAVE_INTROSPECTION

INTROSPECTION_GIRS+=		AdgGtk-1.0.gir
AdgGtk-1.0.gir: libadg-gtk-1.la

AdgGtk_1_0_gir_LIBS=		libadg-gtk-1.la
AdgGtk_1_0_gir_INCLUDES=	Adg-1.0 \
				Gtk-2.0
AdgGtk_1_0_gir_PACKAGES=	adg-1 \
				gtk+-2.0
AdgGtk_1_0_gir_FILES=		adg-gtk-introspection.h \
				$(h_sources) \
				$(c_sources)
AdgGtk_1_0_gir_CFLAGS=		$(AM_CPPFLAGS) \
				$(AM_CFLAGS)
AdgGtk_1_0_gir_LDFLAGS=		$(AM_LDFLAGS) \
				-L$(top_builddir)/src/cpml/.libs \
				-lcpml-1 \
				-L$(top_builddir)/src/adg/.libs \
				-ladg-1
AdgGtk_1_gir_SCANNERFLAGS=	--c-include="adg-gtk.h"
girdir=				$(datadir)/gir-1.0
gir_DATA=			$(INTROSPECTION_GIRS)
typelibdir=			$(libdir)/girepository-1.0
typelib_DATA=			$(INTROSPECTION_GIRS:.gir=.typelib)
CLEANFILES+=			$(gir_DATA) \
				$(typelib_DATA)

endif


mkenums_command=		$(AM_V_GEN)$(GLIB_MKENUMS) \
				--template
genmarshal_command=		$(AM_V_GEN)$(GLIB_GENMARSHAL) \
				--prefix=adg_gtk_marshal


adg-gtk-marshal.c: $(srcdir)/adg-gtk-marshal.genmarshal
	$(genmarshal_command) --body $(srcdir)/adg-gtk-marshal.genmarshal > $@

adg-gtk-marshal.h: $(srcdir)/adg-gtk-marshal.genmarshal
	$(genmarshal_command) --header $(srcdir)/adg-gtk-marshal.genmarshal > $@
