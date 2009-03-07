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
 * @title: CpmlSegment
 * @short_description: Contiguous lines
 *
 * A segment is a single contiguous line got from a cairo path.
 *
 * Every #cairo_path_t struct can contain more than one segment:
 * the CPML library provides iteration APIs to browse these segments.
 * The <structname>CpmlSegment</structname> struct internally keeps
 * a reference to the source #cairo_path_t so it can be used both
 * for getting segment data and browsing the segments (that is,
 * it is used also as an iterator).
 *
 * Use cpml_segment_reset() to reset the iterator at the start of the
 * cairo path (will point the first segment) and cpml_segment_next()
 * to get the next segment. When initialized,
 * <structname>CpmlSegment</structname> yet refers to the first
 * segment, so the initial reset is useless.
 *
 * Getting the previous segment is not provided, as the underlying
 * cairo struct is not accessible in reverse order.
 **/

/**
 * CpmlSegment:
 * @cairo_path: the source #cairo_path_t struct
 * @data: the segment data
 * @num_data: size of @data
 *
 * This is an unobtrusive struct to identify a segment inside a
 * cairo path. Unobtrusive means that the real coordinates are
 * still stored in @source: CpmlSegment only provides a way to
 * access the underlying cairo path.
 **/

#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-line.h"
#include "cpml-curve.h"
#include "cpml-pair.h"
#include "cpml-alloca.h"

#include <stdio.h>
#include <string.h>

static cairo_bool_t     segment_normalize       (CpmlSegment       *segment);
static void             join_primitives         (cairo_path_data_t *last_data,
                                                 const CpmlVector  *last_vector,
                                                 const CpmlPair    *new_point,
                                                 const CpmlVector  *new_vector);


/**
 * cpml_segment_from_cairo:
 * @segment: a #CpmlSegment
 * @cairo_path: the source #cairo_path_t
 *
 * Builds a CpmlSegment from a cairo_path_t structure. This operation
 * involves stripping the leading %CAIRO_PATH_MOVE_TO primitives and
 * setting the internal segment structure accordling. A pointer to the
 * source cairo path is kept.
 *
 * This function will fail if @cairo_path is null, empty or if its
 * <structfield>status</structfield> member is not %CAIRO_STATUS_SUCCESS.
 * Also, the first primitive must be a %CAIRO_PATH_MOVE_TO, so no
 * dependency on the cairo context is needed.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_segment_from_cairo(CpmlSegment *segment, cairo_path_t *cairo_path)
{
    /* The cairo path should be defined and in perfect state */
    if (cairo_path == NULL || cairo_path->num_data == 0 ||
        cairo_path->status != CAIRO_STATUS_SUCCESS)
        return 0;

    segment->cairo_path = cairo_path;
    segment->data = cairo_path->data;
    segment->num_data = cairo_path->num_data;

    return segment_normalize(segment);
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
 * cpml_segment_dump:
 * @segment: a #CpmlSegment
 *
 * Dumps the specified @segment to stdout. Useful for debugging purposes.
 **/
