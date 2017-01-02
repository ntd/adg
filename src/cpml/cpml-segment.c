/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-segment
 * @Section_Id:Segment
 * @title: CpmlSegment
 * @short_description: Contiguous segment that can be a fragment
 *                     or a whole cairo path
 *
 * A segment is a single contiguous line got from a cairo path. The
 * CPML library relies on one assumption to let the data be independent
 * from the current point (and thus from the cairo context): any segment
 * MUST be preceded by at least one %CPML_MOVE primitive.
 * This means a valid segment in cairo could be rejected by CPML.
 *
 * #CpmlSegment provides an unobtrusive way to access a cairo path.
 * This means #CpmlSegment itsself does not hold any coordinates but
 * instead a bunch of pointers to the original #cairo_path_t struct:
 * modifying data throught this struct also changes the original path.
 *
 * Every #cairo_path_t struct can contain more than one segment: the CPML
 * library provides iteration APIs to browse the segments of a path.
 * Use cpml_segment_reset() to reset the iterator at the start of the
 * cairo path (will point the first segment) and cpml_segment_next()
 * to get the next segment. Getting the previous segment is not provided
 * as the underlying cairo struct is not accessible in reverse order.
 *
 * When initialized, #CpmlSegment yet refers to the first segment so
 * the initial reset is not required.
 *
 * Since: 1.0
 **/

/**
 * CpmlSegment:
 * @path:     the source #cairo_path_t struct
 * @data:     the data points of the segment; the first primitive
 *            will always be a %CPML_MOVE
 * @num_data: size of @data
 *
 * This is an unobtrusive struct to identify a segment inside a
 * cairo path. Unobtrusive means that the real coordinates are
 * still stored in @path: CpmlSegment only provides a way to
 * access them.
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-curve.h"
#include <string.h>

static int              normalize               (CpmlSegment       *segment);
static int              ensure_one_leading_move (CpmlSegment       *segment);
static int              reshape                 (CpmlSegment       *segment);


/**
 * cpml_segment_from_cairo:
 * @segment: a #CpmlSegment
 * @path: (type gpointer): the source #cairo_path_t
 *
 * Builds a CpmlSegment from a #cairo_path_t structure. This operation
 * involves stripping duplicate %CPML_MOVE primitives at the beginning
 * of @path and including all the primitives up to the end of the
 * contiguous line, that is before a %CPML_MOVE, when the original path
 * data finish or up to a %CPML_CLOSE. This is done unobtrusively by
 * setting the @segment fields appropriately, i.e. @path is not touched.
 *
 * The first primitive must be a %CPML_MOVE, so no dependency on the
 * cairo context (to know the current position) is needed.
 *
 * This function will fail if @path is empty or if its
 * <structfield>status</structfield> member is not %CAIRO_STATUS_SUCCESS.
 *
 * Returns: (type gboolean): 1 if @segment has been succesfully computed, 0 on errors.
 *
 * Since: 1.0
 **/
int
cpml_segment_from_cairo(CpmlSegment *segment, cairo_path_t *path)
{
    /* The cairo path should be in a good state */
    if (path->num_data == 0 || path->status != CAIRO_STATUS_SUCCESS)
        return 0;

    segment->path = path;
    segment->data = path->data;
    segment->num_data = path->num_data;

    return normalize(segment);
}

/**
 * cpml_segment_copy:
 * @segment: a #CpmlSegment
 * @src: the source segment to copy
 *
 * Makes a shallow copy of @src into @segment.
 *
 * Since: 1.0
 **/
void
cpml_segment_copy(CpmlSegment *segment, const CpmlSegment *src)
{
    memcpy(segment, src, sizeof(CpmlSegment));
}

