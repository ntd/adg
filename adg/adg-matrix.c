/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


/**
 * SECTION:adg-matrix
 * @Section_Id:AdgMatrix
 * @title: AdgMatrix
 * @short_description: A wrapper for #cairo_matrix_t
 *
 * AdgMatrix is a wrapper in #GType syntax of the #cairo_matrix_t struct.
 **/

/**
 * AdgMatrix:
 *
 * Another name for #cairo_matrix_t: check its documentation for the
 * fields description and visibility details.
 **/


#include "adg-matrix.h"

#include <string.h>
#include <math.h>


GType
adg_matrix_get_type(void)
{
    static int matrix_type = 0;

    if (G_UNLIKELY(matrix_type == 0))
        matrix_type = g_boxed_type_register_static("AdgMatrix",
                                                   (GBoxedCopyFunc) adg_matrix_dup,
                                                   g_free);

    return matrix_type;
}

/**
 * adg_matrix_identity:
 *
 * A constant identity matrix provided as facility.
 *
 * Returns: a pointer to the identity matrix
 **/
const AdgMatrix *
adg_matrix_identity(void)
{
    static AdgMatrix identity_matrix;
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(!initialized)) {
        cairo_matrix_init_identity(&identity_matrix);
        initialized = TRUE;
    }

    return &identity_matrix;
}

/**
 * adg_matrix_copy:
 * @matrix: the destination #AdgMatrix
 * @src: the source #AdgMatrix
 *
 * Copies @matrix to @dst.
 *
 * Returns: @matrix
 **/
AdgMatrix *
adg_matrix_copy(AdgMatrix *matrix, const AdgMatrix *src)
{
    g_return_val_if_fail(matrix != NULL, matrix);
    g_return_val_if_fail(src != NULL, matrix);

    memcpy(matrix, src, sizeof(AdgMatrix));

    return matrix;
}

/**
 * adg_matrix_dup:
 * @matrix: the souce #AdgMatrix
 *
 * Duplicates @matrix.
 *
 * Returns: the duplicate of @matrix: must be freed with g_free()
 *          when no longer needed.
 **/
AdgMatrix *
adg_matrix_dup(const AdgMatrix *matrix)
{
    g_return_val_if_fail(matrix != NULL, NULL);

    return g_memdup(matrix, sizeof(AdgMatrix));
}

/**
 * adg_matrix_equal:
 * @matrix1: an #AdgMatrix
 * @matrix2: an #AdgMatrix
 *
 * Compares @matrix1 and @matrix2 and returns %TRUE if the matrices are equal.
 *
 * Returns: %TRUE if @matrix1 is equal to @matrix2, %FALSE otherwise
 **/
gboolean
adg_matrix_equal(const AdgMatrix *matrix1, const AdgMatrix *matrix2)
{
    g_return_val_if_fail(matrix1 != NULL, FALSE);
    g_return_val_if_fail(matrix2 != NULL, FALSE);

    /* XXX: I don't know if the following is always correct */
    return memcmp(matrix1, matrix2, sizeof(AdgMatrix)) == 0;
}

