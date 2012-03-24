include $(top_srcdir)/build/Makefile.am.common

DOC_MODULE=		cpml
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/src/cpml
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--ignore-decorators="CPML_GNUC_CONST" \
			--rebuild-sections
MKDB_OPTIONS=		--sgml-mode \
			--output-format=xml
MKHTML_OPTIONS=
HTML_OPTIONS=		--stringparam chunker.output.doctype-public '-//W3C//DTD XHTML 1.0 Strict//EN' \
			--stringparam chunker.output.doctype-system 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/src/cpml/*.h
CFILE_GLOB=		$(top_srcdir)/src/cpml/*.c
IGNORE_HFILES=		cpml.h \
			cpml-internal.h \
			cpml-introspection.h \
			cpml-primitive-private.h \
			test-internal.h
HTML_IMAGES=
content_files=
expand_content_files=

include $(top_srcdir)/build/Makefile.am.gtk-doc

DISTCLEANFILES=		$(DOC_MODULE)-sections.txt