/**
 * cpml_segment_copy_data:
 * @segment: a #CpmlSegment structure
 * @src: the source segment to copy
 *
 * Copies the memory referenced by the <structfield>data</structfield> field
 * from @src to @segment. For a shallow copy, check out cpml_segment_copy().
 *
 * This could seem a somewhat unusual operation because @segment should
 * be compatible with @src, i.e. it is expected that they have the same
 * <structfield>num_data</structfield> value. Anyway it is convenient
 * in some situation, such as when restoring the original data from a
 * backup segment, e.g.:
 *
 * <informalexample><programlisting language="C">
 * CpmlSegment *backup;
 *
 * backup = cpml_segment_deep_dup(&segment);
 * // Now &segment points can be freely modified
 * ...
 * // Let's restore &segment original points
 * cpml_segment_copy_data(&segment, backup);
 * g_free(backup);
 * </programlisting></informalexample>
 *
 * Returns: (type gboolean): 1 if the data has been succesfully copied, 0 on errors.
 *
 * Since: 1.0
 **/
int
cpml_segment_copy_data(CpmlSegment *segment, const CpmlSegment *src)
{
    if (segment->num_data != src->num_data)
        return 0;

    if (src->num_data > 0) {
        size_t n = sizeof(cairo_path_data_t) * segment->num_data;
        memcpy(segment->data, src->data, n);
    }

    return 1;
}

/**
 * cpml_segment_reset:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the first segment of the source cairo path.
 *
 * Since: 1.0
 **/
void
cpml_segment_reset(CpmlSegment *segment)
{
    segment->data = segment->path->data;
    segment->num_data = segment->path->num_data;
    normalize(segment);
}

/**
 * cpml_segment_next:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the next segment of the source cairo path.
 *
 * Returns: (type gboolean): 1 on success, 0 if no next segment found or errors.
 *
 * Since: 1.0
 **/
int
cpml_segment_next(CpmlSegment *segment)
{
    cairo_path_data_t *new_data;
    const cairo_path_data_t *end_data;

    new_data = segment->data + segment->num_data;
    end_data = segment->path->data + segment->path->num_data;

    if (new_data >= end_data)
        return 0;

    segment->data = new_data;
    segment->num_data = end_data - new_data;

    return normalize(segment);
}

/**
 * cpml_segment_get_length:
 * @segment: a #CpmlSegment
 *
 * Gets the whole length of @segment.
 *
 * Returns: the requested length
 *
 * Since: 1.0
 **/
double
cpml_segment_get_length(const CpmlSegment *segment)
{
    CpmlPrimitive primitive;
    double length;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);
    length = 0;

    do {
        length += cpml_primitive_get_length(&primitive);
    } while (cpml_primitive_next(&primitive));

    return length;
}

/**
 * cpml_segment_put_extents:
 * @segment: a #CpmlSegment
 * @extents: where to store the extents
 *
 * Gets the whole extents of @segment.
 *
 * Since: 1.0
 **/
void
cpml_segment_put_extents(const CpmlSegment *segment, CpmlExtents *extents)
{
    CpmlPrimitive primitive;
    CpmlExtents primitive_extents;

    extents->is_defined = 0;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    do {
        cpml_primitive_put_extents(&primitive, &primitive_extents);
        cpml_extents_add(extents, &primitive_extents);
    } while (cpml_primitive_next(&primitive));
}

/**
 * cpml_segment_put_pair_at:
 * @segment: a #CpmlSegment
 * @pos:     the position value
 * @pair:    the destination #CpmlPair
 *
 * Gets the coordinates of the point lying on @segment at position
 * @pos. @pos is an homogeneous factor where 0 is the start point,
 * 1 the end point, 0.5 the mid point and so on.
 * The relation <constant>0 < @pos < 1</constant> should be satisfied,
 * although some cases accept value outside this range.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>The actual implementation returns only the start and end points,
 *           that is only when @pos is 0 or 1.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Since: 1.0
 **/
