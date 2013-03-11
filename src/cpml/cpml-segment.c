/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
 * @Section_Id:CpmlSegment
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
 * involves stripping the duplicate %CPML_MOVE primitives at the
 * start of the path and setting <structfield>num_data</structfield>
 * field to the end of the contiguous line, that is when another
 * %CPML_MOVE primitive is found or at the end of the path.
 * A pointer to the source cairo path is kept though.
 *
 * This function will fail if @path is null, empty or if its
 * <structfield>status</structfield> member is not %CAIRO_STATUS_SUCCESS.
 * Also, the first primitive must be a %CPML_MOVE, so no
 * dependency on the cairo context is needed.
 *
 * Returns: (type gboolean): %1 if @segment has been succesfully computed,
 *                           %0 on errors
 *
 * Since: 1.0
 **/
int
cpml_segment_from_cairo(CpmlSegment *segment, cairo_path_t *path)
{
    /* The cairo path should be defined and in a perfect state */
    if (path == NULL || path->num_data == 0 ||
        path->status != CAIRO_STATUS_SUCCESS)
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
 * Returns: (type gboolean): %1 on success,
 *                           %0 if no next segment found or errors
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
 * @pos. @pos is an homogeneous factor where %0 is the start point,
 * %1 the end point, %0.5 the mid point and so on.
 * The relation %0 < @pos < %1 should be satisfied, although some
 * cases accept value outside this range.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>The actual implementation returns only the start and end points,
 *           that is only when @pos is %0 or %1.</listitem>
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
 * @pos. @pos is an homogeneous factor where %0 is the start point,
 * %1 the end point, %0.5 the mid point and so on.
 * The relation %0 < @pos < %1 should be satisfied, although some
 * cases accept value outside this range.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>The actual implementation returns only the start and end
 *           steepness, that is only when @pos is %0 or %1.</listitem>
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
 *           so obvious: use cpml_close_offset() when available.</listitem>
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
        if (data->header.type != CAIRO_PATH_CLOSE_PATH) {
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
 * It is assumed that @segment has yet been sanitized, that is returned
 * by some CPML API function or it is a path yet conforming to the
 * segment rules described by the cpml_segment_from_cairo() function.
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
    end_x = segment->data[1].point.x;
    end_y = segment->data[1].point.y;

    n = segment->data->header.length;
    data->header.type = CAIRO_PATH_CLOSE_PATH;
    data->header.length = n;

    while (n < segment->num_data) {
        src_data = segment->data + n;
        n_points = cpml_primitive_type_get_n_points(src_data->header.type);
        length = src_data->header.length;
        n += length;
        dst_data = data + segment->num_data - n + data->header.length;
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

    data[1].point.x = end_x;
    data[1].point.y = end_y;
    memcpy(segment->data, data, data_size);

    free(data);
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
 * Returns: %1 on success, %0 on no leading MOVE_TOs or on errors
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
 * Strips the leading %CPML_MOVE primitives, updating the
 * <structname>CpmlSegment</structname> structure accordingly.
 * One, and only one, %CPML_MOVE primitive is left.
 *
 * Returns: %1 on success, %0 on no leading MOVE_TOs or on empty path
 **/
static int
ensure_one_leading_move(CpmlSegment *segment)
{
    cairo_path_data_t *new_data;
    int new_num_data, move_length;

    /* Check for at least one move to */
    if (segment->data->header.type != CAIRO_PATH_MOVE_TO)
        return 0;

    new_data = segment->data;
    new_num_data = segment->num_data;

    while (1) {
        move_length = new_data->header.length;

        /* Check for the end of cairo path data, that is when
         * @segment is composed by only CAIRO_PATH_MOVE_TO */
        if (new_num_data <= move_length)
            return 0;

        /* Check if this is the last CAIRO_PATH_MOVE_TO */
        if (new_data[move_length].header.type != CAIRO_PATH_MOVE_TO)
            break;

        new_data += move_length;
        new_num_data -= move_length;
    }

    segment->data = new_data;
    segment->num_data = new_num_data;

    return 1;
}

/*
 * reshape:
 * @segment: a #CpmlSegment
 *
 * Looks for the segment termination, that is the end of the underlying
 * cairo path or a %CPML_MOVE operation. <structfield>num_data</structfield>
 * field is modified to properly point to the end of @segment.
 * @segment must have only one leading %CPML_MOVE and it is supposed
 * to be non-empty, conditions yet imposed by the ensure_one_leading_move().
 *
 * This function also checks that all the components of @segment
 * are valid primitives.
 *
 * Returns: %1 on success, %0 on invalid primitive found
 **/
static int
reshape(CpmlSegment *segment)
{
    const cairo_path_data_t *data;
    int trailing_data, new_num_data, length;

    data = segment->data;
    new_num_data = 0;
    trailing_data = segment->num_data;

    while (1) {
        length = data->header.length;
        new_num_data += length;
        trailing_data -= length;
        data += length;

        /* Check for invalid data size */
        if (trailing_data < 0)
            return 0;

        if (trailing_data == 0 || data->header.type == CAIRO_PATH_MOVE_TO)
            break;

        /* Ensure that all the components are valid primitives */
        if (cpml_primitive_type_get_n_points(data->header.type) == 0)
            return 0;
    }

    segment->num_data = new_num_data;
    return 1;
}
