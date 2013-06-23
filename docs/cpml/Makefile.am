include $(top_srcdir)/build/Makefile.am.common

DOC_MODULE=		cpml
DOC_MAIN_SGML_FILE=	$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=		$(abs_top_srcdir)/src/cpml
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=		--ignore-decorators="CPML_GNUC_CONST" \
			--rebuild-sections
MKDB_OPTIONS=		--name-space=cpml \
			--sgml-mode \
			--output-format=xml \
			--default-includes="$(DOC_MODULE)-1/$(DOC_MODULE).h"
MKHTML_OPTIONS=
FIXXREF_OPTIONS=	$(EXTRA_DIR)
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


# Use an hand-crafted cpml.hierarchy file instead of an empty one.
# By default gtk-doc does not allow to have a file name that matches
# the type name to avoid id conflicts between the main section and
# the struct subsection: this is only allowed for GBoxed and GObject
# types.
# I *absolutely* want to have file names that match the type names,
# so using a fake cpml.hierarchy let gtk-doc treat the plain C
# structs of CPML as GBoxed types.

EXTRA_DIST+=		cpml.hierarchy.fake

scan-hook:
	$(AM_V_at)test -f cpml.hierarchy || \
	    cp $(abs_srcdir)/cpml.hierarchy.fake cpml.hierarchy
