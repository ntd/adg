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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_TRAIL_H__
#define __ADG_TRAIL_H__

#include "adg-model.h"
#include "adg-primitive.h"
#include "adg-segment.h"
#include <cpml.h>


G_BEGIN_DECLS

#define ADG_TYPE_TRAIL             (adg_trail_get_type())
#define ADG_TRAIL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TRAIL, AdgTrail))
#define ADG_TRAIL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TRAIL, AdgTrailClass))
#define ADG_IS_TRAIL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TRAIL))
#define ADG_IS_TRAIL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TRAIL))
#define ADG_TRAIL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TRAIL, AdgTrailClass))


typedef struct _AdgTrail        AdgTrail;
typedef struct _AdgTrailClass   AdgTrailClass;
typedef CpmlPath * (*AdgTrailCallback) (AdgTrail *trail, gpointer user_data);

struct _AdgTrail {
    /*< private >*/
    AdgModel             parent;
    gpointer             data;
};

struct _AdgTrailClass {
    /*< private >*/
    AdgModelClass        parent_class;
    /*< public >*/
    CpmlPath *          (*get_cpml_path)        (AdgTrail       *trail);
};


GType               adg_trail_get_type          (void) G_GNUC_CONST;
AdgTrail *          adg_trail_new               (AdgTrailCallback callback,
                                                 gpointer         user_data);

const cairo_path_t *adg_trail_get_cairo_path    (AdgTrail        *trail);
CpmlPath *          adg_trail_cpml_path         (AdgTrail        *trail);
gboolean            adg_trail_put_segment       (AdgTrail        *trail,
                                                 guint            n_segment,
                                                 AdgSegment      *segment);
const CpmlExtents * adg_trail_get_extents       (AdgTrail        *trail);
void                adg_trail_dump              (AdgTrail        *trail);

G_END_DECLS


#endif /* __ADG_TRAIL_H__ */