void
cpml_segment_put_pair_at(const CpmlSegment *segment, double pos,
                         CpmlPair *pair)
{
    CpmlPrimitive primitive;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    /* Handle the common cases: start and end points */
    if (pos == 0) {
        cpml_primitive_put_pair_at(&primitive, 0, pair);
    } if (pos == 1) {
        while (cpml_primitive_next(&primitive))
            ;
        cpml_primitive_put_pair_at(&primitive, 1, pair);
    }
}

/**
 * cpml_segment_put_vector_at:
 * @segment: a #CpmlSegment
 * @pos:     the position value
 * @vector:  the destination #CpmlVector
 *
 * Gets the steepness of the point lying on @segment at position
 * @pos. @pos is an homogeneous factor where 0 is the start point,
 * 1 the end point, 0.5 the mid point and so on.
 * The relation <constant>0 < @pos < 1</constant> should be satisfied,
 * although some cases accept value outside this range.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>The actual implementation returns only the start and end
 *           steepness, that is only when @pos is 0 or 1.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Since: 1.0
 **/
void
cpml_segment_put_vector_at(const CpmlSegment *segment, double pos,
                           CpmlVector *vector)
{
    CpmlPrimitive primitive;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    /* Handle the common cases: start and end points */
    if (pos == 0) {
        cpml_primitive_put_vector_at(&primitive, 0, vector);
        return;
    }

    if (pos == 1) {
        while (cpml_primitive_next(&primitive))
            ;
        cpml_primitive_put_vector_at(&primitive, 1, vector);
        return;
    }
}

/**
 * cpml_segment_put_intersections:
 * @segment:  the first #CpmlSegment
 * @segment2: the second #CpmlSegment
 * @n_dest:   maximum number of intersections to return
 * @dest:     the destination vector of #CpmlPair
 *
 * Computes the intersections between @segment and @segment2 and
 * returns the found points in @dest. If the intersections are more
 * than @n_dest, only the first @n_dest pairs are stored in @dest.
 *
 * To get the job done, the primitives of @segment are sequentially
 * scanned for intersections with any primitive in @segment2. This
 * means @segment has a higher precedence over @segment2.
 *
 * Returns: the number of intersections found
 *
 * Since: 1.0
 **/
size_t
cpml_segment_put_intersections(const CpmlSegment *segment,
                               const CpmlSegment *segment2,
                               size_t n_dest, CpmlPair *dest)
{
    CpmlPrimitive portion;
    size_t partial, total;

    cpml_primitive_from_segment(&portion, (CpmlSegment *) segment);
    total = 0;

    do {
        partial = cpml_primitive_put_intersections_with_segment(&portion,
                                                                segment2,
                                                                n_dest - total,
                                                                dest + total);
        total += partial;
    } while (total < n_dest && cpml_primitive_next(&portion));

    return total;
}

/**
 * cpml_segment_offset:
 * @segment: a #CpmlSegment
 * @offset: the offset distance
 *
 * Offsets a segment of the specified amount, that is builds a "parallel"
 * segment at the @offset distance from the original one and returns the
 * result by replacing the original @segment.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>Closed path are not yet managed: an elegant solution is not
 *           so obvious: use <function>cpml_close_offset</function> when
 *           will be available.</listitem>
 * <listitem>Degenerated primitives, such as lines of length 0, are not
 *           managed properly.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Since: 1.0
 **/
void
cpml_segment_offset(CpmlSegment *segment, double offset)
{
    CpmlPrimitive primitive;
    CpmlPrimitive last_primitive;
    CpmlPair old_end;
    cairo_path_data_t org, *old_org;
    int first_cycle;

    cpml_primitive_from_segment(&primitive, segment);
    first_cycle = 1;

    do {
        if (! first_cycle) {
            cpml_pair_to_cairo(&old_end, &org);
            old_org = primitive.org;
            primitive.org = &org;
        }

        cpml_primitive_put_point(&primitive, -1, &old_end);
        cpml_primitive_offset(&primitive, offset);

        if (! first_cycle) {
            cpml_primitive_join(&last_primitive, &primitive);
            primitive.org = old_org;
        }

        cpml_primitive_copy(&last_primitive, &primitive);
        first_cycle = 0;
    } while (cpml_primitive_next(&primitive));
}

