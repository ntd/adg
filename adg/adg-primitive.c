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
 * SECTION:adg-primitive
 * @Section_Id:AdgPrimitive-GBoxed
 * @title: AdgPrimitive
 * @short_description: A wrapper for #CpmlPrimitive
 *
 * AdgPrimitive is a wrapper in #GType syntax of the #CpmlPrimitive struct.
 **/

/**
 * AdgPrimitive:
 *
 * Another name for #CpmlPrimitive: check its documentation for the
 * fields description and visibility details.
 **/


#include "adg-primitive.h"

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
 * Returns: a shallow duplicate of @primitive: must be freed
 *          with g_free() when no longer needed.
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
 * The <structfield>segment</structfield> field instead is set to
 * %NULL as the parent segment is not duplicated.
 *
 * All the data is allocated in the same chunk of memory so freeing
 * the returned pointer releases all the occupied memory.
 *
 * Returns: a deep duplicate of @primitive: must be freed
 *          with g_free() when no longer needed.
 **/
AdgPrimitive *
adg_primitive_deep_dup(const AdgPrimitive *primitive)
{
    AdgPrimitive *dest;
    gsize primitive_size, org_size, data_size;
    cairo_path_data_t *p_data;

    primitive_size = sizeof(AdgPrimitive);
    org_size = primitive->org == NULL ? 0 : sizeof(cairo_path_data_t);
    data_size = primitive->data == NULL ?
        0 : sizeof(cairo_path_data_t) * primitive->data->header.length;
    dest = (AdgPrimitive *) g_malloc(primitive_size + org_size + data_size);
    p_data = (cairo_path_data_t *) ((gchar *) dest + primitive_size);

    dest->segment = NULL;

    if (primitive->org != NULL) {
        dest->org = p_data;
        memcpy(p_data, primitive->org, org_size);
        ++ p_data;
    } else {
        dest->org = NULL;
    }

    if (data_size > 0)
        dest->data = memcpy(p_data, primitive->data, data_size);
    else
        dest->data = NULL;

    return dest;
}
