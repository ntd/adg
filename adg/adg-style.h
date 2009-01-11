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


#ifndef __ADG_STYLE_H__
#define __ADG_STYLE_H__

#include <adg/adg-pattern.h>


G_BEGIN_DECLS


#define ADG_TYPE_STYLE             (adg_style_get_type ())
#define ADG_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_STYLE, AdgStyle))
#define ADG_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_STYLE, AdgStyleClass))
#define ADG_IS_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_STYLE))
#define ADG_IS_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_STYLE))
#define ADG_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_STYLE, AdgStyleClass))

typedef struct _AdgStyle        AdgStyle;
typedef struct _AdgStyleClass   AdgStyleClass;
typedef struct _AdgStylePrivate AdgStylePrivate;
typedef gint                    AdgStyleId;
typedef gint                    AdgStyleSlot;

struct _AdgStyle {
    GObject              object;
    /*< private >*/
    AdgStylePrivate     *priv;
};

struct _AdgStyleClass {
    GObjectClass         parent_class;
    /* Virtual table */
    GPtrArray *         (*get_pool)             (void);
    void                (*apply)                (AdgStyle       *style,
                                                 cairo_t        *cr);
};

GType                   adg_style_get_type      (void) G_GNUC_CONST;
AdgStyleId              adg_style_register_id   (AdgStyle       *style);
AdgStyle *              adg_style_from_id       (GType           type,
                                                 AdgStyleId      id);
AdgStyle *              adg_style_get_default   (AdgStyleClass  *klass);

void                    adg_style_apply         (AdgStyle       *style,
                                                 cairo_t        *cr);
const AdgPattern *      adg_style_get_pattern   (AdgStyle       *style);
void                    adg_style_set_pattern   (AdgStyle       *style,
                                                 AdgPattern     *pattern);


G_END_DECLS


#endif /* __ADG_STYLE_H__ */
