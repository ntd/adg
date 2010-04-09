/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010  Nicola Fontana <ntd at entidi.it>
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
_adg_test_detached(void)
{
    AdgDim *dim;
    AdgThreeState valid_three_state_1, valid_three_state_2, invalid_three_state;
    AdgThreeState detached;

    dim = ADG_DIM(adg_adim_new());
    valid_three_state_1 = ADG_THREE_STATE_UNKNOWN;
    valid_three_state_2 = ADG_THREE_STATE_OFF;
    invalid_three_state = 1234;

    /* Using the public APIs */
    adg_dim_set_detached(dim, valid_three_state_1);
    detached = adg_dim_get_detached(dim);
    g_assert_cmpint(detached, ==, valid_three_state_1);

    adg_dim_set_detached(dim, invalid_three_state);
    detached = adg_dim_get_detached(dim);
    g_assert_cmpint(detached, ==, valid_three_state_1);

    adg_dim_set_detached(dim, valid_three_state_2);
    detached = adg_dim_get_detached(dim);
    g_assert_cmpint(detached, ==, valid_three_state_2);

    /* Using GObject property methods */
    g_object_set(dim, "detached", valid_three_state_1, NULL);
    g_object_get(dim, "detached", &detached, NULL);
    g_assert_cmpint(detached, ==, valid_three_state_1);

    g_object_set(dim, "detached", invalid_three_state, NULL);
    g_object_get(dim, "detached", &detached, NULL);
    g_assert_cmpint(detached, ==, valid_three_state_1);

    g_object_set(dim, "detached", valid_three_state_2, NULL);
    g_object_get(dim, "detached", &detached, NULL);
    g_assert_cmpint(detached, ==, valid_three_state_2);

    g_object_unref(dim);
}

static void
_adg_test_dim_dress(void)
{
    AdgDim *dim;
    AdgDress valid_dress, incompatible_dress;
    AdgDress dim_dress;

    dim = ADG_DIM(adg_adim_new());
    valid_dress = ADG_DRESS_DIMENSION;
    incompatible_dress = ADG_DRESS_LINE;

    /* Using the public APIs */
    adg_dim_set_dim_dress(dim, valid_dress);
    dim_dress = adg_dim_get_dim_dress(dim);
    g_assert_cmpint(dim_dress, ==, valid_dress);

    adg_dim_set_dim_dress(dim, incompatible_dress);
    dim_dress = adg_dim_get_dim_dress(dim);
    g_assert_cmpint(dim_dress, ==, valid_dress);

    /* Using GObject property methods */
    g_object_set(dim, "dim-dress", valid_dress, NULL);
    g_object_get(dim, "dim-dress", &dim_dress, NULL);
    g_assert_cmpint(dim_dress, ==, valid_dress);

    g_object_set(dim, "dim-dress", incompatible_dress, NULL);
    g_object_get(dim, "dim-dress", &dim_dress, NULL);
    g_assert_cmpint(dim_dress, ==, valid_dress);

    g_object_unref(dim);
}

static void
_adg_test_level(void)
{
    AdgDim *dim;
    gdouble valid_value_1, valid_value_2;
    gdouble level;

    dim = ADG_DIM(adg_ldim_new());
    valid_value_1 = 4321;
    valid_value_2 = 1234;

    /* Using the public APIs */
    adg_dim_set_level(dim, valid_value_1);
    level = adg_dim_get_level(dim);
    g_assert_cmpfloat(level, ==, valid_value_1);

    adg_dim_set_level(dim, valid_value_2);
    level = adg_dim_get_level(dim);
    g_assert_cmpfloat(level, ==, valid_value_2);

    /* Using GObject property methods */
    g_object_set(dim, "level", valid_value_1, NULL);
    g_object_get(dim, "level", &level, NULL);
    g_assert_cmpfloat(level, ==, valid_value_1);

    g_object_set(dim, "level", valid_value_2, NULL);
    g_object_get(dim, "level", &level, NULL);
    g_assert_cmpfloat(level, ==, valid_value_2);

    g_object_unref(dim);
}

