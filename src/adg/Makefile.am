include $(top_srcdir)/build/Makefile.am.common


if HAVE_TEST_FRAMEWORK
SUBDIRS=			tests
endif


AM_CFLAGS=			$(CPML_CFLAGS) \
				$(ADG_CFLAGS)
AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src \
				-DLOCALEDIR=\""$(localedir)"\"


# file groups
h_sources=			adg-adim.h \
				adg-alignment.h \
				adg-arrow.h \
				$(builddir)/adg-canvas.h \
				adg-color-style.h \
				adg-container.h \
				adg-dash.h \
				adg-dim.h \
				adg-dim-style.h \
				adg-dress.h \
				adg-edges.h \
				adg-entity.h \
				adg-enums.h \
				adg-fill-style.h \
				adg-font-style.h \
				adg-forward-declarations.h \
				adg-hatch.h \
				adg-ldim.h \
				adg-line-style.h \
				adg-logo.h \
				adg-marker.h \
				adg-matrix.h \
				adg-model.h \
				adg-param-dress.h \
				adg-path.h \
				adg-point.h \
				adg-projection.h \
				adg-rdim.h \
				adg-ruled-fill.h \
				adg-stroke.h \
				adg-style.h \
				adg-table.h \
				adg-table-cell.h \
				adg-table-row.h \
				adg-table-style.h \
				adg-textual.h \
				adg-title-block.h \
				adg-toy-text.h \
				adg-trail.h \
				adg-utils.h
built_h_sources=		adg-type-builtins.h
private_h_sources=		adg-adim-private.h \
				adg-alignment-private.h \
				adg-arrow-private.h \
				adg-canvas-private.h \
				adg-color-style-private.h \
				adg-container-private.h \
				adg-dash-private.h \
				adg-dim-private.h \
				adg-dim-style-private.h \
				adg-dress-private.h \
				adg-edges-private.h \
				adg-entity-private.h \
				adg-fill-style-private.h \
				adg-font-style-private.h \
				adg-hatch-private.h \
				adg-internal.h \
				adg-ldim-private.h \
				adg-line-style-private.h \
				adg-logo-private.h \
				adg-marker-private.h \
				adg-model-private.h \
				adg-path-private.h \
				adg-projection-private.h \
				adg-rdim-private.h \
				adg-ruled-fill-private.h \
				adg-stroke-private.h \
				adg-table-private.h \
				adg-table-style-private.h \
				adg-text-internal.h \
				adg-title-block-private.h \
				adg-toy-text-private.h \
				adg-trail-private.h
built_private_h_sources=	adg-marshal.h
fallback_h_sources=		adg-cairo-fallback.h
c_sources=			adg-adim.c \
				adg-alignment.c \
				adg-arrow.c \
				adg-canvas.c \
				adg-color-style.c \
				adg-container.c \
				adg-dash.c \
				adg-dim.c \
				adg-dim-style.c \
				adg-dress.c \
				adg-edges.c \
				adg-entity.c \
				adg-enums.c \
				adg-fill-style.c \
				adg-font-style.c \
				adg-hatch.c \
				adg-ldim.c \
				adg-line-style.c \
				adg-logo.c \
				adg-marker.c \
				adg-matrix.c \
				adg-model.c \
				adg-param-dress.c \
				adg-path.c \
				adg-point.c \
				adg-projection.c \
				adg-rdim.c \
				adg-ruled-fill.c \
				adg-stroke.c \
				adg-style.c \
				adg-table.c \
				adg-table-cell.c \
				adg-table-row.c \
				adg-table-style.c \
				adg-textual.c \
				adg-title-block.c \
				adg-trail.c \
				adg-toy-text.c \
				adg-utils.c
built_c_sources=		adg-type-builtins.c \
				adg-marshal.c
fallback_c_sources=		adg-cairo-fallback.c
template_sources=		adg-type-builtins.c.mkenums \
				adg-type-builtins.h.mkenums \
				adg-marshal.genmarshal


# targets
EXTRA_DIST=			$(template_sources) \
				adg-1.pc.in
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources)
#				built_c_sources are properly tracked
#				by nodist_libadg_1_la_SOURCES
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)


## Optional GTK+ related stuff

if HAVE_GTK
h_sources+=			adg-gtk-area.h \
				adg-gtk-layout.h \
				adg-gtk-utils.h