/**
 * adg_matrix_normalize:
 * @matrix: the source/destination #AdgMatrix
 *
 * Gets rid of the scaling component of a matrix: considering the
 * (0,0) and (1,1) points, this function normalizes @matrix by
 * forcing a %M_SQRT2 distance (that is the distance these points
 * have on the identity matrix). This is done by applying a common
 * factor to the <varname>xx</varname> and <varname>yy</varname>
 * components of @matrix, without considering translations.
 *
 * From the documentation of the
 * <ulink url="http://cairographics.org/manual/cairo-matrix.html#cairo-matrix-t">cairo matrix</ulink>
 * we have:
 *
 * |[
 * x_new = xx * x + xy * y + x0;
 * y_new = yx * x + yy * y + y0.
 * ]|
 *
 * Calling P0 = (0, 0) and P1 = (1, 1), their coordinates in @matrix,
 * applying an arbitrary factor (<varname>k</varname>) on the scaling
 * component, are:
 *
 * |[
 * P0.x = x0;
 * P0.y = y0;
 * P1.x = k * matrix->xx + matrix->xy + x0;
 * P1.y = k * matrix->yx + matrix->yy + y0.
 * ]|
 *
 * Translating @matrix of (-x0, -y0), as I don't care of the translation
 * component (the original translation will be kept intact):
 *
 * |[
 * P0.x = 0;
 * P0.y = 0;
 * P1.x = k * matrix->xx + matrix->xy;
 * P1.y = k * matrix->yx + matrix->yy.
 * ]|
 *
 * Let's force a distance of %2 on the sum of their squared difference,
 * as Pythagoras suggests:
 *
 * |[
 * (P1.x - P0.x)² + (P1.y - P0.y)² = 2.
 * (k matrix->xx + matrix->xy)² + (k matrix->yy + matrix->yx)² = 2.
 * k² matrix->xx² + matrix->xy² + 2 k matrix->xx matrix->xy +
 *     k² matrix->yy² + matrix->yx² + 2 k matrix->yy matrix->yx = 2.
 * k² (matrix->xx² + matrix->yy²) +
 *     2k (matrix->xx matrix->xy + matrix->yy matrix->yx) +
 *     matrix->xy² + matrix->yx² - 2  = 0.
 * ]|
 *
 * Now we'll calculate the <varname>k</varname> factor by calling
 * <varname>a</varname>, <varname>b</varname> and <varname>c</varname>
 * the respective coefficients and using the second degree equation
 * solver, taking only the greater root:
 *
 * |[
 * a = matrix->xx² + matrix->yy²;
 * b = 2 (matrix->xx matrix->xy + matrix->yy matrix->yx);
 * c = matrix->xy² + matrix->yx² - 2;
 *     -b + √(b² - 4ac)
 * k = ----------------.
 *            2a
 * ]|
 *
 * If the discriminant is less than %0 the negative square root is not
 * possible with real numbers, so @matrix is left untouched and
 * %FALSE is returned. Otherwise, once the <varname>k</varname> factor
 * is found, it is applied to the original matrix and %TRUE is returned.
 *
 * As a bonus, in the quite common case the original matrix has a scaling
 * ratio of %1 (that is matrix->xx == matrix->yy) and it is not rotated
 * (matrix->xy and matrix->yx are %0), we have:
 *
 * |[
 * // Shortcut for uniform scaled matrices not rotated:
 * a = 2 matrix->xx²;
 * b = 0;
 * c = -2;
 *     -b + √(b² - 4ac)   √(16 matrix->xx²)       1
 * k = ---------------- = ----------------- = ----------.
 *            2a            4 matrix->xx²     matrix->xx
 * ]|
 *
 * avoiding square root and a lot of multiplications.
 *
 * Returns: %TRUE on success, %FALSE on errors
 **/
gboolean
adg_matrix_normalize(AdgMatrix *matrix)
{
    gdouble k;

    g_return_val_if_fail(matrix != NULL, FALSE);

    if (matrix->xx == matrix->yy && matrix->xy == 0 && matrix->yx == 0) {
        /* Common situation: original matrix scaled with a
         * ratio of 1 and not rotated */
        k = 1. / matrix->xx;
    } else {
        gdouble a, b, c, d;

        a = matrix->xx * matrix->xx + matrix->yy * matrix->yy;
        b = (matrix->xx * matrix->xy + matrix->yy * matrix->yx) * 2;
        c = matrix->xy * matrix->xy + matrix->yx * matrix->yx - 2;

        /* Compute and check the discriminant */
        d = b * b - a * c * 4;
        if (d < 0)
            return FALSE;

        k = (sqrt(d) - b) / (a * 2);
    }

    /* Apply the normalization factor */
    matrix->xx *= k;
    matrix->yy *= k;

    return TRUE;
}

/**
 * adg_matrix_transform:
 * @matrix: the source/destination #AdgMatrix
 * @transformation: the transformation to apply
 * @mode: how @transformation should be applied
 *
 * Modifies @matrix applying @transformation in the way specified by
 * @mode.
 **/
void
adg_matrix_transform(AdgMatrix *matrix, const AdgMatrix *transformation,
                     AdgTransformMode mode)
{
    AdgMatrix tmp_matrix;

    g_return_if_fail(matrix != NULL);
    g_return_if_fail(transformation != NULL);

    switch (mode) {
    case ADG_TRANSFORM_NONE:
        break;
    case ADG_TRANSFORM_BEFORE:
        cairo_matrix_multiply(matrix, matrix, transformation);
        break;
    case ADG_TRANSFORM_AFTER:
        cairo_matrix_multiply(matrix, transformation, matrix);
        break;
    case ADG_TRANSFORM_BEFORE_NORMALIZED:
        adg_matrix_copy(&tmp_matrix, transformation);
        adg_matrix_normalize(&tmp_matrix);
        cairo_matrix_multiply(matrix, matrix, &tmp_matrix);
        break;
    case ADG_TRANSFORM_AFTER_NORMALIZED:
        adg_matrix_copy(&tmp_matrix, transformation);
        adg_matrix_normalize(&tmp_matrix);
        cairo_matrix_multiply(matrix, &tmp_matrix, matrix);
        break;
    }
}
