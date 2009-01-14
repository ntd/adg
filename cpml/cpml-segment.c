/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
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


#include "cpml-segment.h"
#include "cpml-alloca.h"

#include <stdio.h>
#include <string.h>

static cairo_bool_t     normalize_segment       (CpmlSegment       *segment);


/**
 * cpml_segment_init:
 * @segment: a #CpmlSegment
 * @src: the source cairo_path_t
 *
 * Builds a CpmlSegment from a cairo_path_t structure. This operation
 * involves stripping the leading %MOVE_TO primitives and setting the
 * internal segment structure accordling. A pointer to the source
 * path segment is kept.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_segment_init(CpmlSegment *segment, cairo_path_t *src)
{
    /* The cairo path should be defined and in perfect state */
    if (src == NULL || src->num_data == 0 ||
        src->status != CAIRO_STATUS_SUCCESS)
        return 0;

    segment->original = src;
    memcpy(&segment->path, src, sizeof(cairo_path_t));

    return normalize_segment(segment);
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
    const cairo_path_t *path;
    cairo_path_data_t *data;
    int n_data, n_point;

    if (segment == NULL) {
        printf("Trying to dump a NULL segment!\n");
        return;
    }

    path = &segment->path;
    for (n_data = 0; n_data < path->num_data; ++n_data) {
	data = path->data + n_data;

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

	n_data += n_point - 1;
	printf("\n");
    }
}

/**
 * cpml_segment_reset:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the first segment of the original path.
 **/
void
cpml_segment_reset(CpmlSegment *segment)
{
    memcpy(&segment->path, segment->original, sizeof(cairo_path_t));
    normalize_segment(segment);
}

/**
 * cpml_segment_next:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the next segment of the original path.
 *
 * Return value: 1 on success, 0 if no next segment found or errors
 **/
cairo_bool_t
cpml_segment_next(CpmlSegment *segment)
{
    int num_data = segment->path.num_data;
    int offset = segment->path.data - segment->original->data;

    segment->path.num_data = segment->original->num_data - num_data - offset;
    segment->path.data += num_data;

    return normalize_segment(segment);
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
    int num_data, n_data;
    int num_points, n_point;
    const cairo_path_data_t *src_data;

    num_data = segment->path.num_data;
    data_size = sizeof(cairo_path_data_t) * num_data;
    data = cpml_alloca(data_size);
    end_x = segment->path.data[1].point.x;
    end_y = segment->path.data[1].point.y;

    for (n_data = 2; n_data < num_data; ++n_data) {
        src_data = segment->path.data + n_data;
	num_points = src_data->header.length;

        dst_data = data + num_data - n_data - num_points + 2;
        dst_data->header.type = src_data->header.type;
        dst_data->header.length = num_points;

	for (n_point = 1; n_point < num_points; ++n_point) {
            dst_data[num_points - n_point].point.x = end_x;
            dst_data[num_points - n_point].point.y = end_y;
            end_x = src_data[n_point].point.x;
            end_y = src_data[n_point].point.y;
        }

	n_data += n_point - 1;
    }

    data[0].header.type = CAIRO_PATH_MOVE_TO;
    data[0].header.length = 2;
    data[1].point.x = end_x;
    data[1].point.y = end_y;
    memcpy(segment->path.data, data, data_size);
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
    int n_data, num_data;
    int n_point, num_points;

    data = segment->path.data;
    num_data = segment->path.num_data;

    for (n_data = 0; n_data < num_data; n_data += num_points) {
        num_points = data->header.length;
        ++data;
        for (n_point = 1; n_point < num_points; ++n_point) {
            cairo_matrix_transform_point(matrix, &data->point.x, &data->point.y);
            ++data;
        }
    }
}

/**
 * normalize_segment:
 * @segment: a #CpmlSegment
 *
 * Strips the leading CAIRO_PATH_MOVE_TO primitives, updating the CpmlSegment
 * structure accordling. One, and only once, MOVE_TO primitive is left.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on errors
 **/
static cairo_bool_t
normalize_segment(CpmlSegment *segment)
{
    cairo_path_data_t *data;

    if (segment == NULL || segment->path.num_data <= 0)
        return 0;

    data = segment->path.data;
    if (data->header.type != CAIRO_PATH_MOVE_TO) {
        segment->path.status = CAIRO_STATUS_INVALID_PATH_DATA;
        return 0;
    }

    while (segment->path.num_data >= 0) {
        data += 2;
        if (data->header.type != CAIRO_PATH_MOVE_TO)
            return 1;

        segment->path.data = data;
        segment->path.num_data -= 2;
    }

    return 0;
}
