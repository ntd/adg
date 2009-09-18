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
