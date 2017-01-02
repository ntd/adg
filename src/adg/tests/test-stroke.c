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


#include <adg-test.h>
#include <adg.h>


static void
_adg_property_line_dress(void)
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
_adg_property_trail(void)
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
    g_assert_null(trail);

    adg_stroke_set_trail(stroke, valid_trail);
    trail = adg_stroke_get_trail(stroke);
    g_assert_true(trail == valid_trail);

    adg_stroke_set_trail(stroke, invalid_trail);
    trail = adg_stroke_get_trail(stroke);
    g_assert_true(trail == valid_trail);

    adg_stroke_set_trail(stroke, NULL);
    trail = adg_stroke_get_trail(stroke);
    g_assert_null(trail);

    /* Using GObject property methods */
    g_object_set(stroke, "trail", NULL, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert_null(trail);

    g_object_set(stroke, "trail", valid_trail, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert_true(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(stroke, "trail", invalid_trail, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert_true(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(stroke, "trail", NULL, NULL);
    g_object_get(stroke, "trail", &trail, NULL);
    g_assert_null(trail);

    adg_entity_destroy(ADG_ENTITY(stroke));
    g_object_unref(valid_trail);
}


int
main(int argc, char *argv[])
{
    AdgPath *path;

    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/stroke/type/object", ADG_TYPE_STROKE);
    adg_test_add_entity_checks("/adg/stroke/type/entity", ADG_TYPE_STROKE);

    path = adg_path_new();
    adg_path_move_to_explicit(path, 1, 2);
    adg_path_line_to_explicit(path, 4, 5);
    adg_path_line_to_explicit(path, 7, 8);
    adg_path_close(path);
    adg_test_add_global_space_checks("/adg/stroke/behavior/global-space", adg_stroke_new(ADG_TRAIL(path)));
    adg_test_add_local_space_checks("/adg/stroke/behavior/local-space", adg_stroke_new(ADG_TRAIL(path)));
    g_object_unref(path);

    g_test_add_func("/adg/stroke/property/line-dress", _adg_property_line_dress);
    g_test_add_func("/adg/stroke/property/trail", _adg_property_trail);

    return g_test_run();
}
