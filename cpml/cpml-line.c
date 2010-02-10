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
 * SECTION:cpml-line
 * @Section_Id:CpmlLine
 * @title: CpmlLine
 * @short_description: APIs manipulating straight lines
 *
 * The following functions manipulate %CAIRO_PATH_LINE_TO #CpmlPrimitive.
 * No validation is made on the input so use the following methods
 * only when you are sure the <varname>primitive</varname> argument
 * is effectively a straingt line.
 **/


#include "cpml-line.h"
#include "cpml-pair.h"
#include <stdlib.h>


static cairo_bool_t     intersection            (const CpmlPair *p,
                                                 CpmlPair       *dest,
                                                 double         *get_factor);


/**
 * cpml_line_type_get_npoints:
 *
 * Returns the number of point needed to properly specify a line primitive.
 *
 * Returns: 2
 **/
int
cpml_line_type_get_npoints(void)
{
    return 2;
}

/**
 * cpml_line_get_length:
 * @line: the #CpmlPrimitive line data
 *
 * Given the @line primitive, returns the distance between its
 * start and end points.
 *
 * Returns: the requested distance, that is the @line length
 **/
double
cpml_line_get_length(const CpmlPrimitive *line)
{
    CpmlPair p1, p2;

    cpml_pair_from_cairo(&p1, cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p2, cpml_primitive_get_point(line, -1));

    return cpml_pair_distance(&p1, &p2);
}

/**
 * cpml_line_put_extents:
 * @line: the #CpmlPrimitive line data
 * @extents: where to store the extents
 *
 * Given a @line primitive, returns its boundary box in @extents.
 **/
void
cpml_line_put_extents(const CpmlPrimitive *line, CpmlExtents *extents)
{
    CpmlPair p1, p2;

    extents->is_defined = 0;

    cpml_pair_from_cairo(&p1, cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p2, cpml_primitive_get_point(line, -1));

    cpml_extents_pair_add(extents, &p1);
    cpml_extents_pair_add(extents, &p2);
}

/**
 * cpml_line_put_pair_at:
 * @line: the #CpmlPrimitive line data
 * @pair: the destination pair
 * @pos:  the position value
 *
 * Given the @line line, finds the coordinates at position @pos
 * (where 0 is the start and 1 is the end) and stores the result
 * in @pair.
 *
 * @pos can be less than 0 or greater than 1, in which case the
 * coordinates are interpolated.
 **/
void
cpml_line_put_pair_at(const CpmlPrimitive *line, double pos, CpmlPair *pair)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    pair->x = p1->point.x + (p2->point.x - p1->point.x) * pos;
    pair->y = p1->point.y + (p2->point.y - p1->point.y) * pos;
}

/**
 * cpml_line_put_vector_at:
 * @line:   the #CpmlPrimitive line data
 * @vector: the destination vector
 * @pos:    the position value
 *
 * Gets the slope on @line at the position @pos. Being the
 * line a straight segment, the vector is always the same, so
 * @pos is not used. Mathematically speaking, the equation
 * performed is:
 *
 * @vector = endpoint(@line) - startpoint(@line).
 **/
void
cpml_line_put_vector_at(const CpmlPrimitive *line, double pos,
                        CpmlVector *vector)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    vector->x = p2->point.x - p1->point.x;
    vector->y = p2->point.y - p1->point.y;
}

/**
 * cpml_line_get_closest_pos:
 * @line: the #CpmlPrimitive line data
 * @pair: the coordinates of the subject point
 *
 * Returns the pos value of the point on @line nearest to @pair.
 * The returned value is always between 0 and 1.
 *
 * The point nearest to @pair is got by finding the its
 * projection on @line, as this is when the point is closer to
 * a line primitive.
 *
 * Returns: the pos value, always between 0 and 1
 **/
double
cpml_line_get_closest_pos(const CpmlPrimitive *line, const CpmlPair *pair)
{
    CpmlPair p[4];
    CpmlVector normal;
    double pos;

    cpml_pair_from_cairo(&p[0], cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p[1], cpml_primitive_get_point(line, -1));

    cpml_pair_copy(&normal, &p[1]);
    cpml_pair_sub(&normal, &p[2]);
    cpml_vector_normal(&normal);

    cpml_pair_copy(&p[2], pair);
    cpml_pair_copy(&p[3], pair);
    cpml_pair_add(&p[3], &normal);

    /* Ensure to return 0 if intersection() fails */
    pos = 0;
    intersection(p, NULL, &pos);

    /* Clamp the result to 0..1 */
    if (pos < 0)
        pos = 0;
    else if (pos > 1.)
        pos = 1.;

    return pos;
}

/**
 * cpml_line_put_intersections:
 * @line:  the first line
 * @line2: the second line
 * @max:   maximum number of intersections to return
 *         (that is, the size of @dest)
 * @dest:  a vector of #CpmlPair
 *
 * Given two lines (@line and @line2), gets their intersection point
 * and store the result in @dest.
 *
 * If @max is 0, the function returns 0 immediately without any
 * further processing. If @line and @line2 are cohincident,
 * their intersections are not considered.
 *
 * Returns: the number of intersections found (max 1)
 *          or 0 if the primitives do not intersect
 **/
int
cpml_line_put_intersections(const CpmlPrimitive *line,
                            const CpmlPrimitive *line2,
                            int max, CpmlPair *dest)
{
    CpmlPair p[4];

    if (max == 0)
        return 0;

    cpml_pair_from_cairo(&p[0], cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p[1], cpml_primitive_get_point(line, -1));
    cpml_pair_from_cairo(&p[2], cpml_primitive_get_point(line2, 0));
    cpml_pair_from_cairo(&p[3], cpml_primitive_get_point(line2, -1));

    return intersection(p, dest, NULL) ? 1 : 0;
}

/**
 * cpml_line_offset:
 * @line:   the #CpmlPrimitive line data
 * @offset: distance for the computed parallel line
 *
 * Given a line segment specified by the @line primitive data,
 * computes the parallel line distant @offset from the original one
 * and returns the result by changing @line.
 **/
void
cpml_line_offset(CpmlPrimitive *line, double offset)
{
    cairo_path_data_t *p1, *p2;
    CpmlVector normal;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    cpml_line_put_vector_at(line, 0, &normal);
    cpml_vector_normal(&normal);
    cpml_vector_set_length(&normal, offset);

    p1->point.x += normal.x;
    p1->point.y += normal.y;
    p2->point.x += normal.x;
    p2->point.y += normal.y;
}


static cairo_bool_t
intersection(const CpmlPair *p, CpmlPair *dest, double *get_factor)
{
    CpmlVector v[2];
    double factor;

    cpml_pair_copy(&v[0], &p[1]);
    cpml_pair_sub(&v[0], &p[0]);
    cpml_pair_copy(&v[1], &p[3]);
    cpml_pair_sub(&v[1], &p[2]);
    factor = v[0].x * v[1].y - v[0].y * v[1].x;

    /* Check for equal slopes (the lines are parallel) */
    if (factor == 0)
        return 0;

    factor = ((p[0].y - p[2].y) * v[1].x -
              (p[0].x - p[2].x) * v[1].y) / factor;

    if (dest != NULL) {
        dest->x = p[0].x + v[0].x * factor;
        dest->y = p[0].y + v[0].y * factor;
    }

    if (get_factor != NULL)
        *get_factor = factor;

    return 1;
}
