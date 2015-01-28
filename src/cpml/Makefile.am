include $(top_srcdir)/build/Makefile.am.common


if HAVE_TEST_FRAMEWORK
SUBDIRS=			tests
endif


AM_CFLAGS=			$(CPML_CFLAGS)
AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src


# file groups
h_sources=			cpml-arc.h \
				cpml-curve.h \
				cpml-extents.h \
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
EXTRA_DIST=			cpml-introspection.h \
				cpml-1.pc.in

# targets
BUILT_SOURCES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
CLEANFILES=			$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)

pkgconfig_DATA=			cpml-1.pc

cpml_includedir=		$(includedir)/adg-1/cpml
cpml_include_DATA=		$(h_sources) \
				$(built_h_sources)

lib_LTLIBRARIES= 		libcpml-1.la
libcpml_1_la_SOURCES=		$(h_sources) \
				$(private_h_sources) \
				$(c_sources)
nodist_libcpml_1_la_SOURCES=	$(built_h_sources) \
				$(built_private_h_sources) \
				$(built_c_sources)
libcpml_1_la_LDFLAGS=		-no-undefined \
				-version-info $(CPML_LT_VERSION)
libcpml_1_la_LIBADD=		$(CPML_LIBS)


## GObject introspection

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS=
INTROSPECTION_SCANNER_ARGS=
INTROSPECTION_COMPILER_ARGS=

if HAVE_INTROSPECTION

Cpml-1.0.gir: libcpml-1.la

INTROSPECTION_GIRS+=		Cpml-1.0.gir
Cpml_1_0_gir_LIBS=		libcpml-1.la
Cpml_1_0_gir_INCLUDES=		cairo-1.0
Cpml_1_0_gir_FILES=		cpml-introspection.h \
				$(h_sources) \
				$(c_sources)
Cpml_1_0_gir_SCANNERFLAGS=	$(AM_CPPFLAGS) \
				--c-include="cpml.h" \
				--warn-all
girdir=				$(GIRDIR)
dist_gir_DATA=			$(INTROSPECTION_GIRS)
typelibdir=			$(TYPELIBDIR)
typelib_DATA=			$(INTROSPECTION_GIRS:.gir=.typelib)
CLEANFILES+=			$(dist_gir_DATA) \
				$(typelib_DATA)

endif


# Although actually GObject wrappers are always compiled,
# this feature can be easily made optional if needed.

h_sources+=			cpml-gobject.h
c_sources+=			cpml-gobject.c


coverage:
	$(GCOV) $(srcdir)/*.c -s $(srcdir) -o .libs

# Possibly remove files created by 'make coverage'
mostlyclean-local:
	-rm -f *.gcov

.PHONY: coverage