/**
 * cpml_segment_transform:
 * @segment: a #CpmlSegment
 * @matrix: the matrix to be applied
 *
 * Applies @matrix on all the points of @segment.
 *
 * Since: 1.0
 **/
void
cpml_segment_transform(CpmlSegment *segment, const cairo_matrix_t *matrix)
{
    CpmlPrimitive primitive;
    cairo_path_data_t *data;
    size_t n_points;

    cpml_primitive_from_segment(&primitive, segment);
    cairo_matrix_transform_point(matrix, &(primitive.org)->point.x,
                                 &(primitive.org)->point.y);

    do {
        data = primitive.data;
        if (data->header.type != CPML_CLOSE) {
            n_points = cpml_primitive_get_n_points(&primitive);

            while (--n_points > 0) {
                ++data;
                cairo_matrix_transform_point(matrix,
                                             &data->point.x, &data->point.y);
            }
        }
    } while (cpml_primitive_next(&primitive));
}

/**
 * cpml_segment_reverse:
 * @segment: a #CpmlSegment
 *
 * Reverses @segment in-place. The resulting rendering will be the same,
 * but with the primitives generated in reverse order.
 *
 * It is assumed that @segment has already been sanitized, e.g. when it
 * is returned by some CPML API or it is a cairo path already conforming
 * to the segment rules described in cpml_segment_from_cairo().
 *
 * Since: 1.0
 **/
void
cpml_segment_reverse(CpmlSegment *segment)
{
    cairo_path_data_t *data, *dst_data;
    size_t data_size;
    double end_x, end_y;
    int n, length;
    size_t n_points, n_point;
    const cairo_path_data_t *src_data;

    data_size = sizeof(cairo_path_data_t) * segment->num_data;
    data = malloc(data_size);
    src_data = segment->data;
    dst_data = data + segment->num_data;
    end_x = src_data[1].point.x;
    end_y = src_data[1].point.y;

    n = src_data->header.length;
    data->header.type = CPML_MOVE;
    data->header.length = n;

    while (n < segment->num_data) {
        src_data = segment->data + n;
        if (src_data->header.type == CPML_CLOSE)
            break;
        n_points = cpml_primitive_type_get_n_points(src_data->header.type);
        length = src_data->header.length;
        n += length;
        dst_data -= length;
        dst_data->header.type = src_data->header.type;
        dst_data->header.length = length;

        for (n_point = 1; n_point < n_points; ++n_point) {
            dst_data[n_points - n_point].point.x = end_x;
            dst_data[n_points - n_point].point.y = end_y;
            end_x = src_data[n_point].point.x;
            end_y = src_data[n_point].point.y;
        }

        /* Copy also the embedded data, if any */
        if (n_points < length) {
            size_t size = (length - n_points) * sizeof(cairo_path_data_t);
            memcpy(dst_data + n_points, src_data + n_points, size);
        }
    }

    if (src_data->header.type == CPML_CLOSE) {
        memcpy(segment->data + segment->data->header.length, dst_data,
               data_size - ((char *) dst_data - (char *) data));
    } else {
        memcpy(segment->data, data, data_size);
    }

    free(data);

    segment->data[1].point.x = end_x;
    segment->data[1].point.y = end_y;
}

/**
 * cpml_segment_to_cairo:
 * @segment: a #CpmlSegment
 * @cr: the destination cairo context
 *
 * Appends the path of @segment to @cr. The segment is "flattened",
 * that is %CPML_ARC primitives are approximated by one or more
 * %CPML_CURVE using cpml_arc_to_cairo(). Check its documentation
 * for further details.
 *
 * Since: 1.0
 **/
