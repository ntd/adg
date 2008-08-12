/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


#ifndef __ADG_ARROW_STYLE_H__
#define __ADG_ARROW_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-enums.h>
#include <cpml/cpml.h>


G_BEGIN_DECLS

#define ADG_TYPE_ARROW_STYLE             (adg_arrow_style_get_type ())
#define ADG_SLOT_ARROW_STYLE             (adg_arrow_style_get_slot ())
#define ADG_ARROW_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_ARROW_STYLE, AdgArrowStyle))
#define ADG_ARROW_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_ARROW_STYLE, AdgArrowStyleClass))
#define ADG_IS_ARROW_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_ARROW_STYLE))
#define ADG_IS_ARROW_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_ARROW_STYLE))
#define ADG_ARROW_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_ARROW_STYLE, AdgArrowStyleClass))

typedef struct _AdgArrowStyle        AdgArrowStyle;
typedef struct _AdgArrowStyleClass   AdgArrowStyleClass;
typedef struct _AdgArrowStylePrivate AdgArrowStylePrivate;

/**
 * AdgArrowRenderer:
 * @arrow_style: an #AdgArrowStyle object
 * @cr: a #cairo_t drawing context
 * @segment: the CpmlPath segment where rendering the arrow
 *
 * Callback that renders a custom arrow at the start position in @segment.
 * If you need an ending arrow, you should reverse the segment and recall
 * this function.
 **/
typedef void (*AdgArrowRenderer)                (AdgArrowStyle  *arrow_style,
                                                 cairo_t        *cr,
                                                 CpmlPath       *segment);

struct _AdgArrowStyle {
    AdgStyle              style;
    /*< private >*/
    AdgArrowStylePrivate *priv;
};

struct _AdgArrowStyleClass {
    AdgStyleClass         parent_class;
};


GType           adg_arrow_style_get_type        (void) G_GNUC_CONST;
AdgStyleSlot    adg_arrow_style_get_slot        (void) G_GNUC_CONST;
AdgStyle *      adg_arrow_style_new             (void);

void            adg_arrow_style_render          (AdgArrowStyle  *arrow_style,
                                                 cairo_t        *cr,
                                                 CpmlPath       *segment);

gdouble         adg_arrow_style_get_size        (AdgArrowStyle  *arrow_style);
void            adg_arrow_style_set_size        (AdgArrowStyle  *arrow_style,
                                                 gdouble         size);
gdouble         adg_arrow_style_get_angle       (AdgArrowStyle  *arrow_style);
void            adg_arrow_style_set_angle       (AdgArrowStyle  *arrow_style,
                                                 gdouble         angle);
gdouble         adg_arrow_style_get_margin      (AdgArrowStyle  *arrow_style);
void            adg_arrow_style_set_margin      (AdgArrowStyle  *arrow_style,
                                                 gdouble         margin);
const AdgArrowRenderer
                adg_arrow_style_get_renderer    (AdgArrowStyle  *arrow_style);
void            adg_arrow_style_set_renderer    (AdgArrowStyle  *arrow_style,
                                                 AdgArrowRenderer renderer);

G_END_DECLS


#endif /* __ADG_ARROW_STYLE_H__ */
