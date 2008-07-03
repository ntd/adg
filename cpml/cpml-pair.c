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
 * The name comes from MetaFont.
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

#include <stdlib.h>
#include <math.h>


static CpmlPair fallback_pair = {0., 0.};


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
cpml_pair_copy(CpmlPair *pair, const CpmlPair *src)
{
	pair->x = src->x;
	pair->y = src->y;
	return 1;
}

/**
 * cpml_pair_distance:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 * @distance: where to store the result
 *
 * Gets the distance between @from and @to, storing the result in @distance.
 * If you need this value only for comparation purpose, you could use
 * cpm_pair_squared_distance() instead that is a lot faster, since no square
 * root operation is involved.
 *
 * Return value: 1 if @distance was properly set, 0 on errors
 */
cairo_bool_t
cpml_pair_distance(const CpmlPair *from, const CpmlPair *to, double *distance)
{
	if (!cpml_pair_square_distance(from, to, distance))
		return 0;

	*distance = sqrt(*distance);
	return 1;
}

/**
 * cpml_pair_square_distance:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 * @distance: where to store the result
 *
 * Gets the square distance between @from and @to, storing the result in
 * @distance. This value is useful for comparation purpose: if you need to
 * get the real distance, use cpml_pair_distance().
 *
 * Return value: 1 if @distance was properly set, 0 on errors
 */
cairo_bool_t
cpml_pair_square_distance(const CpmlPair *from, const CpmlPair *to,
			  double *distance)
{
	double x, y;

	if (distance == NULL)
		return 0;
	if (from == NULL)
		from = &fallback_pair;
	if (to == NULL)
		to = &fallback_pair;

	x = to->x - from->x;
	y = to->y - from->y;
	*distance = x*x + y*y;
	return 1;
}
