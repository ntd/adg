/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
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
 **/

/**
 * CpmlVector:
 * @x: the x component of the vector
 * @y: the y component of the vector
 *
 * Another name for #CpmlPair. It is used to clarify when a function expects
 * a pair or a vector.
 *
 * A vector represents a line starting from the origin (0,0) and ending
 * to the given coordinates pair. Vectors are useful to define directions
 * and length at once.
 **/

#include "cpml-pair.h"

#include <stdlib.h>
#include <string.h>


static CpmlPair fallback_pair = { 0., 0. };


/**
 * cpml_pair_copy:
 * @pair: the destination #CpmlPair struct
 * @src: the source #CpmlPair struct
 *
 * Assign @src to @pair.
 *
 * Return value: the destination pair
 **/
CpmlPair *
cpml_pair_copy(CpmlPair *pair, const CpmlPair *src)
{
    return memcpy(pair, src, sizeof(CpmlPair));
}

/**
 * cpml_pair_transform:
 * @pair: the destination #CpmlPair struct
 * @matrix: the transformation matrix
 *
 * Shortcut to apply a specific transformation matrix to @pair.
 **/
void
cpml_pair_transform(CpmlPair *pair, const cairo_matrix_t *matrix)
{
    cairo_matrix_transform_point(matrix, &pair->x, &pair->y);
}


/**
 * cpml_pair_squared_distance:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 *
 * Gets the squared distance between @from and @to. This value is useful
 * for comparation purpose: if you need to get the real distance, use
 * cpml_pair_distance().
 *
 * @from or @to could be %NULL, in which case the fallback (0, 0) pair
 * will be used.
 *
 * Return value: the squared distance
 **/
double
cpml_pair_squared_distance(const CpmlPair *from, const CpmlPair *to)
{
    double x, y;

    if (from == NULL)
        from = &fallback_pair;
    if (to == NULL)
        to = &fallback_pair;

    x = to->x - from->x;
    y = to->y - from->y;

    return x * x + y * y;
}

/**
 * cpml_pair_distance:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 * @distance: where to store the result
 *
 * Gets the distance between @from and @to, storing the result in @distance.
 * If you need this value only for comparation purpose, you could use
 * cpm_pair_squared_distance() instead.
 *
 * @from or @to could be %NULL, in which case the fallback (0, 0) pair
 * will be used.
 *
 * The algorithm used is adapted from:
 * "Replacing Square Roots by Pythagorean Sums"
 * by Clave Moler and Donald Morrison (1983)
 * IBM Journal of Research and Development 27 (6): 577–581
 * http://www.research.ibm.com/journal/rd/276/ibmrd2706P.pdf
 *
 * Return value: the distance
 **/
double
cpml_pair_distance(const CpmlPair *from, const CpmlPair *to)
{
    double x, y;
    double p, q, r, s;

    if (from == NULL)
        from = &fallback_pair;
    if (to == NULL)
        to = &fallback_pair;

    x = to->x - from->x;
    y = to->y - from->y;

    if (x < 0)
        x = -x;
    if (y < 0)
        y = -y;

    if (x > y) {
        p = x;
        q = y;
    } else {
        p = y;
        q = x;
    }

    if (p > 0) {
        for (;;) {
            r = q/p;
            r *= r;
            if (r == 0)
                break;

            s = r / (4+r);
            p += 2*s*p;
            q *= s;
        }
    }

    return p;
}

/**
 * cpml_pair_angle:
 * @from: the first #CpmlPair struct
 * @to: the second #CpmlPair struct
 * @angle: where to store the result
 *
 * Returns the angle between @from and @to, in radians. @from or @to
 * could be %NULL, in which case the fallback (0, 0) pair will be used.
 *
 * Return value: the angle in radians
 **/
double
cpml_pair_angle(const CpmlPair *from, const CpmlPair *to)
{
    static CpmlPair cached_pair = { 1., 0. };
    static double cached_angle = 0.;
    CpmlPair pair;

    if (from == NULL)
        from = &fallback_pair;
    if (to == NULL)
        to = &fallback_pair;

    pair.x = to->x - from->x;
    pair.y = to->y - from->y;

    /* Check for cached result */
    if (pair.x == cached_pair.x && pair.y == cached_pair.y)
        return cached_angle;
    if (pair.y == 0.)
        return pair.x >= 0. ? CPML_DIR_RIGHT : CPML_DIR_LEFT;
    if (pair.x == 0.)
        return pair.y > 0. ? CPML_DIR_UP : CPML_DIR_DOWN;
    if (pair.x == pair.y)
        return pair.x > 0. ? M_PI / 4. : 5. * M_PI / 4.;
    if (pair.x == -pair.y)
        return pair.x > 0. ? 7. * M_PI / 4. : 3. * M_PI / 4.;

    /* Cache registration */
    cached_angle = atan(pair.y / pair.x);
    cpml_pair_copy(&cached_pair, &pair);

    return cached_angle;
}

/**
 * cpml_vector_from_pair:
 * @vector: the destination vector
 * @pair: the source pair
 * @length: the final vector length
 *
 * Unitizes @pair, that is given the line L passing throught the origin and
 * @pair, gets the coordinate of the point on this line far @length from
 * the origin, and store the result in @vector. If @pair itsself is the origin,
 * (0, 0) is returned.
 *
 * @pair and @vector can be the same struct.
 **/
void
cpml_vector_from_pair(CpmlVector *vector, const CpmlPair *pair, double length)
{
    double distance, factor;

    distance = cpml_pair_distance(NULL, pair);
    factor = length / distance;

    vector->x = pair->x * factor;
    vector->y = pair->y * factor;
}

/**
 * cpml_vector_from_angle:
 * @vector: the destination vector
 * @angle: angle of direction, in radians
 * @length: the final vector length
 *
 * Calculates the coordinates of the point far @length from the origin
 * in the @angle direction. The result is stored in @vector.
 **/
void
cpml_vector_from_angle(CpmlVector *vector, double angle, double length)
{
    static double cached_angle = 0.;
    static CpmlPair cached_vector = { 1., 0. };

    /* Check for cached result */
    if (angle == cached_angle) {
        vector->x = cached_vector.x;
        vector->y = cached_vector.y;
    } else {
        vector->x = cos(angle);
        vector->y = sin(angle);

        /* Cache registration */
        cached_angle = angle;
        cpml_pair_copy(&cached_vector, vector);
    }

    vector->x *= length;
    vector->y *= length;
}

/**
 * cpml_vector_normal:
 * @vector: the destination vector
 * @src: the source vector
 *
 * Stores in @vector a vector normal to @src with the same length.
 * @vector and @src can be the same struct.
 **/
void
cpml_vector_normal(CpmlVector *vector, const CpmlVector *src)
{
    double x, y;

    x = src->x;
    y = src->y;

    vector->x = -y;
    vector->y = x;
}
