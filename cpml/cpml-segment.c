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
 * SECTION:segment
 * @Section_Id:CpmlSegment
 * @title: CpmlSegment
 * @short_description: Contiguous segment that can be a fragment
 *                     or a whole cairo path
 *
 * A segment is a single contiguous line got from a cairo path. The
 * CPML library relies on one assumption to let the data be independent
 * from the current point (and thus from the cairo context): any segment
 * MUST be preceded by at least one %CAIRO_PATH_MOVE_TO primitive.
 * This means a valid segment in cairo could be rejected by CPML.
 *
 * #CpmlSegment provides an unobtrusive way to access a cairo path.
 * This means #CpmlSegment itsself does not hold any coordinates but
 * instead a bunch of pointers to the original #cairo_path_t struct:
 * modifying data throught this struct also changes the original path.
 *
 * Every #CpmlPath struct can contain more than one segment: the CPML
 * library provides iteration APIs to browse the segments of a path.
 * Use cpml_segment_reset() to reset the iterator at the start of the
 * cairo path (will point the first segment) and cpml_segment_next()
 * to get the next segment. Getting the previous segment is not provided
 * as the underlying cairo struct is not accessible in reverse order.
 *
 * When initialized, #CpmlSegment yet refers to the first segment so
 * the initial reset is not required.
 **/

/**
 * CpmlPath:
 *
 * This is another name for the #cairo_path_t type. Although phisically
 * they are the same struct, #CpmlPath conceptually embodies an important
 * difference: it is a cairo path that can embed %CAIRO_PATH_ARC_TO
 * primitives. This is not a native cairo primitive and having two
 * different data types is a good way to make clear when a function
 * expect or not embedded arc-to primitives.
 **/

/**
 * CpmlSegment:
 * @path:     the source #CpmlPath struct
 * @data:     the data points of the segment; the first primitive
 *            will always be a %CAIRO_PATH_MOVE_TO
 * @num_data: size of @data
 *
 * This is an unobtrusive struct to identify a segment inside a
 * cairo path. Unobtrusive means that the real coordinates are
 * still stored in @path: CpmlSegment only provides a way to
 * access them.
 **/


#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-line.h"
#include "cpml-curve.h"
#include "cpml-pair.h"
#include "cpml-alloca.h"

#include <stdio.h>
#include <string.h>

static cairo_bool_t     normalize               (CpmlSegment       *segment);
static cairo_bool_t     ensure_one_move_to      (CpmlSegment       *segment);
static void             reshape                 (CpmlSegment       *segment);


/**
 * cpml_segment_from_cairo:
 * @segment: a #CpmlSegment
 * @path: the source #CpmlPath
 *
 * Builds a CpmlSegment from a #CpmlPath structure. This operation
 * involves stripping the leading %CAIRO_PATH_MOVE_TO primitives and
 * setting the internal segment structure accordling. A pointer to the
 * source cairo path is kept.
 *
 * This function will fail if @path is null, empty or if its
 * <structfield>status</structfield> member is not %CAIRO_STATUS_SUCCESS.
 * Also, the first primitive must be a %CAIRO_PATH_MOVE_TO, so no
 * dependency on the cairo context is needed.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_segment_from_cairo(CpmlSegment *segment, CpmlPath *path)
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
 * Return value: @segment or %NULL on errors
 **/
CpmlSegment *
cpml_segment_copy(CpmlSegment *segment, const CpmlSegment *src)
{
    if (segment == NULL || src == NULL)
        return NULL;

    return memcpy(segment, src, sizeof(CpmlSegment));
}


/**
 * cpml_segment_reset:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the first segment of the source cairo path.
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
 * Return value: 1 on success, 0 if no next segment found or errors
 **/
cairo_bool_t
cpml_segment_next(CpmlSegment *segment)
{
    int rest = segment->path->num_data - segment->num_data +
        segment->path->data - segment->data;

    if (rest <= 0)
        return 0;

    segment->data += segment->num_data;
    segment->num_data = rest;

    return normalize(segment);
}


/**
 * cpml_segment_length:
 * @segment: a #CpmlSegment
 *
 * Gets the whole length of @segment.
 *
 * Returns: the requested length
 **/
double
cpml_segment_length(const CpmlSegment *segment)
{
    CpmlPrimitive primitive;
    double length;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);
    length = 0;

    do {
        length += cpml_primitive_length(&primitive);
    } while (cpml_primitive_next(&primitive));

    return length;
}

