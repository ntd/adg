/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
_adg_test_line_dress(void)
{
    AdgStroke *stroke;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress line_dress;

    stroke = adg_stroke_new(NULL);
    valid_dress_1 = ADG_DRESS_LINE_GRID;
    valid_dress_2 = ADG_DRESS_LINE_DIMENSION;
    incompatible_dress = ADG_DRESS_FONT_ANNOTATION;

    /* Using the public APIs */
    adg_stroke_set_line_dress(stroke, valid_dress_1);
    line_dress = adg_stroke_get_line_dress(stroke);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_stroke_set_line_dress(stroke, incompatible_dress);
    line_dress = adg_stroke_get_line_dress(stroke);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_stroke_set_line_dress(stroke, valid_dress_2);
    line_dress = adg_stroke_get_line_dress(stroke);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(stroke, "line-dress", valid_dress_1, NULL);
    g_object_get(stroke, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(stroke, "line-dress", incompatible_dress, NULL);
    g_object_get(stroke, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(stroke, "line-dress", valid_dress_2, NULL);
    g_object_get(stroke, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(stroke));
}

static void
_adg_test_trail(void)
{
    AdgStroke *stroke;
    AdgTrail *valid_trail, *invalid_trail, *trail;

    stroke = adg_stroke_new(NULL);
    valid_trail = ADG_TRAIL(adg_path_new());
    invalid_trail = adg_test_invalid_pointer();

    g_object_ref(valid_trail);

    /* Using the public APIs */
    adg_stroke_set_trail(stroke, NULL);
    trail = adg_stroke_get_trail(stroke);
    g_assert(trail == NULL);

    adg_stroke_set_trail(stroke, valid_trail);
    trail = adg_stroke_get_trail(stroke);
    g_assert(trail == valid_trail);

    adg_stroke_set_trail(stroke, invalid_trail);
    trail = adg_stroke_get_trail(stroke);
    g_assert(trail == valid_trail);

    adg_stroke_set_trail(stroke, NULL);
    trail = adg_stroke_get_trail(stroke);
    g_assert(trail == NULL);

    /* Using GObject property methods */
    g_object_set(stroke, "trail", NULL, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert(trail == NULL);

    g_object_set(stroke, "trail", valid_trail, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(stroke, "trail", invalid_trail, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(stroke, "trail", NULL, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert(trail == NULL);

    adg_entity_destroy(ADG_ENTITY(stroke));
    g_object_unref(valid_trail);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/stroke/line-dress", _adg_test_line_dress);
    adg_test_add_func("/adg/stroke/trail", _adg_test_trail);

    return g_test_run();
}
