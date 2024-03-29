include $(top_srcdir)/autotools/Makefile.am.common

DOC_MODULE=		adg
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(top_srcdir)/src/adg
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--rebuild-types \
			--rebuild-sections
MKDB_OPTIONS=		--name-space=adg \
			--sgml-mode \
			--output-format=xml \
			--default-includes="$(DOC_MODULE)-1/$(DOC_MODULE).h"
MKHTML_OPTIONS=
FIXXREF_OPTIONS=	--extra-dir=$(top_builddir)/docs/cpml/html \
			$(EXTRADIR)
HFILE_GLOB=		$(DOC_SOURCE_DIR)/*.h
EXTRA_HFILES=		$(ADG_CANVAS_H)
CFILE_GLOB=		$(DOC_SOURCE_DIR)/*.c
IGNORE_HFILES=		adg.h \
			adg-adim-private.h \
			adg-alignment-private.h \
			adg-arrow-private.h \
			adg-cairo-fallback.h \
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
			adg-forward-declarations.h \
			adg-gtk-area-private.h \
			adg-gtk-layout-private.h \
			adg-hatch-private.h \
			adg-internal.h \
			adg-introspection.h \
			adg-ldim-private.h \
			adg-line-style-private.h \
			adg-logo-private.h \
			adg-marker-private.h \
			adg-marshal.h \
			adg-matrix-fallback.h \
			adg-model-private.h \
			adg-pango-style-private.h \
			adg-path-private.h \
			adg-projection-private.h \
			adg-rdim-private.h \
			adg-ruled-fill-private.h \
			adg-stroke-private.h \
			adg-table-private.h \
			adg-table-style-private.h \
			adg-text-private.h \
			adg-title-block-private.h \
			adg-toy-text-private.h \
			adg-trail-private.h \
			adg-type-builtins.h \
			test-internal.h
HTML_IMAGES=
content_files=		CONTRIBUTING.xml \
			HACKING.xml \
			NEWS.xml \
			README.xml \
			TODO.xml
expand_content_files=

GTKDOC_CFLAGS=		-I$(top_srcdir)/src \
			$(CAIRO_CFLAGS) \
			$(CAIRO_GOBJECT_CFLAGS) \
			$(GOBJECT2_CFLAGS)
GTKDOC_LIBS=		$(top_builddir)/src/adg/libadg-1.la \
			$(top_builddir)/src/cpml/libcpml-1.la

if HAVE_GTK2
GTKDOC_CFLAGS+=		$(GTK2_CFLAGS)
GTKDOC_LIBS+=		$(GTK2_LIBS)
endif

if HAVE_PANGO
GTKDOC_CFLAGS+=		$(PANGO_CFLAGS)
GTKDOC_LIBS+=		$(PANGO_LIBS)
endif

GTKDOC_LIBS+=		$(CAIRO_LIBS) \
			$(CAIRO_GOBJECT_LIBS) \
			$(GOBJECT2_LIBS)

include $(top_srcdir)/autotools/Makefile.am.gtk-doc

DISTCLEANFILES=		$(DOC_MODULE)-sections.txt \
			$(DOC_MODULE).types

EXTRA_DIST+=		$(content_files)
