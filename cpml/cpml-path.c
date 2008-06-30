/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


#include "cpml-path.h"

#include <string.h>

static cairo_bool_t	strip_leadings		(CpmlPath	*path);
static cairo_bool_t	path_to_segment		(CpmlPath	*segment,
						 const CpmlPath	*path);
static cairo_bool_t	path_to_primitive	(CpmlPath	*primitive,
						 const CpmlPath	*path);

/**
 * cpml_path_from_cairo:
 * @path: an allocated #CpmlPath structure
 * @src: the source cairo_path_t
 * @cr: a cairo context
 *
 * Builds a CpmlPath from a cairo_path_t structure. This operations involves
 * stripping the leading MOVE_TO data and setting @path->org
 *
 * @cr is used to get the current point (if needed): it can be %NULL,
 * in which case (0,0) will be used.
 *
 * @path and @src can be the same object (a #CpmlPath structure).
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_path_from_cairo(CpmlPath *path, const cairo_path_t *src, cairo_t *cr)
{
	CpmlPair org = {0., 0.};

	if (cr && cairo_has_current_point(cr))
		cairo_get_current_point(cr, &org.x, &org.y);

	return cpml_path_from_cairo_explicit(path, src, &org);
}

/**
 * cpml_path_from_cairo_explicit:
 * @path: an allocated #CpmlPath structure
 * @src: the source cairo_path_t
 * @org: a specific origin
 *
 * Similar to cpml_path_from_cairo() but using an explicit origin.
 *
 * @path and @src can be the same object (a #CpmlPath structure).
 *
 * @org can be %NULL, in which case the original path->org pair
 * is left untouched.
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_path_from_cairo_explicit(CpmlPath *path, const cairo_path_t *src,
			      const CpmlPair *org)
{
	if (src->status != CAIRO_STATUS_SUCCESS)
		return 0;

	if (path != (CpmlPath *) src)
		memcpy(path, src, sizeof(cairo_path_t));

	if (strip_leadings(path)) {
		/* org taken from leadings MOVE_TO */
	} else if (path->cairo_path.status != CAIRO_STATUS_SUCCESS) {
		/* Error: probably an empty path provided */
		return 0;
	} else if (org) {
		/* Use the provided org */
		cpml_pair_copy(&path->org, org);
	}

	return 1;
}

/**
 * cpml_path_copy:
 * @path: an allocated #CpmlPath
 * @src: the source path to copy
 *
 * Strips the leading CAIRO_PATH_MOVE_TO primitives, updating the CpmlPath
 * structure accordling. Also the path org is update.
 *
 * Return value: @path or %NULL on errors
 */
CpmlPath *
cpml_path_copy(CpmlPath *path, const CpmlPath *src)
{
	if (path == NULL || src == NULL)
		return NULL;

	return memcpy(path, src, sizeof(CpmlPath));
}

/**
 * cpml_segment_from_path:
 * @segment: an allocated #CpmlPath struct
 * @path: the source path
 * @index: the segment to retrieve (starting from 1);
 *         %CPML_FIRST or %CPML_LAST can be used
 *
 * Gets a specific segment from a path.
 *
 * Return value: 1 if a valid segment was found, 0 on errors
 */
cairo_bool_t
cpml_segment_from_path(CpmlPath *segment, const CpmlPath *path, int index)
{
	CpmlPath residue, result;
	int i;

	if (!cpml_path_copy(&residue, path))
		return 0;

	i = 0;

	do {
		if (!path_to_segment(&result, &residue))
			return index == CPML_LAST && i > 0;

		residue.cairo_path.data += result.cairo_path.num_data;
		residue.cairo_path.num_data -= result.cairo_path.num_data;
		++i;

		if (index == CPML_LAST) {
			cpml_path_copy(segment, &result);
			continue;
		}
	} while (i < index);

	cpml_path_copy(segment, &result);
	return 1;
}

/**
 * cpml_primitive_from_path:
 * @primitive: an allocated #CpmlPath struct
 * @path: the source path
 * @index: the primitive to retrieve (starting from 1);
 *         %CPML_FIRST or %CPML_LAST can be used
 *
 * Gets a specific primitive from a path.
 *
 * Return value: 1 if a valid primitive was found, 0 on errors
 */
cairo_bool_t
cpml_primitive_from_path(CpmlPath *primitive, const CpmlPath *path, int index)
{
	CpmlPath residue, result;
	int i;

	if (!cpml_path_copy(&residue, path))
		return 0;

	i = 0;

	do {
		if (!path_to_primitive(&result, &residue))
			return index == CPML_LAST && i > 0;

		residue.cairo_path.data += result.cairo_path.num_data;
		residue.cairo_path.num_data -= result.cairo_path.num_data;
		++i;

		if (index == CPML_LAST) {
			cpml_path_copy(primitive, &result);
			continue;
		}
	} while (i < index);

	cpml_path_copy(primitive, &result);
	return 1;
}

