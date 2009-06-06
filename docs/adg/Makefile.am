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
IGNORE_HFILES=
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

TESTS=			($GTKDOC_CHECK)
