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


#if !defined(__CPML_H__)
#error "Only <cpml/cpml.h> can be included directly."
#endif


#ifndef __CPML_CURVE_H__
#define __CPML_CURVE_H__

typedef enum {
    CPML_CURVE_OFFSET_ALGORITHM_NONE,
    CPML_CURVE_OFFSET_ALGORITHM_DEFAULT,
    CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL,
    CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT,
    CPML_CURVE_OFFSET_ALGORITHM_BAIOCA,
} CpmlCurveOffsetAlgorithm;

CAIRO_BEGIN_DECLS

CpmlCurveOffsetAlgorithm
        cpml_curve_offset_algorithm     (CpmlCurveOffsetAlgorithm new_algorithm);
void    cpml_curve_put_pair_at_time     (const CpmlPrimitive     *curve,
                                         double                   t,
                                         CpmlPair                *pair);
void    cpml_curve_put_vector_at_time   (const CpmlPrimitive     *curve,
                                         double                   t,
                                         CpmlVector              *vector);
void    cpml_curve_put_offset_at_time   (const CpmlPrimitive     *curve,
                                         double                   t,
                                         double                   offset,
                                         CpmlPair                *pair);

CAIRO_END_DECLS


#endif /* __CPML_CURVE_H__ */
