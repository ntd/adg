/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ADIM_PRIVATE_H__
#define __ADG_ADIM_PRIVATE_H__

#include <cairo.h>


G_BEGIN_DECLS

/*
 * The cpml.data array is structured in the following way:
 *
 * [0]  = MOVE_TO
 * [1]  = arc start
 * [2]  = ARC_TO
 * [3]  = arc middle
 * [4]  = arc end
 * [5]  = MOVE_TO
 * [6]  = first extension line start
 * [7]  = LINE_TO
 * [8]  = first extension line end
 * [9]  = MOVE_TO
 * [10] = second extension line start
 * [11] = LINE_TO
 * [12] = second extension line end
 */

typedef struct _AdgADimPrivate AdgADimPrivate;

struct _AdgADimPrivate {
    AdgPair              org1;
    AdgPair              org2;
    gboolean             has_extension1;
    gboolean             has_extension2;

    gdouble              angle1;
    gdouble              angle2;
    AdgTrail            *trail;
    AdgMarker           *marker1;
    AdgMarker           *marker2;

    struct {
        CpmlPath path;
        cairo_path_data_t data[13];
    }            cpml;

    struct {
        gboolean         is_arranged;
        CpmlPair         from1;
        CpmlPair         from2;
        CpmlPair         marker1;
        CpmlPair         marker2;
        CpmlPair         to1;
        CpmlPair         to2;
    }                    shift;
};

G_END_DECLS


#endif /* __ADG_ADIM_PRIVATE_H__ */
