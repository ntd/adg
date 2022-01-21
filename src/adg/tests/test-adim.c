/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
_adg_property_dim_dress(void)
{
    AdgADim *adim;

    /* Check that the dimension dress for AdgADim instances is
     * ADG_DRESS_DIMENSION by default */
    adim = adg_adim_new();
    g_assert_cmpint(adg_dim_get_dim_dress(ADG_DIM(adim)), ==, ADG_DRESS_DIMENSION);

    adg_entity_destroy(ADG_ENTITY(adim));
}

static void
_adg_property_org1(void)
{
    AdgADim *adim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *org1;

    adim = adg_adim_new();
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair", (CpmlPair *) explicit_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    org1 = adg_adim_get_org1(adim);
    g_assert_null(org1);

    /* Using the public APIs */
    adg_adim_set_org1_explicit(adim, 0, 0);
    org1 = adg_adim_get_org1(adim);
    g_assert_true(adg_point_equal(org1, origin));

    adg_adim_set_org1(adim, NULL);
    org1 = adg_adim_get_org1(adim);
    g_assert_null(org1);

    adg_adim_set_org1(adim, explicit_point);
    org1 = adg_adim_get_org1(adim);
    g_assert_true(adg_point_equal(org1, explicit_point));

    adg_adim_set_org1_from_model(adim, model, "dummy");
    org1 = adg_adim_get_org1(adim);
    g_assert_nonnull(org1);

    adg_adim_set_org1_from_model(adim, model, "named-pair");
    org1 = adg_adim_get_org1(adim);
    g_assert_true(adg_point_equal(org1, model_point));

    /* Using GObject property methods */
    g_object_set(adim, "org1", origin, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert_true(adg_point_equal(org1, origin));
    adg_point_destroy(org1);

    g_object_set(adim, "org1", NULL, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert_null(org1);

    g_object_set(adim, "org1", explicit_point, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert_true(adg_point_equal(org1, explicit_point));
    adg_point_destroy(org1);

    adg_adim_set_org1_from_model(adim, model, "dummy");
    g_object_get(adim, "org1", &org1, NULL);
    g_assert_nonnull(org1);
    adg_point_destroy(org1);

    g_object_set(adim, "org1", model_point, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert_true(adg_point_equal(org1, model_point));
    adg_point_destroy(org1);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    adg_entity_destroy(ADG_ENTITY(adim));
    g_object_unref(model);
}

static void
_adg_property_org2(void)
{
    AdgADim *adim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *org2;

    adim = adg_adim_new();
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair", (CpmlPair *) explicit_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    org2 = adg_adim_get_org2(adim);
    g_assert_null(org2);

    /* Using the public APIs */
    adg_adim_set_org2_explicit(adim, 0, 0);
    org2 = adg_adim_get_org2(adim);
    g_assert_true(adg_point_equal(org2, origin));

    adg_adim_set_org2(adim, NULL);
    org2 = adg_adim_get_org2(adim);
    g_assert_null(org2);

    adg_adim_set_org2(adim, explicit_point);
    org2 = adg_adim_get_org2(adim);
    g_assert_true(adg_point_equal(org2, explicit_point));

    adg_adim_set_org2_from_model(adim, model, "dummy");
    org2 = adg_adim_get_org2(adim);
    g_assert_nonnull(org2);

    adg_adim_set_org2_from_model(adim, model, "named-pair");
    org2 = adg_adim_get_org2(adim);
    g_assert_true(adg_point_equal(org2, model_point));

    /* Using GObject property methods */
    g_object_set(adim, "org2", origin, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert_true(adg_point_equal(org2, origin));
    adg_point_destroy(org2);

    g_object_set(adim, "org2", NULL, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert_null(org2);

    g_object_set(adim, "org2", explicit_point, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert_true(adg_point_equal(org2, explicit_point));
    adg_point_destroy(org2);

    adg_adim_set_org2_from_model(adim, model, "dummy");
    g_object_get(adim, "org2", &org2, NULL);
    g_assert_nonnull(org2);
    adg_point_destroy(org2);

    g_object_set(adim, "org2", model_point, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert_true(adg_point_equal(org2, model_point));
    adg_point_destroy(org2);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    adg_entity_destroy(ADG_ENTITY(adim));
    g_object_unref(model);
}

static void
_adg_property_has_extension1(void)
{
    AdgADim *adim;
    gboolean invalid_boolean;
    gboolean has_extension1;

    adim = adg_adim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_adim_switch_extension1(adim, FALSE);
    has_extension1 = adg_adim_has_extension1(adim);
    g_assert_false(has_extension1);

    adg_adim_switch_extension1(adim, invalid_boolean);
    has_extension1 = adg_adim_has_extension1(adim);
    g_assert_false(has_extension1);

    adg_adim_switch_extension1(adim, TRUE);
    has_extension1 = adg_adim_has_extension1(adim);
    g_assert_true(has_extension1);

    /* Using GObject property methods */
    g_object_set(adim, "has-extension1", FALSE, NULL);
    g_object_get(adim, "has-extension1", &has_extension1, NULL);
    g_assert_false(has_extension1);

    g_object_set(adim, "has-extension1", invalid_boolean, NULL);
    g_object_get(adim, "has-extension1", &has_extension1, NULL);
    g_assert_false(has_extension1);

    g_object_set(adim, "has-extension1", TRUE, NULL);
    g_object_get(adim, "has-extension1", &has_extension1, NULL);
    g_assert_true(has_extension1);

    adg_entity_destroy(ADG_ENTITY(adim));
}

static void
_adg_property_has_extension2(void)
{
    AdgADim *adim;
    gboolean invalid_boolean;
    gboolean has_extension2;

    adim = adg_adim_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_adim_switch_extension2(adim, FALSE);
    has_extension2 = adg_adim_has_extension2(adim);
    g_assert_false(has_extension2);

    adg_adim_switch_extension2(adim, invalid_boolean);
    has_extension2 = adg_adim_has_extension2(adim);
    g_assert_false(has_extension2);

    adg_adim_switch_extension2(adim, TRUE);
    has_extension2 = adg_adim_has_extension2(adim);
    g_assert_true(has_extension2);

    /* Using GObject property methods */
    g_object_set(adim, "has-extension2", FALSE, NULL);
    g_object_get(adim, "has-extension2", &has_extension2, NULL);
    g_assert_false(has_extension2);

    g_object_set(adim, "has-extension2", invalid_boolean, NULL);
    g_object_get(adim, "has-extension2", &has_extension2, NULL);
    g_assert_false(has_extension2);

    g_object_set(adim, "has-extension2", TRUE, NULL);
    g_object_get(adim, "has-extension2", &has_extension2, NULL);
    g_assert_true(has_extension2);

    adg_entity_destroy(ADG_ENTITY(adim));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/adim/type/object", ADG_TYPE_ADIM);
    adg_test_add_entity_checks("/adg/adim/type/entity", ADG_TYPE_ADIM);

    adg_test_add_global_space_checks("/adg/adim/behavior/global-space",
                                     adg_adim_new_full_explicit(1, 2, 3, 4,
                                                                9, 6, 7, 8,
                                                                9, 10));
    adg_test_add_local_space_checks("/adg/adim/behavior/local-space",
                                    adg_adim_new_full_explicit(4, 3, 2, 1,
                                                               8, 7, 6, 9,
                                                               10, 9));

    g_test_add_func("/adg/adim/property/dim-dress", _adg_property_dim_dress);
    g_test_add_func("/adg/adim/property/org1", _adg_property_org1);
    g_test_add_func("/adg/adim/property/org2", _adg_property_org2);
    g_test_add_func("/adg/adim/property/has-extension1", _adg_property_has_extension1);
    g_test_add_func("/adg/adim/property/has-extension2", _adg_property_has_extension2);

    return g_test_run();
}
