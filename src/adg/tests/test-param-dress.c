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
_adg_behavior_misc(void)
{
    GParamSpec *param = g_param_spec_internal(ADG_TYPE_PARAM_DRESS,
                                              "name", "nick", "blurb",
                                              G_PARAM_READWRITE);

    g_param_spec_ref_sink(param);

    g_assert_true(G_IS_PARAM_SPEC(param));
    g_assert_true(ADG_IS_PARAM_DRESS(param));

    g_param_spec_unref(param);
}

static void
_adg_method_set_default(void)
{
    GParamSpec *param = g_param_spec_internal(ADG_TYPE_PARAM_DRESS,
                                              "param", "nick", "blurb",
                                               G_PARAM_READWRITE);
    GValue value = { 0 };

    g_param_spec_ref_sink(param);

    g_value_init(&value, ADG_TYPE_DRESS);
    g_assert_true(ADG_VALUE_HOLDS_DRESS(&value));
    g_assert_cmpint(g_value_get_enum(&value), ==, ADG_DRESS_UNDEFINED);

    g_value_set_enum(&value, ADG_DRESS_TABLE);
    g_assert_cmpint(g_value_get_enum(&value), ==, ADG_DRESS_TABLE);

    g_param_value_set_default(param, &value);
    g_assert_cmpint(g_value_get_enum(&value), ==, ADG_DRESS_UNDEFINED);

    g_param_spec_unref(param);
}

static void
_adg_method_values_cmp(void)
{
    GParamSpec *param = g_param_spec_internal(ADG_TYPE_PARAM_DRESS,
                                              "param", "nick", "blurb",
                                               G_PARAM_READWRITE);
    GValue value1 = { 0 }, value2 = { 0 };

    g_param_spec_ref_sink(param);

    g_value_init(&value1, ADG_TYPE_DRESS);
    g_value_init(&value2, ADG_TYPE_DRESS);

    g_value_set_enum(&value1, ADG_DRESS_FONT);
    g_value_set_enum(&value2, ADG_DRESS_FONT);

    g_assert_cmpint(g_param_values_cmp(param, &value1, &value2), ==, 0);

    g_value_set_enum(&value2, ADG_DRESS_TABLE);
    g_assert_cmpint(g_param_values_cmp(param, &value1, &value2), ==, -1);
    g_assert_cmpint(g_param_values_cmp(param, &value2, &value1), ==, +1);

    g_value_set_enum(&value1, ADG_DRESS_TABLE);
    g_assert_cmpint(g_param_values_cmp(param, &value1, &value2), ==, 0);

    g_param_spec_unref(param);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/adg/param-dress/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/param-dress/method/set-default", _adg_method_set_default);
    g_test_add_func("/adg/param-dress/method/values-cmp", _adg_method_values_cmp);

    return g_test_run();
}
