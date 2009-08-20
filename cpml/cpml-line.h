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

#ifndef __CPML_LINE_H__
#define __CPML_LINE_H__

#include <cpml/cpml-primitive.h>
#include <cpml/cpml-macros.h>


CAIRO_BEGIN_DECLS

int     cpml_line_type_get_npoints      (void) CPML_GNUC_CONST;
double  cpml_line_length                (const CpmlPrimitive    *line);
void    cpml_line_pair_at               (const CpmlPrimitive    *line,
                                         CpmlPair               *pair,
                                         double                  pos);
void    cpml_line_vector_at             (const CpmlPrimitive    *line,
                                         CpmlVector             *vector,
                                         double                  pos);
double  cpml_line_near_pos              (const CpmlPrimitive    *line,
                                         const CpmlPair         *pair);
int     cpml_line_intersection          (const CpmlPrimitive    *line,
                                         const CpmlPrimitive    *line2,
                                         CpmlPair               *dest,
                                         int                     max);
void    cpml_line_offset                (CpmlPrimitive          *line,
                                         double                  offset);

CAIRO_END_DECLS


#endif /* __CPML_LINE_H__ */
