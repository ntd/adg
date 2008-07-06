/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


/**
 * SECTION:adgpair
 * @title: AdgPair
 * @short_description: A generic (x,y) structure
 *
 * The AdgPair is a generic 2D structure. It can be used to represent points,
 * sizes, offsets or whatever have %x and %y components.
 *
 * The name comes from MetaPost.
 */

/**
 * AdgPair:
 * @x: the x component of the pair
 * @y: the y component of the pair
 *
 * A generic 2D structure.
 */

/**
 * AdgVector:
 * @x: the x component of the vector
 * @y: the y component of the vector
 *
 * A subclass of an #AdgPair. A vector represents the coordinates of a point
 * distant 1 from the origin. The vectors are useful to define a direction
 * instead of angles and are better suited to simplify interpolations.
 */

#include "adg-pair.h"
#include "adg-util.h"

#include <string.h>


GType
adg_pair_get_type (void)
{
  static int pair_type = 0;
  
  if G_UNLIKELY (pair_type == 0)
    pair_type = g_boxed_type_register_static ("AdgPair", (GBoxedCopyFunc) adg_pair_dup, g_free);

  return pair_type;
}

/**
 * adg_pair_dup:
 * @pair: an #AdgPair structure
 *
 * Duplicates @pair.
 *
 * Return value: the duplicate of @pair: must be freed with g_free()
 *               when no longer needed.
 */
AdgPair *
adg_pair_dup (const AdgPair *pair)
{
  g_return_val_if_fail (pair != NULL, NULL);

  return g_memdup (pair, sizeof (AdgPair));
}

/**
 * adg_pair_get_angle:
 * @pair: an #AdgPair or #AdgVector structure
 *
 * Gets the angle (in radians) of the line passing throught @pair and the
 * origin. If @pair is the origin, the function returns #CPML_DIR_RIGHT.
 *
 * Return value: requested angle in radians (between 0 and 2pi)
 *               or #ADG_NAN on errors
 */
double
adg_pair_get_angle (const AdgPair *pair)
{
  static AdgPair cached_pair = { 0.0, 0.0 };
  static double  cached_angle = 0.0;
  double         angle;

  g_return_val_if_fail (adg_pair_is_set (pair), ADG_NAN);

  /* Check for cached result */
  if (pair->x == cached_pair.x && pair->y == cached_pair.y)
    angle = cached_angle;
  /* Catch common cases */
  else if (pair->y == 0.0)
    angle = pair->x >= 0.0 ? CPML_DIR_RIGHT : CPML_DIR_LEFT;
  else if (pair->x == 0.0)
    angle = pair->y > 0.0 ? CPML_DIR_UP : CPML_DIR_DOWN;
  else if (pair->x == pair->y)
    angle = pair->x > 0.0 ? G_PI_4 : 5.0 * G_PI_4;
  else if (pair->x == -pair->y)
    angle = pair->x > 0.0 ? 7.0 * G_PI_4 : 3.0 * G_PI_4;
  /* Grab other cases: pair->x is != 0.0 because of the previous checks */
  else
    {
      angle = atan (pair->y / pair->x);

      if (pair->x < 0.0)
        angle += G_PI;
      else if (pair->y < 0.0)
        angle += 2.0 * G_PI;

      /* Cache registration */
      cached_pair = *pair;
      cached_angle = angle;
    }

  return angle;
}

/**
 * adg_pair_mid:
 * @pair: an #AdgPair structure
 * @pair2: the second #AdgPair point
 *
 * Gets the mid pair between @pair and @pair2 and store the result in @pair.
 *
 * Return value: @pair
 */
AdgPair *
adg_pair_mid (AdgPair       *pair,
              const AdgPair *pair2)
{
  g_return_val_if_fail (adg_pair_is_set (pair), pair);
  g_return_val_if_fail (adg_pair_is_set (pair2), pair);

  pair->x = (pair->x + pair2->x) / 2.0;
  pair->y = (pair->y + pair2->y) / 2.0;

  return pair;
}

/**
 * adg_pair_transform:
 * @pair: an #AdgPair structure
 * @matrix: an #AdgMatrix matrix
 *
 * Transforms @pair using the specified matrix and store the result in @pair.
 * This is merely a wrapper that calls cairo_matrix_transform_distance().
 *
 * Return value: @pair
 */
AdgPair *
adg_pair_transform (AdgPair         *pair,
                    const AdgMatrix *matrix)
{
  g_return_val_if_fail (adg_pair_is_set (pair), pair);
  g_return_val_if_fail (matrix != NULL, pair);

  cairo_matrix_transform_distance (matrix, &pair->x, &pair->y);

  return pair;
}

/**
 * adg_pair_intersection:
 * @pair: an #AdgPair structure
 * @vector: an #AdgVector structure
 * @pair2: the second #AdgPair structure
 * @vector2: the second #AdgVector structure
 *
 * Calculates the intersection of the line passing throught @pair with the
 * @vector direction and the line passing throught @pair2 with the @vector2
 * direction. The resulting coordinates are stored in @pair.
 *
 * Return value: @pair
 */
AdgPair *
adg_pair_intersection (AdgPair         *pair,
                       const AdgVector *vector,
                       const AdgPair   *pair2,
                       const AdgVector *vector2)
{
  AdgPair p2;
  double  k;

  g_return_val_if_fail (adg_pair_is_set (pair), pair);
  g_return_val_if_fail (adg_pair_is_set (vector), pair);
  g_return_val_if_fail (adg_pair_is_set (pair2), pair);
  g_return_val_if_fail (adg_pair_is_set (vector2), pair);

  /* To simplify the operations, I want to move the first point to the origin,
   * so I must translate the second point of -pair. */
  p2.x = pair2->x - pair->x;
  p2.y = pair2->y - pair->y;
  k = (p2.y*vector2->x - p2.x*vector2->y) / (vector->y*vector2->x - vector->x*vector2->y);

  pair->x += k*vector->x;
  pair->y += k*vector->y;
  return pair;
}

/**
 * adg_pair_projection:
 * @pair: an #AdgPair structure
 * @pair2: the second #AdgPair structure
 * @vector2: an #AdgVector structure
 *
 * Calculates the projection of @pair on the line passing throught @pair2
 * with the @vector2 direction and stores the result in @pair.
 *
 * Return value: @pair
 */
AdgPair *
adg_pair_projection (AdgPair         *pair,
                     const AdgPair   *pair2,
                     const AdgVector *vector2)
{
  AdgVector      vector;
  cairo_matrix_t matrix;

  g_return_val_if_fail (adg_pair_is_set (vector2), pair);

  cairo_matrix_init_rotate (&matrix, M_PI/2.);
  cpml_pair_copy (&vector, vector2);
  cairo_matrix_transform_distance (&matrix, &vector.x, &vector.y);

  return adg_pair_intersection (pair, &vector, pair2, vector2);
}
