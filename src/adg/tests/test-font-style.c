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
_adg_test_antialias(void)
{
    AdgFontStyle *font_style;
    cairo_antialias_t valid_antialias_1, valid_antialias_2, antialias;

    font_style = adg_font_style_new();
    valid_antialias_1 = CAIRO_ANTIALIAS_SUBPIXEL;
    valid_antialias_2 = CAIRO_ANTIALIAS_DEFAULT;

    /* Using the public APIs */
    adg_font_style_set_antialias(font_style, valid_antialias_1);
    antialias = adg_font_style_get_antialias(font_style);
    g_assert_cmpint(antialias, ==, valid_antialias_1);

    adg_font_style_set_antialias(font_style, valid_antialias_2);
    antialias = adg_font_style_get_antialias(font_style);
    g_assert_cmpint(antialias, ==, valid_antialias_2);

    /* Using GObject property methods */
    g_object_set(font_style, "antialias", valid_antialias_1, NULL);
    g_object_get(font_style, "antialias", &antialias, NULL);
    g_assert_cmpint(antialias, ==, valid_antialias_1);

    g_object_set(font_style, "antialias", valid_antialias_2, NULL);
    g_object_get(font_style, "antialias", &antialias, NULL);
    g_assert_cmpint(antialias, ==, valid_antialias_2);

    g_object_unref(font_style);
}

