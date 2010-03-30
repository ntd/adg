include $(top_srcdir)/build/Makefile.am.common

DOC_MODULE=		cpml
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/src/cpml
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--ignore-decorators="CPML_GNUC_CONST" \
			--rebuild-sections
MKDB_OPTIONS=		--sgml-mode \
			--type-name-mangling \
			--output-format=xml
MKHTML_OPTIONS=		--path=$(abs_builddir)
FIXXREF_OPTIONS=
HFILE_GLOB=		$(top_srcdir)/src/cpml/*.h
CFILE_GLOB=		$(top_srcdir)/src/cpml/*.c
IGNORE_HFILES=		cpml.h \
			cpml-alloca.h \
			cpml-internal.h \
			cpml-primitive-private.h
HTML_IMAGES=
content_files=
expand_content_files=

include $(top_srcdir)/build/Makefile.am.gtk-doc

DISTCLEANFILES=		$(DOC_MODULE)-sections.txt
