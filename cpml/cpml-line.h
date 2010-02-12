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


#ifndef __CPML_LINE_H__
#define __CPML_LINE_H__


CAIRO_BEGIN_DECLS

void    cpml_line_put_pair_at           (const CpmlPrimitive    *line,
                                         double                  pos,
                                         CpmlPair               *pair);
void    cpml_line_put_vector_at         (const CpmlPrimitive    *line,
                                         double                  pos,
                                         CpmlVector             *vector);
double  cpml_line_get_closest_pos       (const CpmlPrimitive    *line,
                                         const CpmlPair         *pair);
int     cpml_line_put_intersections     (const CpmlPrimitive    *line,
                                         const CpmlPrimitive    *line2,
                                         int                     max,
                                         CpmlPair               *dest);
void    cpml_line_offset                (CpmlPrimitive          *line,
                                         double                  offset);

CAIRO_END_DECLS


#endif /* __CPML_LINE_H__ */
