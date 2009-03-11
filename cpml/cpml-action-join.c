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

#include "cpml-action.h"

/**
 * cpml_join:
 * @primitive1: the first #CpmlPrimitive
 * @primitive2: the second #CpmlPrimitive
 *
 * Joins two primitive modifying the end point of @primitive1 and the
 * start point of @primitive2 so that the resulting points will overlap.
 * 
 * This is done by extending the end vector of @primitive1 and the
 * start vector of @primitive2 and interpolating the intersection.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>This approach is quite naive when curves are involved.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: 1 on success, 0 if the end vector of @primitive1
 *               and the start vector of @primitive2 are parallel
 **/
cairo_bool_t
cpml_join(CpmlPrimitive *primitive1, CpmlPrimitive *primitive2)
{
    cairo_path_data_t *end1, *start2;
    CpmlPrimitive line1, line2;
    cairo_path_data_t data1[2], data2[2];
    CpmlPair joint;

    end1 = cpml_primitive_get_point(primitive1, -1);
    start2 = cpml_primitive_get_point(primitive2, 0);

    /* Check if the primitives are yet connected */
    if (end1->point.x == start2->point.x && end1->point.y == start2->point.y)
        return 1;

    line1.org = cpml_primitive_get_point(primitive1, -2);
    line1.data = data1;
    data1[0].header.type = CAIRO_PATH_LINE_TO;
    data1[1] = *end1;

    line2.org = start2;
    line2.data = data2;
    data2[0].header.type = CAIRO_PATH_LINE_TO;
    data2[1] = *cpml_primitive_get_point(primitive2, 1);

    if (!cpml_intersection(&line1, &line2, &joint))
        return 0;

    cpml_pair_to_cairo(&joint, end1);
    cpml_pair_to_cairo(&joint, start2);

    return 1;
}
