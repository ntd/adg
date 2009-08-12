## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS=	1.6
DOC_MODULE=		cpml
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/cpml
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--ignore-decorators="CPML_GNUC_CONST"
MKDB_OPTIONS=		--sgml-mode --output-format=xml
MKHTML_OPTIONS=		--path=$(abs_builddir)
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/cpml/*.h
CFILE_GLOB=		$(top_srcdir)/cpml/*.c
IGNORE_HFILES=		cpml-alloca.h
HTML_IMAGES=
CONTENT_FILES=
EXPAND_CONTENT_FILES=

include $(top_srcdir)/gtk-doc.make
