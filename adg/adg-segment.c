/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-segment
 * @title: AdgSegment
 * @short_description: A GObject based wrapper for the #CpmlSegment struct
 *
 * The AdgSegment is a wrapper typedef in #GType syntax of the
 * #CpmlSegment struct. Furthermore, some dynamic memory functions
 * are provided, such as shallow and deep duplication, as the CPML
 * library does not have dynamic memory APIs.
 **/

/**
 * AdgSegment:
 *
 * Another name for the #CpmlSegment type: check its documentation
 * for fields description.
 **/


#include "adg-segment.h"

#include <string.h>


GType
adg_segment_get_type(void)
{
    static int segment_type = 0;

    if (G_UNLIKELY(segment_type == 0))
        segment_type = g_boxed_type_register_static("AdgSegment",
                                                    (GBoxedCopyFunc) adg_segment_dup,
                                                    g_free);

    return segment_type;
}

/**
 * adg_segment_dup:
 * @segment: an #AdgSegment structure
 *
 * Duplicates @segment. This function makes a shallow duplication,
 * that is the internal pointers of the resulting segment struct
 * refer to the same memory as the original @segment. Check out
 * adg_segment_deep_dup() if it is required also the content
 * duplication.
 *
 * Return value: a shallow duplicate of @segment: must be freed
 *               with g_free() when no longer needed.
 **/
AdgSegment *
adg_segment_dup(const AdgSegment *segment)
{
    return g_memdup(segment, sizeof(AdgSegment));
}

/**
 * adg_segment_deep_dup:
 * @segment: an #AdgSegment structure
 *
 * Duplicates @segment. This function makes a deep duplication,
 * that is it duplicates also the underlying data that defines
 * the segment. The <structfield>path</structfield> field
 * is set to %NULL as <structfield>data</structfield> is no
 * more referring to the original cairo path.
 *
 * All the data is allocated in the same chunk of memory so freeing
 * the returned pointer releases all the occupied memory.
 *
 * Return value: a deep duplicate of @segment: must be freed
 *               with g_free() when no longer needed.
 **/
AdgSegment *
adg_segment_deep_dup(const AdgSegment *segment)
{
    AdgSegment *dest;
    gsize segment_size, data_size;
    cairo_path_data_t *p_data;

    segment_size = sizeof(AdgSegment);
    data_size = segment->data == NULL ?
        0 : sizeof(cairo_path_data_t) * segment->num_data;
    dest = (AdgSegment *) g_malloc(segment_size + data_size);
    p_data = (cairo_path_data_t *) ((gchar *) dest + segment_size);

    dest->path = NULL;

    if (data_size > 0) {
        dest->data = memcpy(p_data, segment->data, data_size);
        dest->num_data = segment->num_data;
    } else {
        dest->data = NULL;
        dest->num_data = 0;
    }

    return dest;
}
