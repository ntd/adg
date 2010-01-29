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
#include "adg-intl.h"

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
 * A convenient constant providing an identity matrix.
 *
 * Returns: a pointer to the identity matrix
 **/
const AdgMatrix *
adg_matrix_identity(void)
{
    static AdgMatrix *identity_matrix = NULL;

    if (G_UNLIKELY(identity_matrix == NULL)) {
        identity_matrix = g_new(AdgMatrix, 1);
        cairo_matrix_init_identity(identity_matrix);
    }

    return identity_matrix;
}

/**
 * adg_matrix_null:
 *
 * A convenient constant providing an null matrix, that is a matrix
 * where all components are 0.
 *
 * Returns: a pointer to the null matrix
 **/
const AdgMatrix *
adg_matrix_null(void)
{
    static AdgMatrix *null_matrix = NULL;

    if (G_UNLIKELY(null_matrix == NULL))
        null_matrix = g_new0(AdgMatrix, 1);

    return null_matrix;
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
 * Gets rid of the scaling component of a matrix. The algorithm used
 * has been found sperimentally so it could luckely be plain wrong.
 *
 * Returns: %TRUE on success, %FALSE on errors
 **/
gboolean
adg_matrix_normalize(AdgMatrix *matrix)
{
    gdouble k;

    g_return_val_if_fail(matrix != NULL, FALSE);

    if (matrix->xx != matrix->yy || matrix->xy != -matrix->yx) {
        /* TODO: does normalization make sense on these matrices? */
        g_warning(_("%s: anamorphic matrices not supported"), G_STRLOC);
        return FALSE;
    }

    if (matrix->xy == 0) {
        k = matrix->xx;
    } else if (matrix->xx == 0) {
        k = matrix->xy;
    } else {
        k = sqrt(matrix->xx * matrix->xx + matrix->xy * matrix->xy);
    }

    g_return_val_if_fail(k != 0, FALSE);

    matrix->xx /= k;
    matrix->xy /= k;
    matrix->yy /= k;
    matrix->yx /= k;

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
    AdgMatrix normalized;

    g_return_if_fail(matrix != NULL);
    g_return_if_fail(transformation != NULL);

    switch (mode) {
    case ADG_TRANSFORM_NONE:
        break;
    case ADG_TRANSFORM_BEFORE:
        cairo_matrix_multiply(matrix, transformation, matrix);
        break;
    case ADG_TRANSFORM_AFTER:
        cairo_matrix_multiply(matrix, matrix, transformation);
        break;
    case ADG_TRANSFORM_BEFORE_NORMALIZED:
        adg_matrix_copy(&normalized, transformation);
        adg_matrix_normalize(&normalized);
        cairo_matrix_multiply(matrix, &normalized, matrix);
        break;
    case ADG_TRANSFORM_AFTER_NORMALIZED:
        adg_matrix_copy(&normalized, transformation);
        adg_matrix_normalize(&normalized);
        cairo_matrix_multiply(matrix, matrix, &normalized);
        break;
    default:
        g_return_if_reached();
    }
}

/**
 * adg_matrix_dump:
 * @matrix: an #AdgMatrix
 *
 * Dumps the specified @matrix to stdout. Useful for debugging purposes.
 **/
void
adg_matrix_dump(const AdgMatrix *matrix)
{
    g_return_if_fail(matrix != NULL);

    g_print("[%8.3lf %8.3lf] [%8.3lf]\n"
            "[%8.3lf %8.3lf] [%8.3lf]\n",
            matrix->xx, matrix->xy, matrix->x0,
            matrix->yx, matrix->yy, matrix->y0);
}
