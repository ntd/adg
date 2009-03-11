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
    cairo_path_data_t *endcairopoint, *startcairopoint;
    CpmlPair endpoint, startpoint;
    CpmlVector endvector, startvector;
    CpmlPair joint;

    endcairopoint = cpml_primitive_get_point(from, -1);
    startcairopoint = cpml_primitive_get_point(to, 0);
    cpml_pair_from_cairo(&endpoint, endcairopoint);
    cpml_pair_from_cairo(&startpoint, startcairopoint);
    cpml_primitive_vector_at(from, &endvector, 1.0);
    cpml_primitive_vector_at(to, &startvector, 0.);

    if (!cpml_pair_intersection_pv_pv(&joint, &endpoint, &endvector,
                                      &startpoint, &startvector))
        return 0;

    cpml_pair_to_cairo(&joint, endcairopoint);
    cpml_pair_to_cairo(&joint, startcairopoint);

    return 1;
}
