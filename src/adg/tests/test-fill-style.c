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
_adg_test_pattern(void)
{
    AdgFillStyle *fill_style;
    cairo_pattern_t *valid_pattern_1, *valid_pattern_2;
    const cairo_pattern_t *pattern;
    cairo_pattern_t *pattern_dup;

    fill_style = ADG_FILL_STYLE(adg_ruled_fill_new());
    valid_pattern_1 = cairo_pattern_create_rgba(0, 0, 0, 0);
    valid_pattern_2 = cairo_pattern_create_linear(1, 2, 3, 4);

    /* Using the public APIs */
    adg_fill_style_set_pattern(fill_style, valid_pattern_1);
    pattern = adg_fill_style_get_pattern(fill_style);
    g_assert_true(pattern == valid_pattern_1);

    adg_fill_style_set_pattern(fill_style, NULL);
    pattern = adg_fill_style_get_pattern(fill_style);
    g_assert_null(pattern);

    adg_fill_style_set_pattern(fill_style, valid_pattern_2);
    pattern = adg_fill_style_get_pattern(fill_style);
    g_assert_true(pattern == valid_pattern_2);

    /* Using GObject property methods */
    g_object_set(fill_style, "pattern", valid_pattern_1, NULL);
    g_object_get(fill_style, "pattern", &pattern_dup, NULL);
    g_assert_true(pattern_dup == valid_pattern_1);
    cairo_pattern_destroy(pattern_dup);

    g_object_set(fill_style, "pattern", NULL, NULL);
    g_object_get(fill_style, "pattern", &pattern_dup, NULL);
    g_assert_null(pattern_dup);
    cairo_pattern_destroy(pattern_dup);

    g_object_set(fill_style, "pattern", valid_pattern_2, NULL);
    g_object_get(fill_style, "pattern", &pattern_dup, NULL);
    g_assert_true(pattern_dup == valid_pattern_2);
    cairo_pattern_destroy(pattern_dup);

    cairo_pattern_destroy(valid_pattern_1);
    cairo_pattern_destroy(valid_pattern_2);
    g_object_unref(fill_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/fill-style/property/pattern", _adg_test_pattern);
    /* ADG_TYPE_FILL_STYLE is an abstract type so it cannot be allocated:
     * adg_test_add_property_check("/adg/fill-style/property/???", ADG_TYPE_FILL_STYLE);
     */

    return g_test_run();
}
