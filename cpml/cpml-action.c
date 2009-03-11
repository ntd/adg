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
 * SECTION:action
 * @title: Action
 * @short_description: Operation that can be executed on two primitives
 *
 * This section includes operations between two primitives, such as
 * joining, finding their intersection points and so on.
 **/

#include "cpml-action.h"

/**
 * cpml_join_primitives:
 * @from: the first #CpmlPrimitive
 * @to: the second #CpmlPrimitive
 *
 * Joins two primitive modifying the end point of @from and the
 * start point of @to so that, when terminated, the points will overlap.
 * 
 * This is done by extending the end vector of @from and the
 * start vector of @to and interpolating the intersection.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>This approach is quite naive when curves are involved.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: 1 on success, 0 if the end vector of @from
 *               and the start vector of @to are parallel
 **/
cairo_bool_t
cpml_join_primitives(CpmlPrimitive *from, CpmlPrimitive *to)
{
    cairo_path_data_t *end1, *start2;
    CpmlPrimitive line1, line2;
    cairo_path_data_t data1[2], data2[2];
    CpmlPair joint;

    end1 = cpml_primitive_get_point(from, -1);
    start2 = cpml_primitive_get_point(to, 0);

    /* Check if the primitives are yet connected */
    if (end1->point.x == start2->point.x && end1->point.y == start2->point.y)
        return 1;

    line1.org = cpml_primitive_get_point(from, -2);
    line1.data = data1;
    data1[0].header.type = CAIRO_PATH_LINE_TO;
    data1[1] = *end1;

    line2.org = start2;
    line2.data = data2;
    data2[0].header.type = CAIRO_PATH_LINE_TO;
    data2[1] = *cpml_primitive_get_point(to, 1);

    if (!cpml_intersection(&line1, &line2, &joint))
        return 0;

    cpml_pair_to_cairo(&joint, end1);
    cpml_pair_to_cairo(&joint, start2);

    return 1;
}
