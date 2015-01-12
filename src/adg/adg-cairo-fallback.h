/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_CAIRO_FALLBACK_H__
#define __ADG_CAIRO_FALLBACK_H__


G_BEGIN_DECLS


/* Provide some GObject wrappers around cairo_matrix_t and cairo_pattern_t
 * if cairo-gobject support has not been provided by cairo itself or
 * if the cairo version is too old to provide them. */


#ifndef CAIRO_GOBJECT_TYPE_MATRIX

#define ADG_MISSING_GBOXED_MATRIX       1
#define CAIRO_GOBJECT_TYPE_MATRIX       (cairo_gobject_matrix_get_type())

GType           cairo_gobject_matrix_get_type
                                        (void);
cairo_matrix_t *cairo_gobject_cairo_matrix_copy
                                        (const cairo_matrix_t   *matrix);

#endif /* CAIRO_GOBJECT_TYPE_MATRIX */


#ifndef CAIRO_GOBJECT_TYPE_PATTERN

#define ADG_MISSING_GBOXED_PATTERN          1
#define CAIRO_GOBJECT_TYPE_PATTERN          (cairo_gobject_pattern_get_type())

GType   cairo_gobject_pattern_get_type      (void);
cairo_pattern_t *
        cairo_gobject_cairo_pattern_copy    (const cairo_pattern_t  *pattern);

#endif /* CAIRO_GOBJECT_TYPE_PATTERN */


G_END_DECLS


#endif /* __ADG_CAIRO_FALLBACK_H__ */