void
cpml_segment_to_cairo(const CpmlSegment *segment, cairo_t *cr)
{
    CpmlPrimitive primitive;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    do {
        cpml_primitive_to_cairo(&primitive, cr);
    } while (cpml_primitive_next(&primitive));
}

/**
 * cpml_segment_dump:
 * @segment: a #CpmlSegment
 *
 * Dumps the specified @segment to stdout. Useful for debugging purposes.
 *
 * Since: 1.0
 **/
void
cpml_segment_dump(const CpmlSegment *segment)
{
    CpmlPrimitive primitive;
    int first_call = 1;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    do {
        cpml_primitive_dump(&primitive, first_call);
        first_call = 0;
    } while (cpml_primitive_next(&primitive));
}


/*
 * normalize:
 * @segment: a #CpmlSegment
 *
 * Sanitizes @segment by calling ensure_one_leading_move() and reshape().
 *
 * Returns: 1 on success, 0 on no leading MOVE_TOs or on errors.
 **/
static int
normalize(CpmlSegment *segment)
{
    if (!ensure_one_leading_move(segment))
        return 0;

    return reshape(segment);
}

/*
 * ensure_one_leading_move:
 * @segment: a #CpmlSegment
 *
 * Jumps to the first %CPML_MOVE primitives and skips all eventual
 * subsequent %CPML_MOVE but the last. This is done unobtrusively,
 * by updating the <structname>CpmlSegment</structname> fields
 * accordingly. One, and only one, %CPML_MOVE primitive is left.
 *
 * Returns: 1 on success, 0 on no leading MOVE_TOs or on empty path
 **/
static int
ensure_one_leading_move(CpmlSegment *segment)
{
    cairo_path_data_t *data = segment->data;
    int length, num_data = segment->num_data;

    /* Look up the first MOVE_TO */
    while (num_data > 0 && data->header.type != CPML_MOVE) {
        length = data->header.length;
        data += length;
        num_data -= length;
    }

    /* Skip all duplicate CPML_MOVE but the last one */
    while (length = data->header.length,
           num_data > length && data[length].header.type == CPML_MOVE) {
        data += length;
        num_data -= length;
    }

    if (num_data <= length)
        return 0;

    segment->data = data;
    segment->num_data = num_data;
    return 1;
}

/*
 * reshape:
 * @segment: a #CpmlSegment
 *
 * Looks for the segment termination, that is the end of the underlying
 * cairo path or if the current primitive is a %CPML_MOVE or if the
 * last primitive was a %CPML_CLOSE. <structfield>num_data</structfield>
 * field is modified to properly point to the end of @segment. @segment
 * must have only one leading %CPML_MOVE and it is supposed to be non-empty,
 * conditions already imposed by ensure_one_leading_move().
 *
 * This function also checks that all the components of @segment
 * are valid primitives.
 *
 * Returns: 1 on success, 0 on invalid primitive found.
 **/
static int
reshape(CpmlSegment *segment)
{
    cairo_path_data_t *data = segment->data;
    cairo_path_data_type_t type;
    int length, n_points, num_data = 0;

    /* Always include the trailing CPML_MOVE */
    length = data->header.length;
    data += length;
    num_data += length;

    while (num_data < segment->num_data) {
        type = data->header.type;

        /* A CPML_MOVE is usually the start of the next segment */
        if (type == CPML_MOVE)
            break;

        length = data->header.length;
        data += length;
        num_data += length;

        /* Ensure the next primitive is valid and has enough data points */
        n_points = type == CPML_CLOSE ? 1 : cpml_primitive_type_get_n_points(type);
        if (length < 1 || n_points == 0 || length < n_points)
            return 0;

        /* A CPML_CLOSE ends the current segment */
        if (type == CPML_CLOSE)
            break;
    }

    /* The sum of the primitive lengths must *not* be greater than
     * the whole segment length */
    if (num_data > segment->num_data)
        return 0;

    segment->num_data = num_data;
    return 1;
}
