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
_adg_test_angle(void)
{
    AdgRuledFill *ruled_fill;
    gdouble angle;

    ruled_fill = adg_ruled_fill_new();

    /* Using the public APIs */
    adg_ruled_fill_set_angle(ruled_fill, G_PI_2);
    angle = adg_ruled_fill_get_angle(ruled_fill);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    adg_ruled_fill_set_angle(ruled_fill, G_PI + 1);
    angle = adg_ruled_fill_get_angle(ruled_fill);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    adg_ruled_fill_set_angle(ruled_fill, -1);
    angle = adg_ruled_fill_get_angle(ruled_fill);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    adg_ruled_fill_set_angle(ruled_fill, 0);
    angle = adg_ruled_fill_get_angle(ruled_fill);
    g_assert_cmpfloat(angle, ==, 0);

    /* Using GObject property methods */
    g_object_set(ruled_fill, "angle", G_PI_2, NULL);
    g_object_get(ruled_fill, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    g_object_set(ruled_fill, "angle", G_PI + 1, NULL);
    g_object_get(ruled_fill, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    g_object_set(ruled_fill, "angle", (gdouble) -1, NULL);
    g_object_get(ruled_fill, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, G_PI_2);

    g_object_set(ruled_fill, "angle", (gdouble) 0, NULL);
    g_object_get(ruled_fill, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, ==, 0);

    g_object_unref(ruled_fill);
}

static void
_adg_test_line_dress(void)
{
    AdgRuledFill *ruled_fill;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress line_dress;

    ruled_fill = adg_ruled_fill_new();
    valid_dress_1 = ADG_DRESS_LINE_AXIS;
    valid_dress_2 = ADG_DRESS_LINE_HIDDEN;
    incompatible_dress = ADG_DRESS_TABLE;

    /* Using the public APIs */
    adg_ruled_fill_set_line_dress(ruled_fill, valid_dress_1);
    line_dress = adg_ruled_fill_get_line_dress(ruled_fill);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_ruled_fill_set_line_dress(ruled_fill, incompatible_dress);
    line_dress = adg_ruled_fill_get_line_dress(ruled_fill);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_ruled_fill_set_line_dress(ruled_fill, valid_dress_2);
    line_dress = adg_ruled_fill_get_line_dress(ruled_fill);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(ruled_fill, "line-dress", valid_dress_1, NULL);
    g_object_get(ruled_fill, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(ruled_fill, "line-dress", incompatible_dress, NULL);
    g_object_get(ruled_fill, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(ruled_fill, "line-dress", valid_dress_2, NULL);
    g_object_get(ruled_fill, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    g_object_unref(ruled_fill);
}

static void
_adg_test_spacing(void)
{
    AdgRuledFill *ruled_fill;
    gdouble spacing;

    ruled_fill = adg_ruled_fill_new();

    /* Using the public APIs */
    adg_ruled_fill_set_spacing(ruled_fill, 0);
    spacing = adg_ruled_fill_get_spacing(ruled_fill);
    g_assert_cmpfloat(spacing, ==, 0);

    adg_ruled_fill_set_spacing(ruled_fill, -1);
    spacing = adg_ruled_fill_get_spacing(ruled_fill);
    g_assert_cmpfloat(spacing, ==, 0);

    adg_ruled_fill_set_spacing(ruled_fill, 123);
    spacing = adg_ruled_fill_get_spacing(ruled_fill);
    g_assert_cmpfloat(spacing, ==, 123);

    /* Using GObject property methods */
    g_object_set(ruled_fill, "spacing", (gdouble) 0, NULL);
    g_object_get(ruled_fill, "spacing", &spacing, NULL);
    g_assert_cmpfloat(spacing, ==, 0);

    g_object_set(ruled_fill, "spacing", (gdouble) -1, NULL);
    g_object_get(ruled_fill, "spacing", &spacing, NULL);
    g_assert_cmpfloat(spacing, ==, 0);

    g_object_set(ruled_fill, "spacing", (gdouble) 123, NULL);
    g_object_get(ruled_fill, "spacing", &spacing, NULL);
    g_assert_cmpfloat(spacing, ==, 123);

    g_object_unref(ruled_fill);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/ruled-fill/type/object", ADG_TYPE_RULED_FILL);

    adg_test_add_func("/adg/ruled-fill/property/angle", _adg_test_angle);
    adg_test_add_func("/adg/ruled-fill/property/line-dress", _adg_test_line_dress);
    adg_test_add_func("/adg/ruled-fill/property/spacing", _adg_test_spacing);

    return g_test_run();
}
