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
 * SECTION:cpml-primitive
 * @Section_Id:CpmlPrimitive
 * @title: CpmlPrimitive
 * @short_description: Basic component of segments
 *
 * A primitive is an atomic geometric element found inside #CpmlSegment.
 * The available primitives are the same defined by #cairo_path_data_type_t
 * with the additional %CAIRO_PATH_ARC_TO type (check #CpmlPrimitiveType
 * for further information) and without %CAIRO_PATH_MOVE_TO as it is not
 * considered a primitive and it is managed in different way: the move-to
 * primitives are only used to define the origin of a segment.
 **/

/**
 * CpmlPrimitiveType:
 *
 * This is another name for #cairo_path_data_type_t type. Although
 * phisically they are the same struct, #CpmlPrimitiveType conceptually
 * embodies an important difference: it can be used to specify the
 * special %CAIRO_PATH_ARC_TO primitive. This is not a native cairo
 * primitive and having two different types is a good way to make clear
 * when a function expect or not embedded arc-to primitives.
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
 * the underlying #CpmlPath struct).
 **/


#include "cpml-primitive.h"
#include "cpml-line.h"
#include "cpml-arc.h"
#include "cpml-curve.h"
#include "cpml-close.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static void     dump_cairo_point        (const cairo_path_data_t *path_data);


/**
 * cpml_primitive_copy:
 * @primitive: the destination #CpmlPrimitive
 * @src: the source #CpmlPrimitive
 *
 * Copies @src in @primitive. This is a shallow copy: the internal fields
 * of @primitive refer to the same memory as the original @src primitive.
 *
 * Returns: @primitive
 **/
CpmlPrimitive *
cpml_primitive_copy(CpmlPrimitive *primitive, const CpmlPrimitive *src)
{
    return memcpy(primitive, src, sizeof(CpmlPrimitive));
}

/**
 * cpml_primitive_from_segment:
 * @primitive: the destination #CpmlPrimitive struct
 * @segment: the source segment
 *
 * Initializes @primitive to the first primitive of @segment.
 *
 * Returns: @primitive
 **/
