/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_STYLE_H__
#define __ADG_STYLE_H__


G_BEGIN_DECLS

#define ADG_TYPE_STYLE             (adg_style_get_type())
#define ADG_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_STYLE, AdgStyle))
#define ADG_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_STYLE, AdgStyleClass))
#define ADG_IS_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_STYLE))
#define ADG_IS_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_STYLE))
#define ADG_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_STYLE, AdgStyleClass))


typedef struct _AdgStyleClass   AdgStyleClass;

struct _AdgStyle {
    /*< private >*/
    GObject              parent;
};

struct _AdgStyleClass {
    /*< private >*/
    GObjectClass         parent_class;

    /*< public >*/
    /* Virtual table */
    AdgStyle *          (*clone)                (AdgStyle       *style);

    /* Signals */
    void                (*invalidate)           (AdgStyle       *style);
    void                (*apply)                (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);
};


GType                   adg_style_get_type      (void);

AdgStyle *              adg_style_clone         (AdgStyle       *style);
void                    adg_style_invalidate    (AdgStyle       *style);
void                    adg_style_apply         (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);

G_END_DECLS


#endif /* __ADG_STYLE_H__ */
