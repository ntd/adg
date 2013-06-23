/* ADG - Automatic Drawing Generation
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
 * SECTION:adg-cairo-fallback
 * @title: Cairo wrappers
 * @short_description: GObject wrappers for cairo structs
 *
 * If GObject support has not been compiled in cairo, either by
 * explicitely disabling it or because the installed cairo version
 * does not provide them, a compatible selection of wrappers used
 * by ADG is provided anyway.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <string.h>
#include <math.h>

#include "adg-cairo-fallback.h"


#ifdef ADG_MISSING_GBOXED_MATRIX

GType
cairo_gobject_matrix_get_type(void)
{
    static GType matrix_type = 0;

    if (G_UNLIKELY(matrix_type == 0))
        matrix_type = g_boxed_type_register_static("CairoMatrix",
                                                   (GBoxedCopyFunc) cairo_gobject_cairo_matrix_copy,
                                                   g_free);

    return matrix_type;
}

cairo_matrix_t *
cairo_gobject_cairo_matrix_copy(const cairo_matrix_t *matrix)
{
    return g_memdup(matrix, sizeof(cairo_matrix_t));
}

#endif /* ADG_MISSING_GBOXED_MATRIX */


#ifdef ADG_MISSING_GBOXED_PATTERN

GType
cairo_gobject_pattern_get_type(void)
{
    static GType pattern_type = 0;

    if (G_UNLIKELY(pattern_type == 0))
        pattern_type = g_boxed_type_register_static("CairoPattern",
                                                    (GBoxedCopyFunc) cairo_pattern_reference,
                                                    (GBoxedFreeFunc) cairo_pattern_destroy);

    return pattern_type;
}

#endif /* ADG_MISSING_GBOXED_PATTERN */
