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

#ifndef __CPML_CLOSE_H__
#define __CPML_CLOSE_H__

#include <cpml/cpml-primitive.h>
#include <cpml/cpml-macros.h>


CAIRO_BEGIN_DECLS

int             cpml_close_get_npoints          (void) CPML_GNUC_CONST;
void            cpml_close_offset               (CpmlPrimitive  *close,
                                                 double          offset);

CAIRO_END_DECLS


#endif /* __CPML_CLOSE_H__ */