include $(top_srcdir)/build/Makefile.am.common


AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_srcdir)/src/tests \
				-I$(top_builddir)/src \
				-DSRCDIR=\"$(srcdir)\"
AM_CFLAGS=			$(CAIRO_CFLAGS) \
				$(GOBJECT_CFLAGS)
LDADD=				$(top_builddir)/src/adg/libadg-1.la \
				$(top_builddir)/src/cpml/libcpml-1.la


if HAVE_TEST_FRAMEWORK

LDADD+=				$(top_builddir)/src/tests/libadgtest.la

TEST_PROGS+=			test-utils$(EXEEXT)
test_utils_SOURCES=		test-utils.c

TEST_PROGS+=			test-model$(EXEEXT)
test_model_SOURCES=		test-model.c

TEST_PROGS+=			test-point$(EXEEXT)
test_point_SOURCES=		test-point.c

TEST_PROGS+=			test-trail$(EXEEXT)
test_trail_SOURCES=		test-trail.c

TEST_PROGS+=			test-edges$(EXEEXT)
test_edges_SOURCES=		test-edges.c

TEST_PROGS+=			test-entity$(EXEEXT)
test_entity_SOURCES=		test-entity.c

TEST_PROGS+=			test-container$(EXEEXT)
test_container_SOURCES=		test-container.c

TEST_PROGS+=			test-alignment$(EXEEXT)
test_alignment_SOURCES=		test-alignment.c

TEST_PROGS+=			test-stroke$(EXEEXT)
test_stroke_SOURCES=		test-stroke.c

TEST_PROGS+=			test-hatch$(EXEEXT)
test_hatch_SOURCES=		test-hatch.c

TEST_PROGS+=			test-toy-text$(EXEEXT)
test_toy_text_SOURCES=		test-toy-text.c

TEST_PROGS+=			test-table$(EXEEXT)
test_table_SOURCES=		test-table.c

TEST_PROGS+=			test-logo$(EXEEXT)
test_logo_SOURCES=		test-logo.c

TEST_PROGS+=			test-projection$(EXEEXT)
test_projection_SOURCES=	test-projection.c

TEST_PROGS+=			test-title-block$(EXEEXT)
test_title_block_SOURCES=	test-title-block.c

TEST_PROGS+=			test-dim$(EXEEXT)
test_dim_SOURCES=		test-dim.c

TEST_PROGS+=			test-ldim$(EXEEXT)
test_ldim_SOURCES=		test-ldim.c

TEST_PROGS+=			test-rdim$(EXEEXT)
test_rdim_SOURCES=		test-rdim.c

TEST_PROGS+=			test-adim$(EXEEXT)
test_adim_SOURCES=		test-adim.c

TEST_PROGS+=			test-marker$(EXEEXT)
test_marker_SOURCES=		test-marker.c

TEST_PROGS+=			test-arrow$(EXEEXT)
test_arrow_SOURCES=		test-arrow.c

TEST_PROGS+=			test-dash$(EXEEXT)
test_dash_SOURCES=		test-dash.c

TEST_PROGS+=			test-color-style$(EXEEXT)
test_color_style_SOURCES=	test-color-style.c

TEST_PROGS+=			test-line-style$(EXEEXT)
test_line_style_SOURCES=	test-line-style.c

TEST_PROGS+=			test-fill-style$(EXEEXT)
test_fill_style_SOURCES=	test-fill-style.c

TEST_PROGS+=			test-ruled-fill$(EXEEXT)
test_ruled_fill_SOURCES=	test-ruled-fill.c

TEST_PROGS+=			test-font-style$(EXEEXT)
test_font_style_SOURCES=	test-font-style.c

TEST_PROGS+=			test-dim-style$(EXEEXT)
test_dim_style_SOURCES=		test-dim-style.c

TEST_PROGS+=			test-table-style$(EXEEXT)
test_table_style_SOURCES=	test-table-style.c

TEST_PROGS+=			test-dress$(EXEEXT)
test_dress_SOURCES=		test-dress.c

TEST_PROGS+=			test-canvas$(EXEEXT)
test_canvas_SOURCES=		test-canvas.c

if HAVE_PANGO

TEST_PROGS+=			test-text$(EXEEXT)
test_text_SOURCES=		test-text.c
AM_CFLAGS+=			$(PANGO_CFLAGS)

endif

if HAVE_GTK

TEST_PROGS+=			test-gtk-area$(EXEEXT)
test_gtk_area_SOURCES=		test-gtk-area.c

TEST_PROGS+=			test-gtk-layout$(EXEEXT)
test_gtk_layout_SOURCES=	test-gtk-layout.c

TEST_PROGS+=			test-gtk-utils$(EXEEXT)
test_gtk_utils_SOURCES=		test-gtk-utils.c

AM_CFLAGS+=			$(GTK3_CFLAGS) \
				$(GTK2_CFLAGS)
LDADD+=				$(GTK3_LIBS) \
				$(GTK2_LIBS)

endif

endif


LDADD+=				$(CAIRO_LIBS) \
				$(GOBJECT_LIBS)


# targets
check_PROGRAMS=			$(TEST_PROGS)


# Possibly remove files created on test coverage builds
mostlyclean-local:
	-rm -f *.gcda *gcno
