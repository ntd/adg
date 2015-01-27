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
_adg_test_color_dress(void)
{
    AdgLineStyle *line_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress color_dress;

    line_style = adg_line_style_new();
    valid_dress_1 = ADG_DRESS_COLOR_DIMENSION;
    valid_dress_2 = ADG_DRESS_COLOR_FILL;
    incompatible_dress = ADG_DRESS_FONT_QUOTE_ANNOTATION;

    /* Using the public APIs */
    adg_line_style_set_color_dress(line_style, valid_dress_1);
    color_dress = adg_line_style_get_color_dress(line_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_line_style_set_color_dress(line_style, incompatible_dress);
    color_dress = adg_line_style_get_color_dress(line_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_line_style_set_color_dress(line_style, valid_dress_2);
    color_dress = adg_line_style_get_color_dress(line_style);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(line_style, "color-dress", valid_dress_1, NULL);
    g_object_get(line_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(line_style, "color-dress", incompatible_dress, NULL);
    g_object_get(line_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(line_style, "color-dress", valid_dress_2, NULL);
    g_object_get(line_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    g_object_unref(line_style);
}

static void
_adg_test_width(void)
{
    AdgLineStyle *line_style;
    gdouble valid_width_1, valid_width_2, invalid_width;
    gdouble width;

    line_style = adg_line_style_new();
    valid_width_1 = 0;
    valid_width_2 = 999;
    invalid_width = -1;

    /* Using the public APIs */
    adg_line_style_set_width(line_style, valid_width_1);
    width = adg_line_style_get_width(line_style);
    g_assert_cmpfloat(width, ==, valid_width_1);

    adg_line_style_set_width(line_style, invalid_width);
    width = adg_line_style_get_width(line_style);
    g_assert_cmpfloat(width, ==, valid_width_1);

    adg_line_style_set_width(line_style, valid_width_2);
    width = adg_line_style_get_width(line_style);
    g_assert_cmpfloat(width, ==, valid_width_2);

    /* Using GObject property methods */
    g_object_set(line_style, "width", valid_width_1, NULL);
    g_object_get(line_style, "width", &width, NULL);
    g_assert_cmpfloat(width, ==, valid_width_1);

    g_object_set(line_style, "width", invalid_width, NULL);
    g_object_get(line_style, "width", &width, NULL);
    g_assert_cmpfloat(width, ==, valid_width_1);

    g_object_set(line_style, "width", valid_width_2, NULL);
    g_object_get(line_style, "width", &width, NULL);
    g_assert_cmpfloat(width, ==, valid_width_2);

    g_object_unref(line_style);
}

static void
_adg_test_cap(void)
{
    AdgLineStyle *line_style;
    cairo_line_cap_t valid_cap_1, valid_cap_2, cap;

    line_style = adg_line_style_new();
    valid_cap_1 = CAIRO_LINE_CAP_SQUARE;
    valid_cap_2 = CAIRO_LINE_CAP_BUTT;

    /* Using the public APIs */
    adg_line_style_set_cap(line_style, valid_cap_1);
    cap = adg_line_style_get_cap(line_style);
    g_assert_cmpint(cap, ==, valid_cap_1);

    adg_line_style_set_cap(line_style, valid_cap_2);
    cap = adg_line_style_get_cap(line_style);
    g_assert_cmpint(cap, ==, valid_cap_2);

    /* Using GObject property methods */
    g_object_set(line_style, "cap", valid_cap_1, NULL);
    g_object_get(line_style, "cap", &cap, NULL);
    g_assert_cmpint(cap, ==, valid_cap_1);

    g_object_set(line_style, "cap", valid_cap_2, NULL);
    g_object_get(line_style, "cap", &cap, NULL);
    g_assert_cmpint(cap, ==, valid_cap_2);

    g_object_unref(line_style);
}

static void
_adg_test_join(void)
{
    AdgLineStyle *line_style;
    cairo_line_join_t valid_join_1, valid_join_2, join;

    line_style = adg_line_style_new();
    valid_join_1 = CAIRO_LINE_JOIN_BEVEL;
    valid_join_2 = CAIRO_LINE_JOIN_MITER;

    /* Using the public APIs */
    adg_line_style_set_join(line_style, valid_join_1);
    join = adg_line_style_get_join(line_style);
    g_assert_cmpint(join, ==, valid_join_1);

    adg_line_style_set_join(line_style, valid_join_2);
    join = adg_line_style_get_join(line_style);
    g_assert_cmpint(join, ==, valid_join_2);

    /* Using GObject property methods */
    g_object_set(line_style, "join", valid_join_1, NULL);
    g_object_get(line_style, "join", &join, NULL);
    g_assert_cmpint(join, ==, valid_join_1);

    g_object_set(line_style, "join", valid_join_2, NULL);
    g_object_get(line_style, "join", &join, NULL);
    g_assert_cmpint(join, ==, valid_join_2);

    g_object_unref(line_style);
}

static void
_adg_test_miter_limit(void)
{
    AdgLineStyle *line_style;
    gdouble valid_miter_limit_1, valid_miter_limit_2, invalid_miter_limit;
    gdouble miter_limit;

    line_style = adg_line_style_new();
    valid_miter_limit_1 = 0;
    valid_miter_limit_2 = 1000;
    invalid_miter_limit = -1;

    /* Using the public APIs */
    adg_line_style_set_miter_limit(line_style, valid_miter_limit_1);
    miter_limit = adg_line_style_get_miter_limit(line_style);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_1);

    adg_line_style_set_miter_limit(line_style, invalid_miter_limit);
    miter_limit = adg_line_style_get_miter_limit(line_style);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_1);

    adg_line_style_set_miter_limit(line_style, valid_miter_limit_2);
    miter_limit = adg_line_style_get_miter_limit(line_style);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_2);

    /* Using GObject property methods */
    g_object_set(line_style, "miter-limit", valid_miter_limit_1, NULL);
    g_object_get(line_style, "miter-limit", &miter_limit, NULL);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_1);

    g_object_set(line_style, "miter-limit", invalid_miter_limit, NULL);
    g_object_get(line_style, "miter-limit", &miter_limit, NULL);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_1);

    g_object_set(line_style, "miter-limit", valid_miter_limit_2, NULL);
    g_object_get(line_style, "miter-limit", &miter_limit, NULL);
    g_assert_cmpfloat(miter_limit, ==, valid_miter_limit_2);

    g_object_unref(line_style);
}

