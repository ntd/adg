/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_RDIM_PRIVATE_H__
#define __ADG_RDIM_PRIVATE_H__

#include "adg-trail.h"
#include "adg-marker.h"


G_BEGIN_DECLS

/*
 * The cpml.data array is structured in the following way:
 *
 * [0]  = MOVE_TO
 * [1]  = baseline start
 * [2]  = LINE_TO
 * [3]  = baseline end
 * [4]  = MOVE_TO
 * [5]  = outside line start
 * [6]  = LINE_TO
 * [7]  = outside line end
 */

typedef struct _AdgRDimPrivate AdgRDimPrivate;

struct _AdgRDimPrivate {
    AdgTrail            *trail;
    AdgMarker           *marker;

    gboolean             geometry_arranged;
    gdouble              radius;
    gdouble              angle;

    struct {
        AdgPair          base;
    }                    point;

    struct {
        AdgPair          base;
    }                    shift;

    struct {
        AdgMatrix        global_map;
    }                    quote;

    struct {
        CpmlPath path;
        cairo_path_data_t data[8];
    }                    cpml;
};

G_END_DECLS


#endif /* __ADG_RDIM_PRIVATE_H__ */
