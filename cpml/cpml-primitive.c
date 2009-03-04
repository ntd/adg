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
 * @data: a pointer to the path data, including the header
 *
 * As for #CpmlSegment, also the primitive is unobtrusive. This
 * means CpmlPrimitive does not include any coordinates but instead
 * keeps pointers to the original segment (and, by transition, to
 * the underlying #cairo_path_t struct).
 **/

#include "cpml-primitive.h"

#include <stdio.h>

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

    primitive->org = cpml_primitive_get_endpoint(primitive);
    primitive->data = new_data;
}

/**
 * cpml_primitive_get_startpoint:
 * @primitive: a #CpmlPrimitive
 *
 * Gets the start point of a primitive.
 *
 * Return value: a pointer to the start point (in cairo format)
 **/
cairo_path_data_t *
cpml_primitive_get_startpoint(CpmlPrimitive *primitive)
{
    return primitive->org;
}

/**
 * cpml_primitive_get_endpoint:
 * @primitive: a #CpmlPrimitive
 *
 * Gets the end point of a primitive.
 *
 * Return value: a pointer to the end point (in cairo format)
 **/
cairo_path_data_t *
cpml_primitive_get_endpoint(CpmlPrimitive *primitive)
{
    cairo_path_data_t *data = primitive->data;

    switch (data->header.type) {
    case CAIRO_PATH_LINE_TO:
        return &data[1];
        break;
    case CAIRO_PATH_CURVE_TO:
        return &data[3];
        break;
    case CAIRO_PATH_CLOSE_PATH:
        /* For close path, the end point is the start of the segment */
        return &primitive->source->data[1];
        break;
    }

    printf("Unhandled entity (%d)", data->header.type);
    return NULL;
}
