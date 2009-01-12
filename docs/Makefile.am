## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS=	1.6
DOC_MODULE=		adg
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(top_srcdir)/adg
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=
MKDB_OPTIONS=		--sgml-mode --output-format=xml
MKTMPL_OPTIONS=
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/adg/*.h
CFILE_GLOB=		$(top_srcdir)/adg/*.c
IGNORE_HFILES=		adg-adim-private.h \
			adg-canvas-private.h \
			adg-container-private.h \
			adg-context-private.h \
			adg-dim-private.h \
			adg-entity-private.h \
			adg-intl.h \
			adg-ldim-private.h \
			adg-model-private.h \
			adg-path-private.h \
			adg-type-builtins.h
HTML_IMAGES=
CONTENT_FILES=
EXPAND_CONTENT_FILES=
AM_CPPFLAGS=		-I$(top_srcdir) \
			@GTK2_CFLAGS@
GTKDOC_LIBS=		$(top_builddir)/adg/libadg.la \
			@GTK2_LIBS@

include $(top_srcdir)/gtk-doc.make

EXTRA_DIST +=		gnu/docbook2txt.xsl \
			gnu/README.xml \
			gnu/NEWS.xml \
			gnu/TODO.xml