/**
 * cpml_primitive_get_pair:
 * @primitive: the source primitive
 * @pair: the allocated CpmlPair destination
 * @index: index of the pair to retrieve, starting from 1
 *
 * Shortcut to get a pair from a primitive.
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_primitive_get_pair(const CpmlPath *primitive, CpmlPair *pair, int index)
{
	cairo_path_data_t *data = primitive->cairo_path.data;

	if (index == 0 || index > data[0].header.length)
		return 0;

	pair->x = data[index].point.x;
	pair->y = data[index].point.y;
	return 1;
}

/**
 * cpml_primitive_set_pair:
 * @primitive: an allocated CpmlPath primitive
 * @pair: the source pair
 * @index: index of the pair to change, starting from 1
 *
 * Shortcut to set a pair on a primitive.
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_primitive_set_pair(CpmlPath *primitive, const CpmlPair *pair, int index)
{
	cairo_path_data_t *data = primitive->cairo_path.data;

	if (index == 0 || index > data[0].header.length)
		return 0;

	data[index].point.x = pair->x;
	data[index].point.y = pair->y;
	return 1;
}

/**
 * cpml_primitive_get_point:
 * @primitive: the source primitive
 * @point: the allocated CpmlPair destination
 * @pos: the position factor, being 0 the starting and 1 the ending point
 *
 * Gets a point lying on @primitive at a specific percentual position.
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_primitive_get_point(const CpmlPath *primitive,
			 CpmlPair *point, double pos)
{
	int type = primitive->cairo_path.data[0].header.type;

	if (type != CAIRO_PATH_CLOSE_PATH && type != CAIRO_PATH_LINE_TO &&
	    type != CAIRO_PATH_CURVE_TO)
		return 0;

	/* Common cases */
	if (type == CAIRO_PATH_CLOSE_PATH || pos == 0.) {
		return cpml_pair_copy(point, &primitive->org);
	} else if (pos == 1.0) {
		int n = type == CAIRO_PATH_LINE_TO ? 1 : 3;
		return cpml_primitive_get_pair(primitive, point, n);
	}

	/* TODO */
	return 0;
}

/**
 * cpml_primitive_reverse:
 * @primitive: an allocated #CpmlPath struct
 *
 * Reverses a primitive.
 *
 * Return value: 1 on success, 0 on errors
 */
cairo_bool_t
cpml_primitive_reverse(CpmlPath *primitive)
{
	CpmlPair tmp;

	switch (primitive->cairo_path.data[0].header.type) {
	case CAIRO_PATH_LINE_TO:
		cpml_pair_copy(&tmp, &primitive->org);
		cpml_primitive_get_pair(primitive, &primitive->org, 1);
		cpml_primitive_set_pair(primitive, &tmp, 1);
		break;
	case CAIRO_PATH_CURVE_TO:
		cpml_pair_copy(&tmp, &primitive->org);
		cpml_primitive_get_pair(primitive, &primitive->org, 3);
		cpml_primitive_set_pair(primitive, &tmp, 3);

		cpml_primitive_get_pair(primitive, &tmp, 2);
		primitive->cairo_path.data[2].point.x =
			primitive->cairo_path.data[3].point.x;
		primitive->cairo_path.data[2].point.y =
			primitive->cairo_path.data[3].point.y;
		cpml_primitive_set_pair(primitive, &tmp, 3);
		break;
	default:
		return 0;
	}

	return 1;
}

/**
 * strip_leadings:
 * @path: a #CpmlPath
 *
 * Strips the leading CAIRO_PATH_MOVE_TO primitives, updating the CpmlPath
 * structure accordling. Also the path org is update.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on errors
 *               (check for path->cairo_path.status == CAIRO_STATUS_SUCCESS)
 */
static cairo_bool_t
strip_leadings(CpmlPath *path)
{
	if (path->cairo_path.data[0].header.type != CAIRO_PATH_MOVE_TO)
		return 0;

	do {
		++path->cairo_path.data;
		path->org.x = path->cairo_path.data->point.x;
		path->org.y = path->cairo_path.data->point.y;
		++path->cairo_path.data;
		path->cairo_path.num_data -= 2;
		if (path->cairo_path.num_data <= 0) {
			path->cairo_path.status =
				CAIRO_STATUS_INVALID_PATH_DATA;
			return 0;
		}
	} while (path->cairo_path.data->header.type == CAIRO_PATH_MOVE_TO);

	return 1;
}

/**
 * path_to_segment:
 * @segment: an allocated #CpmlPath struct
 * @path: the source path
 *
 * Converts a path to a segment. @segment and @path can be the same struct.
 *
 * Return value: 1 if a valid segment is found, 0 on errors
 */
static cairo_bool_t
path_to_segment(CpmlPath *segment, const CpmlPath *path)
{
	cairo_path_data_t *path_data;
	int i;

	if (segment != path)
		cpml_path_copy(segment, path);

	if (!strip_leadings(segment) &&
	    segment->cairo_path.status != CAIRO_STATUS_SUCCESS)
		return 0;

	path_data = segment->cairo_path.data;
	i = 0;

	do {
		if (path_data->header.type == CAIRO_PATH_MOVE_TO) {
			--i;
			break;
		} else if (path_data->header.type == CAIRO_PATH_CLOSE_PATH) {
			break;
		}
		i += path_data->header.length;
		path_data += path_data->header.length;
	} while (i < segment->cairo_path.num_data);

	segment->cairo_path.num_data = i;
	return 1;
}

/**
 * path_to_primitive:
 * @primitive: an allocated #CpmlPath struct
 * @path: the source path
 *
 * Converts a path to a primitive. @primitive and @path can be the same struct.
 *
 * Return value: 1 if a valid primitive is found, 0 on errors
 */
static cairo_bool_t
path_to_primitive(CpmlPath *primitive, const CpmlPath *path)
{
	cairo_path_data_t *path_data;

	if (primitive != path)
		cpml_path_copy(primitive, path);

	if (!strip_leadings(primitive) &&
	    primitive->cairo_path.status != CAIRO_STATUS_SUCCESS)
		return 0;

	primitive->cairo_path.num_data = 1;
	return 1;
}
