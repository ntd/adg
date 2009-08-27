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


#ifndef __ADG_MARKER_H__
#define __ADG_MARKER_H__

#include <adg/adg-entity.h>


G_BEGIN_DECLS

#define ADG_TYPE_MARKER             (adg_marker_get_type())
#define ADG_MARKER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_MARKER, AdgMarker))
#define ADG_MARKER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_MARKER, AdgMarker))
#define ADG_IS_MARKER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_MARKER))
#define ADG_IS_MARKER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_MARKER))
#define ADG_MARKER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_MARKER, AdgMarker))

typedef struct _AdgMarker       AdgMarker;
typedef struct _AdgMarkerClass  AdgMarkerClass;

struct _AdgMarker {
    /*< private >*/
    AdgEntity            parent;
    gpointer             data;
};

struct _AdgMarkerClass {
    /*< private >*/
    AdgEntityClass       parent_class;
};


GType           adg_marker_get_type             (void) G_GNUC_CONST;

gdouble         adg_marker_get_size             (AdgMarker      *marker);
void            adg_marker_set_size             (AdgMarker      *marker,
                                                 gdouble         size);
gdouble         adg_marker_get_angle            (AdgMarker      *marker);
void            adg_marker_set_angle            (AdgMarker      *marker,
                                                 gdouble         angle);

G_END_DECLS


#endif /* __ADG_MARKER_H__ */
