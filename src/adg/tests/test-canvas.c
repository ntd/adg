/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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


#include <adg-test.h>
#include <adg.h>


static void
_adg_test_background_dress(void)
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
_adg_test_frame_dress(void)
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
_adg_test_title_block(void)
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
    adg_entity_destroy(ADG_ENTITY(title_block));

    g_object_set(canvas, "title-block", invalid_title_block, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_true(title_block == valid_title_block);
    adg_entity_destroy(ADG_ENTITY(title_block));

    g_object_set(canvas, "title-block", NULL, NULL);
    g_object_get(canvas, "title-block", &title_block, NULL);
    g_assert_null(title_block);

    adg_entity_destroy(ADG_ENTITY(canvas));
    adg_entity_destroy(ADG_ENTITY(valid_title_block));
}

static void
_adg_test_size(void)
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
_adg_test_scales(void)
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

    /* Check autoscale does not crash on no scales */
    g_object_set(canvas, "scales", NULL, NULL);
    adg_canvas_autoscale(canvas);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_top_margin(void)
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
    g_assert_cmpfloat(top_margin, ==, valid_value_1);

    adg_canvas_set_margins(canvas, valid_value_2, 0, 0, 0);
    top_margin = adg_canvas_get_top_margin(canvas);
    g_assert_cmpfloat(top_margin, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "top-margin", valid_value_1, NULL);
    g_object_get(canvas, "top-margin", &top_margin, NULL);
    g_assert_cmpfloat(top_margin, ==, valid_value_1);

    g_object_set(canvas, "top-margin", valid_value_2, NULL);
    g_object_get(canvas, "top-margin", &top_margin, NULL);
    g_assert_cmpfloat(top_margin, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_right_margin(void)
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
    g_assert_cmpfloat(right_margin, ==, valid_value_1);

    adg_canvas_set_margins(canvas, 0, valid_value_2, 0, 0);
    right_margin = adg_canvas_get_right_margin(canvas);
    g_assert_cmpfloat(right_margin, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "right-margin", valid_value_1, NULL);
    g_object_get(canvas, "right-margin", &right_margin, NULL);
    g_assert_cmpfloat(right_margin, ==, valid_value_1);

    g_object_set(canvas, "right-margin", valid_value_2, NULL);
    g_object_get(canvas, "right-margin", &right_margin, NULL);
    g_assert_cmpfloat(right_margin, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_bottom_margin(void)
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
    g_assert_cmpfloat(bottom_margin, ==, valid_value_1);

    adg_canvas_set_margins(canvas, 0, 0, valid_value_2, 0);
    bottom_margin = adg_canvas_get_bottom_margin(canvas);
    g_assert_cmpfloat(bottom_margin, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "bottom-margin", valid_value_1, NULL);
    g_object_get(canvas, "bottom-margin", &bottom_margin, NULL);
    g_assert_cmpfloat(bottom_margin, ==, valid_value_1);

    g_object_set(canvas, "bottom-margin", valid_value_2, NULL);
    g_object_get(canvas, "bottom-margin", &bottom_margin, NULL);
    g_assert_cmpfloat(bottom_margin, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_left_margin(void)
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
    g_assert_cmpfloat(left_margin, ==, valid_value_1);

    adg_canvas_set_margins(canvas, 0, 0, 0, valid_value_2);
    left_margin = adg_canvas_get_left_margin(canvas);
    g_assert_cmpfloat(left_margin, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "left-margin", valid_value_1, NULL);
    g_object_get(canvas, "left-margin", &left_margin, NULL);
    g_assert_cmpfloat(left_margin, ==, valid_value_1);

    g_object_set(canvas, "left-margin", valid_value_2, NULL);
    g_object_get(canvas, "left-margin", &left_margin, NULL);
    g_assert_cmpfloat(left_margin, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_has_frame(void)
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
_adg_test_top_padding(void)
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
    g_assert_cmpfloat(top_padding, ==, valid_value_1);

    adg_canvas_set_paddings(canvas, valid_value_2, 0, 0, 0);
    top_padding = adg_canvas_get_top_padding(canvas);
    g_assert_cmpfloat(top_padding, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "top-padding", valid_value_1, NULL);
    g_object_get(canvas, "top-padding", &top_padding, NULL);
    g_assert_cmpfloat(top_padding, ==, valid_value_1);

    g_object_set(canvas, "top-padding", valid_value_2, NULL);
    g_object_get(canvas, "top-padding", &top_padding, NULL);
    g_assert_cmpfloat(top_padding, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_right_padding(void)
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
    g_assert_cmpfloat(right_padding, ==, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, valid_value_2, 0, 0);
    right_padding = adg_canvas_get_right_padding(canvas);
    g_assert_cmpfloat(right_padding, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "right-padding", valid_value_1, NULL);
    g_object_get(canvas, "right-padding", &right_padding, NULL);
    g_assert_cmpfloat(right_padding, ==, valid_value_1);

    g_object_set(canvas, "right-padding", valid_value_2, NULL);
    g_object_get(canvas, "right-padding", &right_padding, NULL);
    g_assert_cmpfloat(right_padding, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_bottom_padding(void)
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
    g_assert_cmpfloat(bottom_padding, ==, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, 0, valid_value_2, 0);
    bottom_padding = adg_canvas_get_bottom_padding(canvas);
    g_assert_cmpfloat(bottom_padding, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "bottom-padding", valid_value_1, NULL);
    g_object_get(canvas, "bottom-padding", &bottom_padding, NULL);
    g_assert_cmpfloat(bottom_padding, ==, valid_value_1);

    g_object_set(canvas, "bottom-padding", valid_value_2, NULL);
    g_object_get(canvas, "bottom-padding", &bottom_padding, NULL);
    g_assert_cmpfloat(bottom_padding, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_test_left_padding(void)
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
    g_assert_cmpfloat(left_padding, ==, valid_value_1);

    adg_canvas_set_paddings(canvas, 0, 0, 0, valid_value_2);
    left_padding = adg_canvas_get_left_padding(canvas);
    g_assert_cmpfloat(left_padding, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(canvas, "left-padding", valid_value_1, NULL);
    g_object_get(canvas, "left-padding", &left_padding, NULL);
    g_assert_cmpfloat(left_padding, ==, valid_value_1);

    g_object_set(canvas, "left-padding", valid_value_2, NULL);
    g_object_get(canvas, "left-padding", &left_padding, NULL);
    g_assert_cmpfloat(left_padding, ==, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_set_margins(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());

    adg_canvas_set_margins(canvas, 0, 2, 4, 6);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 0);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 2);
    g_assert_cmpfloat(adg_canvas_get_bottom_margin(canvas), ==, 4);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 6);

    adg_canvas_set_margins(canvas, 1, 3, 5, 7);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 3);
    g_assert_cmpfloat(adg_canvas_get_bottom_margin(canvas), ==, 5);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 7);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_set_paddings(void)
{
    AdgCanvas *canvas = ADG_CANVAS(adg_canvas_new());

    adg_canvas_set_paddings(canvas, 0, 2, 4, 6);

    g_assert_cmpfloat(adg_canvas_get_top_padding(canvas),    ==, 0);
    g_assert_cmpfloat(adg_canvas_get_right_padding(canvas),  ==, 2);
    g_assert_cmpfloat(adg_canvas_get_bottom_padding(canvas), ==, 4);
    g_assert_cmpfloat(adg_canvas_get_left_padding(canvas),   ==, 6);

    adg_canvas_set_paddings(canvas, 1, 3, 5, 7);

    g_assert_cmpfloat(adg_canvas_get_top_padding(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_padding(canvas),  ==, 3);
    g_assert_cmpfloat(adg_canvas_get_bottom_padding(canvas), ==, 5);
    g_assert_cmpfloat(adg_canvas_get_left_padding(canvas),   ==, 7);

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
    g_assert_cmpfloat(size->x, ==, 0);
    g_assert_cmpfloat(size->y, ==, 0);

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
    g_assert_cmpfloat(size->x, ==, width - 1);
    g_assert_cmpfloat(size->y, ==, height - 2);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_get_page_setup(void)
{
    AdgCanvas *canvas;
    GtkPageSetup *page_setup;

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

    gtk_page_setup_set_top_margin(page_setup,    1, GTK_UNIT_POINTS);
    gtk_page_setup_set_right_margin(page_setup,  2, GTK_UNIT_POINTS);
    gtk_page_setup_set_left_margin(page_setup,   3, GTK_UNIT_POINTS);
    gtk_page_setup_set_bottom_margin(page_setup, 4, GTK_UNIT_POINTS);

    adg_canvas_set_page_setup(canvas, page_setup);
    g_object_unref(page_setup);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 2);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 3);
    g_assert_cmpfloat(adg_canvas_get_bottom_margin(canvas), ==, 4);

    adg_canvas_set_page_setup(canvas, NULL);

    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);
    g_assert_cmpfloat(adg_canvas_get_right_margin(canvas),  ==, 2);
    g_assert_cmpfloat(adg_canvas_get_left_margin(canvas),   ==, 3);
    g_assert_cmpfloat(adg_canvas_get_bottom_margin(canvas), ==, 4);

    size = adg_canvas_get_size(canvas);
    width = size->x;
    height = size->y;
    g_assert_cmpfloat(adg_canvas_get_top_margin(canvas),    ==, 1);

    /* When no page setup is bound to canvas, changing the margins
     * should not change the page size */
    adg_canvas_set_margins(canvas, 10, 20, 30, 40);
    size = adg_canvas_get_size(canvas);
    g_assert_cmpfloat(size->x, ==, width);
    g_assert_cmpfloat(size->y, ==, height);

    adg_entity_destroy(ADG_ENTITY(canvas));
}
#endif

int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/canvas/property/background-dress", _adg_test_background_dress);
    adg_test_add_func("/adg/canvas/property/frame-dress", _adg_test_frame_dress);
    adg_test_add_func("/adg/canvas/property/title-block", _adg_test_title_block);
    adg_test_add_func("/adg/canvas/property/size", _adg_test_size);
    adg_test_add_func("/adg/canvas/property/scales", _adg_test_scales);
    adg_test_add_func("/adg/canvas/property/top-margin", _adg_test_top_margin);
    adg_test_add_func("/adg/canvas/property/right-margin", _adg_test_right_margin);
    adg_test_add_func("/adg/canvas/property/bottom-margin", _adg_test_bottom_margin);
    adg_test_add_func("/adg/canvas/property/left-margin", _adg_test_left_margin);
    adg_test_add_func("/adg/canvas/property/has-frame", _adg_test_has_frame);
    adg_test_add_func("/adg/canvas/property/top-padding", _adg_test_top_padding);
    adg_test_add_func("/adg/canvas/property/right-padding", _adg_test_right_padding);
    adg_test_add_func("/adg/canvas/property/bottom-padding", _adg_test_bottom_padding);
    adg_test_add_func("/adg/canvas/property/left-padding", _adg_test_left_padding);
    adg_test_add_property_check("/adg/canvas/property/???", ADG_TYPE_CANVAS);

    adg_test_add_func("/adg/canvas/method/set_margins", _adg_method_set_margins);
    adg_test_add_func("/adg/canvas/method/set_paddings", _adg_method_set_paddings);
#if GTK3_ENABLED || GTK2_ENABLED
    adg_test_add_func("/adg/canvas/method/set_paper", _adg_method_set_paper);
    adg_test_add_func("/adg/canvas/method/get_page_setup", _adg_method_get_page_setup);
    adg_test_add_func("/adg/canvas/method/set_page_setup", _adg_method_set_page_setup);
#endif

    return g_test_run();
}
