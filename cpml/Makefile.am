## Process this file with automake to produce Makefile.in

AM_CFLAGS=			@CAIRO_CFLAGS@ \
				-I$(top_srcdir)
AM_LDFLAGS=			@CAIRO_LIBS@


# file groups
cpml_h_sources=			cpml.h \
				cpml-action.h \
				cpml-close.h \
				cpml-curve.h \
				cpml-line.h \
				cpml-macros.h \
				cpml-pair.h \
				cpml-primitive.h \
				cpml-segment.h
cpml_built_h_sources=
cpml_private_h_sources=		cpml-alloca.h
cpml_built_private_h_sources=
cpml_c_sources=			cpml-action.c \
				cpml-action-intersection.c \
				cpml-action-join.c \
				cpml-close.c \
				cpml-curve.c \
				cpml-line.c \
				cpml-pair.c \
				cpml-primitive.c \
				cpml-segment.c
cpml_built_c_sources=

# targets
BUILT_SOURCES=			$(cpml_built_h_sources) \
				$(cpml_built_private_h_sources) \
				$(cpml_built_c_sources)

libcpml_includedir=		$(includedir)/cpml
libcpml_include_DATA=		$(cpml_h_sources) \
				$(cpml_built_h_sources)

lib_LTLIBRARIES= 		libcpml.la
libcpml_la_LDFLAGS=		-release @PACKAGE_VERSION@
libcpml_la_LIBADD=		$(LTALLOCA)
libcpml_la_SOURCES=		$(cpml_h_sources) \
				$(cpml_built_h_sources) \
				$(cpml_private_h_sources) \
				$(cpml_built_private_h_sources) \
				$(cpml_c_sources) \
				$(cpml_built_c_sources)
