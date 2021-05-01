/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ARROW_H__
#define __ADG_ARROW_H__


G_BEGIN_DECLS

#define ADG_TYPE_ARROW             (adg_arrow_get_type())
#define ADG_ARROW(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ARROW, AdgArrow))
#define ADG_ARROW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_ARROW, AdgArrowClass))
#define ADG_IS_ARROW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ARROW))
#define ADG_IS_ARROW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_ARROW))
#define ADG_ARROW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_ARROW, AdgArrowClass))

typedef struct _AdgArrow       AdgArrow;
typedef struct _AdgArrowClass  AdgArrowClass;

struct _AdgArrow {
    /*< private >*/
    AdgMarker            parent;
};

struct _AdgArrowClass {
    /*< private >*/
    AdgMarkerClass       parent_class;
};


GType           adg_arrow_get_type              (void);

AdgArrow *      adg_arrow_new                   (void);
AdgArrow *      adg_arrow_new_with_trail        (AdgTrail       *trail,
                                                 gdouble         pos);
void            adg_arrow_set_angle             (AdgArrow       *arrow,
                                                 gdouble         angle);
gdouble         adg_arrow_get_angle             (AdgArrow       *arrow);

G_END_DECLS


#endif /* __ADG_ARROW_H__ */