static void
_adg_test_max(void)
{
    AdgDim *dim;
    const gchar *valid_text, *latin1_text;
    const gchar *max;
    gchar *max_dup;

    dim = ADG_DIM(adg_ldim_new());
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_dim_set_max(dim, valid_text);
    max = adg_dim_get_max(dim);
    g_assert_cmpstr(max, ==, valid_text);

    adg_dim_set_max(dim, latin1_text);
    max = adg_dim_get_max(dim);
    g_assert_cmpstr(max, ==, latin1_text);

    adg_dim_set_max(dim, NULL);
    max = adg_dim_get_max(dim);
    g_assert(max == NULL);

    /* Using GObject property methods */
    g_object_set(dim, "max", valid_text, NULL);
    g_object_get(dim, "max", &max_dup, NULL);
    g_assert_cmpstr(max_dup, ==, valid_text);
    g_free(max_dup);

    g_object_set(dim, "max", latin1_text, NULL);
    g_object_get(dim, "max", &max_dup, NULL);
    g_assert_cmpstr(max_dup, ==, latin1_text);
    g_free(max_dup);

    g_object_set(dim, "max", NULL, NULL);
    g_object_get(dim, "max", &max_dup, NULL);
    g_assert(max_dup == NULL);

    g_object_unref(dim);
}

static void
_adg_test_min(void)
{
    AdgDim *dim;
    const gchar *valid_text, *latin1_text;
    const gchar *min;
    gchar *min_dup;

    dim = ADG_DIM(adg_ldim_new());
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_dim_set_min(dim, valid_text);
    min = adg_dim_get_min(dim);
    g_assert_cmpstr(min, ==, valid_text);

    adg_dim_set_min(dim, latin1_text);
    min = adg_dim_get_min(dim);
    g_assert_cmpstr(min, ==, latin1_text);

    adg_dim_set_min(dim, NULL);
    min = adg_dim_get_min(dim);
    g_assert(min == NULL);

    /* Using GObject property methods */
    g_object_set(dim, "min", valid_text, NULL);
    g_object_get(dim, "min", &min_dup, NULL);
    g_assert_cmpstr(min_dup, ==, valid_text);
    g_free(min_dup);

    g_object_set(dim, "min", latin1_text, NULL);
    g_object_get(dim, "min", &min_dup, NULL);
    g_assert_cmpstr(min_dup, ==, latin1_text);
    g_free(min_dup);

    g_object_set(dim, "min", NULL, NULL);
    g_object_get(dim, "min", &min_dup, NULL);
    g_assert(min_dup == NULL);

    g_object_unref(dim);
}

static void
_adg_test_outside(void)
{
    AdgDim *dim;
    AdgThreeState valid_three_state_1, valid_three_state_2, invalid_three_state;
    AdgThreeState outside;

    dim = ADG_DIM(adg_rdim_new());
    valid_three_state_1 = ADG_THREE_STATE_UNKNOWN;
    valid_three_state_2 = ADG_THREE_STATE_OFF;
    invalid_three_state = 1234;

    /* Using the public APIs */
    adg_dim_set_outside(dim, valid_three_state_1);
    outside = adg_dim_get_outside(dim);
    g_assert_cmpint(outside, ==, valid_three_state_1);

    adg_dim_set_outside(dim, invalid_three_state);
    outside = adg_dim_get_outside(dim);
    g_assert_cmpint(outside, ==, valid_three_state_1);

    adg_dim_set_outside(dim, valid_three_state_2);
    outside = adg_dim_get_outside(dim);
    g_assert_cmpint(outside, ==, valid_three_state_2);

    /* Using GObject property methods */
    g_object_set(dim, "outside", valid_three_state_1, NULL);
    g_object_get(dim, "outside", &outside, NULL);
    g_assert_cmpint(outside, ==, valid_three_state_1);

    g_object_set(dim, "outside", invalid_three_state, NULL);
    g_object_get(dim, "outside", &outside, NULL);
    g_assert_cmpint(outside, ==, valid_three_state_1);

    g_object_set(dim, "outside", valid_three_state_2, NULL);
    g_object_get(dim, "outside", &outside, NULL);
    g_assert_cmpint(outside, ==, valid_three_state_2);

    g_object_unref(dim);
}

