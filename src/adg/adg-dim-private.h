/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_DIM_PRIVATE_H__
#define __ADG_DIM_PRIVATE_H__


G_BEGIN_DECLS

typedef struct _AdgDimPrivate AdgDimPrivate;
typedef struct _AdgDimReplaceData AdgDimReplaceData;

struct _AdgDimPrivate {
    AdgDress             dim_dress;
    AdgPoint            *ref1;
    AdgPoint            *ref2;
    AdgPoint            *pos;
    gdouble              level;
    AdgThreeState        outside;
    AdgThreeState        detached;
    gchar               *value;
    gchar               *min;
    gchar               *max;

    AdgDimStyle         *dim_style;

    struct {
        AdgAlignment    *entity;
        AdgTextual      *value;
        AdgTextual      *min;
        AdgTextual      *max;
    }                    quote;

    struct {
        gboolean         computed;
        gchar           *notice;
    }                    geometry;
};

struct _AdgDimReplaceData {
    AdgDimStyle         *dim_style;
    gdouble              value;
    const gchar         *format;
    const gchar         *argument;
    AdgThreeState        valorized;
    /* Just for conveniency */
    GRegex              *regex;
};

G_END_DECLS


#endif /* __ADG_DIM_PRIVATE_H__ */