private_h_sources+=		adg-gtk-area-private.h \
				adg-gtk-layout-private.h
c_sources+=			adg-gtk-area.c \
				adg-gtk-layout.c \
				adg-gtk-utils.c
endif


## Optional pangocairo related stuff

if HAVE_PANGO
h_sources+=			adg-pango-style.h \
				adg-text.h
private_h_sources+=		adg-pango-style-private.h \
				adg-text-private.h
c_sources+=			adg-pango-style.c \
				adg-text.c
endif


pkgconfig_DATA=			adg-1.pc

adg_includedir=			$(includedir)/adg-1/adg
adg_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libadg-1.la
libadg_1_la_SOURCES=		$(h_sources) \
				$(private_h_sources) \
				$(fallback_h_sources) \
				$(c_sources) \
				$(fallback_c_sources)
nodist_libadg_1_la_SOURCES=	$(built_c_sources)
libadg_1_la_LDFLAGS=		-no-undefined \
				-version-info $(ADG_LT_VERSION)
libadg_1_la_LIBADD=		$(CPML_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la \
				$(ADG_LIBS)


## GObject introspection

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS=
INTROSPECTION_SCANNER_ARGS=	--add-include-path=$(top_builddir)/src/cpml
INTROSPECTION_COMPILER_ARGS=	--includedir=$(top_builddir)/src/cpml

if HAVE_INTROSPECTION

Adg-1.0.gir: libadg-1.la

INTROSPECTION_GIRS+=		Adg-1.0.gir
Adg_1_0_gir_LIBS=		libadg-1.la
Adg_1_0_gir_INCLUDES=
Adg_1_0_gir_PACKAGES=

if HAVE_GTK3
Adg_1_0_gir_INCLUDES+=		Gtk-3.0
endif

if HAVE_GTK2
Adg_1_0_gir_INCLUDES+=		Gtk-2.0
# gir doc says INCLUDES will already fetch the packages (it works for GTK+3)
# but without this the build after a ./configure --with-gtk=gtk2 will fail
Adg_1_0_gir_PACKAGES+=		gtk+-2.0
endif

if HAVE_PANGO
Adg_1_0_gir_INCLUDES+=		PangoCairo-1.0
endif

Adg_1_0_gir_INCLUDES+=		Cpml-1.0 \
				cairo-1.0 \
				GObject-2.0
Adg_1_0_gir_FILES=		../adg.h \
				$(h_sources) \
				$(c_sources)
Adg_1_0_gir_SCANNERFLAGS=	$(AM_CPPFLAGS) \
				--c-include='adg.h' \
				--warn-all
girdir=				$(ADG_GIRDIR)
dist_gir_DATA=			$(INTROSPECTION_GIRS)
typelibdir=			$(ADG_TYPELIBDIR)
typelib_DATA=			Adg-1.0.typelib
CLEANFILES+=			$(dist_gir_DATA) \
				$(typelib_DATA)

endif


mkenums_command=		$(AM_V_GEN)$(GLIB_MKENUMS) \
				--template
genmarshal_command=		$(AM_V_GEN)$(GLIB_GENMARSHAL) \
				--nostdinc --internal --prefix=adg_marshal


adg-type-builtins.c: $(srcdir)/adg-type-builtins.c.mkenums $(srcdir)/adg-enums.h
	$(mkenums_command) $(srcdir)/adg-type-builtins.c.mkenums $(srcdir)/adg-enums.h > $@

adg-type-builtins.h: $(srcdir)/adg-type-builtins.h.mkenums $(srcdir)/adg-enums.h
	$(mkenums_command) $(srcdir)/adg-type-builtins.h.mkenums $(srcdir)/adg-enums.h > $@

adg-marshal.c: $(srcdir)/adg-marshal.genmarshal
	@echo '#include <glib-object.h>' > $@ ;
	$(genmarshal_command) --body $(srcdir)/adg-marshal.genmarshal >> $@

adg-marshal.h: $(srcdir)/adg-marshal.genmarshal
	$(genmarshal_command) --header $(srcdir)/adg-marshal.genmarshal > $@


coverage:
	$(GCOV) $(srcdir)/*.c -s $(srcdir) -o .libs

# Possibly remove files created by 'make coverage'
mostlyclean-local:
	-rm -f *.gcov

.PHONY: coverage
