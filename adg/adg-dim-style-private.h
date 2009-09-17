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


#ifndef __ADG_DIM_STYLE_PRIVATE_H__
#define __ADG_DIM_STYLE_PRIVATE_H__

#include <adg/adg-style.h>


G_BEGIN_DECLS

typedef struct _AdgMarkerData AdgMarkerData;
typedef struct _AdgDimStylePrivate AdgDimStylePrivate;

struct _AdgMarkerData {
    GType                type;
    guint                n_parameters;
    GParameter          *parameters;
};

struct _AdgDimStylePrivate {
    AdgMarkerData        marker1;
    AdgMarkerData        marker2;
    AdgDress             color_dress;
    AdgDress             value_dress;
    AdgDress             min_dress;
    AdgDress             max_dress;
    AdgDress             line_dress;
    AdgDress             marker_dress;
    gdouble              from_offset;
    gdouble              to_offset;
    gdouble              beyond;
    gdouble              baseline_spacing;
    gdouble              limits_spacing;
    AdgPair              quote_shift;
    AdgPair              limits_shift;
    gchar               *number_format;
    gchar               *number_tag;
};

G_END_DECLS


#endif /* __ADG_DIM_STYLE_PRIVATE_H__ */
