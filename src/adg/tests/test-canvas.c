/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010  Nicola Fontana <ntd at entidi.it>
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


#include "test-internal.h"


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

    g_object_unref(canvas);
}

static void
_adg_test_top_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble top_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 1234;

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

    g_object_unref(canvas);
}

static void
_adg_test_right_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble right_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 1234;

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

    g_object_unref(canvas);
}

static void
_adg_test_bottom_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble bottom_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 1234;

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

    g_object_unref(canvas);
}

static void
_adg_test_left_margin(void)
{
    AdgCanvas *canvas;
    gdouble valid_value_1, valid_value_2;
    gdouble left_margin;

    canvas = ADG_CANVAS(adg_canvas_new());
    valid_value_1 = 4321;
    valid_value_2 = 1234;

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

    g_object_unref(canvas);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/canvas/background-dress", _adg_test_background_dress);
    adg_test_add_func("/adg/canvas/top-margin", _adg_test_top_margin);
    adg_test_add_func("/adg/canvas/right-margin", _adg_test_right_margin);
    adg_test_add_func("/adg/canvas/bottom-margin", _adg_test_bottom_margin);
    adg_test_add_func("/adg/canvas/left-margin", _adg_test_left_margin);

    return g_test_run();
}
