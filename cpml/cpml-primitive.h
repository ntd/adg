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


#ifndef __CPML_PRIMITIVE_H__
#define __CPML_PRIMITIVE_H__


CAIRO_BEGIN_DECLS

typedef struct _CpmlPrimitive CpmlPrimitive;
typedef cairo_path_data_type_t CpmlPrimitiveType;

struct _CpmlPrimitive {
    CpmlSegment       *segment;
    cairo_path_data_t *org;
    cairo_path_data_t *data;
};


CpmlPrimitive * cpml_primitive_copy        (CpmlPrimitive       *primitive,
                                            const CpmlPrimitive *src);
CpmlPrimitive * cpml_primitive_from_segment(CpmlPrimitive       *primitive,
                                            CpmlSegment         *segment);
void            cpml_primitive_reset       (CpmlPrimitive       *primitive);
cairo_bool_t    cpml_primitive_next        (CpmlPrimitive       *primitive);

int             cpml_primitive_get_n_points(const CpmlPrimitive *primitive);
cairo_path_data_t *
                cpml_primitive_get_point   (const CpmlPrimitive *primitive,
                                            int                  n_point);
void            cpml_primitive_to_cairo    (const CpmlPrimitive *primitive,
                                            cairo_t             *cr);
void            cpml_primitive_dump        (const CpmlPrimitive *primitive,
                                            cairo_bool_t         org_also);
int             cpml_primitive_put_intersections_with_segment
                                           (const CpmlPrimitive *primitive,
                                            const CpmlSegment   *segment,
                                            CpmlPair            *dest,
                                            int                  max);

/* To be implemented by the primitives */
int             cpml_primitive_type_get_n_points
                                           (CpmlPrimitiveType    type);
double          cpml_primitive_get_length  (const CpmlPrimitive *primitive);
void            cpml_primitive_put_extents (const CpmlPrimitive *primitive,
                                            CpmlExtents         *extents);
void            cpml_primitive_put_pair_at (const CpmlPrimitive *primitive,
                                            double               pos,
                                            CpmlPair            *pair);
void            cpml_primitive_put_vector_at
                                           (const CpmlPrimitive *primitive,
                                            double               pos,
                                            CpmlVector          *vector);
double          cpml_primitive_get_closest_pos
                                           (const CpmlPrimitive *primitive,
                                            const CpmlPair      *pair);
cairo_bool_t    cpml_primitive_join        (CpmlPrimitive       *primitive,
                                            CpmlPrimitive       *primitive2);
int             cpml_primitive_put_intersections
                                           (const CpmlPrimitive *primitive,
                                            const CpmlPrimitive *primitive2,
                                            int                  max,
                                            CpmlPair            *dest);
void            cpml_primitive_offset      (CpmlPrimitive       *primitive,
                                            double               offset);

CAIRO_END_DECLS


#endif /* __CPML_PRIMITIVE_H__ */
