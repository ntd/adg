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
_adg_behavior_misc(void)
{
    const cairo_matrix_t *matrix;

    matrix = adg_matrix_identity();
    adg_assert_isapprox(matrix->xx, 1);
    adg_assert_isapprox(matrix->yx, 0);
    adg_assert_isapprox(matrix->xy, 0);
    adg_assert_isapprox(matrix->yy, 1);
    adg_assert_isapprox(matrix->x0, 0);
    adg_assert_isapprox(matrix->y0, 0);

    matrix = adg_matrix_null();
    adg_assert_isapprox(matrix->xx, 0);
    adg_assert_isapprox(matrix->yx, 0);
    adg_assert_isapprox(matrix->xy, 0);
    adg_assert_isapprox(matrix->yy, 0);
    adg_assert_isapprox(matrix->x0, 0);
    adg_assert_isapprox(matrix->y0, 0);
}

static void
_adg_method_copy(void)
{
    cairo_matrix_t original = { 1, 2, 3, 4, 5, 6 };
    cairo_matrix_t matrix = { 0 };

    /* Check sanity */
    adg_matrix_copy(&matrix, NULL);
    adg_matrix_copy(NULL, &original);

    g_assert_cmpfloat(original.xx, !=, matrix.xx);
    g_assert_cmpfloat(original.yx, !=, matrix.yx);
    g_assert_cmpfloat(original.xy, !=, matrix.xy);
    g_assert_cmpfloat(original.yy, !=, matrix.yy);
    g_assert_cmpfloat(original.x0, !=, matrix.x0);
    g_assert_cmpfloat(original.y0, !=, matrix.y0);

    adg_matrix_copy(&matrix, &original);

    adg_assert_isapprox(original.xx, matrix.xx);
    adg_assert_isapprox(original.yx, matrix.yx);
    adg_assert_isapprox(original.xy, matrix.xy);
    adg_assert_isapprox(original.yy, matrix.yy);
    adg_assert_isapprox(original.x0, matrix.x0);
    adg_assert_isapprox(original.y0, matrix.y0);
}

static void
_adg_method_dup(void)
{
    cairo_matrix_t original = { 1, 2, 3, 4, 5, 6 };
    cairo_matrix_t *matrix;

    /* Check sanity */
    g_assert_null(adg_matrix_dup(NULL));

    matrix = adg_matrix_dup(&original);

    adg_assert_isapprox(original.xx, matrix->xx);
    adg_assert_isapprox(original.yx, matrix->yx);
    adg_assert_isapprox(original.xy, matrix->xy);
    adg_assert_isapprox(original.yy, matrix->yy);
    adg_assert_isapprox(original.x0, matrix->x0);
    adg_assert_isapprox(original.y0, matrix->y0);

    g_free(matrix);
}

static void
_adg_method_equal(void)
{
    cairo_matrix_t original = { 1, 2, 3, 4, 5, 6 };
    cairo_matrix_t matrix = { 0 };

    /* Check sanity */
    adg_matrix_equal(&matrix, NULL);
    adg_matrix_equal(NULL, &original);

    g_assert_false(adg_matrix_equal(&matrix, &original));
    adg_matrix_copy(&matrix, &original);
    g_assert_true(adg_matrix_equal(&matrix, &original));
    matrix.xy = 0;
    g_assert_false(adg_matrix_equal(&matrix, &original));
}

static void
_adg_method_normalize(void)
{
    cairo_matrix_t matrix;

    /* Check sanity */
    g_assert_false(adg_matrix_normalize(NULL));
    adg_matrix_copy(&matrix, adg_matrix_null());
    g_assert_false(adg_matrix_normalize(&matrix));

    /* Normalization of anamorphic matrices is not supported */
    adg_matrix_copy(&matrix, adg_matrix_identity());
    matrix.xx = 2;
    matrix.yy = 3;
    g_assert_false(adg_matrix_normalize(&matrix));
    adg_matrix_copy(&matrix, adg_matrix_identity());
    matrix.xy = 2;
    matrix.yx = 3;
    g_assert_false(adg_matrix_normalize(&matrix));

    adg_matrix_copy(&matrix, adg_matrix_identity());
    g_assert_true(adg_matrix_normalize(&matrix));
    adg_matrix_equal(&matrix, adg_matrix_identity());
    matrix.xx = 5;
    matrix.yy = 5;
    g_assert_true(adg_matrix_normalize(&matrix));
    adg_assert_isapprox(matrix.xx, 1);
    adg_assert_isapprox(matrix.yy, 1);
    adg_assert_isapprox(matrix.xy, 0);
    adg_assert_isapprox(matrix.yx, 0);

    matrix.xx = 3;
    matrix.yy = 3;
    matrix.xy = 4;
    matrix.yx = -4;
    g_assert_true(adg_matrix_normalize(&matrix));
    adg_assert_isapprox(matrix.xx, 0.6);
    adg_assert_isapprox(matrix.yy, 0.6);
    adg_assert_isapprox(matrix.xy, 0.8);
    adg_assert_isapprox(matrix.yx, -0.8);

    g_assert_true(adg_matrix_normalize(&matrix));
    adg_assert_isapprox(matrix.xx, 0.6);
    adg_assert_isapprox(matrix.yy, 0.6);
    adg_assert_isapprox(matrix.xy, 0.8);
    adg_assert_isapprox(matrix.yx, -0.8);

    matrix.xx = 0;
    matrix.yy = 0;
    matrix.xy = -3;
    matrix.yx = 3;
    g_assert_true(adg_matrix_normalize(&matrix));
    adg_assert_isapprox(matrix.xx, 0);
    adg_assert_isapprox(matrix.yy, 0);
    adg_assert_isapprox(matrix.xy, 1);
    adg_assert_isapprox(matrix.yx, -1);
}

