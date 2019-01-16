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
_adg_property_alpha(void)
{
    AdgColorStyle *color_style;
    gdouble valid_alpha1, valid_alpha2, invalid_alpha, alpha;

    color_style = adg_color_style_new();
    valid_alpha1 = 1;
    valid_alpha2 = 0;
    invalid_alpha = -1;

    /* Using the public APIs */
    adg_color_style_set_alpha(color_style, valid_alpha1);
    alpha = adg_color_style_get_alpha(color_style);
    adg_assert_isapprox(alpha, valid_alpha1);

    adg_color_style_set_alpha(color_style, invalid_alpha);
    alpha = adg_color_style_get_alpha(color_style);
    adg_assert_isapprox(alpha, valid_alpha1);

    adg_color_style_set_alpha(color_style, valid_alpha2);
    alpha = adg_color_style_get_alpha(color_style);
    adg_assert_isapprox(alpha, valid_alpha2);

    /* Using GObject property methods */
    g_object_set(color_style, "alpha", valid_alpha1, NULL);
    g_object_get(color_style, "alpha", &alpha, NULL);
    adg_assert_isapprox(alpha, valid_alpha1);

    g_object_set(color_style, "alpha", invalid_alpha, NULL);
    g_object_get(color_style, "alpha", &alpha, NULL);
    adg_assert_isapprox(alpha, valid_alpha1);

    g_object_set(color_style, "alpha", valid_alpha2, NULL);
    g_object_get(color_style, "alpha", &alpha, NULL);
    adg_assert_isapprox(alpha, valid_alpha2);

    g_object_unref(color_style);
}

static void
_adg_property_blue(void)
{
    AdgColorStyle *color_style;
    gdouble valid_blue1, valid_blue2, invalid_blue, blue;

    color_style = adg_color_style_new();
    valid_blue1 = 1;
    valid_blue2 = 0;
    invalid_blue = -1;

    /* Using the public APIs */
    adg_color_style_set_blue(color_style, valid_blue1);
    blue = adg_color_style_get_blue(color_style);
    adg_assert_isapprox(blue, valid_blue1);

    adg_color_style_set_blue(color_style, invalid_blue);
    blue = adg_color_style_get_blue(color_style);
    adg_assert_isapprox(blue, valid_blue1);

    adg_color_style_set_blue(color_style, valid_blue2);
    blue = adg_color_style_get_blue(color_style);
    adg_assert_isapprox(blue, valid_blue2);

    /* Using GObject property methods */
    g_object_set(color_style, "blue", valid_blue1, NULL);
    g_object_get(color_style, "blue", &blue, NULL);
    adg_assert_isapprox(blue, valid_blue1);

    g_object_set(color_style, "blue", invalid_blue, NULL);
    g_object_get(color_style, "blue", &blue, NULL);
    adg_assert_isapprox(blue, valid_blue1);

    g_object_set(color_style, "blue", valid_blue2, NULL);
    g_object_get(color_style, "blue", &blue, NULL);
    adg_assert_isapprox(blue, valid_blue2);

    g_object_unref(color_style);
}

static void
_adg_property_green(void)
{
    AdgColorStyle *color_style;
    gdouble valid_green1, valid_green2, invalid_green, green;

    color_style = adg_color_style_new();
    valid_green1 = 1;
    valid_green2 = 0;
    invalid_green = -1;

    /* Using the public APIs */
    adg_color_style_set_green(color_style, valid_green1);
    green = adg_color_style_get_green(color_style);
    adg_assert_isapprox(green, valid_green1);

    adg_color_style_set_green(color_style, invalid_green);
    green = adg_color_style_get_green(color_style);
    adg_assert_isapprox(green, valid_green1);

    adg_color_style_set_green(color_style, valid_green2);
    green = adg_color_style_get_green(color_style);
    adg_assert_isapprox(green, valid_green2);

    /* Using GObject property methods */
    g_object_set(color_style, "green", valid_green1, NULL);
    g_object_get(color_style, "green", &green, NULL);
    adg_assert_isapprox(green, valid_green1);

    g_object_set(color_style, "green", invalid_green, NULL);
    g_object_get(color_style, "green", &green, NULL);
    adg_assert_isapprox(green, valid_green1);

    g_object_set(color_style, "green", valid_green2, NULL);
    g_object_get(color_style, "green", &green, NULL);
    adg_assert_isapprox(green, valid_green2);

    g_object_unref(color_style);
}

static void
_adg_property_red(void)
{
    AdgColorStyle *color_style;
    gdouble valid_red1, valid_red2, invalid_red, red;

    color_style = adg_color_style_new();
    valid_red1 = 1;
    valid_red2 = 0;
    invalid_red = -1;

    /* Using the public APIs */
    adg_color_style_set_red(color_style, valid_red1);
    red = adg_color_style_get_red(color_style);
    adg_assert_isapprox(red, valid_red1);

    adg_color_style_set_red(color_style, invalid_red);
    red = adg_color_style_get_red(color_style);
    adg_assert_isapprox(red, valid_red1);

    adg_color_style_set_red(color_style, valid_red2);
    red = adg_color_style_get_red(color_style);
    adg_assert_isapprox(red, valid_red2);

    /* Using GObject property methods */
    g_object_set(color_style, "red", valid_red1, NULL);
    g_object_get(color_style, "red", &red, NULL);
    adg_assert_isapprox(red, valid_red1);

    g_object_set(color_style, "red", invalid_red, NULL);
    g_object_get(color_style, "red", &red, NULL);
    adg_assert_isapprox(red, valid_red1);

    g_object_set(color_style, "red", valid_red2, NULL);
    g_object_get(color_style, "red", &red, NULL);
    adg_assert_isapprox(red, valid_red2);

    g_object_unref(color_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/color-style/type/object", ADG_TYPE_COLOR_STYLE);

    g_test_add_func("/adg/color-style/property/alpha", _adg_property_alpha);
    g_test_add_func("/adg/color-style/property/blue", _adg_property_blue);
    g_test_add_func("/adg/color-style/property/green", _adg_property_green);
    g_test_add_func("/adg/color-style/property/red", _adg_property_red);

    return g_test_run();
}
