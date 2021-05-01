/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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
    AdgRDim *rdim;

    /* Check that the dimension dress for AdgRDim instances is
     * ADG_DRESS_DIMENSION by default */
    rdim = adg_rdim_new();
    g_assert_cmpint(adg_dim_get_dim_dress(ADG_DIM(rdim)), ==, ADG_DRESS_DIMENSION);

    adg_entity_destroy(ADG_ENTITY(rdim));
}

static void
_adg_method_new_full_from_model(void)
{
    AdgModel *model;
    AdgDim *dim;
    CpmlPair *pair;

    model = ADG_MODEL(adg_path_new());
    adg_model_set_named_pair_explicit(model, "P1", 1, 2);
    adg_model_set_named_pair_explicit(model, "P2", 3, 4);
    adg_model_set_named_pair_explicit(model, "P3", 5, 6);

    /* Sanity check */
    dim = ADG_DIM(adg_rdim_new_full_from_model(NULL, "P1", "P2", "P3"));
    g_assert_null(dim);


    /* The center point corresponds to AdgDim:ref1, the radius to
     * AdgDim:ref2 and pos to AdgDim:pos */
    dim = ADG_DIM(adg_rdim_new_full_from_model(model, NULL, NULL, NULL));
    g_assert_nonnull(dim);

    pair = (CpmlPair *) adg_dim_get_ref1(dim);
    g_assert_null(pair);

    pair = (CpmlPair *) adg_dim_get_ref2(dim);
    g_assert_null(pair);

    pair = (CpmlPair *) adg_dim_get_pos(dim);
    g_assert_null(pair);

    adg_entity_destroy(ADG_ENTITY(dim));


    dim = ADG_DIM(adg_rdim_new_full_from_model(model, "P1", "P2", "P3"));

    pair = (CpmlPair *) adg_dim_get_ref1(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);
    g_assert_true(adg_point_update((AdgPoint *) pair));
    adg_assert_isapprox(pair->x, 1);
    adg_assert_isapprox(pair->y, 2);

    pair = (CpmlPair *) adg_dim_get_ref2(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);
    g_assert_true(adg_point_update((AdgPoint *) pair));
    adg_assert_isapprox(pair->x, 3);
    adg_assert_isapprox(pair->y, 4);

    pair = (CpmlPair *) adg_dim_get_pos(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);
    g_assert_true(adg_point_update((AdgPoint *) pair));
    adg_assert_isapprox(pair->x, 5);
    adg_assert_isapprox(pair->y, 6);

    /* Manually change all the points */
    adg_dim_set_ref1_from_model(dim, model, "P3");
    adg_dim_set_ref2_from_model(dim, model, "P1");
    adg_dim_set_pos_from_model(dim, model, "P2");

    /* The point will be updated only after an invalidate */
    pair = (CpmlPair *) adg_dim_get_ref1(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);

    pair = (CpmlPair *) adg_dim_get_ref2(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);

    pair = (CpmlPair *) adg_dim_get_pos(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 0);
    adg_assert_isapprox(pair->y, 0);

    adg_entity_arrange(ADG_ENTITY(dim));

    pair = (CpmlPair *) adg_dim_get_ref1(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 5);
    adg_assert_isapprox(pair->y, 6);

    pair = (CpmlPair *) adg_dim_get_ref2(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 1);
    adg_assert_isapprox(pair->y, 2);

    pair = (CpmlPair *) adg_dim_get_pos(dim);
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 3);
    adg_assert_isapprox(pair->y, 4);

    adg_entity_destroy(ADG_ENTITY(dim));

    g_object_unref(model);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/rdim/type/object", ADG_TYPE_RDIM);
    adg_test_add_entity_checks("/adg/rdim/type/entity", ADG_TYPE_RDIM);

    adg_test_add_global_space_checks("/adg/rdim/behavior/global-space",
                                    adg_rdim_new_full_explicit(1, 5, 2, 4, 9, 7));
    /* Radial dimension does not enlarge on local space changes
     * adg_test_add_local_space_checks("/adg/rdim/behavior/local-space",
     *                                 adg_rdim_new_full_explicit(1, 5, 2, 4, 9, 7));
     */

    g_test_add_func("/adg/adim/property/dim-dress", _adg_property_dim_dress);

    g_test_add_func("/adg/rdim/method/new-full-from-model", _adg_method_new_full_from_model);

    return g_test_run();
}