void
cpml_segment_dump(const CpmlSegment *segment)
{
    cairo_path_data_t *data;
    int n, n_point;

    for (n = 0; n < segment->num_data; ++n) {
	data = segment->data + n;

	switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
	    printf("Move to ");
	    break;
	case CAIRO_PATH_LINE_TO:
	    printf("Line to ");
	    break;
	case CAIRO_PATH_CURVE_TO:
	    printf("Curve to ");
	    break;
	case CAIRO_PATH_CLOSE_PATH:
	    printf("Path close");
	    break;
	default:
	    printf("Unknown entity (%d)", data->header.type);
	    break;
	}

	for (n_point = 1; n_point < data->header.length; ++n_point)
	    printf("(%lf, %lf) ", data[n_point].point.x,
                   data[n_point].point.y);

	n += n_point - 1;
	printf("\n");
    }
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
    segment->data = segment->cairo_path->data;
    segment->num_data = segment->cairo_path->num_data;
    segment_normalize(segment);
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
    int rest = segment->cairo_path->num_data - segment->num_data +
        segment->cairo_path->data - segment->data;

    if (rest <= 0)
        return 0;

    segment->data += segment->num_data;
    segment->num_data = rest;

    return segment_normalize(segment);
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
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_segment_offset(CpmlSegment *segment, double offset)
{
    int n, num_points, n_point;
    cairo_path_data_t *data;
    cairo_path_data_t *last_data;
    CpmlVector v_old, v_new;
    CpmlPair p_old;
    CpmlPair p[4];

    data = segment->data;
    last_data = NULL;
    p_old.x = 0;
    p_old.y = 0;

    for (n = 0; n < segment->num_data; n += data->header.length) {
        data = segment->data + n;
        num_points = data->header.length - 1;

        /* Fill the p[] vector */
        cpml_pair_copy(&p[0], &p_old);
        for (n_point = 1; n_point <= num_points; ++ n_point) {
            p[n_point].x = data[n_point].point.x;
            p[n_point].y = data[n_point].point.y;
        }

        /* Save the last direction vector in v_old */
        cpml_pair_copy(&v_old, &v_new);

        switch (data->header.type) {

        case CAIRO_PATH_MOVE_TO:
            v_new.x = 0;
            v_new.y = 0;
            break;

        case CAIRO_PATH_LINE_TO:
            {
                CpmlPrimitive line;
                cairo_path_data_t dummy[3];

                cpml_pair_to_cairo(&p[0], &dummy[0]);
                dummy[1].header.type = CAIRO_PATH_LINE_TO;
                dummy[1].header.length = 2;
                cpml_pair_to_cairo(&p[1], &dummy[2]);

                line.segment = NULL;
                line.org = &dummy[0];
                line.data = &dummy[1];

                cpml_line_offset(&line, offset);

                cpml_pair_from_cairo(&p[0], &dummy[0]);
                cpml_pair_from_cairo(&p[1], &dummy[2]);

                cpml_line_vector_at(&line, &v_new, 1.);
            }
            break;

        case CAIRO_PATH_CURVE_TO:
            {
                CpmlPrimitive curve;
                cairo_path_data_t dummy[5];

                cpml_pair_to_cairo(&p[0], &dummy[0]);
                dummy[1].header.type = CAIRO_PATH_CURVE_TO;
                dummy[1].header.length = 4;
                cpml_pair_to_cairo(&p[1], &dummy[2]);
                cpml_pair_to_cairo(&p[2], &dummy[3]);
                cpml_pair_to_cairo(&p[3], &dummy[4]);

                curve.segment = NULL;
                curve.org = &dummy[0];
                curve.data = &dummy[1];

                cpml_curve_offset(&curve, offset);

                cpml_pair_from_cairo(&p[0], &dummy[0]);
                cpml_pair_from_cairo(&p[1], &dummy[2]);
                cpml_pair_from_cairo(&p[2], &dummy[3]);
                cpml_pair_from_cairo(&p[3], &dummy[4]);

                cpml_curve_vector_at_time(&curve, &v_new, 1.);
            }
            break;

        case CAIRO_PATH_CLOSE_PATH:
            return 1;
        }

        join_primitives(last_data, &v_old, &p[0], &v_new);

        /* Save the end point of the original primitive in p_old */
        last_data = data + num_points;
        p_old.x = last_data->point.x;
        p_old.y = last_data->point.y;

        /* Store the results got from the p[] vector in the cairo path */
        for (n_point = 1; n_point <= num_points; ++ n_point) {
            data[n_point].point.x = p[n_point].x;
            data[n_point].point.y = p[n_point].y;
        }
    }

    return 1;
}

/**
 * segment_normalize:
 * @segment: a #CpmlSegment
 *
 * Strips the leading %CAIRO_PATH_MOVE_TO primitives, updating
 * the CpmlSegment structure accordling. One, and only one,
 * %CAIRO_PATH_MOVE_TO primitive is left.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on errors
 **/
static cairo_bool_t
segment_normalize(CpmlSegment *segment)
{
    cairo_path_data_t *data;

    if (segment->cairo_path->data->header.type != CAIRO_PATH_MOVE_TO) {
        return 0;
    }

    data = segment->data;

    while (segment->num_data >= 0) {
        data += 2;
        if (data->header.type != CAIRO_PATH_MOVE_TO)
            return 1;

        segment->data = data;
        segment->num_data -= 2;
    }

    return 0;
}

/**
 * join_primitives:
 * @last_data: the previous primitive end data point (if any)
 * @last_vector: @last_data direction vector
 * @new_point: the new primitive starting point
 * @new_vector: @new_point direction vector
 *
 * Joins two primitive modifying the end point of the first one (stored
 * as cairo path data in @last_data).
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>This approach is quite naive when curves are involved.</listitem>
 * </itemizedlist>
 * </important>
 **/
static void
join_primitives(cairo_path_data_t *last_data, const CpmlVector *last_vector,
                const CpmlPair *new_point, const CpmlVector *new_vector)
{
    CpmlPair last_point;

    if (last_data == NULL)
        return;

    last_point.x = last_data->point.x;
    last_point.y = last_data->point.y;

    if (cpml_pair_intersection_pv_pv(&last_point,
                                     &last_point, last_vector,
                                     new_point, new_vector)) {
        last_data->point.x = last_point.x;
        last_data->point.y = last_point.y;
    } else {
        last_data->point.x = new_point->x;
        last_data->point.y = new_point->y;
    }
}
