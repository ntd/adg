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
 * @from or @to could be %NULL, in which case the fallback (0, 0) pair
 * will be used.
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
 * @from or @to could be %NULL, in which case the fallback (0, 0) pair
 * will be used.
 *
 * Return value: 1 if @distance was properly set, 0 on errors
 */
cairo_bool_t
cpml_pair_square_distance(const CpmlPair *from, const CpmlPair *to,
			  double *distance)
{
	double x, y;

	if (from == NULL)
		from = &fallback_pair;
	if (to == NULL)
		to = &fallback_pair;

	x = to->x - from->x;
	y = to->y - from->y;
	*distance = x*x + y*y;
	return 1;
}

/**
 * cpml_pair_angle:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 * @angle: where to store the result
 *
 * Gets the angle between @from and @to, storing the result in @angle.
 *
 * @from or @to could be %NULL, in which case the fallback (0, 0) pair
 * will be used.
 *
 * Return value: 1 if @angle was properly set, 0 on errors
 */
cairo_bool_t
cpml_pair_angle(const CpmlPair *from, const CpmlPair *to, double *angle)
{
	static CpmlPair cached_pair = { 1., 0. };
	static double   cached_angle = 0.;
	CpmlPair        pair;

	if (from == NULL)
		from = &fallback_pair;
	if (to == NULL)
		to = &fallback_pair;

	pair.x = to->x - from->x;
	pair.y = to->y - from->y;

	/* Check for cached result */
	if (pair.x == cached_pair.x && pair.y == cached_pair.y) {
		*angle = cached_angle;
	} else if (pair.y == 0.) {
		*angle = pair.x >= 0. ? CPML_DIR_RIGHT : CPML_DIR_LEFT;
	} else if (pair.x == 0.) {
		*angle = pair.y > 0. ? CPML_DIR_UP : CPML_DIR_DOWN;
	} else if (pair.x == pair.y) {
		*angle = pair.x > 0. ? M_PI / 4. : 5. * M_PI / 4.;
	} else if (pair.x == -pair.y) {
		*angle = pair.x > 0. ? 7. * M_PI / 4. : 3. * M_PI / 4.;
	} else {
		*angle = atan(pair.y / pair.x);

		if (pair.x < 0.0)
			*angle += M_PI;
		else if (pair.y < 0.0)
			*angle += 2.0 * M_PI;

		/* Cache registration */
		cached_angle = *angle;
		cpml_pair_copy(&cached_pair, &pair);
	}

	return 1;
}

/**
 * cpml_vector_from_pair:
 * @vector: an allocated #CpmlPair struct
 * @pair: the source pair
 *
 * Unitizes @pair, that is given the line L passing throught the origin and
 * @pair, gets the coordinate of the point on this line far 1.0 from
 * the origin, and store the result in @vector.
 *
 * @pair and @vector can be the same struct.
 *
 * Return value: 1 if @vector was properly set, 0 on errors
 */
cairo_bool_t
cpml_vector_from_pair(CpmlPair *vector, const CpmlPair *pair)
{
	double length;

	if (!cpml_pair_distance(pair, NULL, &length) || length == 0.)
		return 0;

	vector->x = pair->x/length;
	vector->y = pair->y/length;
	return 1;
}

/**
 * cpml_vector_from_angle:
 * @vector: an allocated #CpmlPair struct
 * @angle: angle of direction, in radians
 *
 * Calculates the coordinates of the point far 1.0 from the origin in the
 * @angle direction. The result is stored in @vector.
 *
 * Return value: 1 if @vector was properly set, 0 on errors
 */
cairo_bool_t
cpml_vector_from_angle(CpmlPair *vector, double angle)
{
	static double cached_angle = 0.;
	static CpmlPair cached_vector = { 1., 0. };

	/* Check for cached result */
	if (angle == cached_angle) {
		vector->x = cached_vector.x;
		vector->y = cached_vector.y;
	} else if (angle == CPML_DIR_RIGHT) {
		vector->x = +1.;
		vector->y =  0.;
	} else if (angle == CPML_DIR_UP) {
		vector->x =  0.;
		vector->y = -1.;
	} else if (angle == CPML_DIR_LEFT) {
		vector->x = -1.;
		vector->y =  0.;
	} else if (angle == CPML_DIR_DOWN) {
		vector->x =  0.;
		vector->y = +1.;
	} else {
		vector->x = cos(angle);
		vector->y = sin(angle);

		/* Cache registration */
		cached_angle = angle;
		cpml_pair_copy(&cached_vector, vector);
	}

	return 1;
}
