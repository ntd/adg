MSG_GEN=			echo '  GEN   '$@;
AM_CFLAGS=			-I$(top_srcdir) \
				-I$(top_builddir)
AM_LDFLAGS=
PATTERN= # file groups
h_sources=			adg-adim.h \
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
				adg-fill-style.h \
				adg-font-style.h \
				adg-hatch.h \
				adg-ldim.h \
				adg-line-style.h \
				adg-marker.h \
				adg-matrix.h \
				adg-model.h \
				adg-pair.h \
				adg-path.h \
				adg-pattern.h \
				adg-point.h \
				adg-primitive.h \
				adg-rdim.h \
				adg-ruled-fill.h \
				adg-segment.h \
				adg-stroke.h \
				adg-style.h \
				adg-table.h \
				adg-table-style.h \
				adg-title-block.h \
				adg-trail.h \
				adg-toy-text.h \
				adg-util.h
built_h_sources=		$(builddir)/adg-type-builtins.h \
				$(builddir)/adg-marshal.h \
				$(builddir)/adg.h
private_h_sources=		adg-adim-private.h \
				adg-arrow-private.h \
				adg-canvas-private.h \
				adg-color-style-private.h \
				adg-container-private.h \
				adg-dim-private.h \
				adg-dim-style-private.h \
				adg-dress-private.h \
				adg-edges-private.h \
				adg-entity-private.h \
				adg-enums.h \
				adg-fill-style-private.h \
				adg-font-style-private.h \
				adg-hatch-private.h \
				adg-intl.h \
				adg-ldim-private.h \
				adg-line-style-private.h \
				adg-marker-private.h \
				adg-model-private.h \
				adg-path-private.h \
				adg-rdim-private.h \
				adg-ruled-fill-private.h \
				adg-stroke-private.h \
				adg-table-private.h \
				adg-table-style-private.h \
				adg-title-block-private.h \
				adg-trail-private.h \
				adg-toy-text-private.h
built_private_h_sources= 
c_sources=			adg-adim.c \
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
				adg-marker.c \
				adg-matrix.c \
				adg-model.c \
				adg-pair.c \
				adg-path.c \
				adg-pattern.c \
				adg-point.c \
				adg-primitive.c \
				adg-rdim.c \
				adg-ruled-fill.c \
				adg-segment.c \
				adg-stroke.c \
				adg-style.c \
				adg-table.c \
				adg-table-style.c \
				adg-title-block.c \
				adg-trail.c \
				adg-toy-text.c \
				adg-util.c
built_c_sources=		$(builddir)/adg-type-builtins.c \
				$(builddir)/adg-marshal.c
EXTRA_DIST=			adg-type-builtins.c.mkenums \
				adg-type-builtins.h.mkenums \
				adg-marshal.genmarshal \
				adg.h.in
if HAVE_GTK2
AM_CFLAGS+=			@GTK2_CFLAGS@
AM_LDFLAGS+=			@GTK2_LIBS@
h_sources+=			adg-widget.h
private_h_sources+=		adg-widget-private.h
c_sources+=			adg-widget.c
PATTERN+=			/\/\* if GTK2 \*\//d;/\/\* endif GTK2 \*\//d;

else

AM_CFLAGS+=			@GOBJECT2_CFLAGS@ \
				@CAIRO_CFLAGS@
AM_LDFLAGS+=			@GOBJECT2_LIBS@ \
				@GTK2_LIBS@
PATTERN+=			/\/\* if GTK2 \*\//,/\/\* endif GTK2 \*\//d;

endif


# targets
BUILT_SOURCES=			$(built_h_sources)
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

libadg_includedir=		$(includedir)/adg
libadg_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libadg.la
libadg_la_LDFLAGS=		-release @PACKAGE_VERSION@
libadg_la_SOURCES=		$(h_sources) \
				$(private_h_sources) \
				$(c_sources)
nodist_libadg_la_SOURCES=	$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
libadg_la_LIBADD=		$(top_builddir)/cpml/libcpml.la
libadg_la_DEPENDENCIES=		$(top_builddir)/cpml/libcpml.la


adg-type-builtins.c: adg-type-builtins.c.mkenums adg-enums.h
	$(MSG_GEN)$(GLIB_MKENUMS) --template $^ > $@

adg-type-builtins.h: adg-type-builtins.h.mkenums adg-enums.h
	$(MSG_GEN)$(GLIB_MKENUMS) --template $^ > $@

adg-marshal.c: adg-marshal.genmarshal
	$(MSG_GEN)$(GLIB_GENMARSHAL) --body --prefix=adg_marshal $< > $@

adg-marshal.h: adg-marshal.genmarshal
	$(MSG_GEN)$(GLIB_GENMARSHAL) --header --prefix=adg_marshal $< > $@

# Regenerate the adg.h header file: forcing a dependency on config.log
# is an easy way to invalidate this file whenever a ./configure is recall
$(builddir)/adg.h: adg.h.in $(top_builddir)/config.log
	$(MSG_GEN)rm -f $@.tmp
	sed -e '$(PATTERN)' $< > $@.tmp
	mv $@.tmp $@
