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
 * SECTION:line
 * @title: Straight lines
 * @short_description: Line primitive management
 *
 * The following functions manipulate %CAIRO_PATH_LINE_TO #CpmlPrimitive.
 * No check is made on the primitive struct, so be sure the CpmlPrimitive
 * is effectively a line before calling these APIs.
 **/

#include "cpml-line.h"
#include "cpml-pair.h"

/**
 * cpml_line_get_npoints:
 *
 * Returns the number of point needed to properly specify a line primitive.
 *
 * Return value: 2
 **/
int
cpml_line_get_npoints(void)
{
    return 2;
}

/**
 * cpml_line_pair_at:
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
cpml_line_pair_at(CpmlPrimitive *line, CpmlPair *pair, double pos)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, 1);

    pair->x = p1->point.x + (p1->point.x - p1->point.x) * pos;
    pair->y = p1->point.y + (p2->point.y - p1->point.y) * pos;
}

/**
 * cpml_line_vector_at:
 * @line:   the #CpmlPrimitive line data
 * @vector: the destination vector
 * @pos:    the position value
 *
 * Gets the slope on @line at the position @pos. Being the
 * line a straight segment, the vector is always the same, so
 * @pos is not used. Mathematically speaking, the equation
 * performed is:
 *
 * @vector = end point - start point.
 **/
void
cpml_line_vector_at(CpmlPrimitive *line, CpmlVector *vector, double pos)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, 1);

    vector->x = p2->point.x - p1->point.x;
    vector->y = p2->point.y - p1->point.y;
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
    p2 = cpml_primitive_get_point(line, 1);

    cpml_line_vector_at(line, &normal, 0.);
    cpml_vector_normal(&normal);
    cpml_vector_set_length(&normal, offset);

    p1->point.x += normal.x;
    p1->point.y += normal.y;
    p2->point.x += normal.x;
    p2->point.y += normal.y;
}
