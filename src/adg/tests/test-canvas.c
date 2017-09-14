/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#include <config.h>
#include <adg-test.h>
#include <adg.h>

#ifdef G_OS_WIN32

#define NULL_FILE "NUL"

#else

#define NULL_FILE "/dev/null"

#endif


static void
_adg_behavior_entity(void)
{
    AdgCanvas *canvas;
    const CpmlExtents *extents;
    AdgTitleBlock *title_block;

    canvas = ADG_CANVAS(adg_test_canvas());

    adg_entity_arrange(ADG_ENTITY(canvas));
    extents = adg_entity_get_extents(ADG_ENTITY(canvas));
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->size.x, 1);
    adg_assert_isapprox(extents->size.y, 1);

    adg_entity_invalidate(ADG_ENTITY(canvas));
    extents = adg_entity_get_extents(ADG_ENTITY(canvas));
    g_assert_false(extents->is_defined);

    title_block = adg_title_block_new();
    adg_canvas_set_title_block(canvas, title_block);
    adg_entity_arrange(ADG_ENTITY(canvas));
    extents = adg_entity_get_extents(ADG_ENTITY(canvas));
    g_assert_true(extents->is_defined);
    g_assert_cmpfloat(extents->size.x, >, 1);
    g_assert_cmpfloat(extents->size.y, >, 1);
    g_assert_cmpfloat(extents->size.x, <, 5000);
    g_assert_cmpfloat(extents->size.y, <, 5000);

    adg_canvas_set_size_explicit(canvas, 5000, 5000);
    adg_entity_arrange(ADG_ENTITY(canvas));
    extents = adg_entity_get_extents(ADG_ENTITY(canvas));
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->size.x, 5000);
    adg_assert_isapprox(extents->size.y, 5000);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_behavior_misc(void)
{
    g_assert_cmpuint(ADG_CANVAS_ERROR, !=, 0);
}

