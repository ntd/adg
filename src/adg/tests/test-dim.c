/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
_adg_property_detached(void)
{
    AdgDim *dim;
    AdgThreeState valid_three_state_1, valid_three_state_2, invalid_three_state;
    AdgThreeState detached;

    dim = ADG_DIM(adg_ldim_new());
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

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_dim_dress(void)
{
    AdgDim *dim;
    AdgDress valid_dress, incompatible_dress;
    AdgDress dim_dress;

    dim = ADG_DIM(adg_ldim_new());
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

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_level(void)
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
    adg_assert_isapprox(level, valid_value_1);

    adg_dim_set_level(dim, valid_value_2);
    level = adg_dim_get_level(dim);
    adg_assert_isapprox(level, valid_value_2);

    /* Using GObject property methods */
    g_object_set(dim, "level", valid_value_1, NULL);
    g_object_get(dim, "level", &level, NULL);
    adg_assert_isapprox(level, valid_value_1);

    g_object_set(dim, "level", valid_value_2, NULL);
    g_object_get(dim, "level", &level, NULL);
    adg_assert_isapprox(level, valid_value_2);

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_max(void)
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
    g_assert_null(max);

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
    g_assert_null(max_dup);

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_min(void)
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
    g_assert_null(min);

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
    g_assert_null(min_dup);

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_outside(void)
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

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_property_pos(void)
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
    adg_model_set_named_pair(model, "named-pair", (CpmlPair *) explicit_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    pos = adg_dim_get_pos(dim);
    g_assert_null(pos);

    /* Using the public APIs */
    adg_dim_set_pos_explicit(dim, 0, 0);
    pos = adg_dim_get_pos(dim);
    g_assert_true(adg_point_equal(pos, origin));

    adg_dim_set_pos(dim, NULL);
    pos = adg_dim_get_pos(dim);
    g_assert_null(pos);

    adg_dim_set_pos(dim, explicit_point);
    pos = adg_dim_get_pos(dim);
    g_assert_true(adg_point_equal(pos, explicit_point));

    adg_dim_set_pos_from_model(dim, model, "dummy");
    pos = adg_dim_get_pos(dim);
    g_assert_nonnull(pos);

    adg_dim_set_pos_from_model(dim, model, "named-pair");
    pos = adg_dim_get_pos(dim);
    g_assert_true(adg_point_equal(pos, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "pos", origin, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert_true(adg_point_equal(pos, origin));
    adg_point_destroy(pos);

    g_object_set(dim, "pos", NULL, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert_null(pos);

    g_object_set(dim, "pos", explicit_point, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert_true(adg_point_equal(pos, explicit_point));
    adg_point_destroy(pos);

    adg_dim_set_pos_from_model(dim, model, "dummy");
    g_object_get(dim, "pos", &pos, NULL);
    g_assert_nonnull(pos);
    adg_point_destroy(pos);

    g_object_set(dim, "pos", model_point, NULL);
    g_object_get(dim, "pos", &pos, NULL);
    g_assert_true(adg_point_equal(pos, model_point));
    adg_point_destroy(pos);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    adg_entity_destroy(ADG_ENTITY(dim));
    g_object_unref(model);
}

static void
_adg_property_ref1(void)
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
    adg_model_set_named_pair(model, "named-pair", (CpmlPair *) explicit_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    ref1 = adg_dim_get_ref1(dim);
    g_assert_null(ref1);

    /* Using the public APIs */
    adg_dim_set_ref1_explicit(dim, 0, 0);
    ref1 = adg_dim_get_ref1(dim);
    g_assert_true(adg_point_equal(ref1, origin));

    adg_dim_set_ref1(dim, NULL);
    ref1 = adg_dim_get_ref1(dim);
    g_assert_null(ref1);

    adg_dim_set_ref1(dim, explicit_point);
    ref1 = adg_dim_get_ref1(dim);
    g_assert_true(adg_point_equal(ref1, explicit_point));

    adg_dim_set_ref1_from_model(dim, model, "dummy");
    ref1 = adg_dim_get_ref1(dim);
    g_assert_nonnull(ref1);

    adg_dim_set_ref1_from_model(dim, model, "named-pair");
    ref1 = adg_dim_get_ref1(dim);
    g_assert_true(adg_point_equal(ref1, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "ref1", origin, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert_true(adg_point_equal(ref1, origin));
    adg_point_destroy(ref1);

    g_object_set(dim, "ref1", NULL, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert_null(ref1);

    g_object_set(dim, "ref1", explicit_point, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert_true(adg_point_equal(ref1, explicit_point));
    adg_point_destroy(ref1);

    adg_dim_set_ref1_from_model(dim, model, "dummy");
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert_nonnull(ref1);
    adg_point_destroy(ref1);

    g_object_set(dim, "ref1", model_point, NULL);
    g_object_get(dim, "ref1", &ref1, NULL);
    g_assert_true(adg_point_equal(ref1, model_point));
    adg_point_destroy(ref1);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    adg_entity_destroy(ADG_ENTITY(dim));
    g_object_unref(model);
}

static void
_adg_property_ref2(void)
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
    adg_model_set_named_pair(model, "named-pair", (CpmlPair *) explicit_point);
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert_false(adg_point_equal(explicit_point, model_point));

    ref2 = adg_dim_get_ref2(dim);
    g_assert_null(ref2);

    /* Using the public APIs */
    adg_dim_set_ref2_explicit(dim, 0, 0);
    ref2 = adg_dim_get_ref2(dim);
    g_assert_true(adg_point_equal(ref2, origin));

    adg_dim_set_ref2(dim, NULL);
    ref2 = adg_dim_get_ref2(dim);
    g_assert_null(ref2);

    adg_dim_set_ref2(dim, explicit_point);
    ref2 = adg_dim_get_ref2(dim);
    g_assert_true(adg_point_equal(ref2, explicit_point));

    adg_dim_set_ref2_from_model(dim, model, "dummy");
    ref2 = adg_dim_get_ref2(dim);
    g_assert_nonnull(ref2);

    adg_dim_set_ref2_from_model(dim, model, "named-pair");
    ref2 = adg_dim_get_ref2(dim);
    g_assert_true(adg_point_equal(ref2, model_point));

    /* Using GObject property methods */
    g_object_set(dim, "ref2", origin, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert_true(adg_point_equal(ref2, origin));
    adg_point_destroy(ref2);

    g_object_set(dim, "ref2", NULL, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert_null(ref2);

    g_object_set(dim, "ref2", explicit_point, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert_true(adg_point_equal(ref2, explicit_point));
    adg_point_destroy(ref2);

    adg_dim_set_ref2_from_model(dim, model, "dummy");
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert_nonnull(ref2);
    adg_point_destroy(ref2);

    g_object_set(dim, "ref2", model_point, NULL);
    g_object_get(dim, "ref2", &ref2, NULL);
    g_assert_true(adg_point_equal(ref2, model_point));
    adg_point_destroy(ref2);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    adg_entity_destroy(ADG_ENTITY(dim));
    g_object_unref(model);
}

static void
_adg_property_value(void)
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
    g_assert_null(value);

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
    g_assert_null(value_dup);

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_method_get_text(void)
{
    AdgDim *dim;
    AdgDimStyle *dim_style;
    gchar *text;

    dim = ADG_DIM(adg_ldim_new());
    dim_style = ADG_DIM_STYLE(adg_entity_style(ADG_ENTITY(dim), ADG_DRESS_DIMENSION));

    /* Checking predefined dresses */

    /* Default dress: ADG_DRESS_DIMENSION */
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7.89");
    g_free(text);

    adg_dim_set_dim_dress(dim, ADG_DRESS_DIMENSION_ANGULAR);
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7°53'");
    g_free(text);

    adg_dim_set_dim_dress(dim, ADG_DRESS_DIMENSION_RADIUS);
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "R7.89");
    g_free(text);

    adg_dim_set_dim_dress(dim, ADG_DRESS_DIMENSION_DIAMETER);
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, ADG_UTF8_DIAMETER "7.89");
    g_free(text);

    /* Restore the default dress */
    adg_dim_set_dim_dress(dim, ADG_DRESS_DIMENSION);
    text = adg_dim_get_text(dim, 3.456);
    g_assert_cmpstr(text, ==, "3.46");
    g_free(text);

    /* Decrease the decimals */
    adg_dim_style_set_decimals(dim_style, 1);
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7.9");
    g_free(text);

    /* Change the argument type */
    adg_dim_style_set_number_arguments(dim_style, "a");
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7.891");
    g_free(text);

    adg_dim_style_set_number_arguments(dim_style, "D");
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7");
    g_free(text);

    adg_dim_style_set_number_arguments(dim_style, "d");
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7.9");
    g_free(text);

    /* Change the number format */
    adg_dim_style_set_number_format(dim_style, "%.3f");
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "7.900");
    g_free(text);

    /* Trying a complex formatting with multiple fields */
    adg_dim_style_set_decimals(dim_style, 2);
    adg_dim_style_set_number_arguments(dim_style, "aieDMSdms");
    adg_dim_style_set_number_format(dim_style, "%%Raw: \\(%g, %g, %g\\); Truncated: \\(%g, %g, %g\\); Rounded: \\(%g, %g, %g\\)");
    text = adg_dim_get_text(dim, 7.891);
    g_assert_cmpstr(text, ==, "%Raw: (7.891, 53.46, 27.6); Truncated: (7, 53, 27); Rounded: (7.89, 53.46, 27.6)");
    g_free(text);

    text = adg_dim_get_text(dim, 5.432);
    g_assert_cmpstr(text, ==, "%Raw: (5.432, 25.92, 55.2); Truncated: (5, 25, 55); Rounded: (5.43, 25.92, 55.2)");
    g_free(text);

    /* Testing simple disappearing of unvalorized values */
    adg_dim_style_set_number_arguments(dim_style, "a");
    adg_dim_style_set_number_format(dim_style, "(%g)");

    text = adg_dim_get_text(dim, 123);
    g_assert_cmpstr(text, ==, "123");
    g_free(text);

    text = adg_dim_get_text(dim, 0);
    g_assert_cmpstr(text, ==, "");
    g_free(text);

    adg_dim_style_set_number_arguments(dim_style, "aD");
    adg_dim_style_set_number_format(dim_style, ">(%g)--(%g)<");

    text = adg_dim_get_text(dim, 0);
    g_assert_cmpstr(text, ==, ">--<");
    g_free(text);

    text = adg_dim_get_text(dim, 0.42);
    g_assert_cmpstr(text, ==, ">0.42--<");
    g_free(text);

    text = adg_dim_get_text(dim, 2.34);
    g_assert_cmpstr(text, ==, ">2.34--2<");
    g_free(text);

    /* Testing nested parenthesis */
    adg_dim_style_set_number_arguments(dim_style, "DMs");
    adg_dim_style_set_number_format(dim_style, "%g°(%g'(%g\"))");

    text = adg_dim_get_text(dim, 0);
    g_assert_cmpstr(text, ==, "0°");
    g_free(text);

    text = adg_dim_get_text(dim, 1.5);
    g_assert_cmpstr(text, ==, "1°30'");
    g_free(text);

    text = adg_dim_get_text(dim, 2.002777);
    g_assert_cmpstr(text, ==, "2°0'10\"");
    g_free(text);

    /* Test errors on unmatched parenthesis */
    adg_dim_style_set_number_format(dim_style, "%g°(%g'(%g\")");
    g_assert_null(adg_dim_get_text(dim, 0));

    adg_dim_style_set_number_format(dim_style, "%g°%g'(%g\"))");
    g_assert_null(adg_dim_get_text(dim, 0));

    adg_entity_destroy(ADG_ENTITY(dim));
}

static void
_adg_method_set_limits(void)
{
    AdgDim *dim;

    dim = ADG_DIM(adg_ldim_new());

    /* Sanity checks */
    adg_dim_set_limits(NULL, "", "");
    adg_dim_set_limits(dim, NULL, "");
    adg_dim_set_limits(dim, "", NULL);

    adg_dim_set_limits(dim, "min", "max");
    g_assert_cmpstr(adg_dim_get_min(dim), ==, "min");
    g_assert_cmpstr(adg_dim_get_max(dim), ==, "max");

    adg_entity_destroy(ADG_ENTITY(dim));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/dim/type/object", ADG_TYPE_DIM);
    adg_test_add_entity_checks("/adg/dim/type/entity", ADG_TYPE_DIM);

    g_test_add_func("/adg/dim/property/detached", _adg_property_detached);
    g_test_add_func("/adg/dim/property/dim-dress", _adg_property_dim_dress);
    g_test_add_func("/adg/dim/property/level", _adg_property_level);
    g_test_add_func("/adg/dim/property/max", _adg_property_max);
    g_test_add_func("/adg/dim/property/min", _adg_property_min);
    g_test_add_func("/adg/dim/property/outside", _adg_property_outside);
    g_test_add_func("/adg/dim/property/pos", _adg_property_pos);
    g_test_add_func("/adg/dim/property/ref1", _adg_property_ref1);
    g_test_add_func("/adg/dim/property/ref2", _adg_property_ref2);
    g_test_add_func("/adg/dim/property/value", _adg_property_value);

    g_test_add_func("/adg/dim/method/get_text", _adg_method_get_text);
    g_test_add_func("/adg/dim/method/set_limits", _adg_method_set_limits);

    return g_test_run();
}
