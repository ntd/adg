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


#ifndef __ADG_MATRIX_FALLBACK_H__
#define __ADG_MATRIX_FALLBACK_H__


G_BEGIN_DECLS


/* Backward compatibility: old cairo releases did not wrap cairo_matrix_t */

#ifndef CAIRO_GOBJECT_TYPE_MATRIX

#define ADG_MISSING_GBOXED_MATRIX       1
#define CAIRO_GOBJECT_TYPE_MATRIX       (cairo_gobject_matrix_get_type())

GType           cairo_gobject_matrix_get_type
                                        (void) G_GNUC_CONST;
cairo_matrix_t *cairo_gobject_cairo_matrix_copy
                                        (const cairo_matrix_t   *matrix);

#endif /* CAIRO_GOBJECT_TYPE_MATRIX */


G_END_DECLS


#endif /* __ADG_MATRIX_FALLBACK_H__ */
