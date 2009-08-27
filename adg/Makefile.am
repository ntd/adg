AM_CFLAGS=			-I$(top_srcdir) \
				-I$(top_builddir)
AM_LDFLAGS=
GLIB_MKENUMS=			`which glib-mkenums` \
					--template $(srcdir)/$@.template \
					$(srcdir)/adg-enums.h
PATTERN=

# file groups
h_sources=			adg-adim.h \
				adg-arrow-style.h \
				adg-canvas.h \
				adg-container.h \
				adg-context.h \
				adg-dim.h \
				adg-dim-style.h \
				adg-entity.h \
				adg-font-style.h \
				adg-ldim.h \
				adg-line-style.h \
				adg-marker.h \
				adg-matrix.h \
				adg-model.h \
				adg-pair.h \
				adg-path.h \
				adg-pattern.h \
				adg-primitive.h \
				adg-segment.h \
				adg-stroke.h \
				adg-style.h \
				adg-title-block.h \
				adg-toy-text.h \
				adg-util.h
built_h_sources=		$(builddir)/adg-type-builtins.h \
				$(builddir)/adg.h
private_h_sources=		adg-adim-private.h \
				adg-arrow-style-private.h \
				adg-canvas-private.h \
				adg-container-private.h \
				adg-context-private.h \
				adg-dim-private.h \
				adg-dim-style-private.h \
				adg-entity-private.h \
				adg-enums.h \
				adg-font-style-private.h \
				adg-intl.h \
				adg-ldim-private.h \
				adg-line-style-private.h \
				adg-marker-private.h \
				adg-model-private.h \
				adg-path-private.h \
				adg-stroke-private.h \
				adg-style-private.h \
				adg-title-block-private.h \
				adg-toy-text-private.h
built_private_h_sources= 
c_sources=			adg-adim.c \
				adg-arrow-style.c \
				adg-canvas.c \
				adg-container.c \
				adg-context.c \
				adg-dim.c \
				adg-dim-style.c \
				adg-entity.c \
				adg-enums.c \
				adg-font-style.c \
				adg-ldim.c \
				adg-line-style.c \
				adg-marker.c \
				adg-matrix.c \
				adg-model.c \
				adg-pair.c \
				adg-path.c \
				adg-pattern.c \
				adg-primitive.c \
				adg-segment.c \
				adg-stroke.c \
				adg-style.c \
				adg-title-block.c \
				adg-toy-text.c \
				adg-util.c
built_c_sources=		$(builddir)/adg-type-builtins.c
EXTRA_DIST=			adg-type-builtins.c.template \
				adg-type-builtins.h.template \
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


adg-type-builtins.c: adg-type-builtins.c.template adg-enums.h
	$(GLIB_MKENUMS) > $@

adg-type-builtins.h: adg-type-builtins.h.template adg-enums.h
	$(GLIB_MKENUMS) > $@

# Regenerate the adg.h header file: forcing a dependency on config.log
# is an easy way to invalidate this file whenever a ./configure is recall
$(builddir)/adg.h: adg.h.in $(top_builddir)/config.log
	rm -f $@.tmp
	sed -e '$(PATTERN)' $< > $@.tmp
	mv $@.tmp $@
