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


static void
_adg_test_generic(void)
{
    AdgPoint *point, *dup_point;
    CpmlPair *pair,  *dup_pair;
    CpmlPair dummy_pair = { 3.4, 5.6 };

    point = adg_point_new();
    g_assert_nonnull(point);

    adg_point_set_pair_explicit(point, 1, 2);
    pair = (CpmlPair *) point;
    g_assert_cmpfloat(pair->x, ==, 1);
    g_assert_cmpfloat(pair->y, ==, 2);

    pair = adg_point_get_pair(point);
    g_assert_nonnull(pair);
    g_assert_cmpfloat(pair->x, ==, 1);
    g_assert_cmpfloat(pair->y, ==, 2);

    dup_point = adg_point_dup(point);
    g_assert_nonnull(dup_point);
    g_assert_true(dup_point != point);

    /* Should be a noop with explicit pairs */
    adg_point_invalidate(point);

    dup_pair = adg_point_get_pair(dup_point);
    g_assert_nonnull(dup_pair);
    g_assert_true(dup_pair != pair);

    g_assert_cmpfloat(pair->x, ==, dup_pair->x);
    g_assert_cmpfloat(pair->y, ==, dup_pair->y);
    g_assert_true(adg_point_equal(point, dup_point));

    g_free(dup_pair);
    adg_point_destroy(dup_point);

    dup_point = adg_point_new();
    adg_point_set_pair(dup_point, &dummy_pair);
    dup_pair = (CpmlPair *) dup_point;

    /* Should be a noop with explicit pairs */
    adg_point_invalidate(dup_point);

    g_assert_cmpfloat(dup_pair->x, ==, 3.4);
    g_assert_cmpfloat(dup_pair->y, ==, 5.6);
    g_assert_false(adg_point_equal(point, dup_point));

    adg_point_copy(dup_point, point);
    dup_pair = adg_point_get_pair(dup_point);
    g_assert_nonnull(dup_pair);
    g_assert_nonnull(dup_pair);

    g_assert_cmpfloat(pair->x, ==, dup_pair->x);
    g_assert_cmpfloat(pair->y, ==, dup_pair->y);
    g_assert_true(adg_point_equal(point, dup_point));

    g_free(pair);
    g_free(dup_pair);
    adg_point_destroy(point);
    adg_point_destroy(dup_point);
}

static void
_adg_test_named_pair(void)
{
    CpmlPair p1 = { 123, 456 };
    AdgPoint *explicit_point, *explicit_point2, *model_point;
    AdgModel *model;
    CpmlPair *pair;

    explicit_point = adg_point_new();
    g_assert_nonnull(explicit_point);
    adg_point_set_pair(explicit_point, &p1);

    explicit_point2 = adg_point_new();
    g_assert_nonnull(explicit_point2);
    adg_point_set_pair_explicit(explicit_point2, p1.x, p1.y);

    /* Checking comparison APIs */
    g_assert_true(adg_point_equal(explicit_point, explicit_point2));
    adg_point_set_pair_explicit(explicit_point2, 78, 90);
    g_assert_false(adg_point_equal(explicit_point, explicit_point2));
    pair = (CpmlPair *) explicit_point2;
    g_assert_cmpfloat(pair->x, ==, 78);
    g_assert_cmpfloat(pair->y, ==, 90);

    pair = adg_point_get_pair(explicit_point);
    g_assert_true(cpml_pair_equal(pair, &p1));
    g_free(pair);

    model = ADG_MODEL(adg_path_new());
    g_assert_nonnull(model);
    adg_model_set_named_pair(model, "named-pair", &p1);

    model_point = adg_point_new();
    g_assert_nonnull(model_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    pair = adg_point_get_pair(model_point);
    g_assert_true(cpml_pair_equal(pair, &p1));
    g_free(pair);

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    /* Check for lazy evaluation of named pairs */
    adg_point_set_pair_from_model(model_point, model, "unexistent-pair");
    pair = (CpmlPair *) model_point;
    g_assert_cmpfloat(pair->x, ==, p1.x);
    g_assert_cmpfloat(pair->y, ==, p1.y);

    /* Check behavior on undefined named pair */
    g_assert_false(adg_point_update(model_point));
    g_assert_null(adg_point_get_pair(model_point));

    adg_point_set_pair_from_model(model_point, model, "named-pair");
    g_assert_true(adg_point_update(model_point));

    /* Check for case sensitiveness */
    adg_point_set_pair_from_model(model_point, model, "Named-Pair");
    g_assert_null(adg_point_get_pair(model_point));
    g_assert_false(adg_point_update(model_point));

    /* Check if adg_point_get_pair() triggers an adg_point_update() */
    adg_point_set_pair_from_model(model_point, model, "named-pair");
    pair = adg_point_get_pair(model_point);
    g_assert_true(cpml_pair_equal(pair, &p1));
    g_free(pair);

    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(model);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_boxed_checks("/adg/point/type/boxed", ADG_TYPE_POINT, adg_point_new());

    g_test_add_func("/adg/point/behavior/generic", _adg_test_generic);
    g_test_add_func("/adg/point/behavior/named-pair", _adg_test_named_pair);

    return g_test_run();
}
