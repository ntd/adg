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


static cairo_path_t *
_adg_path_callback(AdgTrail *trail, gpointer user_data)
{
    static cairo_path_data_t data[] = {
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 1 }},
        { .header = { CPML_LINE, 2 }},
        { .point = { 2, 3 }},
        { .header = { CPML_LINE, 2 }},
        { .point = { 4, 5 }}
    };
    static cairo_path_t path = {
        CAIRO_STATUS_SUCCESS,
        data,
        G_N_ELEMENTS(data)
    };

    return &path;
}


static void
_adg_test_max_angle(void)
{
    AdgTrail *trail;
    gdouble valid_value, invalid_value;
    gdouble max_angle;

    trail = adg_trail_new(_adg_path_callback, NULL);
    valid_value = G_PI / 10;
    invalid_value = G_PI + 1;

    /* Using the public APIs */
    adg_trail_set_max_angle(trail, valid_value);
    max_angle = adg_trail_get_max_angle(trail);
    g_assert_cmpfloat(max_angle, ==, valid_value);

    adg_trail_set_max_angle(trail, invalid_value);
    max_angle = adg_trail_get_max_angle(trail);
    g_assert_cmpfloat(max_angle, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(trail, "max-angle", valid_value, NULL);
    g_object_get(trail, "max-angle", &max_angle, NULL);
    g_assert_cmpfloat(max_angle, ==, valid_value);

    g_object_set(trail, "max-angle", invalid_value, NULL);
    g_object_get(trail, "max-angle", &max_angle, NULL);
    g_assert_cmpfloat(max_angle, !=, invalid_value);

    g_object_unref(trail);
}

static void
_adg_test_put_segment(void)
{
    AdgTrail *trail;
    AdgPath *path;
    CpmlSegment segment;

    path = adg_path_new();

    /* First segment: a simple CPML_LINE */
    adg_path_move_to_explicit(path, 1, 2);
    adg_path_line_to_explicit(path, 3, 4);

    /* Second segment: a closed curve with a bogus CPML_MOVE prepended */
    adg_path_move_to_explicit(path, 5, 6);
    adg_path_move_to_explicit(path, 7, 8);
    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    adg_path_close(path);

    /* Junk */
    adg_path_close(path);
    adg_path_arc_to_explicit(path, 15, 16, 17, 18);

    /* Third segment: a closed CPML_ARC */
    adg_path_move_to_explicit(path, 19, 20);
    adg_path_arc_to_explicit(path, 21, 22, 23, 24);
    adg_path_close(path);

    /* Other junk */
    adg_path_line_to_explicit(path, 25, 26);

    trail = ADG_TRAIL(path);

    /* Sanity checks */
    g_assert_false(adg_trail_put_segment(NULL, 1, &segment));
    g_assert_false(adg_trail_put_segment(trail, 0, &segment));
    g_assert_true(adg_trail_put_segment(trail, 1, NULL));

    /* Check segment browsing */
    g_assert_true(adg_trail_put_segment(trail, 1, &segment));
    g_assert_cmpint(segment.num_data, ==, 4);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);

    g_assert_true(adg_trail_put_segment(trail, 2, &segment));
    g_assert_cmpint(segment.num_data, ==, 7);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_CURVE);
    g_assert_cmpint(segment.data[6].header.type, ==, CPML_CLOSE);

    g_assert_true(adg_trail_put_segment(trail, 3, &segment));
    g_assert_cmpint(segment.num_data, ==, 6);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_ARC);
    g_assert_cmpint(segment.data[5].header.type, ==, CPML_CLOSE);

    g_assert_false(adg_trail_put_segment(trail, 4, &segment));

    g_object_unref(path);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/trail/type/object", ADG_TYPE_TRAIL);

    g_test_add_func("/adg/trail/property/max-angle", _adg_test_max_angle);

    g_test_add_func("/adg/trail/method/put-segment", _adg_test_put_segment);

    return g_test_run();
}
