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
_adg_test_direction(void)
{
    AdgLDim *ldim;
    gdouble valid_value, invalid_value;
    gdouble direction;

    ldim = adg_ldim_new();
    valid_value = -G_PI_2;
    invalid_value = G_PI + 1;

    /* Using the public APIs */
    adg_ldim_set_direction(ldim, valid_value);
    direction = adg_ldim_get_direction(ldim);
    g_assert_cmpfloat(direction, ==, valid_value);

    adg_ldim_set_direction(ldim, -G_PI);
    direction = adg_ldim_get_direction(ldim);
    g_assert_cmpfloat(direction, ==, G_PI);

    adg_ldim_set_direction(ldim, invalid_value);
    direction = adg_ldim_get_direction(ldim);
    g_assert_cmpfloat(direction, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(ldim, "direction", valid_value, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    g_assert_cmpfloat(direction, ==, valid_value);

    g_object_set(ldim, "direction", -G_PI, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    g_assert_cmpfloat(direction, ==, G_PI);

    g_object_set(ldim, "direction", invalid_value, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    g_assert_cmpfloat(direction, !=, invalid_value);

    adg_entity_destroy(ADG_ENTITY(ldim));
}

static void
_adg_test_has_extension1(void)
{
    AdgLDim *ldim;
    gboolean invalid_boolean;
    gboolean has_extension1;

    ldim = adg_ldim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_ldim_switch_extension1(ldim, FALSE);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert(!has_extension1);

    adg_ldim_switch_extension1(ldim, invalid_boolean);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert(!has_extension1);

    adg_ldim_switch_extension1(ldim, TRUE);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert(has_extension1);

    /* Using GObject property methods */
    g_object_set(ldim, "has-extension1", FALSE, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert(!has_extension1);

    g_object_set(ldim, "has-extension1", invalid_boolean, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert(!has_extension1);

    g_object_set(ldim, "has-extension1", TRUE, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert(has_extension1);

    adg_entity_destroy(ADG_ENTITY(ldim));
}

static void
_adg_test_has_extension2(void)
{
    AdgLDim *ldim;
    gboolean invalid_boolean;
    gboolean has_extension2;

    ldim = adg_ldim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_ldim_switch_extension2(ldim, FALSE);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert(!has_extension2);

    adg_ldim_switch_extension2(ldim, invalid_boolean);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert(!has_extension2);

    adg_ldim_switch_extension2(ldim, TRUE);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert(has_extension2);

    /* Using GObject property methods */
    g_object_set(ldim, "has-extension2", FALSE, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert(!has_extension2);

    g_object_set(ldim, "has-extension2", invalid_boolean, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert(!has_extension2);

    g_object_set(ldim, "has-extension2", TRUE, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert(has_extension2);

    adg_entity_destroy(ADG_ENTITY(ldim));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/ldim/property/direction", _adg_test_direction);
    adg_test_add_func("/adg/ldim/property/has-extension1", _adg_test_has_extension1);
    adg_test_add_func("/adg/ldim/property/has-extension2", _adg_test_has_extension2);

    return g_test_run();
}
