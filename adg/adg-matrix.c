/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * SECTION:matrix
 * @title: AdgMatrix
 * @short_description: A wrapper for the cairo_matrix_t struct
 *
 * The AdgMatrix is essentially a wrapper for #cairo_matrix_t structures.
 */

#include "adg-matrix.h"
#include <string.h>


static AdgMatrix null_matrix = { 0., 0., 0., 0., 0., 0. };
static AdgMatrix identity_matrix = { 1., 0., 0., 1., 0., 0. };


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
 * adg_matrix_dup:
 * @matrix: an #AdgMatrix structure
 *
 * Duplicates @matrix.
 *
 * Return value: the duplicate of @matrix: must be freed with g_free()
 *               when no longer needed.
 */
AdgMatrix *
adg_matrix_dup(const AdgMatrix * matrix)
{
    g_return_val_if_fail(matrix != NULL, NULL);

    return g_memdup(matrix, sizeof(AdgMatrix));
}

/**
 * adg_matrix_get_fallback:
 *
 * Gets a fallback matrix. The fallback matrix is a statically allocated
 * identity matrix.
 *
 * Return value: the fallback matrix
 */
const AdgMatrix *
adg_matrix_get_fallback(void)
{
    return &identity_matrix;
}

/**
 * adg_matrix_init_null:
 * @matrix: the #AdgMatrix to nullify
 *
 * Nullifies a matrix, setting all its components to 0.
 */
void
adg_matrix_init_null(AdgMatrix * matrix)
{
    memcpy(matrix, &null_matrix, sizeof(AdgMatrix));
}

/**
 * adg_matrix_is_null:
 * @matrix: the #AdgMatrix to check
 *
 * Checks if a matrix is a nullified matrix.
 *
 * Return value: %TRUE if the matrix is a null matrix, %FALSE otherwise
 */
gboolean
adg_matrix_is_null(const AdgMatrix * matrix)
{
    return memcmp(matrix, &null_matrix, sizeof(AdgMatrix)) == 0;
}

/**
 * adg_matrix_set:
 * @matrix: the destination #AdgMatrix
 * @src: the source #AdgMatrix
 *
 * Copies @matrix to @dst.
 *
 * Return value: @matrix
 */
AdgMatrix *
adg_matrix_set(AdgMatrix * matrix, const AdgMatrix * src)
{
    g_return_val_if_fail(matrix != NULL, matrix);
    g_return_val_if_fail(src != NULL, matrix);

    memcpy(matrix, src, sizeof(AdgMatrix));

    return matrix;
}

/**
 * adg_matrix_equal:
 * @matrix1: an #AdgMatrix
 * @matrix2: an #AdgMatrix
 *
 * Compares @matrix1 and @matrix2 and returns %TRUE if the matrices are equal.
 *
 * Return value: %TRUE if @matrix1 is equal to @matrix2, %FALSE otherwise
 */
gboolean
adg_matrix_equal(const AdgMatrix * matrix1, const AdgMatrix * matrix2)
{
    g_return_val_if_fail(matrix1 != NULL, FALSE);
    g_return_val_if_fail(matrix2 != NULL, FALSE);

    return memcmp(matrix1, matrix2, sizeof(AdgMatrix)) == 0;
}
