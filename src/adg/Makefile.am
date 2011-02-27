include $(top_srcdir)/build/Makefile.am.common

SUBDIRS=			tests


# file groups
h_sources=			adg-adim.h \
				adg-alignment.h \
				adg-arrow.h \
				adg-canvas.h \
				adg-color-style.h \
				adg-container.h \
				adg-dim.h \
				adg-dim-style.h \
				adg-dress.h \
				adg-dress-builtins.h \
				adg-edges.h \
				adg-entity.h \
				adg-enums.h \
				adg-fill-style.h \
				adg-font-style.h \
				adg-hatch.h \
				adg-ldim.h \
				adg-line-style.h \
				adg-logo.h \
				adg-marker.h \
				adg-matrix.h \
				adg-model.h \
				adg-pair.h \
				adg-path.h \
				adg-pattern.h \
				adg-point.h \
				adg-primitive.h \
				adg-projection.h \
				adg-rdim.h \
				adg-ruled-fill.h \
				adg-segment.h \
				adg-stroke.h \
				adg-style.h \
				adg-table.h \
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
c_sources=			adg-adim.c \
				adg-alignment.c \
				adg-arrow.c \
				adg-canvas.c \
				adg-color-style.c \
				adg-container.c \
				adg-dim.c \
				adg-dim-style.c \
				adg-dress.c \
				adg-dress-builtins.c \
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
				adg-pair.c \
				adg-path.c \
				adg-pattern.c \
				adg-point.c \
				adg-primitive.c \
				adg-projection.c \
				adg-rdim.c \
				adg-ruled-fill.c \
				adg-segment.c \
				adg-stroke.c \
				adg-style.c \
				adg-table.c \
				adg-table-style.c \
				adg-textual.c \
				adg-title-block.c \
				adg-trail.c \
				adg-toy-text.c \
				adg-utils.c
built_c_sources=		adg-type-builtins.c \
				adg-marshal.c
template_sources=		adg-type-builtins.c.mkenums \
				adg-type-builtins.h.mkenums \
				adg-marshal.genmarshal


# targets
EXTRA_DIST=			$(template_sources)
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources)
#				built_c_sources are properly tracked
#				by nodist_libadg_1_la_SOURCES
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

adg_includedir=			$(includedir)/adg-1/adg
adg_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libadg-1.la
libadg_1_la_SOURCES=		$(h_sources) \
				$(private_h_sources) \
				$(c_sources)
nodist_libadg_1_la_SOURCES=	$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
libadg_1_la_CFLAGS=		$(CAIRO_CFLAGS) \
				$(GOBJECT2_CFLAGS) \
				-I$(top_srcdir)/src \
				-I$(top_builddir)/src
libadg_1_la_LDFLAGS=		-no-undefined
libadg_1_la_LIBADD=		$(GOBJECT2_LIBS) \
				$(top_builddir)/src/cpml/libcpml-1.la


if HAVE_PANGO
h_sources+=			adg-pango-style.h \
				adg-text.h
private_h_sources+=		adg-pango-style-private.h \
				adg-text-private.h
c_sources+=			adg-pango-style.c \
				adg-text.c
libadg_1_la_CFLAGS+=		$(PANGO_CFLAGS)
libadg_1_la_LIBADD+=		$(PANGO_LIBS)
endif


mkenums_command=		$(MSG_GEN) ; \
				$(GLIB_MKENUMS) --template
genmarshal_command=		$(MSG_GEN) ; \
				$(GLIB_GENMARSHAL) --nostdinc --internal --prefix=adg_marshal

adg-type-builtins.c: $(srcdir)/adg-type-builtins.c.mkenums $(srcdir)/adg-enums.h
	$(mkenums_command) $(srcdir)/adg-type-builtins.c.mkenums $(srcdir)/adg-enums.h > $@

adg-type-builtins.h: $(srcdir)/adg-type-builtins.h.mkenums $(srcdir)/adg-enums.h
	$(mkenums_command) $(srcdir)/adg-type-builtins.h.mkenums $(srcdir)/adg-enums.h > $@

adg-marshal.c: $(srcdir)/adg-marshal.genmarshal
	echo '#include <glib-object.h>' > $@ ;
	$(genmarshal_command) --body $(srcdir)/adg-marshal.genmarshal >> $@

adg-marshal.h: $(srcdir)/adg-marshal.genmarshal
	$(genmarshal_command) --header $(srcdir)/adg-marshal.genmarshal > $@