static void
_adg_test_antialias(void)
{
    AdgLineStyle *line_style;
    cairo_antialias_t valid_antialias_1, valid_antialias_2, antialias;

    line_style = adg_line_style_new();
    valid_antialias_1 = CAIRO_ANTIALIAS_SUBPIXEL;
    valid_antialias_2 = CAIRO_ANTIALIAS_DEFAULT;

    /* Using the public APIs */
    adg_line_style_set_antialias(line_style, valid_antialias_1);
    antialias = adg_line_style_get_antialias(line_style);
    g_assert_cmpint(antialias, ==, valid_antialias_1);

    adg_line_style_set_antialias(line_style, valid_antialias_2);
    antialias = adg_line_style_get_antialias(line_style);
    g_assert_cmpint(antialias, ==, valid_antialias_2);

    /* Using GObject property methods */
    g_object_set(line_style, "antialias", valid_antialias_1, NULL);
    g_object_get(line_style, "antialias", &antialias, NULL);
    g_assert_cmpint(antialias, ==, valid_antialias_1);

    g_object_set(line_style, "antialias", valid_antialias_2, NULL);
    g_object_get(line_style, "antialias", &antialias, NULL);
    g_assert_cmpint(antialias, ==, valid_antialias_2);

    g_object_unref(line_style);
}

static void
_adg_test_dash(void)
{
    AdgLineStyle *line_style;
    AdgDash *new_dash;
    const AdgDash *dash;
    gint num_dashes, new_num_dashes;
    const gdouble *dashes, *new_dashes;

    line_style = adg_line_style_new();

    /* Checking default value */
    dash = adg_line_style_get_dash(line_style);
    g_assert_null(dash);
    dash = (AdgDash *) 0xdead;
    g_object_get(line_style, "dash", &dash, NULL);
    g_assert_null(dash);

    /* Using the public APIs */
    new_dash = adg_dash_new_with_dashes(3, 1, 2, 3);
    new_dashes = adg_dash_get_dashes(new_dash);
    new_num_dashes = adg_dash_get_num_dashes(new_dash);
    adg_line_style_set_dash(line_style, new_dash);
    dash = adg_line_style_get_dash(line_style);
    dashes = adg_dash_get_dashes(dash);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(new_num_dashes, ==, num_dashes);
    g_assert_cmpfloat(new_dashes[0], ==, dashes[0]);
    g_assert_cmpfloat(new_dashes[1], ==, dashes[1]);
    g_assert_cmpfloat(new_dashes[2], ==, dashes[2]);
    adg_dash_destroy(new_dash);

    /* Using GObject property methods */
    new_dash = adg_dash_new_with_dashes(2, 4, 5);
    new_dashes = adg_dash_get_dashes(new_dash);
    new_num_dashes = adg_dash_get_num_dashes(new_dash);
    g_object_set(line_style, "dash", new_dash, NULL);
    g_object_get(line_style, "dash", &dash, NULL);
    dashes = adg_dash_get_dashes(dash);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(new_num_dashes, ==, num_dashes);
    g_assert_cmpfloat(new_dashes[0], ==, dashes[0]);
    g_assert_cmpfloat(new_dashes[1], ==, dashes[1]);
    adg_dash_destroy(new_dash);

    /* Checking the unset of dash */
    adg_line_style_set_dash(line_style, NULL);
    dash = adg_line_style_get_dash(line_style);
    g_assert_null(dash);

    g_object_unref(line_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/line-style/type/object", ADG_TYPE_LINE_STYLE);

    adg_test_add_func("/adg/line-style/property/color-dress", _adg_test_color_dress);
    adg_test_add_func("/adg/line-style/property/width", _adg_test_width);
    adg_test_add_func("/adg/line-style/property/cap", _adg_test_cap);
    adg_test_add_func("/adg/line-style/property/join", _adg_test_join);
    adg_test_add_func("/adg/line-style/property/miter-limit", _adg_test_miter_limit);
    adg_test_add_func("/adg/line-style/property/antialias", _adg_test_antialias);
    adg_test_add_func("/adg/line-style/property/dash", _adg_test_dash);

    return g_test_run();
}
