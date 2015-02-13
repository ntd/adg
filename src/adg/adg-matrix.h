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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_MATRIX_H__
#define __ADG_MATRIX_H__


G_BEGIN_DECLS

const cairo_matrix_t *
                adg_matrix_identity     (void);
const cairo_matrix_t *
                adg_matrix_null         (void);
void            adg_matrix_copy         (cairo_matrix_t         *matrix,
                                         const cairo_matrix_t   *src);
cairo_matrix_t *adg_matrix_dup          (const cairo_matrix_t   *matrix);
gboolean        adg_matrix_equal        (const cairo_matrix_t   *matrix1,
                                         const cairo_matrix_t   *matrix2);
gboolean        adg_matrix_normalize    (cairo_matrix_t         *matrix);
void            adg_matrix_transform    (cairo_matrix_t         *matrix,
                                         const cairo_matrix_t   *transformation,
                                         AdgTransformMode        mode);
void            adg_matrix_dump         (const cairo_matrix_t   *matrix);

G_END_DECLS


#endif /* __ADG_MATRIX_H__ */
