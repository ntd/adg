/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ROTABLE_H__
#define __ADG_ROTABLE_H__

#include <adg/adg-point.h>


G_BEGIN_DECLS

#define ADG_TYPE_ROTABLE            (adg_rotable_get_type())
#define ADG_ROTABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ROTABLE, AdgRotable))
#define ADG_IS_ROTABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ROTABLE))
#define ADG_ROTABLE_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE((obj), ADG_TYPE_ROTABLE, AdgRotableIface))

typedef struct _AdgRotable       AdgRotable; /* Dummy typedef */
typedef struct _AdgRotableIface  AdgRotableIface;

struct _AdgRotableIface {
    GTypeInterface base_iface;

    /* Virtual Table */
    gdouble     (*get_angle)                    (AdgRotable     *rotable);
    void        (*set_angle)                    (AdgRotable     *rotable,
                                                 gdouble         angle);
};


GType           adg_rotable_get_type            (void) G_GNUC_CONST;

gdouble         adg_rotable_get_angle           (AdgRotable     *rotable);
void            adg_rotable_set_angle           (AdgRotable     *rotable,
                                                 gdouble         angle);

G_END_DECLS


#endif /* __ADG_ROTABLE_H__ */
