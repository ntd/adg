include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=			@CPML_CFLAGS@ \
				-I$(top_srcdir)
AM_LDFLAGS=			@CPML_LIBS@


# file groups
cpml_h_sources=			cpml.h \
				cpml-arc.h \
				cpml-curve.h \
				cpml-extents.h \
				cpml-line.h \
				cpml-close.h \
				cpml-pair.h \
				cpml-primitive.h \
				cpml-segment.h \
				cpml-util.h
cpml_built_h_sources=
cpml_private_h_sources=		cpml-alloca.h \
				cpml-internal.h \
				cpml-primitive-private.h
cpml_built_private_h_sources=
cpml_c_sources=			cpml-arc.c \
				cpml-curve.c \
				cpml-extents.c \
				cpml-line.c \
				cpml-pair.c \
				cpml-primitive.c \
				cpml-segment.c \
				cpml-util.c
cpml_built_c_sources=
cpml_pkgconfig_file=		cpml.pc

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
pkgconfig_DATA=			$(cpml_pkgconfig_file)
EXTRA_DIST=			$(cpml_pkgconfig_file).in
