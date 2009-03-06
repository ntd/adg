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
 * The following functions manipulate %CAIRO_PATH_CLOSE_PATH #CpmlPrimitive.
 * No check is made on the primitive struct, so be sure the CpmlPrimitive
 * is effectively a close operation before calling these APIs.
 **/

#include "cpml-close.h"
#include "cpml-pair.h"

/**
 * cpml_close_get_npoints:
 *
 * Returns the number of points needed to properly specify a close primitive.
 *
 * Return value: 0
 **/
int
cpml_close_get_npoints(void)
{
    return 0;
}

/**
 * cpml_close_offset:
 * @close:   the #CpmlPrimitive close data
 * @offset: distance for the computed parallel close
 *
 * Given a close segment specified by the @close primitive data,
 * computes the parallel close distant @offset from the original one
 * and returns the result by changing @close.
 *
 * TODO: not yet implemented
 **/
void
cpml_close_offset(CpmlPrimitive *primitive, double offset)
{
}