static void
_adg_property_background_dress(void)
{
    AdgCanvas *canvas;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress background_dress;

    canvas = adg_canvas_new();
    valid_dress_1 = ADG_DRESS_COLOR_DIMENSION;
    valid_dress_2 = ADG_DRESS_COLOR_FILL;
    incompatible_dress = ADG_DRESS_FONT_QUOTE_ANNOTATION;

    /* Using the public APIs */
    adg_canvas_set_background_dress(canvas, valid_dress_1);
    background_dress = adg_canvas_get_background_dress(canvas);
    g_assert_cmpint(background_dress, ==, valid_dress_1);

    adg_canvas_set_background_dress(canvas, incompatible_dress);
    background_dress = adg_canvas_get_background_dress(canvas);
    g_assert_cmpint(background_dress, ==, valid_dress_1);

    adg_canvas_set_background_dress(canvas, valid_dress_2);
    background_dress = adg_canvas_get_background_dress(canvas);
    g_assert_cmpint(background_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(canvas, "background-dress", valid_dress_1, NULL);
    g_object_get(canvas, "background-dress", &background_dress, NULL);
    g_assert_cmpint(background_dress, ==, valid_dress_1);

    g_object_set(canvas, "background-dress", incompatible_dress, NULL);
    g_object_get(canvas, "background-dress", &background_dress, NULL);
    g_assert_cmpint(background_dress, ==, valid_dress_1);

    g_object_set(canvas, "background-dress", valid_dress_2, NULL);
    g_object_get(canvas, "background-dress", &background_dress, NULL);
    g_assert_cmpint(background_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_frame_dress(void)
{
    AdgCanvas *canvas;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress frame_dress;

    canvas = adg_canvas_new();
    valid_dress_1 = ADG_DRESS_LINE_DIMENSION;
    valid_dress_2 = ADG_DRESS_LINE_FILL;
    incompatible_dress = ADG_DRESS_FONT;

    /* Using the public APIs */
    adg_canvas_set_frame_dress(canvas, valid_dress_1);
    frame_dress = adg_canvas_get_frame_dress(canvas);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_canvas_set_frame_dress(canvas, incompatible_dress);
    frame_dress = adg_canvas_get_frame_dress(canvas);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_canvas_set_frame_dress(canvas, valid_dress_2);
    frame_dress = adg_canvas_get_frame_dress(canvas);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(canvas, "frame-dress", valid_dress_1, NULL);
    g_object_get(canvas, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(canvas, "frame-dress", incompatible_dress, NULL);
    g_object_get(canvas, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(canvas, "frame-dress", valid_dress_2, NULL);
    g_object_get(canvas, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_title_block(void)
{
    AdgCanvas *canvas;
    AdgTitleBlock *valid_title_block, *invalid_title_block, *title_block;

    canvas = adg_canvas_new();
    valid_title_block = ADG_TITLE_BLOCK(adg_title_block_new());
    invalid_title_block = adg_test_invalid_pointer();

    g_object_ref(valid_title_block);

    /* Using the public APIs */
    adg_canvas_set_title_block(canvas, NULL);
    title_block = adg_canvas_get_title_block(canvas);
    g_assert_null(title_block);

    adg_canvas_set_title_block(canvas, valid_title_block);
    title_block = adg_canvas_get_title_block(canvas);
    g_assert_true(title_block == valid_title_block);

    adg_canvas_set_title_block(canvas, invalid_title_block);
    title_block = adg_canvas_get_title_block(canvas);
    g_assert_true(title_block == valid_title_block);

    adg_canvas_set_title_block(canvas, NULL);
    title_block = adg_canvas_get_title_block(canvas);
    g_assert_null(title_block);

    /* Using GObject property methods */
    g_object_set(canvas, "title-block", NULL, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_null(title_block);

    g_object_set(canvas, "title-block", valid_title_block, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_true(title_block == valid_title_block);
    g_object_unref(title_block);

    g_object_set(canvas, "title-block", invalid_title_block, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_true(title_block == valid_title_block);
    g_object_unref(title_block);

    g_object_set(canvas, "title-block", NULL, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_null(title_block);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_size(void)
{
    AdgCanvas *canvas;
    CpmlPair null_size, sample_size;
    const CpmlPair *size;
    CpmlPair *size_dup;

    canvas = adg_canvas_new();
    null_size.x = 0;
    null_size.y = 0;
    sample_size.x = 123;
    sample_size.y = 321;

    /* Using the public APIs */
    size = adg_canvas_get_size(canvas);

    /* The default size is 0, 0 (anamorphic canvas) */
    g_assert_true(cpml_pair_equal(size, &null_size));

    adg_canvas_set_size(canvas, &sample_size);
    size = adg_canvas_get_size(canvas);
    g_assert_true(cpml_pair_equal(size, &sample_size));

    adg_canvas_set_size(canvas, NULL);
    size = adg_canvas_get_size(canvas);
    g_assert_true(cpml_pair_equal(size, &sample_size));

    adg_canvas_set_size_explicit(canvas, 0, 0);
    size = adg_canvas_get_size(canvas);
    g_assert_true(cpml_pair_equal(size, &null_size));

    /* Using GObject property methods */
    g_object_set(canvas, "size", &sample_size, NULL);
    g_object_get(canvas, "size", &size_dup, NULL);
    g_assert_true(cpml_pair_equal(size_dup, &sample_size));
    g_free(size_dup);

    g_object_set(canvas, "size", NULL, NULL);
    g_object_get(canvas, "size", &size_dup, NULL);
    g_assert_true(cpml_pair_equal(size_dup, &sample_size));
    g_free(size_dup);

    g_object_set(canvas, "size", &null_size, NULL);
    g_object_get(canvas, "size", &size_dup, NULL);
    g_assert_true(cpml_pair_equal(size_dup, &null_size));
    g_free(size_dup);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_factor(void)
{
    AdgCanvas *canvas;
    gdouble factor;

    canvas = ADG_CANVAS(adg_canvas_new());

    /* Using the public APIs */
    factor = adg_canvas_get_factor(canvas);
    adg_assert_isapprox(factor, 1);

    adg_canvas_set_factor(canvas, 123);
    factor = adg_canvas_get_factor(canvas);
    adg_assert_isapprox(factor, 123);

    adg_canvas_set_factor(canvas, -123);
    factor = adg_canvas_get_factor(canvas);
    adg_assert_isapprox(factor, 123);

    adg_canvas_set_factor(canvas, 0);
    factor = adg_canvas_get_factor(canvas);
    adg_assert_isapprox(factor, 123);

    /* Using GObject property methods */
    g_object_set(canvas, "factor", 321., NULL);
    g_object_get(canvas, "factor", &factor, NULL);
    adg_assert_isapprox(factor, 321);

    g_object_set(canvas, "factor", -321., NULL);
    g_object_get(canvas, "factor", &factor, NULL);
    adg_assert_isapprox(factor, 321);

    g_object_set(canvas, "factor", 0., NULL);
    g_object_get(canvas, "factor", &factor, NULL);
    adg_assert_isapprox(factor, 321);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_scales(void)
{
    AdgCanvas *canvas;
    gchar **scales;
    gchar *scales_array[] = { "a", "b", "c", NULL };

    /* By default, AdgCanvas:scales is prefilled with some default scales */
    canvas = adg_canvas_new();
    scales = adg_canvas_get_scales(canvas);
    g_assert_nonnull(scales);
    adg_entity_destroy(ADG_ENTITY(canvas));

    /* AdgCanvas:scales must be prefilled also when using g_object_new() */
    canvas = g_object_new(ADG_TYPE_CANVAS, NULL);
    scales = adg_canvas_get_scales(canvas);
    g_assert_nonnull(scales);

    /* Using the public APIs */
    adg_canvas_set_scales(canvas, NULL);
    scales = adg_canvas_get_scales(canvas);
    g_assert_null(scales);

    adg_canvas_set_scales(canvas, "0", "1", NULL);
    scales = adg_canvas_get_scales(canvas);
    g_assert_nonnull(scales);
    g_assert_cmpstr(scales[0], ==, "0");
    g_assert_cmpstr(scales[1], ==, "1");
    g_assert_null(scales[2]);

    adg_canvas_set_scales_array(canvas, NULL);
    scales = adg_canvas_get_scales(canvas);
    g_assert_null(scales);

    adg_canvas_set_scales_array(canvas, scales_array);
    scales = adg_canvas_get_scales(canvas);
    g_assert_nonnull(scales);
    g_assert_cmpstr(scales[0], ==, "a");
    g_assert_cmpstr(scales[1], ==, "b");
    g_assert_cmpstr(scales[2], ==, "c");
    g_assert_null(scales[3]);

    /* Using GObject property methods */
    g_object_set(canvas, "scales", NULL, NULL);
    g_object_get(canvas, "scales", &scales, NULL);
    g_assert_null(scales);

    g_object_set(canvas, "scales", scales_array, NULL);
    g_object_get(canvas, "scales", &scales, NULL);
    g_assert_nonnull(scales);
    g_assert_cmpstr(scales[0], ==, "a");
    g_assert_cmpstr(scales[1], ==, "b");
    g_assert_cmpstr(scales[2], ==, "c");
    g_assert_null(scales[3]);
    g_strfreev(scales);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_top_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble top_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -1;

    /* Using the public APIs */
    adg_canvas_set_top_margin(canvas, valid_value_1);
    top_margin = adg_canvas_get_top_margin(canvas);
    adg_assert_isapprox(top_margin, valid_value_1);

    adg_canvas_set_margins(canvas, valid_value_2, 0, 0, 0);
    top_margin = adg_canvas_get_top_margin(canvas);
    adg_assert_isapprox(top_margin, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "top-margin", valid_value_1, NULL);
    g_object_get(canvas, "top-margin", &top_margin, NULL);
    adg_assert_isapprox(top_margin, valid_value_1);

    g_object_set(canvas, "top-margin", valid_value_2, NULL);
    g_object_get(canvas, "top-margin", &top_margin, NULL);
    adg_assert_isapprox(top_margin, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_right_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble right_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 0;

    /* Using the public APIs */
    adg_canvas_set_right_margin(canvas, valid_value_1);
    right_margin = adg_canvas_get_right_margin(canvas);
    adg_assert_isapprox(right_margin, valid_value_1);

    adg_canvas_set_margins(canvas, 0, valid_value_2, 0, 0);
    right_margin = adg_canvas_get_right_margin(canvas);
    adg_assert_isapprox(right_margin, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "right-margin", valid_value_1, NULL);
    g_object_get(canvas, "right-margin", &right_margin, NULL);
    adg_assert_isapprox(right_margin, valid_value_1);

    g_object_set(canvas, "right-margin", valid_value_2, NULL);
    g_object_get(canvas, "right-margin", &right_margin, NULL);
    adg_assert_isapprox(right_margin, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_bottom_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble bottom_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -1234;

    /* Using the public APIs */
    adg_canvas_set_bottom_margin(canvas, valid_value_1);
    bottom_margin = adg_canvas_get_bottom_margin(canvas);
    adg_assert_isapprox(bottom_margin, valid_value_1);

    adg_canvas_set_margins(canvas, 0, 0, valid_value_2, 0);
    bottom_margin = adg_canvas_get_bottom_margin(canvas);
    adg_assert_isapprox(bottom_margin, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "bottom-margin", valid_value_1, NULL);
    g_object_get(canvas, "bottom-margin", &bottom_margin, NULL);
    adg_assert_isapprox(bottom_margin, valid_value_1);

    g_object_set(canvas, "bottom-margin", valid_value_2, NULL);
    g_object_get(canvas, "bottom-margin", &bottom_margin, NULL);
    adg_assert_isapprox(bottom_margin, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_left_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble left_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -4321;

    /* Using the public APIs */
    adg_canvas_set_left_margin(canvas, valid_value_1);
    left_margin = adg_canvas_get_left_margin(canvas);
    adg_assert_isapprox(left_margin, valid_value_1);

    adg_canvas_set_margins(canvas, 0, 0, 0, valid_value_2);
    left_margin = adg_canvas_get_left_margin(canvas);
    adg_assert_isapprox(left_margin, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "left-margin", valid_value_1, NULL);
    g_object_get(canvas, "left-margin", &left_margin, NULL);
    adg_assert_isapprox(left_margin, valid_value_1);

    g_object_set(canvas, "left-margin", valid_value_2, NULL);
    g_object_get(canvas, "left-margin", &left_margin, NULL);
    adg_assert_isapprox(left_margin, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_has_frame(void)
{
    AdgCanvas *canvas;
    gboolean invalid_boolean;
    gboolean has_frame;

    canvas = ADG_CANVAS(adg_canvas_new());
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_canvas_switch_frame(canvas, FALSE);
    has_frame = adg_canvas_has_frame(canvas);
    g_assert_false(has_frame);

    adg_canvas_switch_frame(canvas, invalid_boolean);
    has_frame = adg_canvas_has_frame(canvas);
    g_assert_false(has_frame);

    adg_canvas_switch_frame(canvas, TRUE);
    has_frame = adg_canvas_has_frame(canvas);
    g_assert_true(has_frame);

    /* Using GObject property methods */
    g_object_set(canvas, "has-frame", FALSE, NULL);
    g_object_get(canvas, "has-frame", &has_frame, NULL);
    g_assert_false(has_frame);

    g_object_set(canvas, "has-frame", invalid_boolean, NULL);
    g_object_get(canvas, "has-frame", &has_frame, NULL);
    g_assert_false(has_frame);

    g_object_set(canvas, "has-frame", TRUE, NULL);
    g_object_get(canvas, "has-frame", &has_frame, NULL);
    g_assert_true(has_frame);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_top_padding(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble top_padding;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 0;

    /* Using the public APIs */
    adg_canvas_set_top_padding(canvas, valid_value_1);
    top_padding = adg_canvas_get_top_padding(canvas);
    adg_assert_isapprox(top_padding, valid_value_1);

    adg_canvas_set_paddings(canvas, valid_value_2, 0, 0, 0);
    top_padding = adg_canvas_get_top_padding(canvas);
    adg_assert_isapprox(top_padding, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "top-padding", valid_value_1, NULL);
    g_object_get(canvas, "top-padding", &top_padding, NULL);
    adg_assert_isapprox(top_padding, valid_value_1);

    g_object_set(canvas, "top-padding", valid_value_2, NULL);
    g_object_get(canvas, "top-padding", &top_padding, NULL);
    adg_assert_isapprox(top_padding, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_right_padding(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble right_padding;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -1;

    /* Using the public APIs */
    adg_canvas_set_right_padding(canvas, valid_value_1);
    right_padding = adg_canvas_get_right_padding(canvas);
    adg_assert_isapprox(right_padding, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, valid_value_2, 0, 0);
    right_padding = adg_canvas_get_right_padding(canvas);
    adg_assert_isapprox(right_padding, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "right-padding", valid_value_1, NULL);
    g_object_get(canvas, "right-padding", &right_padding, NULL);
    adg_assert_isapprox(right_padding, valid_value_1);

    g_object_set(canvas, "right-padding", valid_value_2, NULL);
    g_object_get(canvas, "right-padding", &right_padding, NULL);
    adg_assert_isapprox(right_padding, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_bottom_padding(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble bottom_padding;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -4321;

    /* Using the public APIs */
    adg_canvas_set_bottom_padding(canvas, valid_value_1);
    bottom_padding = adg_canvas_get_bottom_padding(canvas);
    adg_assert_isapprox(bottom_padding, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, 0, valid_value_2, 0);
    bottom_padding = adg_canvas_get_bottom_padding(canvas);
    adg_assert_isapprox(bottom_padding, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "bottom-padding", valid_value_1, NULL);
    g_object_get(canvas, "bottom-padding", &bottom_padding, NULL);
    adg_assert_isapprox(bottom_padding, valid_value_1);

    g_object_set(canvas, "bottom-padding", valid_value_2, NULL);
    g_object_get(canvas, "bottom-padding", &bottom_padding, NULL);
    adg_assert_isapprox(bottom_padding, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_left_padding(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble left_padding;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = -1111;

    /* Using the public APIs */
    adg_canvas_set_left_padding(canvas, valid_value_1);
    left_padding = adg_canvas_get_left_padding(canvas);
    adg_assert_isapprox(left_padding, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, 0, 0, valid_value_2);
    left_padding = adg_canvas_get_left_padding(canvas);
    adg_assert_isapprox(left_padding, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "left-padding", valid_value_1, NULL);
    g_object_get(canvas, "left-padding", &left_padding, NULL);
    adg_assert_isapprox(left_padding, valid_value_1);

    g_object_set(canvas, "left-padding", valid_value_2, NULL);
    g_object_get(canvas, "left-padding", &left_padding, NULL);
    adg_assert_isapprox(left_padding, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_autoscale(void)
{
    AdgCanvas *canvas;
    AdgTitleBlock *title_block;
    AdgPath *path;
    AdgStroke *stroke;
    const cairo_matrix_t *matrix;

    canvas = adg_canvas_new();

    /* Check autoscale does not crash on no scales */
    adg_canvas_set_scales(canvas, NULL);
    adg_canvas_autoscale(canvas);

    /* Set an arbitrary size on canvas: the first three scales
     * are invalids and should be ignored*/
    adg_canvas_set_scales(canvas, "0", "-1:1", "1:0", "10:1", "1 a 1", "1:2", "1:10 ", NULL);
    adg_canvas_set_size_explicit(canvas, 100, 100);

    /* Check the scale will be reported on the title block */
    title_block = adg_title_block_new();
    adg_canvas_set_title_block(canvas, title_block);
    g_object_unref(title_block);

    path = adg_path_new();
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to_explicit(path, 5, 5);
    stroke = adg_stroke_new(ADG_TRAIL(path));
    g_object_unref(path);

    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(stroke));

    adg_canvas_autoscale(canvas);
    matrix = adg_entity_get_local_matrix(ADG_ENTITY(canvas));
    adg_assert_isapprox(matrix->xx, 10);
    g_assert_cmpstr(adg_title_block_get_scale(title_block), ==, "10:1");

    adg_path_line_to_explicit(path, 50, 50);

    adg_canvas_autoscale(canvas);
    matrix = adg_entity_get_local_matrix(ADG_ENTITY(canvas));
    adg_assert_isapprox(matrix->xx, 1);
    g_assert_cmpstr(adg_title_block_get_scale(title_block), ==, "1 a 1");

    adg_path_line_to_explicit(path, 0, 100);

    adg_canvas_autoscale(canvas);
    matrix = adg_entity_get_local_matrix(ADG_ENTITY(canvas));
    adg_assert_isapprox(matrix->xx, 0.5);
    g_assert_cmpstr(adg_title_block_get_scale(title_block), ==, "1:2");

    adg_path_line_to_explicit(path, 800, 0);

    adg_canvas_autoscale(canvas);
    matrix = adg_entity_get_local_matrix(ADG_ENTITY(canvas));
    adg_assert_isapprox(matrix->xx, 0.1);
    g_assert_cmpstr(adg_title_block_get_scale(title_block), ==, "1:10 ");

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_set_margins(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());

    adg_canvas_set_margins(canvas, 0, 2, 4, 6);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 0);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 2);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(canvas), 4);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 6);

    adg_canvas_set_margins(canvas, 1, 3, 5, 7);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 3);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(canvas), 5);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 7);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_get_margins(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());
    gdouble top, right, bottom, left;

    adg_canvas_set_margins(canvas, 1, 2, 3, 4);

    /* Check that passing NULL does not crash the process */
    adg_canvas_get_margins(canvas, NULL, NULL, NULL, NULL);

    adg_canvas_get_margins(canvas, &top, &right, &bottom, &left);
    adg_assert_isapprox(top, 1);
    adg_assert_isapprox(right, 2);
    adg_assert_isapprox(bottom, 3);
    adg_assert_isapprox(left, 4);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_get_paddings(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());
    gdouble top, right, bottom, left;

    adg_canvas_set_paddings(canvas, 1, 2, 3, 4);

    /* Check that passing NULL does not crash the process */
    adg_canvas_get_paddings(canvas, NULL, NULL, NULL, NULL);

    adg_canvas_get_paddings(canvas, &top, &right, &bottom, &left);
    adg_assert_isapprox(top, 1);
    adg_assert_isapprox(right, 2);
    adg_assert_isapprox(bottom, 3);
    adg_assert_isapprox(left, 4);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_apply_margins(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());
    CpmlExtents extents = {
        .is_defined = 1,
        .org = { 0, 0 },
        .size = { 10, 10 }
    };

    /* Sanity checks */
    adg_canvas_apply_margins(canvas, NULL);
    //adg_canvas_apply_margins(NULL, &extents);

    adg_canvas_set_margins(canvas, 1, 2, 3, 4);

    adg_canvas_apply_margins(canvas, &extents);

    g_assert_cmpfloat(extents.org.x,  ==, -4);
    g_assert_cmpfloat(extents.org.y,  ==, -1);
    adg_assert_isapprox(extents.size.x, 16);
    adg_assert_isapprox(extents.size.y, 14);

    adg_canvas_apply_margins(canvas, &extents);

    g_assert_cmpfloat(extents.org.x,  ==, -8);
    g_assert_cmpfloat(extents.org.y,  ==, -2);
    adg_assert_isapprox(extents.size.x, 22);
    adg_assert_isapprox(extents.size.y, 18);

    extents.is_defined = 0;
    adg_canvas_apply_margins(canvas, &extents);

    g_assert_cmpfloat(extents.org.x,  ==, -8);
    g_assert_cmpfloat(extents.org.y,  ==, -2);
    adg_assert_isapprox(extents.size.x, 22);
    adg_assert_isapprox(extents.size.y, 18);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_set_paddings(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());

    adg_canvas_set_paddings(canvas, 0, 2, 4, 6);

    g_assert_cmpfloat(adg_canvas_get_top_padding(canvas),    ==, 0);
    g_assert_cmpfloat(adg_canvas_get_right_padding(canvas),  ==, 2);
    adg_assert_isapprox(adg_canvas_get_bottom_padding(canvas), 4);
    g_assert_cmpfloat(adg_canvas_get_left_padding(canvas),   ==, 6);

    adg_canvas_set_paddings(canvas, 1, 3, 5, 7);

    g_assert_cmpfloat(adg_canvas_get_top_padding(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_padding(canvas),  ==, 3);
    adg_assert_isapprox(adg_canvas_get_bottom_padding(canvas), 5);
    g_assert_cmpfloat(adg_canvas_get_left_padding(canvas),   ==, 7);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_export(void)
{
    AdgCanvas *canvas = adg_test_canvas();

    /* Sanity check */
    g_assert_false(adg_canvas_export(NULL, CAIRO_SURFACE_TYPE_IMAGE, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_IMAGE, NULL, NULL));

    /* Supported surface types return TRUE while unsupported ones return FALSE */
    g_assert_true(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_IMAGE, NULL_FILE, NULL));
    g_assert_true(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_PDF, NULL_FILE, NULL));
    g_assert_true(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_PS, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_XLIB, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_XCB, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_GLITZ, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_QUARTZ, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_WIN32, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_BEOS, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_DIRECTFB, NULL_FILE, NULL));
    g_assert_true(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_SVG, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_OS2, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_WIN32_PRINTING, NULL_FILE, NULL));
    g_assert_false(adg_canvas_export(canvas, CAIRO_SURFACE_TYPE_QUARTZ_IMAGE, NULL_FILE, NULL));

    /* An empty canvas can fail depending on the surface type (e.g., the image
     * surface fails while the PDF surface does not) so skipping that tests */

    adg_entity_destroy(ADG_ENTITY(canvas));
}

#if GTK3_ENABLED || GTK2_ENABLED

static void
_adg_method_set_paper(void)
{
    AdgCanvas *canvas;
    const CpmlPair *size;
    gdouble width, height;

    canvas = ADG_CANVAS(adg_canvas_new());

    size = adg_canvas_get_size(canvas);
    adg_assert_isapprox(size->x, 0);
    adg_assert_isapprox(size->y, 0);

    adg_canvas_set_margins(canvas, 0, 0, 0, 0);
    adg_canvas_set_paper(canvas, GTK_PAPER_NAME_A4,
                         GTK_PAGE_ORIENTATION_LANDSCAPE);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    !=, 0);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  !=, 0);
    g_assert_cmpfloat(adg_canvas_get_bottom_margin(canvas), !=, 0);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   !=, 0);

    size = adg_canvas_get_size(canvas);
    width = size->x;
    height = size->y;
    g_assert_cmpfloat(width, !=, 0);
    g_assert_cmpfloat(height, !=, 0);

    adg_canvas_set_left_margin(canvas, 0);
    adg_canvas_set_top_margin(canvas, 0);

    size = adg_canvas_get_size(canvas);
    g_assert_cmpfloat(width,  !=, size->x);
    g_assert_cmpfloat(height, !=, size->y);
    width = size->x;
    height = size->y;

    /* Ensure the page size is decreased when margins are increased */
    adg_canvas_set_left_margin(canvas, adg_canvas_get_left_margin(canvas) + 1);
    adg_canvas_set_top_margin(canvas, adg_canvas_get_top_margin(canvas) + 2);
    size = adg_canvas_get_size(canvas);
    adg_assert_isapprox(size->x, width - 1);
    adg_assert_isapprox(size->y, height - 2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_get_page_setup(void)
{
    AdgCanvas *canvas;
    GtkPageSetup *page_setup;

    /* Sanity checks */
    g_assert_null(adg_canvas_get_page_setup(NULL));

    canvas = ADG_CANVAS(adg_canvas_new());
    page_setup = gtk_page_setup_new();

    g_assert_null(adg_canvas_get_page_setup(canvas));

    adg_canvas_set_page_setup(canvas, page_setup);
    g_assert_true(adg_canvas_get_page_setup(canvas) == page_setup);

    /* canvas should still internally owns a reference to page_setup */
    g_object_unref(page_setup);
    g_assert_true(GTK_IS_PAGE_SETUP(adg_canvas_get_page_setup(canvas)));

    adg_canvas_set_page_setup(canvas, NULL);
    g_assert_null(adg_canvas_get_page_setup(canvas));

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_set_page_setup(void)
{
    AdgCanvas *canvas;
    GtkPageSetup *page_setup;
    const CpmlPair *size;
    gdouble width, height;

    canvas = ADG_CANVAS(adg_canvas_new());
    page_setup = gtk_page_setup_new();

    /* Sanity checks */
    adg_canvas_set_page_setup(NULL, page_setup);

    gtk_page_setup_set_top_margin(page_setup,    1, GTK_UNIT_POINTS);
    gtk_page_setup_set_right_margin(page_setup,  2, GTK_UNIT_POINTS);
    gtk_page_setup_set_left_margin(page_setup,   3, GTK_UNIT_POINTS);
    gtk_page_setup_set_bottom_margin(page_setup, 4, GTK_UNIT_POINTS);

    adg_canvas_set_page_setup(canvas, page_setup);
    g_object_unref(page_setup);

    /* For some unknown reason the margins are rounded, so using
     * adg_assert_isapprox() instead of g_assert_cmpfloat() */
    adg_assert_isapprox(adg_canvas_get_top_margin(canvas),    1);
    adg_assert_isapprox(adg_canvas_get_right_margin(canvas),  2);
    adg_assert_isapprox(adg_canvas_get_left_margin(canvas),   3);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(canvas), 4);

    adg_canvas_set_page_setup(canvas, NULL);

    adg_assert_isapprox(adg_canvas_get_top_margin(canvas),    1);
    adg_assert_isapprox(adg_canvas_get_right_margin(canvas),  2);
    adg_assert_isapprox(adg_canvas_get_left_margin(canvas),   3);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(canvas), 4);

    size = adg_canvas_get_size(canvas);
    width = size->x;
    height = size->y;
    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);

    /* When no page setup is bound to canvas, changing the margins
     * should not change the page size */
    adg_canvas_set_margins(canvas, 10, 20, 30, 40);
    size = adg_canvas_get_size(canvas);
    adg_assert_isapprox(size->x, width);
    adg_assert_isapprox(size->y, height);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

#endif


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/canvas/type/object", ADG_TYPE_CANVAS);
    adg_test_add_entity_checks("/adg/canvas/type/entity", ADG_TYPE_CANVAS);
    adg_test_add_container_checks("/adg/canvas/type/container", ADG_TYPE_CANVAS);

    g_test_add_func("/adg/canvas/behavior/entity", _adg_behavior_entity);
    g_test_add_func("/adg/canvas/behavior/misc", _adg_behavior_misc);
    adg_test_add_global_space_checks("/adg/canvas/behavior/global-space", adg_test_canvas());
    adg_test_add_local_space_checks("/adg/canvas/behavior/local-space", adg_test_canvas());

    g_test_add_func("/adg/canvas/property/background-dress", _adg_property_background_dress);
    g_test_add_func("/adg/canvas/property/frame-dress", _adg_property_frame_dress);
    g_test_add_func("/adg/canvas/property/title-block", _adg_property_title_block);
    g_test_add_func("/adg/canvas/property/size", _adg_property_size);
    g_test_add_func("/adg/canvas/property/factor", _adg_property_factor);
    g_test_add_func("/adg/canvas/property/scales", _adg_property_scales);
    g_test_add_func("/adg/canvas/property/top-margin", _adg_property_top_margin);
    g_test_add_func("/adg/canvas/property/right-margin", _adg_property_right_margin);
    g_test_add_func("/adg/canvas/property/bottom-margin", _adg_property_bottom_margin);
    g_test_add_func("/adg/canvas/property/left-margin", _adg_property_left_margin);
    g_test_add_func("/adg/canvas/property/has-frame", _adg_property_has_frame);
    g_test_add_func("/adg/canvas/property/top-padding", _adg_property_top_padding);
    g_test_add_func("/adg/canvas/property/right-padding", _adg_property_right_padding);
    g_test_add_func("/adg/canvas/property/bottom-padding", _adg_property_bottom_padding);
    g_test_add_func("/adg/canvas/property/left-padding", _adg_property_left_padding);

    g_test_add_func("/adg/canvas/method/autoscale", _adg_method_autoscale);
    g_test_add_func("/adg/canvas/method/set-margins", _adg_method_set_margins);
    g_test_add_func("/adg/canvas/method/get-margins", _adg_method_get_margins);
    g_test_add_func("/adg/canvas/method/apply-margins", _adg_method_apply_margins);
    g_test_add_func("/adg/canvas/method/set-paddings", _adg_method_set_paddings);
    g_test_add_func("/adg/canvas/method/get-paddings", _adg_method_get_paddings);
    g_test_add_func("/adg/canvas/method/export", _adg_method_export);
#if GTK3_ENABLED || GTK2_ENABLED
    g_test_add_func("/adg/canvas/method/set-paper", _adg_method_set_paper);
    g_test_add_func("/adg/canvas/method/get-page-setup", _adg_method_get_page_setup);
    g_test_add_func("/adg/canvas/method/set-page-setup", _adg_method_set_page_setup);
#endif

    return g_test_run();
}