/**
 * cpml_segment_pair_at:
 * @segment: a #CpmlSegment
 * @pair:    the destination #CpmlPair
 * @pos:     the position value
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
 **/
void
cpml_segment_pair_at(const CpmlSegment *segment, CpmlPair *pair, double pos)
{
    CpmlPrimitive primitive;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    /* Handle the common cases: start and end points */
    if (pos == 0)
        return cpml_primitive_pair_at(&primitive, pair, 0);

    if (pos == 1) {
        while (cpml_primitive_next(&primitive))
            ;
        return cpml_primitive_pair_at(&primitive, pair, 1);
    }
}

/**
 * cpml_segment_vector_at:
 * @segment: a #CpmlSegment
 * @vector:  the destination #CpmlVector
 * @pos:     the position value
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
 **/
void
cpml_segment_vector_at(const CpmlSegment *segment,
                       CpmlVector *vector, double pos)
{
    CpmlPrimitive primitive;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    /* Handle the common cases: start and end points */
    if (pos == 0) {
        cpml_primitive_vector_at(&primitive, vector, 0);
        return;
    }

    if (pos == 1) {
        while (cpml_primitive_next(&primitive))
            ;
        cpml_primitive_vector_at(&primitive, vector, 1);
        return;
    }
}


/**
 * cpml_segment_to_cairo:
 * @segment: a #CpmlSegment
 * @cr: the destination cairo context
 *
 * Appends the path of @segment to @cr. The segment is "flattened",
 * that is %CAIRO_PATH_ARC_TO primitives are approximated by one
 * or more %CAIRO_PATH_CURVE_TO using cpml_arc_to_cairo(). Check
 * its documentation for further details.
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
 **/
void
cpml_segment_dump(const CpmlSegment *segment)
{
    CpmlPrimitive primitive;
    cairo_bool_t first_call = 1;

    cpml_primitive_from_segment(&primitive, (CpmlSegment *) segment);

    do {
        cpml_primitive_dump(&primitive, first_call);
        first_call = 0;
    } while (cpml_primitive_next(&primitive));
}


/**
 * cpml_segment_reverse:
 * @segment: a #CpmlSegment
 *
 * Reverses @segment in-place. The resulting rendering will be the same,
 * but with the primitives generated in reverse order.
 **/
void
cpml_segment_reverse(CpmlSegment *segment)
{
    cairo_path_data_t *data, *dst_data;
    size_t data_size;
    double end_x, end_y;
    int n, num_points, n_point;
    const cairo_path_data_t *src_data;

    data_size = sizeof(cairo_path_data_t) * segment->num_data;
    data = cpml_alloca(data_size);
    end_x = segment->data[1].point.x;
    end_y = segment->data[1].point.y;

    for (n = 2; n < segment->num_data; ++n) {
        src_data = segment->data + n;
	num_points = src_data->header.length;

        dst_data = data + segment->num_data - n - num_points + 2;
        dst_data->header.type = src_data->header.type;
        dst_data->header.length = num_points;

	for (n_point = 1; n_point < num_points; ++n_point) {
            dst_data[num_points - n_point].point.x = end_x;
            dst_data[num_points - n_point].point.y = end_y;
            end_x = src_data[n_point].point.x;
            end_y = src_data[n_point].point.y;
        }

	n += n_point - 1;
    }

    data[0].header.type = CAIRO_PATH_MOVE_TO;
    data[0].header.length = 2;
    data[1].point.x = end_x;
    data[1].point.y = end_y;
    memcpy(segment->data, data, data_size);
}

/**
 * cpml_segment_transform:
 * @segment: a #CpmlSegment
 * @matrix: the matrix to be applied
 *
 * Applies @matrix on all the points of @segment.
 **/
void
cpml_segment_transform(CpmlSegment *segment, const cairo_matrix_t *matrix)
{
    cairo_path_data_t *data;
    int n, n_point, num_points;

    data = segment->data;

    for (n = 0; n < segment->num_data; n += num_points) {
        num_points = data->header.length;
        ++data;
        for (n_point = 1; n_point < num_points; ++n_point) {
            cairo_matrix_transform_point(matrix, &data->point.x, &data->point.y);
            ++data;
        }
    }
}

