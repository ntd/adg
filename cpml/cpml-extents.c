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
 * SECTION:extents
 * @Section_Id:CpmlExtents
 * @title: CpmlExtents
 * @short_description: A rectangular area representing a bounding box
 *
 * The #CpmlExtents struct groups two pairs representing the rectangular
 * area of a bounding box. The <fieldname>p1</fieldname> field holds the
 * lowest (x, y) coordinates while <fieldname>p2</fieldname> holds
 * the highest ones. The additional <fieldname>is_defined</fieldname>
 * boolean flag can be checked to know if #CpmlExtents has been computed
 * (when equal to %1) or not (when <fieldname>is_defined</fieldname> is %0).
 **/

/**
 * CpmlExtents:
 * @is_defined: %1 if these extents should be considered, %0 otherwise
 * @p1: the lowest x,y coordinates
 * @p2: the highest x,y coordinates
 *
 * A structure defining a bounding box area.
 **/


#include "cpml-extents.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


/**
 * cpml_extents_copy:
 * @extents: the destination #CpmlExtents
 * @src: the source #CpmlExtents
 *
 * Copies @src in @extents.
 *
 * Return value: @extents
 **/
CpmlExtents *
cpml_extents_copy(CpmlExtents *extents, const CpmlExtents *src)
{
    return memcpy(extents, src, sizeof(CpmlExtents));
}

/**
 * cpml_extents_from_cairo_text:
 * @extents: the destination #CpmlExtents
 * @cairo_extents: the source cairo_text_extents_t struct
 *
 * Converts @cairo_extents in a #CpmlExtents format and stores the
 * result in @extents.
 *
 * Returns: @extents
 **/
CpmlExtents *
cpml_extents_from_cairo_text(CpmlExtents *extents,
                             const cairo_text_extents_t *cairo_extents)
{
    extents->is_defined = 1;
    extents->p1.x = cairo_extents->x_bearing;
    extents->p1.y = cairo_extents->y_bearing;
    extents->p2.x = cairo_extents->x_bearing + cairo_extents->width;
    extents->p1.y = cairo_extents->y_bearing + cairo_extents->height;

    return extents;
}

/**
 * cpml_extents_add:
 * @extents: the destination #CpmlExtents
 * @src: the extents to add
 *
 * Merges @extents and @src and store the result in @extents.
 **/
void
cpml_extents_add(CpmlExtents *extents, const CpmlExtents *src)
{
    if (src->p1.x < extents->p1.x)
        extents->p1.x = src->p1.x;

    if (src->p1.y < extents->p1.y)
        extents->p1.y = src->p1.y;

    if (src->p2.x > extents->p2.x)
        extents->p2.x = src->p2.x;

    if (src->p2.y > extents->p2.y)
        extents->p2.y = src->p2.y;
}
