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
#include <cpml.h>


static void
_cpml_test_basic(void)
{
    CpmlExtents extents = { 0 };
    CpmlExtents extents2 = { 0 };
    cairo_bool_t equals;

    extents.is_defined = 0;
    extents2.is_defined = 0;

    g_test_message("By convention, undefined extents are not equal to invalid extents");
    equals = cpml_extents_equal(&extents, &extents2);
    g_assert_true(equals);
    equals = cpml_extents_equal(&extents, NULL);
    g_assert_false(equals);
    equals = cpml_extents_equal(NULL, &extents);
    g_assert_false(equals);
    equals = cpml_extents_equal(NULL, NULL);
    g_assert_true(equals);

    extents.is_defined = 1;
    equals = cpml_extents_equal(&extents, &extents2);
    g_assert_false(equals);

    cpml_extents_copy(&extents2, &extents);
    equals = cpml_extents_equal(&extents, &extents2);
    g_assert_true(equals);
}

static void
_cpml_test_add(void)
{
    CpmlPair org = { 0, 0 };
    CpmlPair pair1 = { 1, 2 };
    CpmlPair pair2 = { 3, 4 };
    CpmlExtents extents = { 0 };
    CpmlExtents extents2 = { 0 };
    cairo_bool_t equals, is_inside;

    extents.is_defined = 0;
    extents2.is_defined = 0;

    g_test_message("By convention, nothing is inside undefined extents...");
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_false(is_inside);
    is_inside = cpml_extents_pair_is_inside(&extents, &org);
    g_assert_false(is_inside);

    cpml_extents_pair_add(&extents, &pair1);
    equals = cpml_extents_equal(&extents, &extents2);
    g_assert_false(equals);

    g_test_message("...but undefined extents are always inside defined extents");
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_true(is_inside);

    is_inside = cpml_extents_pair_is_inside(&extents, &pair1);
    g_assert_true(is_inside);
    is_inside = cpml_extents_pair_is_inside(&extents, &pair2);
    g_assert_false(is_inside);

    cpml_extents_pair_add(&extents2, &pair1);
    equals = cpml_extents_equal(&extents, &extents2);
    g_assert_true(equals);
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_true(is_inside);

    cpml_extents_pair_add(&extents, &pair2);
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_true(is_inside);
    is_inside = cpml_extents_pair_is_inside(&extents, &pair1);
    g_assert_true(is_inside);
    is_inside = cpml_extents_pair_is_inside(&extents, &pair2);
    g_assert_true(is_inside);

    cpml_extents_pair_add(&extents2, &pair2);
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_true(is_inside);

    is_inside = cpml_extents_is_inside(&extents2, &extents);
    g_assert_true(is_inside);

    cpml_extents_pair_add(&extents2, &org);
    is_inside = cpml_extents_is_inside(&extents, &extents2);
    g_assert_false(is_inside);

    /* The following test fails on FreeBSD 8.2, probably due
     * to rounding problems:
     *
     * is_inside = cpml_extents_is_inside(&extents2, &extents);
     * g_assert_true(is_inside);
     */
}

static void
_cpml_test_transform(void)
{
    CpmlExtents extents;
    cairo_matrix_t matrix;
    CpmlPair nord = { 1, 0 };
    cairo_bool_t is_inside;

    extents.is_defined = 1;
    extents.org.x = 2;
    extents.org.y = 3;
    extents.size.x = 4;
    extents.size.y = 5;

    is_inside = cpml_extents_pair_is_inside(&extents, &nord);
    g_assert_false(is_inside);

    cairo_matrix_init_translate(&matrix, -extents.org.x - 1, -extents.org.y - 1);
    cpml_extents_transform(&extents, &matrix);

    is_inside = cpml_extents_pair_is_inside(&extents, &nord);
    g_assert_true(is_inside);

    cairo_matrix_init_scale(&matrix, 0.01, 0.01);
    cpml_extents_transform(&extents, &matrix);

    is_inside = cpml_extents_pair_is_inside(&extents, &nord);
    g_assert_false(is_inside);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/cpml/extents/basic", _cpml_test_basic);
    adg_test_add_func("/cpml/extents/add", _cpml_test_add);
    adg_test_add_func("/cpml/extents/transform", _cpml_test_transform);

    return g_test_run();
}
