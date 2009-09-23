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
 * @matrix: an #AdgMatrix structure
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

    return memcmp(matrix1, matrix2, sizeof(AdgMatrix)) == 0;
}
