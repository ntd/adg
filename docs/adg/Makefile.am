include $(top_srcdir)/build/Makefile.am.common

GTKDOC_CFLAGS=		@ADG_CFLAGS@ \
			-I$(top_srcdir) \
			-I$(top_builddir)
GTKDOC_LIBS=		@ADG_LIBS@ \
			$(top_builddir)/src/adg/libadg.la

DOC_MODULE=		adg
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/src/adg
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--rebuild-types \
			--rebuild-sections
MKDB_OPTIONS=		--sgml-mode \
			--type-name-mangling \
			--output-format=xml
MKHTML_OPTIONS=		--path=$(abs_builddir)
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/src/adg/*.h
CFILE_GLOB=		$(top_srcdir)/src/adg/*.c
IGNORE_HFILES=		adg.h \
			adg-internal.h \
			adg-type-builtins.h \
			adg-marshal.h \
			adg-adim-private.h \
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
			adg-title-block-private.h \
			adg-toy-text-private.h \
			adg-trail-private.h \
			adg-widget-private.h \
			test-internal.h
HTML_IMAGES=
content_files=		$(top_srcdir)/docs/gnu/README.xml \
			$(top_srcdir)/docs/gnu/NEWS.xml \
			$(top_srcdir)/docs/gnu/TODO.xml \
			$(top_srcdir)/docs/gnu/CONTRIBUTING.xml
expand_content_files=

include $(top_srcdir)/build/Makefile.am.gtk-doc

DISTCLEANFILES=		$(DOC_MODULE).types \
			$(DOC_MODULE)-sections.txt