CpmlPrimitive *
cpml_primitive_from_segment(CpmlPrimitive *primitive, CpmlSegment *segment)
{
    primitive->segment = segment;

    /* The first element of a CpmlSegment is always a CAIRO_PATH_MOVE_TO,
     * as ensured by cpml_segment_from_cairo() and by the browsing APIs,
     * so the origin is in the second data item */
    primitive->org = &segment->data[1];

    /* Also, the segment APIs ensure that @segment is prepended by
     * only one CAIRO_PATH_MOVE_TO */
    primitive->data = segment->data + segment->data[0].header.length;

    return primitive;
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
    cpml_primitive_from_segment(primitive, primitive->segment);
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
 * Returns: 1 on success, 0 if no next primitive found or errors
 **/
cairo_bool_t
cpml_primitive_next(CpmlPrimitive *primitive)
{
    cairo_path_data_t *new_data;

    new_data = primitive->data + primitive->data->header.length;
    if (new_data - primitive->segment->data >= primitive->segment->num_data)
        return 0;

    /* TODO: this is a temporary workaround to be removed as soon as
     * the issue #21 will be resolved */
    if (new_data->header.type == CAIRO_PATH_MOVE_TO)
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
 * Returns: the number of points or -1 on errors
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
 * returned, 1 for the second point and so on. If @npoint is
 * negative, it is considered as a negative index from the end,
 * so that -1 is the end point, -2 the point before the end point
 * and so on.
 *
 * %CAIRO_PATH_CLOSE_PATH is managed in a special way: if @npoint
 * is -1 or 1 and @primitive is a close-path, this function cycles
 * the source #CpmlSegment and returns the first point. This is
 * needed because requesting the end point (or the second point)
 * of a close path is a valid operation and must returns the start
 * of the segment.
 *
 * Returns: a pointer to the requested point (in cairo format)
 *          or %NULL if the point is outside the valid range
 **/
cairo_path_data_t *
cpml_primitive_get_point(const CpmlPrimitive *primitive, int npoint)
{
    int npoints;

    /* For a start point request, simply return the origin
     * without further checking */
    if (npoint == 0)
        return primitive->org;

    /* The CAIRO_PATH_CLOSE_PATH special case */
    if (primitive->data->header.type == CAIRO_PATH_CLOSE_PATH &&
        (npoint == 1 || npoint == -1))
        return &primitive->segment->data[1];

    npoints = cpml_primitive_get_npoints(primitive);
    if (npoints < 0)
        return NULL;

    /* If npoint is negative, consider it as a negative index from the end */
    if (npoint < 0)
        npoint = npoints + npoint;

    /* Out of range condition */
    if (npoint < 0 || npoint >= npoints)
        return NULL;

    return npoint == 0 ? primitive->org : &primitive->data[npoint];
}

/**
 * cpml_primitive_to_cairo:
 * @primitive: a #CpmlPrimitive
 * @cr: the destination cairo context
 *
 * Renders a single @primitive to the @cr cairo context.
 * As a special case, if the primitive is a #CAIRO_PATH_CLOSE_PATH,
 * an equivalent line is rendered, because a close path left alone
 * is not renderable.
 *
 * Also a #CAIRO_PATH_ARC_TO primitive is treated specially, as it
 * is not natively supported by cairo and has its own rendering API.
 **/
void
cpml_primitive_to_cairo(const CpmlPrimitive *primitive, cairo_t *cr)
{
    cairo_path_t path;
    cairo_path_data_t *path_data;

    cairo_move_to(cr, primitive->org->point.x, primitive->org->point.y);

    switch (primitive->data->header.type) {

    case CAIRO_PATH_CLOSE_PATH:
        path_data = cpml_primitive_get_point(primitive, -1);
        cairo_line_to(cr, path_data->point.x, path_data->point.y);
        break;

    case CAIRO_PATH_ARC_TO:
        cpml_arc_to_cairo(primitive, cr);
        break;

    default:
        path.status = CAIRO_STATUS_SUCCESS;
        path.data = primitive->data;
        path.num_data = primitive->data->header.length;
        cairo_append_path(cr, &path);
        break;
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

    case CAIRO_PATH_ARC_TO:
        printf("Arc to ");
        break;

    case CAIRO_PATH_CURVE_TO:
        printf("Curve to ");
        break;

    case CAIRO_PATH_CLOSE_PATH:
        printf("Path close");
        break;

    default:
        printf("Unknown primitive (type = %d)", type);
        break;
    }

    for (n = 1; n < npoints; ++n)
        dump_cairo_point(cpml_primitive_get_point(primitive, n));

    printf("\n");
}

/**
 * cpml_primitive_intersection_with_segment:
 * @primitive: a #CpmlPrimitive
 * @segment:   a #CpmlSegment
 * @dest:      the destination vector of #CpmlPair
 * @max:       maximum number of intersections to return
 *
 * Computes the intersections between @segment and @primitive by
 * sequentially scanning the primitives in @segment and looking
 * for intersections with @primitive.
 * If the intersections are more than @max, only the first @max pairs
 * are stored in @dest.
 *
 * Returns: the number of intersections found
 **/
int
cpml_primitive_intersection_with_segment(const CpmlPrimitive *primitive,
                                         const CpmlSegment *segment,
                                         CpmlPair *dest, int max)
{
    CpmlPrimitive portion;
    int found;

    cpml_primitive_from_segment(&portion, (CpmlSegment *) segment);
    found = 0;

    while (found < max) {
        found += cpml_primitive_intersection(&portion, primitive,
                                             dest+found, max-found);
        if (!cpml_primitive_next(&portion))
            break;
    }

    return found;
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
 * Returns: the number of points or -1 on errors
 **/
int
cpml_primitive_type_get_npoints(CpmlPrimitiveType type)
{
    switch (type) {

    case CAIRO_PATH_LINE_TO:
        return cpml_line_type_get_npoints();

    case CAIRO_PATH_ARC_TO:
        return cpml_arc_type_get_npoints();

    case CAIRO_PATH_CURVE_TO:
        return cpml_curve_type_get_npoints();

    case CAIRO_PATH_CLOSE_PATH:
        return cpml_close_type_get_npoints();

    default:
        break;
    }

    return -1;
}

/**
 * cpml_primitive_length:
 * @primitive: a #CpmlPrimitive
 *
 * Abstracts the length() family functions by providing a common
 * way to access the underlying primitive-specific implementation.
 * The function returns the length of @primitive.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 *
 * Returns: the requested length or 0 on errors
 **/
double
cpml_primitive_length(const CpmlPrimitive *primitive)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
    case CAIRO_PATH_CLOSE_PATH:
        return cpml_line_length(primitive);

    case CAIRO_PATH_ARC_TO:
        return cpml_arc_length(primitive);

    case CAIRO_PATH_CURVE_TO:
        return cpml_curve_length(primitive);

    default:
        break;
    }

    return 0.;
}

/**
 * cpml_primitive_extents:
 * @primitive: a #CpmlPrimitive
 * @extents: where to store the extents
 *
 * Abstracts the extents() family functions by providing a common
 * way to access the underlying primitive-specific implementation.
 * The function stores in @extents the bounding box of @primitive.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 **/
void
cpml_primitive_extents(const CpmlPrimitive *primitive, CpmlExtents *extents)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
    case CAIRO_PATH_CLOSE_PATH:
        return cpml_line_extents(primitive, extents);

    case CAIRO_PATH_ARC_TO:
        return cpml_arc_extents(primitive, extents);

    case CAIRO_PATH_CURVE_TO:
        return cpml_curve_extents(primitive, extents);

    default:
        extents->is_defined = 0;
        break;
    }
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

    case CAIRO_PATH_ARC_TO:
        cpml_arc_pair_at(primitive, pair, pos);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_pair_at(primitive, pair, pos);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_pair_at(primitive, pair, pos);
        break;

    default:
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

    case CAIRO_PATH_ARC_TO:
        cpml_arc_vector_at(primitive, vector, pos);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_vector_at(primitive, vector, pos);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_vector_at(primitive, vector, pos);
        break;

    default:
        break;
    }
}

