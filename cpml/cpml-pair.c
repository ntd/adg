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
#include "cpml-macros.h"

#include <stdlib.h>
#include <string.h>


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
 * cpml_pair_intersection_pv_pv:
 * @pair: the destination #CpmlPair
 * @p1: the start point of the first line
 * @v1: the director of the first line
 * @p2: the start point of the second line
 * @v2: the director of the second line
 *
 * Given two lines (by specifying start point and director), gets
 * their intersection point and store it in @pair.
 *
 * Return value: @pair or %NULL on no intersection
 **/
CpmlPair *
cpml_pair_intersection_pv_pv(CpmlPair *pair,
                             const CpmlPair *p1, const CpmlVector *v1,
                             const CpmlPair *p2, const CpmlVector *v2)
{
    double divisor, factor;

    divisor = v1->x*v2->y - v1->y*v2->x;
    if (divisor == 0)
        return NULL;

    factor = ((p1->y - p2->y)*v2->x - (p1->x - p2->x)*v2->y) / divisor;

    pair->x = p1->x + v1->x * factor;
    pair->y = p1->y + v1->y * factor;

    return pair;
}

/**
 * cpml_pair_at_line:
 * @pair: the destination #CpmlPair
 * @p1: first point of the line
 * @p2: second point of the line
 * @t: the mediation value
 *
 * Given the mediation value @t, where 0 means the start point and
 * 1 the end point (0.5 the midpoint and so on), calculates the coordinates
 * of the point at @t of the way from @p1 and @p2.
 *
 * Return value: @pair
 **/
CpmlPair *
cpml_pair_at_line(CpmlPair *pair, const CpmlPair *p1, const CpmlPair *p2,
                  double t)
{
    CpmlPair delta;

    delta.x = (p2->x - p1->x) * t;
    delta.y = (p2->y - p1->y) * t;

    cpml_pair_add(cpml_pair_copy(pair, p1), &delta);

    return pair;
}

/**
 * cpml_pair_at_curve:
 * @pair: the destination #CpmlPair
 * @p1: start point
 * @p2: first control point
 * @p3: second control point
 * @p4: end point
 * @t: the mediation value
 *
 * Given the time value @t, returns the point on the specified Bézier curve
 * at time @t. Time values on Bézier curves are not evenly distributed, so
 * 0.5 is not necessarily the midpoint.
 *
 * Return value: @pair
 **/
CpmlPair *
cpml_pair_at_curve(CpmlPair *pair, const CpmlPair *p1, const CpmlPair *p2,
                   const CpmlPair *p3, const CpmlPair *p4, double t)
{
    double t1, t1_2, t1_3;
    double t_2, t_3;

    t1 = 1-t;
    t1_2 = t1*t1;
    t1_3 = t1_2*t1;
    t_2 = t*t;
    t_3 = t_2*t;

    pair->x = t1_3*p1->x + 3*t1_2*t*p2->x + 3*t1*t_2*p3->x + t_3*p4->x;
    pair->y = t1_3*p1->y + 3*t1_2*t*p2->y + 3*t1*t_2*p3->y + t_3*p4->y;

    return pair;
}


void
cpml_pair_negate(CpmlPair *pair)
{
    pair->x = - pair->x;
    pair->y = - pair->y;
}

cairo_bool_t
cpml_pair_invert(CpmlPair *pair)
{
    if (pair->x == 0 || pair->y == 0)
        return 0;

    pair->x = 1. / pair->x;
    pair->y = 1. / pair->y;
    return 1;
}

void
cpml_pair_add(CpmlPair *pair, const CpmlPair *src)
{
    pair->x += src->x;
    pair->y += src->y;
}

void
cpml_pair_sub(CpmlPair *pair, const CpmlPair *src)
{
    pair->x -= src->x;
    pair->y -= src->y;
}

void
cpml_pair_mul(CpmlPair *pair, const CpmlPair *src)
{
    pair->x *= src->x;
    pair->y *= src->y;
}

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
 * cpml_vector_from_pair:
 * @vector: the destination vector
 * @pair: the source pair
 * @length: the length of the vector
 *
 * Given the line L passing throught the origin and @pair, gets the
 * coordinate of the point on this line far @length from the origin
 * and store the result in @vector. If @pair itsself is the origin,
 * NULL is returned.
 *
 * @pair and @vector can be the same struct.
 *
 * Return value: @vector
 **/
