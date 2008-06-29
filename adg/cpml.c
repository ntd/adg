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


cairo_bool_t
cpml_segment_get_from_path(cairo_path_t		*segment,
			   const cairo_path_t	*path,
			   int			 index)
{
	cairo_path_data_t *path_data;
	int i, start_data;

	path_data = path->data;
	i = 0;

	do {
		if (path_data->header.type == CAIRO_PATH_MOVE_TO) {
			/* Skip the leading MOVE_TO primitives */
			do {
				i += 2;

				/* No more segments to analyze */
				if (i >= path->num_data)
					return 0;
			} while (path_data[i].header.type == CAIRO_PATH_MOVE_TO);

			/* Include the last MOVE_TO */
			start_data = i - 2;
		} else {
			start_data = 0;
		}

		for (; i < path->num_data; ++i) {
			switch (path_data[i].header.type) {
			case CAIRO_PATH_MOVE_TO:
				--i;
				break;
			case CAIRO_PATH_CLOSE_PATH:
				break;
			case CAIRO_PATH_LINE_TO:
				i += 1;
				continue;
			case CAIRO_PATH_CURVE_TO:
				i += 3;
				continue;
			default:
				return 0;
			}
			break;
		}
	} while (-- index);

	segment->status = CAIRO_STATUS_SUCCESS;
	segment->data = path_data + start_data;
	segment->num_data = i - start_data;
	return 1;
}
