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
 * SECTION:close
 * @title: Closing path lines
 * @short_description: APIs to manage closing path virtual primitives
 *
 * The following functions manipulate %CAIRO_PATH_CLOSE_PATH
 * #CpmlPrimitive. No check is made on the primitive struct, so be
 * sure the <structname>CpmlPrimitive</structname> is effectively
 * a close operation before calling these APIs.
 *
 * This primitive management is almost identical to straight lines,
 * but taking properly start and end points.
 **/

#include "cpml-close.h"
#include "cpml-pair.h"

/**
 * cpml_close_type_get_npoints:
 *
 * Returns the number of points needed to properly specify a close primitive.
 * This is a bit tricky: the close path primitive can be specified with
 * a single point but it has an implicit second point, the start point
 * of the source segment. This means retrieving a second point from a
 * cairo path is a valid operation and must return the start point of
 * the source segment.
 *
 * Return value: 2
 **/
int
cpml_close_type_get_npoints(void)
{
    return 2;
}

/**
 * cpml_close_pair_at:
 * @close: the #CpmlPrimitive close data
 * @pair: the destination pair
 * @pos:  the position value
 *
 * Given the @close path virtual primitive, finds the coordinates
 * at position @pos (where 0 is the start and 1 is the end) and
 * stores the result in @pair.
 *
 * @pos can be less than 0 or greater than 1, in which case the
 * coordinates are interpolated.
 **/
void
cpml_close_pair_at(CpmlPrimitive *close, CpmlPair *pair, double pos)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(close, 0);
    p2 = cpml_primitive_get_point(close, -1);

    pair->x = p1->point.x + (p1->point.x - p1->point.x) * pos;
    pair->y = p1->point.y + (p2->point.y - p1->point.y) * pos;
}

/**
 * cpml_close_vector_at:
 * @close:  the #CpmlPrimitive close data
 * @vector: the destination vector
 * @pos:    the position value
 *
 * Gets the slope on @close at the position @pos. Being the
 * close a straight line, the vector is always the same, so
 * @pos is not used.
 **/
void
cpml_close_vector_at(CpmlPrimitive *close, CpmlVector *vector, double pos)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(close, 0);
    p2 = cpml_primitive_get_point(close, -1);

    vector->x = p2->point.x - p1->point.x;
    vector->y = p2->point.y - p1->point.y;
}

/**
 * cpml_close_offset:
 * @close:  the #CpmlPrimitive close data
 * @offset: distance for the computed parallel close
 *
 * Given a close segment specified by the @close primitive data,
 * computes the parallel close distant @offset from the original one
 * and returns the result by changing @close.
 *
 * The algorithm is practically the same as the one used for
 * cpml_line_offset() but using the proper start and end coordinates.
 **/
void
cpml_close_offset(CpmlPrimitive *close, double offset)
{
    cairo_path_data_t *p1, *p2;
    CpmlVector normal;

    p1 = cpml_primitive_get_point(close, 0);
    p2 = cpml_primitive_get_point(close, -1);

    cpml_close_vector_at(close, &normal, 0.);
    cpml_vector_normal(&normal);
    cpml_vector_set_length(&normal, offset);

    p1->point.x += normal.x;
    p1->point.y += normal.y;
    p2->point.x += normal.x;
    p2->point.y += normal.y;
}
