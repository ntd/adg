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


#ifndef __CPML_ARC_H__
#define __CPML_ARC_H__


CAIRO_BEGIN_DECLS

int     cpml_arc_type_get_npoints       (void) CPML_GNUC_CONST;
cairo_bool_t
        cpml_arc_info                   (const CpmlPrimitive    *arc,
                                         CpmlPair               *center,
                                         double                 *r,
                                         double                 *start,
                                         double                 *end);
double  cpml_arc_get_length             (const CpmlPrimitive    *arc);
void    cpml_arc_put_extents            (const CpmlPrimitive    *arc,
                                         CpmlExtents            *extents);
void    cpml_arc_put_pair_at            (const CpmlPrimitive    *arc,
                                         double                  pos,
                                         CpmlPair               *pair);
void    cpml_arc_put_vector_at          (const CpmlPrimitive    *arc,
                                         double                  pos,
                                         CpmlVector             *vector);
double  cpml_arc_get_closest_pos        (const CpmlPrimitive    *arc,
                                         const CpmlPair         *pair);
int     cpml_arc_put_intersections      (const CpmlPrimitive    *arc,
                                         const CpmlPrimitive    *arc2,
                                         int                     max,
                                         CpmlPair               *dest);
int     cpml_arc_put_intersections_with_line
                                        (const CpmlPrimitive    *arc,
                                         const CpmlPrimitive    *line,
                                         int                     max,
                                         CpmlPair               *dest);
void    cpml_arc_offset                 (CpmlPrimitive          *arc,
                                         double                  offset);
void    cpml_arc_to_cairo               (const CpmlPrimitive    *arc,
                                         cairo_t                *cr);
void    cpml_arc_to_curves              (const CpmlPrimitive    *arc,
                                         CpmlSegment            *segment,
                                         int                     n_curves);

CAIRO_END_DECLS


#endif /* __CPML_ARC_H__ */
