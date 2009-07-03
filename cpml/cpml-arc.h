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

#ifndef __CPML_ARC_H__
#define __CPML_ARC_H__

#include <cpml/cpml-primitive.h>
#include <cpml/cpml-macros.h>

#ifndef CAIRO_PATH_ARC_TO
#define CAIRO_PATH_ARC_TO       100
#undef  CAIRO_HAS_ARC_SUPPORT
#else
#define CAIRO_HAS_ARC_SUPPORT   1
#endif


CAIRO_BEGIN_DECLS

int     cpml_arc_type_get_npoints       (void) CPML_GNUC_CONST;
cairo_bool_t
        cpml_arc_info                   (const CpmlPrimitive    *arc,
                                         CpmlPair               *center,
                                         double                 *r,
                                         double                 *start,
                                         double                 *end);
double  cpml_arc_length                 (const CpmlPrimitive    *arc);
void    cpml_arc_pair_at                (const CpmlPrimitive    *arc,
                                         CpmlPair               *pair,
                                         double                  pos);
void    cpml_arc_vector_at              (const CpmlPrimitive    *arc,
                                         CpmlVector             *vector,
                                         double                  pos);
double  cpml_arc_near_pos               (const CpmlPrimitive    *arc,
                                         const CpmlPair         *pair);
int     cpml_arc_intersection           (const CpmlPrimitive    *arc,
                                         const CpmlPrimitive    *arc2,
                                         CpmlPair               *dest,
                                         int                     max);
int     cpml_arc_intersection_with_line (const CpmlPrimitive    *arc,
                                         const CpmlPrimitive    *line,
                                         CpmlPair               *dest,
                                         int                     max);
void    cpml_arc_offset                 (CpmlPrimitive          *arc,
                                         double                  offset);
void    cpml_arc_to_cairo               (const CpmlPrimitive    *arc,
                                         cairo_t                *cr);
void    cpml_arc_to_curves              (const CpmlPrimitive    *arc,
                                         CpmlSegment            *segment,
                                         int                     n_curves);

CAIRO_END_DECLS


#endif /* __CPML_ARC_H__ */
