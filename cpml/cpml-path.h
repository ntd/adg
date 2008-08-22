/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
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

#ifndef __CPML_PATH_H__
#define __CPML_PATH_H__

#include <cpml/cpml-pair.h>

#define CPML_LAST       0
#define CPML_FIRST      1


CAIRO_BEGIN_DECLS


typedef struct _CpmlPath CpmlPath;

struct _CpmlPath {
        cairo_path_t     cairo_path;
        CpmlPair         org;
};


cairo_bool_t    cpml_path_from_cairo            (CpmlPath *path,
                                                 const cairo_path_t *src,
                                                 cairo_t *cr);
cairo_bool_t    cpml_path_from_cairo_explicit   (CpmlPath *path,
                                                 const cairo_path_t *src,
                                                 const CpmlPair *org);
CpmlPath *      cpml_path_copy                  (CpmlPath *path,
                                                 const CpmlPath *src);
cairo_bool_t    cpml_segment_from_path          (CpmlPath *segment,
                                                 const CpmlPath *path,
                                                 int index);
cairo_bool_t    cpml_primitive_from_path        (CpmlPath *primitive,
                                                 const CpmlPath *path,
                                                 int index);
cairo_bool_t    cpml_primitive_get_pair         (const CpmlPath *primitive,
                                                 CpmlPair *pair, int index);
cairo_bool_t    cpml_primitive_set_pair         (CpmlPath *primitive,
                                                 const CpmlPair *pair,
                                                 int index);
cairo_bool_t    cpml_primitive_get_point        (const CpmlPath *primitive,
                                                 CpmlPair *point, double pos);
cairo_bool_t    cpml_primitive_reverse          (CpmlPath *primitive);


CAIRO_END_DECLS


#endif /* __CPML_PATH_H__ */
