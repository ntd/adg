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

#ifndef __CPML_EXTENTS_H__
#define __CPML_EXTENTS_H__

#include <cpml/cpml-pair.h>


CAIRO_BEGIN_DECLS

typedef struct _CpmlExtents CpmlExtents;

struct _CpmlExtents {
    cairo_bool_t is_defined;
    CpmlPair     org;
    CpmlPair     size;
};


CpmlExtents *   cpml_extents_copy               (CpmlExtents       *extents,
                                                 const CpmlExtents *src);
CpmlExtents *   cpml_extents_from_cairo_text    (CpmlExtents       *extents,
                                                 const cairo_text_extents_t
                                                                   *cairo_extents);
void            cpml_extents_add                (CpmlExtents       *extents,
                                                 const CpmlExtents *src);

CAIRO_END_DECLS


#endif /* __CPML_EXTENTS_H__ */
