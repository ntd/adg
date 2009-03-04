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
 * SECTION:primitive
 * @title: CpmlPrimitive
 * @short_description: Basic component of segments
 *
 * A primitive is an atomic geometric element found inside #CpmlSegment.
 * The available primitives are defined in the #cairo_path_data_type_t
 * enum, excluding #CAIRO_PATH_MOVE_TO, as it is not considered a valid
 * primitive and it is managed in different way (the moveto primitives
 * are used to set the origin of the first primitive in a segment).
 **/

/**
 * CpmlPrimitive:
 * @source: the source #CpmlSegment
 * @org: a pointer to the first point of the primitive
 * @data: the array of the path data, prepended by the header
 *
 * As for #CpmlSegment, also the primitive is unobtrusive. This
 * means CpmlPrimitive does not include any coordinates but instead
 * keeps pointers to the original segment (and, by transition, to
 * the underlying #cairo_path_t struct).
 **/

#include "cpml-primitive.h"

#include <stdlib.h>

/**
 * cpml_primitive_from_segment:
 * @primitive: the destination #CpmlPrimitive struct
 * @segment: the source segment
 *
 * Initializes @primitive to the first primitive of @segment.
 **/
void
cpml_primitive_from_segment(CpmlPrimitive *primitive, CpmlSegment *segment)
{
    primitive->source = segment;

    /* The first element of a CpmlSegment is always a CAIRO_MOVE_TO,
     * as ensured by cpml_segment_from_cairo() and by the browsing APIs,
     * so the origin is in the second data item */
    primitive->org = &segment->data[1];

    /* Also, the segment APIs ensure that @segment is prepended by
     * only one CAIRO_MOVE_TO */
    primitive->data = segment->data + 2;
}

/**
 * cpml_primitive_reset:
 * @primitive: a #CpmlPrimitive
 *
 * Resets @primitive so it refers to the first primitive of the
 * source segment.
 **/
void
cpml_primitive_reset(CpmlPrimitive *primitive)
{
    primitive->org = &primitive->source->data[1];
    primitive->data = primitive->source->data + 2;
}

/**
 * cpml_primitive_next:
 * @primitive: a #CpmlPrimitive
 *
 *
 * Changes @primitive so it refers to the next primitive on the
 * source segment. If there are no more primitives, @primitive is
 * not changed and 0 is returned.
 *
 * Return value: 1 on success, 0 if no next primitive found or errors
 **/
cairo_bool_t
cpml_primitive_next(CpmlPrimitive *primitive)
{
    cairo_path_data_t *new_data;

    new_data = primitive->data + primitive->data->header.length;

    if (new_data - primitive->source->data >= primitive->source->data->header.length) {
        return 0;
    }

    primitive->org = cpml_primitive_get_point(primitive, -1);
    primitive->data = new_data;
}

/**
 * cpml_primitive_get_npoints:
 * @primitive: a #CpmlPrimitive
 *
 * Gets the number of points required to identify the underlying primitive.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 *
 * Return value: the number of points or -1 on errors
 **/
int
cpml_primitive_get_npoints(CpmlPrimitive *primitive)
{
    switch (primitive->data->header.type) {
    case CAIRO_PATH_LINE_TO:
        return 1;
    case CAIRO_PATH_CURVE_TO:
        return 2;
    case CAIRO_PATH_CLOSE_PATH:
        return 0;
    }

    return -1;
}

/**
 * cpml_primitive_get_point:
 * @primitive: a #CpmlPrimitive
 * @npoint: the index of the point to retrieve
 *
 * Gets the specified @npoint from @primitive. The index starts
 * at 0: if @npoint is 0, the start point (the origin) is
 * returned, 1 for the second point and so on. As a special case,
 * @npoint less than 0 means the end point.
 *
 * If it is requested the end point on a primitive that does not
 * owns any point, this function cycles the source #CpmlSegment and
 * returns the first point. This case must be handled this way
 * because requesting the end point of a %CAIRO_PATH_CLOSE_PATH
 * (that does not have any point) is a valid operation.
 *
 * Return value: a pointer to the requested point (in cairo format)
 *               or %NULL if the point is outside the valid range
 **/
cairo_path_data_t *
cpml_primitive_get_point(CpmlPrimitive *primitive, int npoint)
{
    int npoints;
    
    /* For a start point request, simply return the origin */
    if (npoint == 0)
        return primitive->org;

    npoints = cpml_primitive_get_npoints(primitive);
    if (npoints < 0)
        return NULL;

    /* Check for an end point request and modify npoint accordling */
    if (npoint < 0)
        npoint = npoints;

    /* The CAIRO_PATH_CLOSE_PATH special case: cycle the segment */
    if (npoint == 0)
        return &primitive->source->data[1];

    if (npoint > npoints)
        return NULL;

    return &primitive->data[npoint];
}
