/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
_adg_property_fill_dress(void)
{
    AdgHatch *hatch;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress fill_dress;

    hatch = adg_hatch_new(NULL);
    valid_dress_1 = ADG_DRESS_FILL;
    valid_dress_2 = ADG_DRESS_FILL_HATCH;
    incompatible_dress = ADG_DRESS_FONT_TEXT;

    /* Using the public APIs */
    adg_hatch_set_fill_dress(hatch, valid_dress_1);
    fill_dress = adg_hatch_get_fill_dress(hatch);
    g_assert_cmpint(fill_dress, ==, valid_dress_1);

    adg_hatch_set_fill_dress(hatch, incompatible_dress);
    fill_dress = adg_hatch_get_fill_dress(hatch);
    g_assert_cmpint(fill_dress, ==, valid_dress_1);

    adg_hatch_set_fill_dress(hatch, valid_dress_2);
    fill_dress = adg_hatch_get_fill_dress(hatch);
    g_assert_cmpint(fill_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(hatch, "fill-dress", valid_dress_1, NULL);
    g_object_get(hatch, "fill-dress", &fill_dress, NULL);
    g_assert_cmpint(fill_dress, ==, valid_dress_1);

    g_object_set(hatch, "fill-dress", incompatible_dress, NULL);
    g_object_get(hatch, "fill-dress", &fill_dress, NULL);
    g_assert_cmpint(fill_dress, ==, valid_dress_1);

    g_object_set(hatch, "fill-dress", valid_dress_2, NULL);
    g_object_get(hatch, "fill-dress", &fill_dress, NULL);
    g_assert_cmpint(fill_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(hatch));
}


int
main(int argc, char *argv[])
{
    AdgPath *path;

    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/hatch/type/object", ADG_TYPE_HATCH);
    adg_test_add_entity_checks("/adg/hatch/type/entity", ADG_TYPE_HATCH);

    path = adg_path_new();
    adg_path_move_to_explicit(path, 1, 2);
    adg_path_line_to_explicit(path, 4, 5);
    adg_path_line_to_explicit(path, 7, 8);
    adg_path_close(path);
    adg_test_add_global_space_checks("/adg/hatch/behavior/global-space", adg_hatch_new(ADG_TRAIL(path)));
    adg_test_add_local_space_checks("/adg/hatch/behavior/local-space", adg_hatch_new(ADG_TRAIL(path)));
    g_object_unref(path);

    g_test_add_func("/adg/hatch/property/fill-dress", _adg_property_fill_dress);

    return g_test_run();
}
