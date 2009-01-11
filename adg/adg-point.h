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


#ifndef __ADG_POINT_H__
#define __ADG_POINT_H__

#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_POINT  (adg_point_get_type())


typedef struct _AdgPoint AdgPoint;

struct _AdgPoint {
    AdgPair model;
    AdgPair paper;
};


GType           adg_point_get_type      (void) G_GNUC_CONST;
AdgPoint *      adg_point_dup           (const AdgPoint *point);
void            adg_point_copy          (AdgPoint       *point,
                                         const AdgPoint *src);
void            adg_point_set           (AdgPoint       *point,
                                         const AdgPair  *model,
                                         const AdgPair  *paper);
void            adg_point_unset         (AdgPoint       *point);

G_END_DECLS


#endif /* __ADG_POINT_H__ */
