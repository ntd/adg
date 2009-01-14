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


#ifndef __ADG_POSITIONABLE_H__
#define __ADG_POSITIONABLE_H__

#include <adg/adg-point.h>


G_BEGIN_DECLS

#define ADG_TYPE_POSITIONABLE            (adg_positionable_get_type())
#define ADG_POSITIONABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_POSITIONABLE, AdgPositionable))
#define ADG_IS_POSITIONABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_POSITIONABLE))
#define ADG_POSITIONABLE_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE((obj), ADG_TYPE_POSITIONABLE, AdgPositionableIface))

typedef struct _AdgPositionable       AdgPositionable; /* Dummy typedef */
typedef struct _AdgPositionableIface  AdgPositionableIface;

struct _AdgPositionableIface {
    GTypeInterface base_iface;

    /* Virtual Table */
    void        (*get_origin)                   (AdgPositionable *positionable,
                                                 AdgPoint        *dest);
    void        (*set_origin)                   (AdgPositionable *positionable,
                                                 const AdgPoint  *origin);
};


GType           adg_positionable_get_type       (void) G_GNUC_CONST;

void            adg_positionable_get_origin     (AdgPositionable *positionable,
                                                 AdgPoint        *dest);
void            adg_positionable_set_origin     (AdgPositionable *positionable,
                                                 const AdgPoint  *origin);
void            adg_positionable_set_origin_explicit
                                                (AdgPositionable *positionable,
                                                 gdouble          model_x,
                                                 gdouble          model_y,
                                                 gdouble          paper_x,
                                                 gdouble          paper_y);

G_END_DECLS


#endif /* __ADG_POSITIONABLE_H__ */
