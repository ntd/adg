/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
_adg_test_fill_dress(void)
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
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/hatch/fill-dress", _adg_test_fill_dress);

    return g_test_run();
}