static void
_adg_test_color_dress(void)
{
    AdgFontStyle *font_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress color_dress;

    font_style = adg_font_style_new();
    valid_dress_1 = ADG_DRESS_COLOR_DIMENSION;
    valid_dress_2 = ADG_DRESS_COLOR_FILL;
    incompatible_dress = ADG_DRESS_FONT_QUOTE_TEXT;

    /* Using the public APIs */
    adg_font_style_set_color_dress(font_style, valid_dress_1);
    color_dress = adg_font_style_get_color_dress(font_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_font_style_set_color_dress(font_style, incompatible_dress);
    color_dress = adg_font_style_get_color_dress(font_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_font_style_set_color_dress(font_style, valid_dress_2);
    color_dress = adg_font_style_get_color_dress(font_style);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(font_style, "color-dress", valid_dress_1, NULL);
    g_object_get(font_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(font_style, "color-dress", incompatible_dress, NULL);
    g_object_get(font_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(font_style, "color-dress", valid_dress_2, NULL);
    g_object_get(font_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    g_object_unref(font_style);
}

static void
_adg_test_family(void)
{
    AdgFontStyle *font_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *family;
    gchar *family_dup;

    font_style = adg_font_style_new();
    valid_text_1 = "Sans serif 10, bold";
    valid_text_2 = "";

    /* Using the public APIs */
    adg_font_style_set_family(font_style, valid_text_1);
    family = adg_font_style_get_family(font_style);
    g_assert_cmpstr(family, ==, valid_text_1);

    adg_font_style_set_family(font_style, valid_text_2);
    family = adg_font_style_get_family(font_style);
    g_assert_cmpstr(family, ==, valid_text_2);

    adg_font_style_set_family(font_style, NULL);
    family = adg_font_style_get_family(font_style);
    g_assert_null(family);

    /* Using GObject property methods */
    g_object_set(font_style, "family", valid_text_1, NULL);
    g_object_get(font_style, "family", &family_dup, NULL);
    g_assert_cmpstr(family_dup, ==, valid_text_1);
    g_free(family_dup);

    g_object_set(font_style, "family", valid_text_2, NULL);
    g_object_get(font_style, "family", &family_dup, NULL);
    g_assert_cmpstr(family_dup, ==, valid_text_2);
    g_free(family_dup);

    g_object_set(font_style, "family", NULL, NULL);
    g_object_get(font_style, "family", &family_dup, NULL);
    g_assert_null(family_dup);

    g_object_unref(font_style);
}

static void
_adg_test_hint_metrics(void)
{
    AdgFontStyle *font_style;
    cairo_hint_metrics_t valid_hint_metrics_1, valid_hint_metrics_2;
    cairo_hint_metrics_t hint_metrics;

    font_style = adg_font_style_new();
    valid_hint_metrics_1 = CAIRO_HINT_METRICS_DEFAULT;
    valid_hint_metrics_2 = CAIRO_HINT_METRICS_ON;

    /* Using the public APIs */
    adg_font_style_set_hint_metrics(font_style, valid_hint_metrics_1);
    hint_metrics = adg_font_style_get_hint_metrics(font_style);
    g_assert_cmpint(hint_metrics, ==, valid_hint_metrics_1);

    adg_font_style_set_hint_metrics(font_style, valid_hint_metrics_2);
    hint_metrics = adg_font_style_get_hint_metrics(font_style);
    g_assert_cmpint(hint_metrics, ==, valid_hint_metrics_2);

    /* Using GObject property methods */
    g_object_set(font_style, "hint-metrics", valid_hint_metrics_1, NULL);
    g_object_get(font_style, "hint-metrics", &hint_metrics, NULL);
    g_assert_cmpint(hint_metrics, ==, valid_hint_metrics_1);

    g_object_set(font_style, "hint-metrics", valid_hint_metrics_2, NULL);
    g_object_get(font_style, "hint-metrics", &hint_metrics, NULL);
    g_assert_cmpint(hint_metrics, ==, valid_hint_metrics_2);

    g_object_unref(font_style);
}

static void
_adg_test_hint_style(void)
{
    AdgFontStyle *font_style;
    cairo_hint_style_t valid_hint_style_1, valid_hint_style_2;
    cairo_hint_style_t hint_style;

    font_style = adg_font_style_new();
    valid_hint_style_1 = CAIRO_HINT_STYLE_MEDIUM;
    valid_hint_style_2 = CAIRO_HINT_STYLE_SLIGHT;

    /* Using the public APIs */
    adg_font_style_set_hint_style(font_style, valid_hint_style_1);
    hint_style = adg_font_style_get_hint_style(font_style);
    g_assert_cmpint(hint_style, ==, valid_hint_style_1);

    adg_font_style_set_hint_style(font_style, valid_hint_style_2);
    hint_style = adg_font_style_get_hint_style(font_style);
    g_assert_cmpint(hint_style, ==, valid_hint_style_2);

    /* Using GObject property methods */
    g_object_set(font_style, "hint-style", valid_hint_style_1, NULL);
    g_object_get(font_style, "hint-style", &hint_style, NULL);
    g_assert_cmpint(hint_style, ==, valid_hint_style_1);

    g_object_set(font_style, "hint-style", valid_hint_style_2, NULL);
    g_object_get(font_style, "hint-style", &hint_style, NULL);
    g_assert_cmpint(hint_style, ==, valid_hint_style_2);

    g_object_unref(font_style);
}

static void
_adg_test_size(void)
{
    AdgFontStyle *font_style;
    gdouble valid_size_1, valid_size_2, invalid_size;
    gdouble size;

    font_style = adg_font_style_new();
    valid_size_1 = 0;
    valid_size_2 = 999;
    invalid_size = -1;

    /* Using the public APIs */
    adg_font_style_set_size(font_style, valid_size_1);
    size = adg_font_style_get_size(font_style);
    g_assert_cmpfloat(size, ==, valid_size_1);

    adg_font_style_set_size(font_style, invalid_size);
    size = adg_font_style_get_size(font_style);
    g_assert_cmpfloat(size, ==, valid_size_1);

    adg_font_style_set_size(font_style, valid_size_2);
    size = adg_font_style_get_size(font_style);
    g_assert_cmpfloat(size, ==, valid_size_2);

    /* Using GObject property methods */
    g_object_set(font_style, "size", valid_size_1, NULL);
    g_object_get(font_style, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size_1);

    g_object_set(font_style, "size", invalid_size, NULL);
    g_object_get(font_style, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size_1);

    g_object_set(font_style, "size", valid_size_2, NULL);
    g_object_get(font_style, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size_2);

    g_object_unref(font_style);
}

static void
_adg_test_slant(void)
{
    AdgFontStyle *font_style;
    cairo_font_slant_t valid_slant_1, valid_slant_2;
    cairo_font_slant_t slant;

    font_style = adg_font_style_new();
    valid_slant_1 = CAIRO_FONT_SLANT_NORMAL;
    valid_slant_2 = CAIRO_FONT_SLANT_OBLIQUE;

    /* Using the public APIs */
    adg_font_style_set_slant(font_style, valid_slant_1);
    slant = adg_font_style_get_slant(font_style);
    g_assert_cmpint(slant, ==, valid_slant_1);

    adg_font_style_set_slant(font_style, valid_slant_2);
    slant = adg_font_style_get_slant(font_style);
    g_assert_cmpint(slant, ==, valid_slant_2);

    /* Using GObject property methods */
    g_object_set(font_style, "slant", valid_slant_1, NULL);
    g_object_get(font_style, "slant", &slant, NULL);
    g_assert_cmpint(slant, ==, valid_slant_1);

    g_object_set(font_style, "slant", valid_slant_2, NULL);
    g_object_get(font_style, "slant", &slant, NULL);
    g_assert_cmpint(slant, ==, valid_slant_2);

    g_object_unref(font_style);
}

static void
_adg_test_subpixel_order(void)
{
    AdgFontStyle *font_style;
    cairo_subpixel_order_t valid_subpixel_order_1, valid_subpixel_order_2;
    cairo_subpixel_order_t subpixel_order;

    font_style = adg_font_style_new();
    valid_subpixel_order_1 = CAIRO_SUBPIXEL_ORDER_RGB;
    valid_subpixel_order_2 = CAIRO_SUBPIXEL_ORDER_BGR;

    /* Using the public APIs */
    adg_font_style_set_subpixel_order(font_style, valid_subpixel_order_1);
    subpixel_order = adg_font_style_get_subpixel_order(font_style);
    g_assert_cmpint(subpixel_order, ==, valid_subpixel_order_1);

    adg_font_style_set_subpixel_order(font_style, valid_subpixel_order_2);
    subpixel_order = adg_font_style_get_subpixel_order(font_style);
    g_assert_cmpint(subpixel_order, ==, valid_subpixel_order_2);

    /* Using GObject property methods */
    g_object_set(font_style, "subpixel-order", valid_subpixel_order_1, NULL);
    g_object_get(font_style, "subpixel-order", &subpixel_order, NULL);
    g_assert_cmpint(subpixel_order, ==, valid_subpixel_order_1);

    g_object_set(font_style, "subpixel-order", valid_subpixel_order_2, NULL);
    g_object_get(font_style, "subpixel-order", &subpixel_order, NULL);
    g_assert_cmpint(subpixel_order, ==, valid_subpixel_order_2);

    g_object_unref(font_style);
}

static void
_adg_test_weight(void)
{
    AdgFontStyle *font_style;
    cairo_font_weight_t valid_weight_1, valid_weight_2;
    cairo_font_weight_t weight;

    font_style = adg_font_style_new();
    valid_weight_1 = CAIRO_FONT_WEIGHT_NORMAL;
    valid_weight_2 = CAIRO_FONT_WEIGHT_BOLD;

    /* Using the public APIs */
    adg_font_style_set_weight(font_style, valid_weight_1);
    weight = adg_font_style_get_weight(font_style);
    g_assert_cmpint(weight, ==, valid_weight_1);

    adg_font_style_set_weight(font_style, valid_weight_2);
    weight = adg_font_style_get_weight(font_style);
    g_assert_cmpint(weight, ==, valid_weight_2);

    /* Using GObject property methods */
    g_object_set(font_style, "weight", valid_weight_1, NULL);
    g_object_get(font_style, "weight", &weight, NULL);
    g_assert_cmpint(weight, ==, valid_weight_1);

    g_object_set(font_style, "weight", valid_weight_2, NULL);
    g_object_get(font_style, "weight", &weight, NULL);
    g_assert_cmpint(weight, ==, valid_weight_2);

    g_object_unref(font_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/font-style/type/object", ADG_TYPE_FONT_STYLE);

    g_test_add_func("/adg/font-style/property/antialias", _adg_test_antialias);
    g_test_add_func("/adg/font-style/property/color-dress", _adg_test_color_dress);
    g_test_add_func("/adg/font-style/property/family", _adg_test_family);
    g_test_add_func("/adg/font-style/property/hint-metrics", _adg_test_hint_metrics);
    g_test_add_func("/adg/font-style/property/hint-style", _adg_test_hint_style);
    g_test_add_func("/adg/font-style/property/size", _adg_test_size);
    g_test_add_func("/adg/font-style/property/slant", _adg_test_slant);
    g_test_add_func("/adg/font-style/property/subpixel-order", _adg_test_subpixel_order);
    g_test_add_func("/adg/font-style/property/weight", _adg_test_weight);

    return g_test_run();
}
