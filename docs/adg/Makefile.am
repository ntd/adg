## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS=	1.6
DOC_MODULE=		adg
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/adg
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--rebuild-types \
			--rebuild-sections
MKDB_OPTIONS=		--sgml-mode \
			--type-name-mangling \
			--output-format=xml
MKHTML_OPTIONS=		--path=$(abs_builddir)
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/adg/*.h \
			$(top_builddir)/adg/*.h
CFILE_GLOB=		$(top_srcdir)/adg/*.c \
			$(top_builddir)/adg/*.c
IGNORE_HFILES=		adg.h \
			adg-intl.h \
			adg-type-builtins.h \
			adg-adim-private.h \
			adg-arrow-style-private.h \
			adg-canvas-private.h \
			adg-container-private.h \
			adg-context-private.h \
			adg-dim-private.h \
			adg-dim-style-private.h \
			adg-entity-private.h \
			adg-font-style-private.h \
			adg-ldim-private.h \
			adg-line-style-private.h \
			adg-marker-private.h \
			adg-model-private.h \
			adg-path-private.h \
			adg-stroke-private.h \
			adg-style-private.h \
			adg-title-block-private.h \
			adg-toy-text-private.h \
			adg-widget-private.h
HTML_IMAGES=
content_files=		$(top_srcdir)/docs/gnu/README.xml \
			$(top_srcdir)/docs/gnu/NEWS.xml \
			$(top_srcdir)/docs/gnu/TODO.xml
expand_content_files=
AM_CPPFLAGS=		-I$(top_srcdir) \
			@GTK2_CFLAGS@
GTKDOC_LIBS=		$(top_builddir)/adg/libadg.la \
			@GTK2_LIBS@

include $(top_srcdir)/gtk-doc.make

DISTCLEANFILES=		$(DOC_MODULE).types \
			$(DOC_MODULE)-sections.txt
