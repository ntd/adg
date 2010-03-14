include $(top_srcdir)/build/Makefile.am.common

AM_CFLAGS=		-g @ADG_CFLAGS@ \
			-I$(top_srcdir) \
			-I$(top_builddir)
AM_LDFLAGS=		@ADG_LIBS@
LDADD=			$(top_builddir)/adg/libadg.la


# file groups
test_internals=		test-internal.c \
			test-internal.h

if HAVE_TEST_FRAMEWORK

TEST_PROGS+=		test-model
test_model_SOURCES=	test-model.c \
			$(test_internals)

TEST_PROGS+=		test-edges
test_edges_SOURCES=	test-edges.c \
			$(test_internals)

TEST_PROGS+=		test-entity
test_entity_SOURCES=	test-entity.c \
			$(test_internals)

TEST_PROGS+=		test-container
test_container_SOURCES=	test-container.c \
			$(test_internals)

TEST_PROGS+=		test-alignment
test_alignment_SOURCES=	test-alignment.c \
			$(test_internals)

TEST_PROGS+=		test-stroke
test_stroke_SOURCES=	test-stroke.c \
			$(test_internals)

TEST_PROGS+=		test-hatch
test_hatch_SOURCES=	test-hatch.c \
			$(test_internals)

TEST_PROGS+=		test-toy-text
test_toy_text_SOURCES=	test-toy-text.c \
			$(test_internals)

TEST_PROGS+=		test-table
test_table_SOURCES=	test-table.c \
			$(test_internals)

TEST_PROGS+=		test-logo
test_logo_SOURCES=	test-logo.c \
			$(test_internals)

TEST_PROGS+=		test-projection
test_projection_SOURCES=test-projection.c \
			$(test_internals)

TEST_PROGS+=		test-title-block
test_title_block_SOURCES= \
			test-title-block.c \
			$(test_internals)

TEST_PROGS+=		test-dim
test_dim_SOURCES=	test-dim.c \
			$(test_internals)

TEST_PROGS+=		test-ldim
test_ldim_SOURCES=	test-ldim.c \
			$(test_internals)

TEST_PROGS+=		test-adim
test_adim_SOURCES=	test-adim.c \
			$(test_internals)

TEST_PROGS+=		test-marker
test_marker_SOURCES=	test-marker.c \
			$(test_internals)

TEST_PROGS+=		test-arrow
test_arrow_SOURCES=	test-arrow.c \
			$(test_internals)

TEST_PROGS+=		test-widget
test_widget_SOURCES=	test-widget.c \
			$(test_internals)

TEST_PROGS+=		test-color-style
test_color_style_SOURCES= \
			test-color-style.c \
			$(test_internals)

TEST_PROGS+=		test-line-style
test_line_style_SOURCES=test-line-style.c \
			$(test_internals)

TEST_PROGS+=		test-fill-style
test_fill_style_SOURCES=test-fill-style.c \
			$(test_internals)

endif


# targets
check_PROGRAMS=		$(TEST_PROGS)