/**
 * cpml_primitive_near_pos:
 * @primitive: a #CpmlPrimitive
 * @pair:      the coordinates of the subject point
 *
 * Returns the pos value of the point on @primitive nearest to @pair.
 * The returned value is always between 0 and 1 or -1 in case of errors.
 *
 * <note><para>
 * This function is primitive dependent, that is every primitive has
 * its own implementation.
 * </para></note>
 *
 * Returns: the requested pos value between 0 and 1 or -1 on errors
 **/
double
cpml_primitive_near_pos(const CpmlPrimitive *primitive, const CpmlPair *pair)
{
    switch (primitive->data->header.type) {

    case CAIRO_PATH_LINE_TO:
        return cpml_line_near_pos(primitive, pair);

    case CAIRO_PATH_ARC_TO:
        return cpml_arc_near_pos(primitive, pair);

    case CAIRO_PATH_CURVE_TO:
        return cpml_curve_near_pos(primitive, pair);

    case CAIRO_PATH_CLOSE_PATH:
        return cpml_close_near_pos(primitive, pair);

    default:
        break;
    }

    return -1.;
}

/**
 * cpml_primitive_join:
 * @primitive: the first #CpmlPrimitive
 * @primitive2: the second #CpmlPrimitive
 *
 * Joins two primitive modifying the end point of @primitive and the
 * start point of @primitive2 so that the resulting points will overlap.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>Actually, the join is done by extending the end vector
 *           of @primitive and the start vector of @primitive2 and
 *           interpolating the intersection: this means no primitive
 *           dependent code is needed. Anyway, it is likely to change
 *           in the future because this approach is quite naive when
 *           curves are involved.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: 1 on success, 0 if the end vector of @primitive
 *          and the start vector of @primitive2 are parallel
 **/
cairo_bool_t
cpml_primitive_join(CpmlPrimitive *primitive, CpmlPrimitive *primitive2)
{
    cairo_path_data_t *end1, *start2;
    CpmlPrimitive line1, line2;
    cairo_path_data_t data1[2], data2[2];
    CpmlPair joint;

    end1 = cpml_primitive_get_point(primitive, -1);
    start2 = cpml_primitive_get_point(primitive2, 0);

    /* Check if the primitives are yet connected */
    if (end1->point.x == start2->point.x && end1->point.y == start2->point.y)
        return 1;

    line1.org = cpml_primitive_get_point(primitive, -2);
    line1.data = data1;
    data1[0].header.type = CAIRO_PATH_LINE_TO;
    data1[1] = *end1;

    line2.org = start2;
    line2.data = data2;
    data2[0].header.type = CAIRO_PATH_LINE_TO;
    data2[1] = *cpml_primitive_get_point(primitive2, 1);

    if (!cpml_line_intersection(&line1, &line2, &joint, 1))
        return 0;

    cpml_pair_to_cairo(&joint, end1);
    cpml_pair_to_cairo(&joint, start2);

    return 1;
}

