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


#ifndef __ADG_RULED_FILL_H__
#define __ADG_RULED_FILL_H__

#include "adg-fill-style.h"
#include "adg-dress.h"


G_BEGIN_DECLS

#define ADG_TYPE_RULED_FILL             (adg_ruled_fill_get_type())
#define ADG_RULED_FILL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_RULED_FILL, AdgRuledFill))
#define ADG_RULED_FILL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_RULED_FILL, AdgRuledFillClass))
#define ADG_IS_RULED_FILL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_RULED_FILL))
#define ADG_IS_RULED_FILL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_RULED_FILL))
#define ADG_RULED_FILL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_RULED_FILL, AdgRuledFillClass))


typedef struct _AdgRuledFill        AdgRuledFill;
typedef struct _AdgRuledFillClass   AdgRuledFillClass;

struct _AdgRuledFill {
    /*< private >*/
    AdgFillStyle         parent;
    gpointer             data;
};

struct _AdgRuledFillClass {
    /*< private >*/
    AdgFillStyleClass    parent_class;
};


GType           adg_ruled_fill_get_type         (void) G_GNUC_CONST;
AdgRuledFill *  adg_ruled_fill_new              (void);
void            adg_ruled_fill_set_line_dress   (AdgRuledFill   *ruled_fill,
                                                 AdgDress        dress);
AdgDress        adg_ruled_fill_get_line_dress   (AdgRuledFill   *ruled_fill);
void            adg_ruled_fill_set_spacing      (AdgRuledFill   *ruled_fill,
                                                 gdouble         spacing);
gdouble         adg_ruled_fill_get_spacing      (AdgRuledFill   *ruled_fill);
void            adg_ruled_fill_set_angle        (AdgRuledFill   *ruled_fill,
                                                 gdouble         angle);
gdouble         adg_ruled_fill_get_angle        (AdgRuledFill   *ruled_fill);

G_END_DECLS


#endif /* __ADG_RULED_FILL_H__ */