static void
_adg_method_transform(void)
{
    cairo_matrix_t original = { 1, 2, 3, 4, 5, 6 };
    cairo_matrix_t matrix;
    cairo_matrix_t map = { 2, 0, 0, 2, 1, 1 };

    adg_matrix_copy(&matrix, &original);

    /* Check sanity */
    adg_matrix_transform(NULL, &map, ADG_TRANSFORM_NONE);
    adg_matrix_transform(&matrix, NULL, ADG_TRANSFORM_NONE);

    g_assert_true(adg_matrix_equal(&matrix, &original));

    adg_matrix_transform(&matrix, &map, ADG_TRANSFORM_NONE);
    g_assert_true(adg_matrix_equal(&matrix, &original));

    adg_matrix_transform(&matrix, adg_matrix_identity(), ADG_TRANSFORM_BEFORE);
    g_assert_true(adg_matrix_equal(&matrix, &original));

    adg_matrix_transform(&matrix, &map, ADG_TRANSFORM_BEFORE);
    adg_assert_isapprox(matrix.xx, 2);
    adg_assert_isapprox(matrix.yx, 4);
    adg_assert_isapprox(matrix.xy, 6);
    adg_assert_isapprox(matrix.yy, 8);
    adg_assert_isapprox(matrix.x0, 9);
    adg_assert_isapprox(matrix.y0, 12);

    adg_matrix_copy(&matrix, &original);
    adg_matrix_transform(&matrix, &map, ADG_TRANSFORM_AFTER);
    adg_assert_isapprox(matrix.xx, 2);
    adg_assert_isapprox(matrix.yx, 4);
    adg_assert_isapprox(matrix.xy, 6);
    adg_assert_isapprox(matrix.yy, 8);
    adg_assert_isapprox(matrix.x0, 11);
    adg_assert_isapprox(matrix.y0, 13);

    adg_matrix_copy(&matrix, &original);
    adg_matrix_transform(&matrix, &map, ADG_TRANSFORM_BEFORE_NORMALIZED);
    adg_assert_isapprox(matrix.xx, 1);
    adg_assert_isapprox(matrix.yx, 2);
    adg_assert_isapprox(matrix.xy, 3);
    adg_assert_isapprox(matrix.yy, 4);
    adg_assert_isapprox(matrix.x0, 9);
    adg_assert_isapprox(matrix.y0, 12);

    adg_matrix_copy(&matrix, &original);
    adg_matrix_transform(&matrix, &map, ADG_TRANSFORM_AFTER_NORMALIZED);
    adg_assert_isapprox(matrix.xx, 1);
    adg_assert_isapprox(matrix.yx, 2);
    adg_assert_isapprox(matrix.xy, 3);
    adg_assert_isapprox(matrix.yy, 4);
    adg_assert_isapprox(matrix.x0, 6);
    adg_assert_isapprox(matrix.y0, 7);
}

static void
_adg_method_dump(gint i)
{
    switch (i) {
    case 1:
        adg_matrix_dump(adg_matrix_identity());
        break;
    default:
        g_test_trap_assert_passed();
        g_test_trap_assert_stderr_unmatched("?");

        /* The identity matrix has only 0 and 1 */
        g_test_trap_assert_stdout("*1.000*");
        g_test_trap_assert_stdout("*0.000*");
        g_test_trap_assert_stdout_unmatched("*2*");
        g_test_trap_assert_stdout_unmatched("*3*");
        g_test_trap_assert_stdout_unmatched("*4*");
        g_test_trap_assert_stdout_unmatched("*5*");
        g_test_trap_assert_stdout_unmatched("*6*");
        g_test_trap_assert_stdout_unmatched("*7*");
        g_test_trap_assert_stdout_unmatched("*8*");
        g_test_trap_assert_stdout_unmatched("*9*");
    }
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_boxed_checks("/adg/matrix/type/boxed", CAIRO_GOBJECT_TYPE_MATRIX, g_new0(cairo_matrix_t, 1));

    g_test_add_func("/adg/matrix/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/matrix/method/dup", _adg_method_dup);
    g_test_add_func("/adg/matrix/method/copy", _adg_method_copy);
    g_test_add_func("/adg/matrix/method/equal", _adg_method_equal);
    g_test_add_func("/adg/matrix/method/normalize", _adg_method_normalize);
    g_test_add_func("/adg/matrix/method/transform", _adg_method_transform);
    adg_test_add_traps("/adg/matrix/method/dump", _adg_method_dump, 1);

    return g_test_run();
}