static void
_adg_test_pos(void)
{
    AdgDim *dim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *pos;

    dim = ADG_DIM(adg_rdim_new());
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair",
                             adg_point_get_pair(explicit_point));
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert(!adg_point_equal(explicit_point, model_point));

    pos = adg_dim_get_pos(dim);
    g_assert(pos == NULL);

    /* Using the public APIs */
    adg_dim_set_pos_explicit(dim, 0, 0);
    pos = adg_dim_get_pos(dim);
    g_assert(adg_point_equal(pos, origin));

    adg_dim_set_pos(dim, NULL);
    pos = adg_dim_get_pos(dim);
    g_assert(pos == NULL);

    adg_dim_set_pos(dim, explicit_point);
    pos = adg_dim_get_pos(dim);
    g_assert(adg_point_equal(pos, explicit_point));

    /* Checking the lazy assignment feature */
    adg_dim_set_pos_from_model(dim, model, "dummy");
    pos = adg_dim_get_pos(dim);
    g_assert(pos != NULL);
    g_assert(adg_point_get_pair(pos) == NULL);

    adg_dim_set_pos_from_model(dim, model, "named-pair");
    pos = adg_dim_get_pos(dim);
    g_assert(adg_point_equal(pos, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "pos", origin, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert(adg_point_equal(pos, origin));
    adg_point_destroy(pos);

    g_object_set(dim, "pos", NULL, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert(pos == NULL);

    g_object_set(dim, "pos", explicit_point, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert(adg_point_equal(pos, explicit_point));
    adg_point_destroy(pos);

    /* Checking the lazy assignment feature */
    adg_dim_set_pos_from_model(dim, model, "dummy");
    g_object_get(dim, "pos", &pos, NULL);
    g_assert(pos != NULL);
    g_assert(adg_point_get_pair(pos) == NULL);
    adg_point_destroy(pos);

    g_object_set(dim, "pos", model_point, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert(adg_point_equal(pos, model_point));
    adg_point_destroy(pos);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(dim);
    g_object_unref(model);
}

static void
_adg_test_ref1(void)
{
    AdgDim *dim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *ref1;

    dim = ADG_DIM(adg_rdim_new());
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair",
                             adg_point_get_pair(explicit_point));
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert(!adg_point_equal(explicit_point, model_point));

    ref1 = adg_dim_get_ref1(dim);
    g_assert(ref1 == NULL);

    /* Using the public APIs */
    adg_dim_set_ref1_explicit(dim, 0, 0);
    ref1 = adg_dim_get_ref1(dim);
    g_assert(adg_point_equal(ref1, origin));

    adg_dim_set_ref1(dim, NULL);
    ref1 = adg_dim_get_ref1(dim);
    g_assert(ref1 == NULL);

    adg_dim_set_ref1(dim, explicit_point);
    ref1 = adg_dim_get_ref1(dim);
    g_assert(adg_point_equal(ref1, explicit_point));

    /* Checking the lazy assignment feature */
    adg_dim_set_ref1_from_model(dim, model, "dummy");
    ref1 = adg_dim_get_ref1(dim);
    g_assert(ref1 != NULL);
    g_assert(adg_point_get_pair(ref1) == NULL);

    adg_dim_set_ref1_from_model(dim, model, "named-pair");
    ref1 = adg_dim_get_ref1(dim);
    g_assert(adg_point_equal(ref1, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "ref1", origin, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert(adg_point_equal(ref1, origin));
    adg_point_destroy(ref1);

    g_object_set(dim, "ref1", NULL, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert(ref1 == NULL);

    g_object_set(dim, "ref1", explicit_point, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert(adg_point_equal(ref1, explicit_point));
    adg_point_destroy(ref1);

    /* Checking the lazy assignment feature */
    adg_dim_set_ref1_from_model(dim, model, "dummy");
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert(ref1 != NULL);
    g_assert(adg_point_get_pair(ref1) == NULL);
    adg_point_destroy(ref1);

    g_object_set(dim, "ref1", model_point, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert(adg_point_equal(ref1, model_point));
    adg_point_destroy(ref1);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(dim);
    g_object_unref(model);
}

static void
_adg_test_ref2(void)
{
    AdgDim *dim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *ref2;

    dim = ADG_DIM(adg_rdim_new());
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair",
                             adg_point_get_pair(explicit_point));
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert(!adg_point_equal(explicit_point, model_point));

    ref2 = adg_dim_get_ref2(dim);
    g_assert(ref2 == NULL);

    /* Using the public APIs */
    adg_dim_set_ref2_explicit(dim, 0, 0);
    ref2 = adg_dim_get_ref2(dim);
    g_assert(adg_point_equal(ref2, origin));

    adg_dim_set_ref2(dim, NULL);
    ref2 = adg_dim_get_ref2(dim);
    g_assert(ref2 == NULL);

    adg_dim_set_ref2(dim, explicit_point);
    ref2 = adg_dim_get_ref2(dim);
    g_assert(adg_point_equal(ref2, explicit_point));

    /* Checking the lazy assignment feature */
    adg_dim_set_ref2_from_model(dim, model, "dummy");
    ref2 = adg_dim_get_ref2(dim);
    g_assert(ref2 != NULL);
    g_assert(adg_point_get_pair(ref2) == NULL);

    adg_dim_set_ref2_from_model(dim, model, "named-pair");
    ref2 = adg_dim_get_ref2(dim);
    g_assert(adg_point_equal(ref2, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "ref2", origin, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert(adg_point_equal(ref2, origin));
    adg_point_destroy(ref2);

    g_object_set(dim, "ref2", NULL, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert(ref2 == NULL);

    g_object_set(dim, "ref2", explicit_point, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert(adg_point_equal(ref2, explicit_point));
    adg_point_destroy(ref2);

    /* Checking the lazy assignment feature */
    adg_dim_set_ref2_from_model(dim, model, "dummy");
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert(ref2 != NULL);
    g_assert(adg_point_get_pair(ref2) == NULL);
    adg_point_destroy(ref2);

    g_object_set(dim, "ref2", model_point, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert(adg_point_equal(ref2, model_point));
    adg_point_destroy(ref2);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(dim);
    g_object_unref(model);
}

static void
_adg_test_value(void)
{
    AdgDim *dim;
    const gchar *valid_text, *latin1_text;
    const gchar *value;
    gchar *value_dup;

    dim = ADG_DIM(adg_ldim_new());
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_dim_set_value(dim, valid_text);
    value = adg_dim_get_value(dim);
    g_assert_cmpstr(value, ==, valid_text);

    adg_dim_set_value(dim, latin1_text);
    value = adg_dim_get_value(dim);
    g_assert_cmpstr(value, ==, latin1_text);

    adg_dim_set_value(dim, NULL);
    value = adg_dim_get_value(dim);
    g_assert(value == NULL);

    /* Using GObject property methods */
    g_object_set(dim, "value", valid_text, NULL);
    g_object_get(dim, "value", &value_dup, NULL);
    g_assert_cmpstr(value_dup, ==, valid_text);
    g_free(value_dup);

    g_object_set(dim, "value", latin1_text, NULL);
    g_object_get(dim, "value", &value_dup, NULL);
    g_assert_cmpstr(value_dup, ==, latin1_text);
    g_free(value_dup);

    g_object_set(dim, "value", NULL, NULL);
    g_object_get(dim, "value", &value_dup, NULL);
    g_assert(value_dup == NULL);

    g_object_unref(dim);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/dim/detached", _adg_test_detached);
    adg_test_add_func("/adg/dim/dim-dress", _adg_test_dim_dress);
    adg_test_add_func("/adg/dim/level", _adg_test_level);
    adg_test_add_func("/adg/dim/max", _adg_test_max);
    adg_test_add_func("/adg/dim/min", _adg_test_min);
    adg_test_add_func("/adg/dim/outside", _adg_test_outside);
    adg_test_add_func("/adg/dim/pos", _adg_test_pos);
    adg_test_add_func("/adg/dim/ref1", _adg_test_ref1);
    adg_test_add_func("/adg/dim/ref2", _adg_test_ref2);
    adg_test_add_func("/adg/dim/value", _adg_test_value);

    return g_test_run();
}
