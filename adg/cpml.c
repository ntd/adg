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

#include "cpml.h"

#include <stddef.h>


cairo_bool_t
cpml_segment_set_from_path(CpmlSegment		*segment,
			   const cairo_path_t	*path,
			   int			 which)
{
	int i, segment_start;

	if (segment == NULL || path == NULL || path->num_data <= 0)
		return 0;

	i = 0;
	segment->status = CAIRO_STATUS_INVALID_INDEX;

	do {
		/* Skip the leading MOVE_TO primitives */
		while (path->data[i].header.type == CAIRO_PATH_MOVE_TO) {
			i += 2;

			/* No more segments to analyze */
			if (i >= path->num_data)
				return segment->status == CAIRO_STATUS_SUCCESS;
		}

		/* Include the last MOVE_TO, if any */
		segment_start = i > 0 ? i - 2 : 0;

		/* Scan up to the end of the current segment */
		for (; i < path->num_data; ++i) {
			switch (path->data[i].header.type) {
			case CAIRO_PATH_MOVE_TO:
				--i;
				break;
			case CAIRO_PATH_CLOSE_PATH:
				break;
			case CAIRO_PATH_LINE_TO:
				++i;
				continue;
			case CAIRO_PATH_CURVE_TO:
				i += 3;
				continue;
			default:
				segment->status = CAIRO_STATUS_INVALID_PATH_DATA;
				return 0;
			}
			break;
		}

		/* Force recycling on looking for the last segment */
		if (which == CPML_LAST) {
			segment->status = CAIRO_STATUS_SUCCESS;
			segment->data = path->data + segment_start;
			segment->num_data = i - segment_start;
			continue;
		}
	} while (i < path->num_data && --which);

	segment->status = CAIRO_STATUS_SUCCESS;
	segment->data = path->data + segment_start;
	segment->num_data = i - segment_start;
	return 1;
}

cairo_bool_t
cpml_primitive_set_from_segment(CpmlPrimitive		*primitive,
				const CpmlSegment	*segment,
				int			 which)
{
	int i, p, length;
	CpmlPair from;

	if (primitive == NULL || segment == NULL || segment->num_data <= 0)
		return 0;

	from.x = 0.;
	from.y = 0.;

	for (i = 0; i < segment->num_data; ++i) {
		primitive->type = segment->data[i].header.type;
		primitive->p[0].x = from.x;
		primitive->p[0].y = from.y;

		length = segment->data[i].header.length;

		for (p = 1; p < length; ++p) {
			++i;
			from.x = segment->data[i].point.x;
			from.y = segment->data[i].point.y;

			primitive->p[p].x = from.x;
			primitive->p[p].y = from.y;
		}

		if (primitive->type == CAIRO_PATH_MOVE_TO) {
		    /* MOVE_TO is not considered as a primitive */
		} else if (which != CPML_LAST && --which == 0) {
			return 1;
		}
	}

	return which == CPML_LAST;
}

cairo_bool_t
cpml_primitive_invert(CpmlPrimitive *primitive)
{
	CpmlPair tmp;

	switch (primitive->type) {
	case CAIRO_PATH_LINE_TO:
		tmp.x = primitive->p[1].x;
		tmp.y = primitive->p[1].y;
		primitive->p[1].x = primitive->p[0].x;
		primitive->p[1].y = primitive->p[0].y;
		primitive->p[0].x = tmp.x;
		primitive->p[0].y = tmp.y;
		break;
	case CAIRO_PATH_CURVE_TO:
		tmp.x = primitive->p[2].x;
		tmp.y = primitive->p[2].y;
		primitive->p[2].x = primitive->p[1].x;
		primitive->p[2].y = primitive->p[1].y;
		primitive->p[1].x = tmp.x;
		primitive->p[1].y = tmp.y;
		tmp.x = primitive->p[3].x;
		tmp.y = primitive->p[3].y;
		primitive->p[3].x = primitive->p[0].x;
		primitive->p[3].y = primitive->p[0].y;
		break;
	default:
		return 0;
	}

	primitive->p[0].x = tmp.x;
	primitive->p[0].y = tmp.y;
	return 1;
}
