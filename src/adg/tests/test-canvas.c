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
    valid_dress_2 = ADG_DRESS_COLOR_HATCH;
    incompatible_dress = ADG_DRESS_TEXT_LIMIT;

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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/canvas/background-dress", _adg_test_background_dress);

    return g_test_run();
}
