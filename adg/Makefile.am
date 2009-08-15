AM_CFLAGS=			@GTK2_CFLAGS@ \
				-I$(top_srcdir)
AM_LDFLAGS=			@GTK2_LIBS@
GLIB_MKENUMS=			`which glib-mkenums` \
					--template $(srcdir)/$@.template \
					$(srcdir)/adg-enums.h

# file groups
h_sources=			adg.h \
				adg-adim.h \
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
				adg-util.h \
				adg-widget.h
built_h_sources=		$(builddir)/adg-type-builtins.h
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
				adg-model-private.h \
				adg-path-private.h \
				adg-stroke-private.h \
				adg-style-private.h \
				adg-title-block-private.h \
				adg-toy-text-private.h \
				adg-widget-private.h
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
				adg-util.c \
				adg-widget.c
built_c_sources=		$(builddir)/adg-type-builtins.c
EXTRA_DIST=			adg-type-builtins.c.template \
				adg-type-builtins.h.template

# targets
BUILT_SOURCES=			$(built_h_sources)
DISTCLEANFILES=			$(built_h_sources) \
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
