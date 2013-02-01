include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src
AM_CFLAGS=			$(CAIRO_CFLAGS) \
				$(GOBJECT2_CFLAGS)
LDADD=				$(top_builddir)/src/adg/libadg-1.la


# file groups
test_internals=			test-internal.c \
				test-internal.h

if HAVE_TEST_FRAMEWORK

TEST_PROGS+=			test-utils$(EXEEXT)
test_utils_SOURCES=		test-utils.c \
				$(test_internals)

TEST_PROGS+=			test-model$(EXEEXT)
test_model_SOURCES=		test-model.c \
				$(test_internals)

TEST_PROGS+=			test-trail$(EXEEXT)
test_trail_SOURCES=		test-trail.c \
				$(test_internals)

TEST_PROGS+=			test-edges$(EXEEXT)
test_edges_SOURCES=		test-edges.c \
				$(test_internals)

TEST_PROGS+=			test-entity$(EXEEXT)
test_entity_SOURCES=		test-entity.c \
				$(test_internals)

TEST_PROGS+=			test-container$(EXEEXT)
test_container_SOURCES=		test-container.c \
				$(test_internals)

TEST_PROGS+=			test-alignment$(EXEEXT)
test_alignment_SOURCES=		test-alignment.c \
				$(test_internals)

TEST_PROGS+=			test-stroke$(EXEEXT)
test_stroke_SOURCES=		test-stroke.c \
				$(test_internals)

TEST_PROGS+=			test-hatch$(EXEEXT)
test_hatch_SOURCES=		test-hatch.c \
				$(test_internals)

TEST_PROGS+=			test-toy-text$(EXEEXT)
test_toy_text_SOURCES=		test-toy-text.c \
				$(test_internals)

TEST_PROGS+=			test-table$(EXEEXT)
test_table_SOURCES=		test-table.c \
				$(test_internals)

TEST_PROGS+=			test-logo$(EXEEXT)
test_logo_SOURCES=		test-logo.c \
				$(test_internals)

TEST_PROGS+=			test-projection$(EXEEXT)
test_projection_SOURCES=	test-projection.c \
				$(test_internals)

TEST_PROGS+=			test-title-block$(EXEEXT)
test_title_block_SOURCES=	test-title-block.c \
				$(test_internals)

TEST_PROGS+=			test-dim$(EXEEXT)
test_dim_SOURCES=		test-dim.c \
				$(test_internals)

TEST_PROGS+=			test-ldim$(EXEEXT)
test_ldim_SOURCES=		test-ldim.c \
				$(test_internals)

TEST_PROGS+=			test-rdim$(EXEEXT)
test_rdim_SOURCES=		test-rdim.c \
				$(test_internals)

TEST_PROGS+=			test-adim$(EXEEXT)
test_adim_SOURCES=		test-adim.c \
				$(test_internals)

TEST_PROGS+=			test-marker$(EXEEXT)
test_marker_SOURCES=		test-marker.c \
				$(test_internals)

TEST_PROGS+=			test-arrow$(EXEEXT)
test_arrow_SOURCES=		test-arrow.c \
				$(test_internals)

TEST_PROGS+=			test-color-style$(EXEEXT)
test_color_style_SOURCES=	test-color-style.c \
				$(test_internals)

TEST_PROGS+=			test-line-style$(EXEEXT)
test_line_style_SOURCES=	test-line-style.c \
				$(test_internals)

TEST_PROGS+=			test-fill-style$(EXEEXT)
test_fill_style_SOURCES=	test-fill-style.c \
				$(test_internals)

TEST_PROGS+=			test-font-style$(EXEEXT)
test_font_style_SOURCES=	test-font-style.c \
				$(test_internals)

TEST_PROGS+=			test-dim-style$(EXEEXT)
test_dim_style_SOURCES=		test-dim-style.c \
				$(test_internals)

TEST_PROGS+=			test-table-style$(EXEEXT)
test_table_style_SOURCES=	test-table-style.c \
				$(test_internals)

TEST_PROGS+=			test-canvas$(EXEEXT)
test_canvas_SOURCES=		test-canvas.c \
				$(test_internals)

if HAVE_PANGO
TEST_PROGS+=			test-text$(EXEEXT)
test_text_SOURCES=		test-text.c \
				$(test_internals)
AM_CFLAGS+=			$(PANGO_CFLAGS)
endif

if HAVE_GTK
TEST_PROGS+=			test-gtk-area$(EXEEXT)
test_gtk_area_SOURCES=		test-gtk-area.c \
				$(test_internals)
TEST_PROGS+=			test-gtk-layout$(EXEEXT)
test_gtk_layout_SOURCES=	test-gtk-layout.c \
				$(test_internals)
AM_CFLAGS+=			$(GTK3_CFLAGS) \
				$(GTK2_CFLAGS)
endif

endif


# targets
check_PROGRAMS=			$(TEST_PROGS)
