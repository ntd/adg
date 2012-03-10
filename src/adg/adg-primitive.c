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
 * SECTION:adg-primitive
 * @Section_Id:AdgPrimitive
 * @title: AdgPrimitive
 * @short_description: A wrapper for #CpmlPrimitive
 *
 * AdgPrimitive is a wrapper in #GType syntax of the #CpmlPrimitive struct.
 *
 * Since: 1.0
 **/

/**
 * AdgPrimitive:
 *
 * Another name for #CpmlPrimitive: check its documentation for the
 * fields description and visibility details.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <string.h>


GType
adg_primitive_get_type(void)
{
    static int primitive_type = 0;

    if (G_UNLIKELY(primitive_type == 0))
        primitive_type = g_boxed_type_register_static("AdgPrimitive",
                                                      (GBoxedCopyFunc) adg_primitive_dup,
                                                      g_free);

    return primitive_type;
}

/**
 * adg_primitive_dup:
 * @primitive: an #AdgPrimitive structure
 *
 * Duplicates @primitive. This function makes a shallow duplication of
 * @primitives, that is the internal pointers of the resulting primitive
 * struct refer to the same memory as the original @primitive. Check
 * out adg_primitive_deep_dup() if it is required also the content
 * duplication.
 *
 * Returns: (transfer full): a shallow duplicate of @primitive: must be
 *                           freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
AdgPrimitive *
adg_primitive_dup(const AdgPrimitive *primitive)
{
    return g_memdup(primitive, sizeof(AdgPrimitive));
}

/**
 * adg_primitive_deep_dup:
 * @primitive: an #AdgPrimitive structure
 *
 * Duplicates @primitive. This function makes a deep duplication of
 * @primitive, that is it duplicates also the definition data (both
 * <structfield>org</structfield> and <structfield>data</structfield>).
 *
 * Furthermore, the new <structfield>segment</structfield> field will
 * point to a fake duplicated segment with only its first primitive
 * set (the first primitive of a segment should be a #CPML_MOVE).
 * This is needed in order to let a #CPML_CLOSE work as expected.
 *
 * All the data is allocated in the same chunk of memory so freeing
 * the returned pointer releases all the occupied memory.
 *
 * Returns: (transfer full): a deep duplicate of @primitive: must be
 *                           freed with g_free() when no longer needed
 *
 * Since: 1.0
 **/
AdgPrimitive *
adg_primitive_deep_dup(const AdgPrimitive *primitive)
{
    const AdgPrimitive *src;
    AdgPrimitive *dst;
    gsize primitive_size, org_size, data_size, segment_size;
    gchar *ptr;

    src = primitive;
    primitive_size = sizeof(AdgPrimitive);

    if (src->org != NULL)
        org_size = sizeof(cairo_path_data_t);
    else
        org_size = 0;

    if (src->data != NULL)
        data_size = sizeof(cairo_path_data_t) * src->data->header.length;
    else
        data_size = 0;

    if (src->segment != NULL && src->segment->data != NULL)
        segment_size = sizeof(CpmlSegment) +
            sizeof(cairo_path_data_t) * src->segment->data[0].header.length;
    else
        segment_size = 0;

    dst = g_malloc(primitive_size + org_size + data_size + segment_size);
    ptr = (gchar *) dst + primitive_size;

    if (org_size > 0) {
        dst->org = memcpy(ptr, src->org, org_size);
        ptr += org_size;
    } else {
        dst->org = NULL;
    }

    if (data_size > 0) {
        dst->data = memcpy(ptr, src->data, data_size);
        ptr += data_size;
    } else {
        dst->data = NULL;
    }

    if (segment_size > 0) {
        dst->segment = memcpy(ptr, src->segment, sizeof(CpmlSegment));
        ptr += sizeof(CpmlSegment);
        dst->segment->data = memcpy(ptr, src->segment->data,
                                    sizeof(cairo_path_data_t) *
                                    src->segment->data[0].header.length);
    } else {
        dst->segment = NULL;
    }

    return dst;
}
