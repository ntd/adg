/* ADG - Automatic Drawing Generation
 * Copyright (C) 2011,2012  Nicola Fontana <ntd at entidi.it>
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


static CpmlPath *
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
    static CpmlPath path = {
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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/trail/max-angle", _adg_test_max_angle);

    return g_test_run();
}
