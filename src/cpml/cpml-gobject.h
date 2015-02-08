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


#if !defined(__CPML_H__)
#error "Only <cpml/cpml.h> can be included directly."
#endif


#ifndef __CPML_GOBJECT_H__
#define __CPML_GOBJECT_H__


G_BEGIN_DECLS

#define         CPML_TYPE_PAIR              (cpml_pair_get_type())
GType           cpml_pair_get_type          (void);
CpmlPair *      cpml_pair_dup               (const CpmlPair         *pair);

#define         CPML_TYPE_PRIMITIVE_TYPE    (cpml_primitive_type_get_type())
GType           cpml_primitive_type_get_type(void);

#define         CPML_TYPE_PRIMITIVE         (cpml_primitive_get_type())
GType           cpml_primitive_get_type     (void);
CpmlPrimitive * cpml_primitive_dup          (const CpmlPrimitive    *primitive);
CpmlPrimitive * cpml_primitive_deep_dup     (const CpmlPrimitive    *primitive);

#define         CPML_TYPE_SEGMENT           (cpml_segment_get_type())
GType           cpml_segment_get_type       (void);
CpmlSegment *   cpml_segment_dup            (const CpmlSegment      *segment);
CpmlSegment *   cpml_segment_deep_dup       (const CpmlSegment      *segment);

#define         CPML_TYPE_CURVE_OFFSET_ALGORITHM \
                                            (cpml_curve_offset_algorithm_get_type())
GType           cpml_curve_offset_algorithm_get_type
                                            (void);

G_END_DECLS


#endif /* __CPML_GOBJECT_H__ */
