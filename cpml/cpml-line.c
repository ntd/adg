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
 * cpml_line_offset:
 * @line:   the #CpmlPrimitive line data
 * @offset: distance for the computed parallel line
 *
 * Given a line segment specified by the @line primitive data,
 * computes the parallel line distant @offset from the original one
 * and returns the result by changing @line.
 **/
void
cpml_line_offset(CpmlPrimitive *primitive, double offset)
{
    CpmlVector normal;

    normal.x = primitive->data[1].point.x - primitive->org->point.x;
    normal.y = primitive->data[1].point.y - primitive->org->point.y;

    cpml_vector_from_pair(&normal, &normal, offset);
    cpml_vector_normal(&normal);

    primitive->org->point.x += normal.x;
    primitive->org->point.y += normal.y;

    primitive->data[1].point.x += normal.x;
    primitive->data[1].point.y += normal.y;
}
