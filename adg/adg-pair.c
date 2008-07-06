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
