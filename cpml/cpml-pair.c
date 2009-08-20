/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, 2009  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:pair
 * @title: CpmlPair
 * @short_description: A structure holding a couple of values
 *
 * The CpmlPair is a generic 2D structure. It can be used to represent
 * coordinates, sizes, offsets or whatever have two components.
 *
 * The name comes from MetaFont.
 **/

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
 * and length at once. Keep in mind the cairo default coordinates system
 * is not problably what you expect: the x axis increases at right
 * (as usual) but the y axis increases at down (the reverse of a usual
 * cartesian plan). An angle of 0 is at V=(1; 0) (middle right).
 **/

#include "cpml-pair.h"
#include "cpml-macros.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


static CpmlPair fallback_pair = { 0, 0 };


/**
 * cpml_pair_copy:
 * @pair: the destination #CpmlPair
 * @src: the source #CpmlPair
 *
 * Copies @src in @pair.
 *
 * Return value: @pair
 **/
CpmlPair *
cpml_pair_copy(CpmlPair *pair, const CpmlPair *src)
{
    return memcpy(pair, src, sizeof(CpmlPair));
}

/**
 * cpml_pair_from_cairo:
 * @pair: the destination #CpmlPair
 * @path_data: the original path data point
 *
 * Gets @pair from a #cairo_path_data_t struct. @path_data should contains
 * a point data: it is up to the caller to be sure @path_data is valid.
 *
 * Return value: @pair
 **/
CpmlPair *
cpml_pair_from_cairo(CpmlPair *pair, const cairo_path_data_t *path_data)
{
    pair->x = path_data->point.x;
    pair->y = path_data->point.y;
    return pair;
}


/**
 * cpml_pair_negate:
 * @pair: a #CpmlPair
 *
 * Negates the coordinates of @pair.
 **/
void
cpml_pair_negate(CpmlPair *pair)
{
    pair->x = - pair->x;
    pair->y = - pair->y;
}

/**
 * cpml_pair_invert:
 * @pair: a #CpmlPair
 *
 * Inverts (1/x) the coordinates of @pair. If @pair cannot be inverted
 * because one coordinate is 0, 0 is returned and no transformation is
 * performed.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_pair_invert(CpmlPair *pair)
{
    if (pair->x == 0 || pair->y == 0)
        return 0;

    pair->x = 1. / pair->x;
    pair->y = 1. / pair->y;
    return 1;
}

/**
 * cpml_pair_add:
 * @pair: the destination #CpmlPair
 * @src:  the source pair to add
 *
 * Adds @src to @pair and stores the result in @pair. In other words,
 * @pair = @pair + @src.
 **/
void
cpml_pair_add(CpmlPair *pair, const CpmlPair *src)
{
    pair->x += src->x;
    pair->y += src->y;
}

/**
 * cpml_pair_sub:
 * @pair: the destination #CpmlPair
 * @src:  the source pair to subtract
 *
 * Subtracts @src from @pair and stores the result in @pair. In other words,
 * @pair = @pair - @src.
 **/
void
cpml_pair_sub(CpmlPair *pair, const CpmlPair *src)
{
    pair->x -= src->x;
    pair->y -= src->y;
}

/**
 * cpml_pair_mul:
 * @pair: the destination #CpmlPair
 * @src:  the source pair factor
 *
 * Multiplies the x coordinate of @pair by the @src x factor and the
 * y coordinate by the @src y factor.
 **/
void
cpml_pair_mul(CpmlPair *pair, const CpmlPair *src)
{
    pair->x *= src->x;
    pair->y *= src->y;
}

/**
 * cpml_pair_div:
 * @pair: the destination #CpmlPair
 * @src:  the source pair divisor
 *
 * Divides the x coordinate of @pair by the @src x divisor and the
 * y coordinate by the @src y divisor. If @pair cannot be divided
 * because of a division by 0, 0 is returned and no transformation
 * is performed.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_pair_div(CpmlPair *pair, const CpmlPair *src)
{
    if (src->x == 0 || src->y == 0)
        return 0;

    pair->x /= src->x;
    pair->y /= src->y;
    return 1;
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
 * cpml_vector_from_angle:
 * @vector: the destination #CpmlVector
 * @angle: angle of direction, in radians
 * @length: the length of the vector
 *
 * Calculates the coordinates of the point far @length from the origin
 * in the @angle direction. The result is stored in @vector.
 *
 * Return value: @vector
 **/
CpmlVector *
cpml_vector_from_angle(CpmlVector *vector, double angle, double length)
{
    /* Check for common conditions */
    if (angle == -M_PI_2) {
        vector->x = 0;
        vector->y = -1;
        return vector;
    }
    if (angle == M_PI_2) {
        vector->x = 0;
        vector->y = +1;
        return vector;
    }
    if (angle == M_PI || angle == -M_PI) {
        vector->x = -1;
        vector->y = 0;
        return vector;
    }
    if (angle == 0) {
        vector->x = +1;
        vector->y = 0;
        return vector;
    }

    vector->x = cos(angle);
    vector->y = sin(angle);

    return vector;
}

/**
 * cpml_vector_set_length:
 * @vector: a #CpmlVector
 * @length: the new length
 *
 * Imposes the specified @length to @vector. If the old length is 0
 * (and so the direction is not known), nothing happens.
 **/
void
cpml_vector_set_length(CpmlVector *vector, double length)
{
    double divisor = cpml_pair_distance(NULL, vector);

    /* Check for valid length (anything other than 0) */
    if (divisor <= 0)
        return;

    divisor /= length;
    vector->x /= divisor;
    vector->y /= divisor;
}

/**
 * cpml_vector_angle:
 * @vector: the source #CpmlVector
 *
 * Gets the angle of @vector, in radians. If @vector is (0, 0),
 * 0 is returned.
 *
 * Return value: the angle in radians, a value between -M_PI and M_PI
 **/
double
cpml_vector_angle(const CpmlVector *vector)
{
    /* Check for common conditions */
    if (vector->y == 0)
        return vector->x >= 0 ?      0 :  M_PI;
    if (vector->x == 0)
        return vector->y > 0 ?  M_PI_2 : -M_PI_2;
    if (vector->x == vector->y)
        return vector->x > 0 ?  M_PI_4 : -M_PI_4 * 3;
    if (vector->x == -vector->y)
        return vector->x > 0 ? -M_PI_4 :  M_PI_4 * 3;

    return atan2(vector->y, vector->x);
}

/**
 * cpml_vector_normal:
 * @vector: the subject #CpmlVector
 *
 * Stores in @vector a vector normal to the original @vector.
 * The length is retained.
 *
 * The algorithm is really quick because no trigonometry is involved.
 **/
void
cpml_vector_normal(CpmlVector *vector)
{
    double tmp = vector->x;

    vector->x = -vector->y;
    vector->y = tmp;
}

/**
 * cpml_pair_to_cairo:
 * @pair: the destination #CpmlPair
 * @path_data: the original path data point
 *
 * Sets a #cairo_path_data_t struct to @pair. This is exactly the reverse
 * operation of cpml_pair_from_cairo().
 **/
void
cpml_pair_to_cairo(const CpmlPair *pair, cairo_path_data_t *path_data)
{
    path_data->point.x = pair->x;
    path_data->point.y = pair->y;
}
