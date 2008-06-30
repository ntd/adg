/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpmlpair
 * @title: CpmlPair
 * @short_description: A structure holding a couple of values
 *
 * The CpmlPair is a generic 2D structure. It can be used to represent points,
 * sizes, offsets or whatever have two components.
 *
 * The name comes from MetaPost.
 */

/**
 * CpmlPair:
 * @x: the x component of the pair
 * @y: the y component of the pair
 *
 * A generic 2D structure.
 */

/**
 * CpmlVector:
 * @x: the x component of the vector
 * @y: the y component of the vector
 *
 * A subclass of a #CpmlPair. A vector represents the coordinates of a point
 * distant 1 from the origin (0, 0). The vectors are useful to define a
 * direction and are better suited than angles for simplifying interpolations.
 */

#include "cpml-pair.h"


/**
 * cpml_pair_copy:
 * @pair: the destination #CpmlPair struct
 * @src: the source #CpmlPair struct
 *
 * Assign @src to @pair.
 *
 * Return value: 1 if @pair was set, 0 on errors
 */
cairo_bool_t
cpml_pair_copy(CpmlPair       *pair,
	       const CpmlPair *src)
{
	pair->x = src->x;
	pair->y = src->y;
	return 1;
}
