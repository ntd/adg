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
 * enum, excluding %CAIRO_PATH_MOVE_TO, as it is not considered a valid
 * primitive and it is managed in different way (the moveto primitives
 * are used to set the origin of the first primitive in a segment).
 **/

/**
 * CpmlPrimitive:
 * @segment: the source #CpmlSegment
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
#include <stdio.h>

static void     dump_cairo_point        (const cairo_path_data_t *path_data);


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
    primitive->segment = segment;

    /* The first element of a CpmlSegment is always a CAIRO_PATH_MOVE_TO,
     * as ensured by cpml_segment_from_cairo() and by the browsing APIs,
     * so the origin is in the second data item */
    primitive->org = &segment->data[1];

    /* Also, the segment APIs ensure that @segment is prepended by
     * only one CAIRO_PATH_MOVE_TO */
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
    primitive->org = &primitive->segment->data[1];
    primitive->data = primitive->segment->data + 2;
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
    if (new_data - primitive->segment->data >= primitive->segment->num_data)
        return 0;

    primitive->org = cpml_primitive_get_point(primitive, -1);
    primitive->data = new_data;

    return 1;
}

/**
 * cpml_primitive_get_npoints:
 * @primitive: a #CpmlPrimitive
 *
 * Gets the number of points required to identify @primitive.
 * It is similar to cpml_primitive_type_get_npoints() but using
 * a @primitive instance instead of a type.
 *
 * Return value: the number of points or -1 on errors
 **/
int
cpml_primitive_get_npoints(const CpmlPrimitive *primitive)
{
    return cpml_primitive_type_get_npoints(primitive->data->header.type);
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
 * If it is requested the end point on a primitive that owns a
 * single point, this function cycles the source #CpmlSegment and
 * returns the first point. This case must be handled this way
 * because requesting the end point of a %CAIRO_PATH_CLOSE_PATH
 * is a valid operation and must returns the start of the segment.
 *
 * Return value: a pointer to the requested point (in cairo format)
 *               or %NULL if the point is outside the valid range
 **/
cairo_path_data_t *
cpml_primitive_get_point(const CpmlPrimitive *primitive, int npoint)
{
    int npoints;
    
    /* For a start point request, simply return the origin */
    if (npoint == 0)
        return primitive->org;

    npoints = cpml_primitive_get_npoints(primitive);
    if (npoints < 0)
        return NULL;

    /* Out of range condition */
    if (npoint >= npoints)
        return NULL;

    /* The CAIRO_PATH_CLOSE_PATH special case: cycle the segment */
    if (npoints == 1 && npoint < 0)
        return &primitive->segment->data[1];

    /* Check for an end point request and modify npoint accordling */
    if (npoint < 0)
        npoint = npoints-1;

    return &primitive->data[npoint];
}

/**
 * cpml_primitive_to_cairo:
 * @primitive: a #CpmlPrimitive
 * @cr: the destination cairo context
 *
 * Appends the path of @primitive to @cr using cairo_append_path().
 * As a special case, if the primitive is a #CAIRO_PATH_CLOSE_PATH,
 * an equivalent line is rendered, because a close path left alone
 * is not renderable.
 **/
void
cpml_primitive_to_cairo(const CpmlPrimitive *primitive, cairo_t *cr)
{
    cairo_path_t path;
    cairo_path_data_t *path_data;

    cairo_move_to(cr, primitive->org->point.x, primitive->org->point.y);

    if (primitive->data->header.type == CAIRO_PATH_CLOSE_PATH) {
        path_data = cpml_primitive_get_point(primitive, -1);
        cairo_line_to(cr, path_data->point.x, path_data->point.y);
    } else {
        path.status = CAIRO_STATUS_SUCCESS;
        path.data = primitive->data;
        path.num_data = primitive->data->header.length;
        cairo_append_path(cr, &path);
    }
}

/**
 * cpml_primitive_dump:
 * @primitive: a #CpmlPrimitive
 * @org_also:  whether to output also the origin coordinates
 *
 * Dumps info on the specified @primitive to stdout: useful for
 * debugging purposes. If @org_also is 1, a %CAIRO_PATH_MOVE_TO
 * to the origin is prepended to the data otherwise the
 * <structfield>org</structfield> field is not used.
 **/
void
cpml_primitive_dump(const CpmlPrimitive *primitive, cairo_bool_t org_also)
{
    const cairo_path_data_t *data;
    int type, n, npoints;

    data = primitive->data;
    type = data->header.type;
    npoints = cpml_primitive_get_npoints(primitive);
    if (npoints < 0) {
        printf("Unhandled primitive type (%d)\n", type);
        return;
    }

    /* Dump the origin movement, if requested */
    if (org_also) {
        printf("Move to ");
        dump_cairo_point(primitive->org);
        printf("\n");
    }

    switch (type) {

    case CAIRO_PATH_LINE_TO:
        printf("Line to ");
        break;

    case CAIRO_PATH_CURVE_TO:
        printf("Curve to ");
        break;

    case CAIRO_PATH_CLOSE_PATH:
        printf("Path close");
        break;
    }

    for (n = 1; n < npoints; ++n)
        dump_cairo_point(cpml_primitive_get_point(primitive, n));

    printf("\n");
}


/**
 * cpml_primitive_type_get_npoints:
 * @type: a primitive type
 *
 * Gets the number of points required to identify the @type primitive.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 *
 * Return value: the number of points or -1 on errors
 **/
int
cpml_primitive_type_get_npoints(cairo_path_data_type_t type)
{
    switch (type) {

    case CAIRO_PATH_LINE_TO:
        return cpml_line_type_get_npoints();

    case CAIRO_PATH_CURVE_TO:
        return cpml_curve_type_get_npoints();

    case CAIRO_PATH_CLOSE_PATH:
        return cpml_close_type_get_npoints();
    }

    return -1;
}

/**
 * cpml_primitive_pair_at:
 * @primitive: a #CpmlPrimitive
 * @pair:      the destination #CpmlPair
 * @pos:       the position value
 *
 * Abstracts the pair_at() family functions by providing a common
 * way to access the underlying primitive-specific implementation.
 *
 * It gets the coordinates of the point lying on @primitive
 * at position @pos. @pos is an homogeneous factor where 0 is the
 * start point, 1 the end point, 0.5 the mid point and so on.
 * The relation 0 < @pos < 1 should be satisfied, although some
 * primitives accept value outside this range.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 **/
void
cpml_primitive_pair_at(const CpmlPrimitive *primitive,
                       CpmlPair *pair, double pos)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
        cpml_line_pair_at(primitive, pair, pos);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_pair_at(primitive, pair, pos);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_pair_at(primitive, pair, pos);
        break;
    }
}