/**
 * cpml_primitive_intersection:
 * @primitive:  the first #CpmlPrimitive
 * @primitive2: the second #CpmlPrimitive
 * @dest:       the destination #CpmlPair (or a vector of #CpmlPair)
 * @max:        maximum number of intersections to return
 *
 * Finds the intersection points between the given primitives and
 * returns the result in @dest. The size of @dest should be enough
 * to store @max #CpmlPair. The absoulte max number of intersections
 * is dependent from the type of the primitive involved in the
 * operation. If there are at least one Bézier curve involved, up to
 * 4 intersections could be returned. Otherwise, if there is an arc
 * the intersections will be 2 at maximum. For line line primitives,
 * there is only 1 point (or obviously 0 if the lines do not intersect).
 *
 * <note><para>
 * This function is primitive dependent: every new primitive must
 * expose API to get intersections with any other primitive type
 * (excluding %CAIRO_PATH_CLOSE_PATH, as it is converted to a line
 * primitive).</para>
 * <para>The convention used by CPML is that a primitive should
 * expose only intersection APIs dealing with lower complexity
 * primitives. This is required to avoid double functions:
 * you will have only a cpml_curve_intersection_with_line() function,
 * not a cpml_line_intersection_with_curve(), as the latter is
 * easily reproduced by calling the former with @primitive2
 * and @primitive swapped.
 * </para></note>
 *
 * Returns: the number of intersection points found or 0 if the
 *          primitives do not intersect
 **/
int
cpml_primitive_intersection(const CpmlPrimitive *primitive,
                            const CpmlPrimitive *primitive2,
                            CpmlPair *dest, int max)
{
    CpmlPrimitiveType type1, type2;

    type1 = primitive->data->header.type;
    type2 = primitive->data->header.type;

    /* Close path primitives are treated as line-to */
    if (type1 == CAIRO_PATH_CLOSE_PATH)
        type1 = CAIRO_PATH_LINE_TO;
    if (type2 == CAIRO_PATH_CLOSE_PATH)
        type2 = CAIRO_PATH_LINE_TO;

    /* Order the two primitives in ascending complexity, to facilitate
     * the dispatcher logic */
    if (cpml_primitive_type_get_npoints(type1) > cpml_primitive_type_get_npoints(type2)) {
        const CpmlPrimitive *tmp_primitive;
        CpmlPrimitiveType tmp_type;

        tmp_type = type1;
        tmp_primitive = primitive;

        type1 = type2;
        primitive = primitive2;

        type2 = tmp_type;
        primitive2 = tmp_primitive;
    }

    /* Dispatcher: probably there's a smarter way to do this */
    switch (type1) {

    case CAIRO_PATH_LINE_TO:
        if (type2 == CAIRO_PATH_LINE_TO)
            return cpml_line_intersection(primitive2, primitive,
                                          dest, max);
        else if (type2 == CAIRO_PATH_ARC_TO)
            return cpml_arc_intersection_with_line(primitive2, primitive,
                                                   dest, max);
        else if (type2 == CAIRO_PATH_CURVE_TO)
            return cpml_curve_intersection_with_line(primitive2, primitive,
                                                     dest, max);
        break;

    case CAIRO_PATH_ARC_TO:
        if (type2 == CAIRO_PATH_ARC_TO)
            return cpml_arc_intersection(primitive2, primitive,
                                         dest, max);
        else if (type2 == CAIRO_PATH_CURVE_TO)
            return cpml_curve_intersection_with_arc(primitive2, primitive,
                                                    dest, max);
        break;

    case CAIRO_PATH_CURVE_TO:
        if (type2 == CAIRO_PATH_CURVE_TO)
            return cpml_curve_intersection(primitive2, primitive,
                                           dest, max);
        break;

    default:
        break;
    }

    /* Primitive combination not found */
    return 0;
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

    case CAIRO_PATH_ARC_TO:
        cpml_arc_offset(primitive, offset);
        break;

    case CAIRO_PATH_CURVE_TO:
        cpml_curve_offset(primitive, offset);
        break;

    case CAIRO_PATH_CLOSE_PATH:
        cpml_close_offset(primitive, offset);
        break;

    default:
        break;
    }
}

static void
dump_cairo_point(const cairo_path_data_t *path_data)
{
    printf("(%g %g) ", path_data->point.x, path_data->point.y);
}
