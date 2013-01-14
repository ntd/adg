/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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

typedef enum {
    CPML_MOVE = CAIRO_PATH_MOVE_TO,
    CPML_LINE = CAIRO_PATH_LINE_TO,
    CPML_CURVE = CAIRO_PATH_CURVE_TO,
    CPML_CLOSE = CAIRO_PATH_CLOSE_PATH,
    CPML_ARC = 88
} CpmlPrimitiveType;

struct _CpmlPrimitive {
    /*< public >*/
    CpmlSegment       *segment;
    cairo_path_data_t *org;
    cairo_path_data_t *data;
};

size_t          cpml_primitive_type_get_n_points
                                           (CpmlPrimitiveType    type);
void            cpml_primitive_from_segment(CpmlPrimitive       *primitive,
                                            CpmlSegment         *segment);
void            cpml_primitive_copy        (CpmlPrimitive       *primitive,
                                            const CpmlPrimitive *src);
void            cpml_primitive_reset       (CpmlPrimitive       *primitive);
int             cpml_primitive_next        (CpmlPrimitive       *primitive);
size_t          cpml_primitive_get_n_points(const CpmlPrimitive *primitive);
double          cpml_primitive_get_length  (const CpmlPrimitive *primitive);
void            cpml_primitive_put_extents (const CpmlPrimitive *primitive,
                                            CpmlExtents         *extents);
void            cpml_primitive_set_point   (CpmlPrimitive       *primitive,
                                            int                  n_point,
                                            const CpmlPair      *pair);
void            cpml_primitive_put_point   (const CpmlPrimitive *primitive,
                                            int                  n_point,
                                            CpmlPair            *pair);
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
size_t          cpml_primitive_put_intersections
                                           (const CpmlPrimitive *primitive,
                                            const CpmlPrimitive *primitive2,
                                            size_t               n_dest,
                                            CpmlPair            *dest);
size_t          cpml_primitive_put_intersections_with_segment
                                           (const CpmlPrimitive *primitive,
                                            const CpmlSegment   *segment,
                                            size_t               n_dest,
                                            CpmlPair            *dest);
void            cpml_primitive_offset      (CpmlPrimitive       *primitive,
                                            double               offset);
int             cpml_primitive_join        (CpmlPrimitive       *primitive,
                                            CpmlPrimitive       *primitive2);
void            cpml_primitive_to_cairo    (const CpmlPrimitive *primitive,
                                            cairo_t             *cr);
void            cpml_primitive_dump        (const CpmlPrimitive *primitive,
                                            int                  org_also);

CAIRO_END_DECLS


#endif /* __CPML_PRIMITIVE_H__ */
