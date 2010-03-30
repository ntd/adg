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


#ifndef __ADG_HATCH_H__
#define __ADG_HATCH_H__

#include "adg-stroke.h"


G_BEGIN_DECLS

#define ADG_TYPE_HATCH             (adg_hatch_get_type())
#define ADG_HATCH(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_HATCH, AdgHatch))
#define ADG_HATCH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_HATCH, AdgHatchClass))
#define ADG_IS_HATCH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_HATCH))
#define ADG_IS_HATCH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_HATCH))
#define ADG_HATCH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_HATCH, AdgHatchClass))

typedef struct _AdgHatch        AdgHatch;
typedef struct _AdgHatchClass   AdgHatchClass;

struct _AdgHatch {
    /*< private >*/
    AdgStroke           parent;
    gpointer            data;
};

struct _AdgHatchClass {
    /*< private >*/
    AdgStrokeClass      parent_class;
};


GType           adg_hatch_get_type              (void) G_GNUC_CONST;

AdgHatch *      adg_hatch_new                   (AdgTrail       *trail);

void            adg_hatch_set_fill_dress        (AdgHatch       *hatch,
                                                 AdgDress        dress);
AdgDress        adg_hatch_get_fill_dress        (AdgHatch       *hatch);

G_END_DECLS


#endif /* __ADG_HATCH_H__ */
