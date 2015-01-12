/* CPML - Cairo Path Manipulation Library
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


#ifndef __CPML_PRIMITIVE_PRIVATE_H__
#define __CPML_PRIMITIVE_PRIVATE_H__


CAIRO_BEGIN_DECLS

/**
 * _CpmlPrimitiveClass:
 * @name:              descriptive name of the primitive type. This name
 *                     will be used for debugging purpose and while
 *                     dumping the primitive data.
 * @n_points:          exact number of points needed to properly define
 *                     a primitive of this class type.
 * @get_length:        gets the length of a primitive.
 * @put_extents:       gets the bounding box of a primitive.
 * @put_pair_at:       gets the coordinates of a point on the primitive at
 *                     a given factor.
 * @put_vector_at:     gets the vector of a point on the primitive at a
 *                     given factor.
 * @get_closest_pos:   gets the factor of the point on a primitive closest
 *                     to another given point.
 * @put_intersections: gets the intersection points between a primitive of
 *                     this type and a primitive of any type. The number
 *                     of returned intersections can be explicitely limited.
 * @offset:            creates a new primitive of tha same type parallel
 *                     to the original one with a given distance.
 * @join:              join two primitives (the first one of this class type)
 *                     by modifying the end point of the first one and the
 *                     start point of the second one.
 *
 * Any primitive type must implement an instance of this class as a
 * global variable. This will abstract the primitives and allows to
 * access them throught the cpml_primitive_...() APIs.
 */
typedef struct __CpmlPrimitiveClass _CpmlPrimitiveClass;

struct __CpmlPrimitiveClass {
    const char   *name;
    size_t        n_points;

    double       (*get_length)          (const CpmlPrimitive    *primitive);
    void         (*put_extents)         (const CpmlPrimitive    *primitive,
                                         CpmlExtents            *extents);
    void         (*put_pair_at)         (const CpmlPrimitive    *primitive,
                                         double                  pos,
                                         CpmlPair               *pair);
    void         (*put_vector_at)       (const CpmlPrimitive    *primitive,
                                         double                  pos,
                                         CpmlVector             *vector);
    double       (*get_closest_pos)     (const CpmlPrimitive    *primitive,
                                         const CpmlPair         *pair);
    size_t       (*put_intersections)   (const CpmlPrimitive    *primitive,
                                         const CpmlPrimitive    *primitive2,
                                         size_t                  n_dest,
                                         CpmlPair               *dest);
    void         (*offset)              (CpmlPrimitive          *primitive,
                                         double                  offset);
    int          (*join)                (CpmlPrimitive          *primitive,
                                         CpmlPrimitive          *primitive2);
};


const _CpmlPrimitiveClass * _cpml_line_get_class  (void) CPML_GNUC_CONST;
const _CpmlPrimitiveClass * _cpml_arc_get_class   (void) CPML_GNUC_CONST;
const _CpmlPrimitiveClass * _cpml_curve_get_class (void) CPML_GNUC_CONST;
const _CpmlPrimitiveClass * _cpml_close_get_class (void) CPML_GNUC_CONST;


CAIRO_END_DECLS


#endif /* __CPML_PRIMITIVE_PRIVATE_H__ */
