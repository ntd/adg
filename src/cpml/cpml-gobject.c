/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-gobject
 * @Section_Id:GObject
 * @title: GObject wrappers
 * @short_description: Collection of boxed wrappers for CPML structs
 *
 * These wrappers are supposed to make bindings development easier.
 * This file defines the wrappers and the functions needed for
 * implementing a #GBoxed type, most notably the #GBoxedCopyFunc
 * requested by g_boxed_type_register_static() (the #GBoxedFreeFunc
 * will usually be g_free()).
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"

#include <glib-object.h>
#include <string.h>

#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"

#include "cpml-gobject.h"


GType
cpml_pair_get_type(void)
{
    static GType pair_type = 0;

    if (G_UNLIKELY(pair_type == 0))
        pair_type = g_boxed_type_register_static("CpmlPair",
                                                 (GBoxedCopyFunc) cpml_pair_dup,
                                                 g_free);

    return pair_type;
}

/**
 * cpml_pair_dup:
 * @pair: an #CpmlPair structure
 *
 * Duplicates @pair.
 *
 * Returns: (transfer full): the duplicate of @pair: must be freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
CpmlPair *
cpml_pair_dup(const CpmlPair *pair)
{
    /* g_memdup() returns NULL if pair is NULL */
    return g_memdup(pair, sizeof(CpmlPair));
}


GType
cpml_primitive_get_type(void)
{
    static GType primitive_type = 0;

    if (G_UNLIKELY(primitive_type == 0))
        primitive_type = g_boxed_type_register_static("CpmlPrimitive",
                                                      (GBoxedCopyFunc) cpml_primitive_dup,
                                                      g_free);

    return primitive_type;
}

/**
 * cpml_primitive_dup:
 * @primitive: an #CpmlPrimitive structure
 *
 * Duplicates @primitive. This function makes a shallow duplication of
 * @primitives, that is the internal pointers of the resulting primitive
 * struct refer to the same memory as the original @primitive. Check
 * out cpml_primitive_deep_dup() if it is required also the content
 * duplication.
 *
 * Returns: (transfer full): a shallow duplicate of @primitive: must be
 *                           freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
CpmlPrimitive *
cpml_primitive_dup(const CpmlPrimitive *primitive)
{
    return g_memdup(primitive, sizeof(CpmlPrimitive));
}

/**
 * cpml_primitive_deep_dup:
 * @primitive: an #CpmlPrimitive structure
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
CpmlPrimitive *
cpml_primitive_deep_dup(const CpmlPrimitive *primitive)
{
    const CpmlPrimitive *src;
    CpmlPrimitive *dst;
    gsize primitive_size, org_size, data_size, segment_size;
    gchar *ptr;

    src = primitive;
    primitive_size = sizeof(CpmlPrimitive);

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


GType
cpml_segment_get_type(void)
{
    static GType segment_type = 0;

    if (G_UNLIKELY(segment_type == 0))
        segment_type = g_boxed_type_register_static("CpmlSegment",
                                                    (GBoxedCopyFunc) cpml_segment_dup,
                                                    g_free);

    return segment_type;
}

/**
 * cpml_segment_dup:
 * @segment: an #CpmlSegment structure
 *
 * Duplicates @segment. This function makes a shallow duplication,
 * that is the internal pointers of the resulting segment struct
 * refer to the same memory as the original @segment. Check out
 * cpml_segment_deep_dup() if it is required also the content
 * duplication.
 *
 * Returns: (transfer full): a shallow duplicate of @segment: must be freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
CpmlSegment *
cpml_segment_dup(const CpmlSegment *segment)
{
    return g_memdup(segment, sizeof(CpmlSegment));
}

/**
 * cpml_segment_deep_dup:
 * @segment: an #CpmlSegment structure
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
 * Returns: (transfer full): a deep duplicate of @segment: must be freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
CpmlSegment *
cpml_segment_deep_dup(const CpmlSegment *segment)
{
    CpmlSegment *dest;
    int num_data;
    gsize segment_size, data_size;
    cairo_path_data_t *p_data;

    g_return_val_if_fail(segment != NULL, NULL);

    num_data = segment->num_data;
    segment_size = sizeof(CpmlSegment);
    data_size = segment->data ? sizeof(cairo_path_data_t) * num_data : 0;
    dest = (CpmlSegment *) g_malloc(segment_size + data_size);
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
 * cpml_segment_deep_copy:
 * @segment: an #CpmlSegment structure
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
 * <informalexample><programlisting language="C">
 * CpmlSegment *backup;
 *
 * backup = cpml_segment_deep_dup(&segment);
 * // Now &segment can be modified
 * ...
 * cpml_segment_deep_copy(&segment, backup);
 * g_free(backup);
 * </programlisting></informalexample>
 *
 * The struct fields of @segment are left untouched and used only to
 * check if it is compatible with @src.
 *
 * Since: 1.0
 **/
void
cpml_segment_deep_copy(CpmlSegment *segment, const CpmlSegment *src)
{
    g_return_if_fail(segment != NULL);
    g_return_if_fail(src != NULL);
    g_return_if_fail(segment->num_data == src->num_data);

    if (src->num_data > 0) {
        size_t n = sizeof(cairo_path_data_t) * segment->num_data;
        memcpy(segment->data, src->data, n);
    }
}
