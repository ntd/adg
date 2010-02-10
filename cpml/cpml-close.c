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
 * SECTION:cpml-close
 * @Section_Id:CpmlClose
 * @title: CpmlClose
 * @short_description: Straigth line used to close cyclic segments
 *
 * The following functions manipulate %CAIRO_PATH_CLOSE_PATH
 * #CpmlPrimitive. No validation is made on the input so use the
 * following methods only when you are sure the
 * <varname>primitive</varname> argument is effectively a close path.
 *
 * This primitive management is almost identical to straight lines,
 * but taking properly start and end points.
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-close.h"
#include "cpml-line.h"


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
 * Returns: 2
 **/
int
cpml_close_type_get_npoints(void)
{
    return 2;
}

/**
 * cpml_close_put_pair_at:
 * @close: the #CpmlPrimitive close data
 * @pos:  the position value
 * @pair: the destination pair
 *
 * Given the @close path virtual primitive, finds the coordinates
 * at position @pos (where 0 is the start and 1 is the end) and
 * stores the result in @pair.
 *
 * @pos can be less than 0 or greater than 1, in which case the
 * coordinates are interpolated.
 **/
void
cpml_close_put_pair_at(const CpmlPrimitive *close, double pos, CpmlPair *pair)
{
    cpml_line_put_pair_at(close, pos, pair);
}

/**
 * cpml_close_put_vector_at:
 * @close:  the #CpmlPrimitive close data
 * @vector: the destination vector
 * @pos:    the position value
 *
 * Gets the slope on @close at the position @pos. Being the
 * close a straight line, the vector is always the same, so
 * @pos is not used.
 **/
void
cpml_close_put_vector_at(const CpmlPrimitive *close, double pos,
                         CpmlVector *vector)
{
    cpml_line_put_vector_at(close, pos, vector);
}

/**
 * cpml_close_get_closest_pos:
 * @close: the #CpmlPrimitive close data
 * @pair: the coordinates of the subject point
 *
 * Returns the pos value of the point on @close nearest to @pair.
 * The returned value is always between 0 and 1.
 *
 * Returns: the pos value, always between 0 and 1
 **/
double
cpml_close_get_closest_pos(const CpmlPrimitive *close, const CpmlPair *pair)
{
    return cpml_line_get_closest_pos(close, pair);
}

/**
 * cpml_close_offset:
 * @close:  the #CpmlPrimitive close data
 * @offset: distance for the computed parallel close
 *
 * Given a close segment specified by the @close primitive data,
 * computes the parallel close distant @offset from the original one
 * and returns the result by changing @close.
 **/
void
cpml_close_offset(CpmlPrimitive *close, double offset)
{
    cpml_line_offset(close, offset);
}
