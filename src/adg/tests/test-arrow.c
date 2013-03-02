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
_adg_test_angle(void)
{
    AdgArrow *arrow;
    gdouble valid_value, invalid_value;
    gdouble angle;

    arrow = adg_arrow_new();
    valid_value = -G_PI_2;
    invalid_value = G_PI + 1;

    /* Using the public APIs */
    adg_arrow_set_angle(arrow, valid_value);
    angle = adg_arrow_get_angle(arrow);
    g_assert_cmpfloat(angle, ==, valid_value);

    adg_arrow_set_angle(arrow, -G_PI);
    angle = adg_arrow_get_angle(arrow);
    g_assert_cmpfloat(angle, ==, G_PI);

    adg_arrow_set_angle(arrow, invalid_value);
    angle = adg_arrow_get_angle(arrow);
    g_assert_cmpfloat(angle, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(arrow, "angle", valid_value, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, valid_value);

    g_object_set(arrow, "angle", -G_PI, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, G_PI);

    g_object_set(arrow, "angle", invalid_value, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, !=, invalid_value);

    adg_entity_destroy(ADG_ENTITY(arrow));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/arrow/angle", _adg_test_angle);

    return g_test_run();
}
