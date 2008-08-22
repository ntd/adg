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

#ifndef __CPML_PAIR_H__
#define __CPML_PAIR_H__

#include <cairo.h>
#include <math.h>

#define CPML_DIR_RIGHT   0.
#define CPML_DIR_DOWN    (M_PI/2.)
#define CPML_DIR_LEFT    M_PI
#define CPML_DIR_UP      (M_PI*3./2.)


CAIRO_BEGIN_DECLS


typedef struct _CpmlPair        CpmlPair;

struct _CpmlPair {
        double                  x, y;
};


cairo_bool_t    cpml_pair_copy                  (CpmlPair       *pair,
                                                 const CpmlPair *src);
cairo_bool_t    cpml_pair_distance              (double         *distance,
                                                 const CpmlPair *from,
                                                 const CpmlPair *to);
cairo_bool_t    cpml_pair_square_distance       (double         *distance,
                                                 const CpmlPair *from,
                                                 const CpmlPair *to);
cairo_bool_t    cpml_pair_angle                 (double         *angle,
                                                 const CpmlPair *from,
                                                 const CpmlPair *to);
cairo_bool_t    cpml_vector_from_pair           (CpmlPair       *vector,
                                                 const CpmlPair *pair);
cairo_bool_t    cpml_vector_from_angle          (CpmlPair       *vector,
                                                 double          angle);


CAIRO_END_DECLS


#endif /* __CPML_PAIR_H__ */
