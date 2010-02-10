/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, 2009  Nicola Fontana <ntd at entidi.it>
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


#if !defined(__CPML_H__)
#error "Only <cpml/cpml.h> can be included directly."
#endif


#ifndef __CPML_PAIR_H__
#define __CPML_PAIR_H__


CAIRO_BEGIN_DECLS

typedef struct _CpmlPair CpmlPair;
typedef struct _CpmlPair CpmlVector;

struct _CpmlPair {
    double  x, y;
};


void            cpml_pair_copy                  (CpmlPair       *pair,
                                                 const CpmlPair *src);
void            cpml_pair_from_cairo            (CpmlPair       *pair,
                                                 const cairo_path_data_t
                                                                *path_data);

void            cpml_pair_negate                (CpmlPair       *pair);
cairo_bool_t    cpml_pair_invert                (CpmlPair       *pair);
void            cpml_pair_add                   (CpmlPair       *pair,
                                                 const CpmlPair *src);
void            cpml_pair_sub                   (CpmlPair       *pair,
                                                 const CpmlPair *src);
void            cpml_pair_mul                   (CpmlPair       *pair,
                                                 const CpmlPair *src);
cairo_bool_t    cpml_pair_div                   (CpmlPair       *pair,
                                                 const CpmlPair *src);
void            cpml_pair_transform             (CpmlPair       *pair,
                                                 const cairo_matrix_t
                                                                *matrix);
double          cpml_pair_squared_distance      (const CpmlPair *from,
                                                 const CpmlPair *to);
double          cpml_pair_distance              (const CpmlPair *from,
                                                 const CpmlPair *to);
void            cpml_pair_to_cairo              (const CpmlPair *pair,
                                                 cairo_path_data_t
                                                                *path_data);

void            cpml_vector_from_angle          (CpmlVector     *vector,
                                                 double          angle);

void            cpml_vector_set_length          (CpmlVector     *vector,
                                                 double          length);
double          cpml_vector_angle               (const CpmlVector
                                                                *vector);
void            cpml_vector_normal              (CpmlVector     *vector);
void            cpml_vector_transform           (CpmlVector     *vector,
                                                 const cairo_matrix_t
                                                                *matrix);


CAIRO_END_DECLS


#endif /* __CPML_PAIR_H__ */
