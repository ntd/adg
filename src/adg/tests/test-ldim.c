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
_adg_property_direction(void)
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
    adg_assert_isapprox(direction, valid_value);

    adg_ldim_set_direction(ldim, -G_PI);
    direction = adg_ldim_get_direction(ldim);
    adg_assert_isapprox(direction, G_PI);

    adg_ldim_set_direction(ldim, invalid_value);
    direction = adg_ldim_get_direction(ldim);
    g_assert_cmpfloat(direction, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(ldim, "direction", valid_value, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    adg_assert_isapprox(direction, valid_value);

    g_object_set(ldim, "direction", -G_PI, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    adg_assert_isapprox(direction, G_PI);

    g_object_set(ldim, "direction", invalid_value, NULL);
    g_object_get(ldim, "direction", &direction, NULL);
    g_assert_cmpfloat(direction, !=, invalid_value);

    adg_entity_destroy(ADG_ENTITY(ldim));

    /* Checking constructors */
    ldim = adg_ldim_new();
    direction = adg_ldim_get_direction(ldim);
    adg_assert_isapprox(direction, ADG_DIR_RIGHT);
    adg_entity_destroy(ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full(NULL, NULL, NULL, 2);
    direction = adg_ldim_get_direction(ldim);
    adg_assert_isapprox(direction, 2);
    adg_entity_destroy(ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_explicit(0, 1,
                                      2, 3,
                                      4, 5,
                                      3);
    direction = adg_ldim_get_direction(ldim);
    adg_assert_isapprox(direction, 3);
    adg_entity_destroy(ADG_ENTITY(ldim));
}

static void
_adg_property_has_extension1(void)
{
    AdgLDim *ldim;
    gboolean invalid_boolean;
    gboolean has_extension1;

    ldim = adg_ldim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_ldim_switch_extension1(ldim, FALSE);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert_false(has_extension1);

    adg_ldim_switch_extension1(ldim, invalid_boolean);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert_false(has_extension1);

    adg_ldim_switch_extension1(ldim, TRUE);
    has_extension1 = adg_ldim_has_extension1(ldim);
    g_assert_true(has_extension1);

    /* Using GObject property methods */
    g_object_set(ldim, "has-extension1", FALSE, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert_false(has_extension1);

    g_object_set(ldim, "has-extension1", invalid_boolean, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert_false(has_extension1);

    g_object_set(ldim, "has-extension1", TRUE, NULL);
    g_object_get(ldim, "has-extension1", &has_extension1, NULL);
    g_assert_true(has_extension1);

    adg_entity_destroy(ADG_ENTITY(ldim));
}

static void
_adg_property_has_extension2(void)
{
    AdgLDim *ldim;
    gboolean invalid_boolean;
    gboolean has_extension2;

    ldim = adg_ldim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_ldim_switch_extension2(ldim, FALSE);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert_false(has_extension2);

    adg_ldim_switch_extension2(ldim, invalid_boolean);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert_false(has_extension2);

    adg_ldim_switch_extension2(ldim, TRUE);
    has_extension2 = adg_ldim_has_extension2(ldim);
    g_assert_true(has_extension2);

    /* Using GObject property methods */
    g_object_set(ldim, "has-extension2", FALSE, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert_false(has_extension2);

    g_object_set(ldim, "has-extension2", invalid_boolean, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert_false(has_extension2);

    g_object_set(ldim, "has-extension2", TRUE, NULL);
    g_object_get(ldim, "has-extension2", &has_extension2, NULL);
    g_assert_true(has_extension2);

    adg_entity_destroy(ADG_ENTITY(ldim));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/ldim/type/object", ADG_TYPE_LDIM);
    adg_test_add_entity_checks("/adg/ldim/type/entity", ADG_TYPE_LDIM);

    /* Linear dimensions does not properly scale on global space
     * adg_test_add_global_space_checks("/adg/ldim/behavior/global-space",
     *                                  adg_ldim_new_full_explicit(1, 5,
     *                                                             2, 4,
     *                                                             9, 7,
     *                                                             ADG_DIR_UP));
     */
    adg_test_add_local_space_checks("/adg/ldim/behavior/local-space",
                                    adg_ldim_new_full_explicit(1, 5,
                                                               2, 4,
                                                               9, 7,
                                                               ADG_DIR_UP));

    g_test_add_func("/adg/ldim/property/direction", _adg_property_direction);
    g_test_add_func("/adg/ldim/property/has-extension1", _adg_property_has_extension1);
    g_test_add_func("/adg/ldim/property/has-extension2", _adg_property_has_extension2);

    return g_test_run();
}