CpmlVector *
cpml_vector_from_pair(CpmlVector *vector, const CpmlPair *pair, double length)
{
    double divisor = cpml_pair_distance(NULL, pair);

    if (divisor <= 0)
        return NULL;

    divisor /= length;
    vector->x = pair->x / divisor;
    vector->y = pair->y / divisor;

    return vector;
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
    static double cached_angle = 0;
    static CpmlPair cached_vector = { 1, 0 };

    /* Check for cached result */
    if (angle == cached_angle)
        return cpml_pair_copy(vector, &cached_vector);

    /* Check for common conditions */
    if (angle == CPML_DIR_UP) {
        vector->x = 0;
        vector->y = -1;
        return vector;
    }
    if (angle == CPML_DIR_DOWN) {
        vector->x = 0;
        vector->y = +1;
        return vector;
    }
    if (angle == CPML_DIR_LEFT) {
        vector->x = -1;
        vector->y = 0;
        return vector;
    }
    if (angle == CPML_DIR_RIGHT) {
        vector->x = +1;
        vector->y = 0;
        return vector;
    }

    /* Computation and cache registration */
    vector->x = cos(angle);
    vector->y = -sin(angle);
    cached_angle = angle;
    cpml_pair_copy(&cached_vector, vector);

    return vector;
}

/**
 * cpml_vector_at_curve:
 * @vector: the destination #CpmlVector
 * @p1: start point
 * @p2: first control point
 * @p3: second control point
 * @p4: end point
 * @t: the mediation value
 * @length: vector length
 *
 * Given the time value @t, returns the slope on the specified Bézier curve
 * at time @t. The slope is returned as a vector of arbitrary magnitude.
 *
 * Return value: @vector
 **/
CpmlVector *
cpml_vector_at_curve(CpmlVector *vector,
                     const CpmlPair *p1, const CpmlPair *p2,
                     const CpmlPair *p3, const CpmlPair *p4,
                     double t, double length)
{
    CpmlPair p21, p32, p43;
    double t1, t1_2, t_2;

    cpml_pair_sub(cpml_pair_copy(&p21, p2), p1);
    cpml_pair_sub(cpml_pair_copy(&p32, p3), p2);
    cpml_pair_sub(cpml_pair_copy(&p43, p4), p3);
    t1 = 1-t;
    t1_2 = t1*t1;
    t_2 = t*t;

    vector->x = 3*t1_2*p21.x + 6*t1*t*p32.x + 3*t_2*p43.x;
    vector->y = 3*t1_2*p21.y + 6*t1*t*p32.y + 3*t_2*p43.y;

    return cpml_vector_from_pair(vector, vector, length);
}

/**
 * cpml_vector_angle:
 * @vector: the source #CpmlVector
 *
 * Gets the angle of @vector, in radians. If @vector is (0, 0),
 * %CPML_DIR_RIGHT is returned.
 *
 * Return value: the angle in radians
 **/
double
cpml_vector_angle(const CpmlVector *vector)
{
    static CpmlPair cached_vector = { 1., 0. };
    static double cached_angle = 0.;

    /* Check for cached result */
    if (vector->x == cached_vector.x && vector->y == cached_vector.y)
        return cached_angle;

    /* Check for common conditions */
    if (vector->y == 0)
        return vector->x >= 0 ? CPML_DIR_RIGHT : CPML_DIR_LEFT;
    if (vector->x == 0)
        return vector->y > 0 ? CPML_DIR_DOWN : CPML_DIR_UP;
    if (vector->x == vector->y)
        return vector->x > 0 ? M_PI_4 * 7 : M_PI_4 * 3;
    if (vector->x == -vector->y)
        return vector->x > 0 ? M_PI_4 : M_PI_4 * 5;

    /* Computation and cache registration */
    cached_angle = atan(-vector->y / vector->x);
    cpml_pair_copy(&cached_vector, vector);

    return cached_angle;
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