/**
 * cpml_primitive_vector_at:
 * @primitive: a #CpmlPrimitive
 * @vector:    the destination #CpmlVector
 * @pos:       the position value
 *
 * Abstracts the vector_at() family functions by providing a common
 * way to access the underlying primitive-specific implementation.
 *
 * It gets the steepness of the point at position @pos on @primitive.
 * @pos is an homogeneous factor where 0 is the start point, 1 the
 * end point, 0.5 the mid point and so on.
 * The relation 0 < @pos < 1 should be satisfied, although some
 * primitives accept value outside this range.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 **/
void
cpml_primitive_vector_at(const CpmlPrimitive *primitive,
                         CpmlVector *vector, double pos)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
        cpml_line_vector_at(primitive, vector, pos);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_vector_at(primitive, vector, pos);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_vector_at(primitive, vector, pos);
        break;
    }
}

/**
 * cpml_primitive_offset:
 * @primitive: a #CpmlPrimitive
 * @offset: distance for the computed offset primitive
 *
 * Given a primitive, computes the same (or approximated) parallel
 * primitive distant @offset from the original one and returns
 * the result by changing @primitive.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 **/
void
cpml_primitive_offset(CpmlPrimitive *primitive, double offset)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
        cpml_line_offset(primitive, offset);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_offset(primitive, offset);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_offset(primitive, offset);
        break;
    }
}

static void
dump_cairo_point(const cairo_path_data_t *path_data)
{
    printf("(%g %g) ", path_data->point.x, path_data->point.y);
}
