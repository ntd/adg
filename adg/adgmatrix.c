/**
 * SECTION:adgmatrix
 * @title: AdgMatrix
 * @short_description: A cairo_matrix_t structure
 *
 * The AdgMatrix is essentially a wrapper for #cairo_matrix_t structures.
 **/

#include "adgmatrix.h"

#include <string.h>


GType
adg_matrix_get_type (void)
{
  static int matrix_type = 0;
  
  if G_UNLIKELY (matrix_type == 0)
    matrix_type = g_boxed_type_register_static ("AdgMatrix", (GBoxedCopyFunc) adg_matrix_dup, g_free);

  return matrix_type;
}

/**
 * adg_matrix_get_fallback:
 *
 * Gets a fallback matrix. The fallback matrix is a statically allocated
 * identity matrix.
 *
 * Return value: the fallback matrix
 **/
const AdgMatrix *
adg_matrix_get_fallback (void)
{
  static AdgMatrix fallback_matrix = { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

  return &fallback_matrix;
}

/**
 * adg_matrix_dup:
 * @matrix: an #AdgMatrix structure
 *
 * Duplicates @matrix.
 *
 * Return value: the duplicate of @matrix: must be freed with g_free()
 *               when no longer needed.
 **/
AdgMatrix *
adg_matrix_dup (const AdgMatrix *matrix)
{
  g_return_val_if_fail (matrix != NULL, NULL);

  return g_memdup (matrix, sizeof (AdgMatrix));
}

/**
 * adg_matrix_set:
 * @matrix: the destination #AdgMatrix
 * @src: the source #AdgMatrix
 *
 * Copies @matrix to @dst.
 *
 * Return value: @matrix
 **/
AdgMatrix *
adg_matrix_set (AdgMatrix       *matrix,
                const AdgMatrix *src)
{
  g_return_val_if_fail (matrix != NULL, matrix);
  g_return_val_if_fail (src != NULL, matrix);

  memcpy (matrix, src, sizeof (AdgMatrix));

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
 **/
gboolean
adg_matrix_equal (const AdgMatrix *matrix1,
                  const AdgMatrix *matrix2)
{
  g_return_val_if_fail (matrix1 != NULL, FALSE);
  g_return_val_if_fail (matrix2 != NULL, FALSE);

  return memcmp (matrix1, matrix2, sizeof (AdgMatrix)) == 0;
}
