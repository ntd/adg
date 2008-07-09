## Process this file with automake to produce Makefile.in

AM_CFLAGS=			@GCONTAINER_CFLAGS@ \
				@GTK2_CFLAGS@ \
				-I$(top_srcdir)
AM_LDFLAGS=			@GCONTAINER_LIBS@ \
				@GTK2_LIBS@

# file groups
adg_h_sources=			adg.h \
				adg-adim.h \
				adg-arrow-style.h \
				adg-canvas.h \
				adg-container.h \
				adg-context.h \
				adg-dim.h \
				adg-dim-style.h \
				adg-entity.h \
				adg-font-style.h \
				adg-ldim.h \
				adg-line-style.h \
				adg-matrix.h \
				adg-model.h \
				adg-pair.h \
				adg-path.h \
				adg-pattern.h \
				adg-style.h \
				adg-util.h
adg_built_h_sources=		adg-type-builtins.h
adg_private_h_sources=		adg-adim-private.h \
				adg-arrow-style-private.h \
				adg-canvas-private.h \
				adg-container-private.h \
				adg-context-private.h \
				adg-dim-private.h \
				adg-dim-style-private.h \
				adg-entity-private.h \
				adg-enums.h \
				adg-font-style-private.h \
				adg-intl.h \
				adg-ldim-private.h \
				adg-line-style-private.h \
				adg-model-private.h \
				adg-path-private.h \
				adg-style-private.h
adg_built_private_h_sources= 
adg_c_sources=			adg-adim.c \
				adg-arrow-style.c \
				adg-canvas.c \
				adg-container.c \
				adg-context.c \
				adg-dim.c \
				adg-dim-style.c \
				adg-entity.c \
				adg-enums.c \
				adg-font-style.c \
				adg-ldim.c \
				adg-line-style.c \
				adg-matrix.c \
				adg-model.c \
				adg-pair.c \
				adg-path.c \
				adg-pattern.c \
				adg-style.c \
				adg-util.c
adg_built_c_sources=		adg-type-builtins.c

# targets
BUILT_SOURCES=			$(adg_built_h_sources) \
				$(adg_built_private_h_sources) \
				$(adg_built_c_sources)

libadg_includedir=		$(includedir)/adg
libadg_include_DATA=		$(adg_h_sources) \
				$(adg_built_h_sources)

lib_LTLIBRARIES= 		libadg.la
libadg_la_LDFLAGS=		-release @PACKAGE_VERSION@
libadg_la_SOURCES=		$(adg_h_sources) \
				$(adg_built_h_sources) \
				$(adg_private_h_sources) \
				$(adg_built_private_h_sources) \
				$(adg_c_sources) \
				$(adg_built_c_sources)
libadg_la_LIBADD=		$(top_builddir)/cpml/libcpml.la
libadg_la_DEPENDENCIES=		$(top_builddir)/cpml/libcpml.la


adg-type-builtins.h: adg-enums.h Makefile
	glib-mkenums \
	--fhead "#ifndef __ADG_TYPE_BUILTINS_H__\n#define __ADG_TYPE_BUILTINS_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
	--fprod "/* enumerations from \"@filename@\" */\n" \
	--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define ADG_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
	--ftail "G_END_DECLS\n\n#endif /* __ADG_TYPE_BUILTINS_H__ */" \
	adg-enums.h > $@

adg-type-builtins.c: adg-enums.h Makefile
	glib-mkenums \
	--fhead "#include \"adg-enums.h\"\n\n" \
	--fprod "\n/* enumerations from \"@filename@\" */" \
	--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if G_UNLIKELY (etype == 0)\n    {\n      static const G@Type@Value values[] =\n        {" \
	--vprod "          { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
	--vtail "          { 0, NULL, NULL }\n        };\n      etype = g_@type@_register_static (\"@EnumName@\", values);\n    }\n  return etype;\n}\n" \
	adg-enums.h > $@
