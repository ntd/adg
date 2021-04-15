/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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


#include "adg-internal.h"
#include <string.h>
#include <math.h>

#include "adg-cairo-fallback.h"


#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 14, 0)

void
cairo_surface_set_device_scale(cairo_surface_t *surface,
                               double x_scale, double y_scale)
{
}

#endif /* cairo < 1.14.0 */


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
    return cpml_memdup(matrix, sizeof(cairo_matrix_t));
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
