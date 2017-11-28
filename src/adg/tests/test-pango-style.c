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
_adg_property_spacing(void)
{
    AdgPangoStyle *pango_style;
    gint valid_spacing_1, valid_spacing_2;
    gint spacing;

    pango_style = adg_pango_style_new();
    valid_spacing_1 = 123;
    valid_spacing_2 = -123;

    /* Using the public APIs */
    adg_pango_style_set_spacing(pango_style, valid_spacing_1);
    spacing = adg_pango_style_get_spacing(pango_style);
    g_assert_cmpint(spacing, ==, valid_spacing_1);

    adg_pango_style_set_spacing(pango_style, valid_spacing_2);
    spacing = adg_pango_style_get_spacing(pango_style);
    g_assert_cmpint(spacing, ==, valid_spacing_2);

    /* Using GObject property methods */
    g_object_set(pango_style, "spacing", valid_spacing_1, NULL);
    g_object_get(pango_style, "spacing", &spacing, NULL);
    g_assert_cmpint(spacing, ==, valid_spacing_1);

    g_object_set(pango_style, "spacing", valid_spacing_2, NULL);
    g_object_get(pango_style, "spacing", &spacing, NULL);
    g_assert_cmpint(spacing, ==, valid_spacing_2);

    /* Check improper use */
    adg_pango_style_set_spacing(NULL, 111);
    spacing = adg_pango_style_get_spacing(NULL);
    g_assert_cmpint(spacing, ==, 0);
    spacing = adg_pango_style_get_spacing(pango_style);
    g_assert_cmpint(spacing, ==, valid_spacing_2);

    g_object_unref(pango_style);
}

static void
_adg_method_get_description(void)
{
    AdgPangoStyle *pango_style;
    PangoFontDescription *description;

    pango_style = adg_pango_style_new();

    /* Check valid use */
    description = adg_pango_style_get_description(pango_style);
    g_assert_nonnull(description);

    /* Check improper use */
    description = adg_pango_style_get_description(NULL);
    g_assert_null(description);

    g_object_unref(pango_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/font-style/type/object", ADG_TYPE_PANGO_STYLE);

    g_test_add_func("/adg/pango-style/property/spacing", _adg_property_spacing);

    g_test_add_func("/adg/pango-style/get_description", _adg_method_get_description);

    return g_test_run();
}