/**
 * cpml_segment_intersection:
 * @segment:  the first #CpmlSegment
 * @segment2: the second #CpmlSegment
 * @dest:     the destination vector of #CpmlPair
 * @max:      maximum number of intersections to return
 *
 * Computes the intersections between @segment and @segment2 and
 * returns the found points in @dest. If the intersections are more
 * than @max, only the first @max pairs are stored in @dest.
 *
 * To get the job done, the primitives of @segment are sequentially
 * scanned for intersections with any primitive in @segment2. This
 * means @segment has a higher precedence over @segment2.
 *
 * Return value: the number of intersections found
 **/
int
cpml_segment_intersection(const CpmlSegment *segment,
                          const CpmlSegment *segment2,
                          CpmlPair *dest, int max)
{
    CpmlPrimitive portion;
    int partial, total;

    cpml_primitive_from_segment(&portion, (CpmlSegment *) segment);
    total = 0;

    do {
        partial = cpml_primitive_intersection_with_segment(&portion,
                                                           segment2,
                                                           dest + total,
                                                           max - total);
        total += partial;
    } while (total < max && cpml_primitive_next(&portion));

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
 **/
void
cpml_segment_offset(CpmlSegment *segment, double offset)
{
    CpmlPrimitive primitive;
    CpmlPrimitive last_primitive;
    cairo_path_data_t org, old_end;
    cairo_bool_t first_cycle;

    cpml_primitive_from_segment(&primitive, segment);
    first_cycle = 1;

    do {
        if (!first_cycle) {
            org = old_end;
            primitive.org = &org;
        }

        old_end = *cpml_primitive_get_point(&primitive, -1);
        cpml_primitive_offset(&primitive, offset);

        if (!first_cycle) {
            cpml_primitive_join(&last_primitive, &primitive);
            primitive.org = cpml_primitive_get_point(&last_primitive, -1);
        }

        cpml_primitive_copy(&last_primitive, &primitive);
        first_cycle = 0;
    } while (cpml_primitive_next(&primitive));
}

/**
 * normalize:
 * @segment: a #CpmlSegment
 *
 * Strips the leading %CAIRO_PATH_MOVE_TO primitives, updating
 * the CpmlSegment structure accordling. One, and only one,
 * %CAIRO_PATH_MOVE_TO primitive is left.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on errors
 **/
static cairo_bool_t
normalize(CpmlSegment *segment)
{
    if (!ensure_one_move_to(segment))
        return 0;

    reshape(segment);
    return 1;
}

/**
 * ensure_one_move_to:
 * @segment: a #CpmlSegment
 *
 * Strips the leading %CAIRO_PATH_MOVE_TO primitives, updating
 * the <structname>CpmlSegment</structname> structure accordling.
 * One, and only one, %CAIRO_PATH_MOVE_TO primitive is left.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on empty path
 **/
static cairo_bool_t
ensure_one_move_to(CpmlSegment *segment)
{
    cairo_path_data_t *new_data;
    int new_num_data, length;

    new_data = segment->data;

    /* Check for at least one move to */
    if (new_data->header.type != CAIRO_PATH_MOVE_TO)
        return 0;

    new_num_data = segment->num_data;
    length = 0;

    /* Strip the leading CAIRO_PATH_MOVE_TO, leaving only the last one */
    do {
        new_data += length;
        new_num_data -= length;
        length = new_data->header.length;

        /* Check for end of cairo path data */
        if (length >= new_num_data)
            return 0;
    } while (new_data[length].header.type == CAIRO_PATH_MOVE_TO);

    segment->data = new_data;
    segment->num_data = new_num_data;

    return 1;
}

/**
 * reshape:
 * @segment: a #CpmlSegment
 *
 * Looks for the segment termination and modify the
 * <structfield>num_data</structfield> field of @segment accordling.
 * @segment must have only one leading %CAIRO_PATH_MOVE_TO and
 * it is supposed to be non-empty, conditions yet imposed by the
 * ensure_one_move_to() function.
 **/
static void
reshape(CpmlSegment *segment)
{
    cairo_path_data_t *data;
    int num_data, new_num_data, length;

    /* Skip the leading move to */
    new_num_data = 2;
    data = segment->data + new_num_data;

    /* Calculate the remaining data in the cairo path */
    num_data = segment->path->num_data -
               (segment->data - segment->path->data);

    while (new_num_data < num_data) {
        /* A primitive is considered valid if it has implemented
         * its own type_get_npoints() */
        if (cpml_primitive_type_get_npoints(data->header.type) < 0)
            break;

        length = data->header.length;
        data += length;
        new_num_data += length;
    }

    segment->num_data = new_num_data;
}
