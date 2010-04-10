include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			@CPML_CFLAGS@ \
				-I$(top_srcdir)
AM_LDFLAGS=			@CPML_LIBS@


# file groups
h_sources=			cpml-arc.h \
				cpml-curve.h \
				cpml-extents.h \
				cpml-line.h \
				cpml-close.h \
				cpml-pair.h \
				cpml-primitive.h \
				cpml-segment.h \
				cpml-utils.h
built_h_sources=
private_h_sources=		cpml-internal.h \
				cpml-primitive-private.h
built_private_h_sources=
c_sources=			cpml-arc.c \
				cpml-curve.c \
				cpml-extents.c \
				cpml-line.c \
				cpml-pair.c \
				cpml-primitive.c \
				cpml-segment.c \
				cpml-utils.c
built_c_sources=

# targets
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

cpml_includedir=		$(includedir)/adg-1/cpml
cpml_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libcpml-1.la
libcpml_1_la_LDFLAGS=
libcpml_1_la_SOURCES=		$(h_sources) \
				$(built_h_sources) \
				$(private_h_sources) \
				$(built_private_h_sources) \
				$(c_sources) \
				$(built_c_sources)
