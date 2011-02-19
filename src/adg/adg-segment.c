/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * @Section_Id:AdgSegment
 * @title: AdgSegment
 * @short_description: A wrapper for #CpmlSegment
 *
 * AdgSegment is a wrapper in #GType syntax of the #CpmlSegment struct.
 * Furthermore, some dynamic memory functions are provided, such as
 * shallow and deep duplication functions.
 **/

/**
 * AdgSegment:
 *
 * Another name for #CpmlSegment: check its documentation for the
 * fields description and visibility details.
 **/


#include "adg-internal.h"
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
 * Returns: a shallow duplicate of @segment: must be freed
 *          with g_free() when no longer needed.
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
 * Returns: a deep duplicate of @segment: must be freed
 *          with g_free() when no longer needed.
 **/
AdgSegment *
adg_segment_deep_dup(const AdgSegment *segment)
{
    AdgSegment *dest;
    int num_data;
    gsize segment_size, data_size;
    cairo_path_data_t *p_data;

    g_return_val_if_fail(segment != NULL, NULL);

    num_data = segment->num_data;
    segment_size = sizeof(AdgSegment);
    data_size = segment->data ? sizeof(cairo_path_data_t) * num_data : 0;
    dest = (AdgSegment *) g_malloc(segment_size + data_size);
    p_data = (cairo_path_data_t *) ((gchar *) dest + segment_size);

    dest->path = NULL;

    if (data_size > 0) {
        dest->data = memcpy(p_data, segment->data, data_size);
        dest->num_data = num_data;
    } else {
        dest->data = NULL;
        dest->num_data = 0;
    }

    return dest;
}

/**
 * adg_segment_deep_copy:
 * @segment: an #AdgSegment structure
 * @src: the source segment to copy
 *
 * Makes a deep copy of @src to @segment. For a shallow copy, check out
 * the cpml_segment_copy() API provided by the CPML library.
 *
 * This could seem a somewhat unusual operation because @segment should
 * be "compatible" with @src: it is expected that they have the same
 * <structfield>num_data</structfield> value. Anyway, it is convenient
 * in some situation, such as when restoring the original data from a
 * deep duplicated source:
 *
 * |[
 * AdgSegment *backup;
 *
 * backup = adg_segment_deep_dup(&segment);
 * // Now &segment can be modified
 * ...
 * adg_segment_deep_copy(&segment, backup);
 * g_free(backup);
 * ]|
 *
 * The struct fields of @segment are left untouched and used only to
 * check if it is compatible with @src.
 *
 * Returns: @segment
 **/
AdgSegment *
adg_segment_deep_copy(AdgSegment *segment, const AdgSegment *src)
{
    size_t n;

    g_return_val_if_fail(segment != NULL, segment);
    g_return_val_if_fail(src != NULL, segment);
    g_return_val_if_fail(segment->num_data == src->num_data, segment);

    if (src->num_data <= 0)
        return segment;

    n = sizeof(cairo_path_data_t) * segment->num_data;

    memcpy(segment->data, src->data, n);

    return segment;
}
