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


#ifndef __CPML_EXTENTS_H__
#define __CPML_EXTENTS_H__


CAIRO_BEGIN_DECLS

typedef struct _CpmlExtents CpmlExtents;

struct _CpmlExtents {
    /*< public >*/
    int          is_defined;
    CpmlPair     org;
    CpmlVector   size;
};


void            cpml_extents_copy               (CpmlExtents       *extents,
                                                 const CpmlExtents *src);
void            cpml_extents_from_cairo_text    (CpmlExtents       *extents,
                                                 const cairo_text_extents_t
                                                                   *cairo_extents);
int             cpml_extents_equal              (const CpmlExtents *extents,
                                                 const CpmlExtents *src);
void            cpml_extents_add                (CpmlExtents       *extents,
                                                 const CpmlExtents *src);
void            cpml_extents_pair_add           (CpmlExtents       *extents,
                                                 const CpmlPair    *src);
int             cpml_extents_is_inside          (const CpmlExtents *extents,
                                                 const CpmlExtents *src);
int             cpml_extents_pair_is_inside     (const CpmlExtents *extents,
                                                 const CpmlPair    *src);
void            cpml_extents_transform          (CpmlExtents       *extents,
                                                 const cairo_matrix_t *matrix);

CAIRO_END_DECLS


#endif /* __CPML_EXTENTS_H__ */
