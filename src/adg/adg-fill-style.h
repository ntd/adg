/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_FILL_STYLE_H__
#define __ADG_FILL_STYLE_H__


G_BEGIN_DECLS

#define ADG_TYPE_FILL_STYLE             (adg_fill_style_get_type())
#define ADG_FILL_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_FILL_STYLE, AdgFillStyle))
#define ADG_FILL_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_FILL_STYLE, AdgFillStyleClass))
#define ADG_IS_FILL_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_FILL_STYLE))
#define ADG_IS_FILL_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_FILL_STYLE))
#define ADG_FILL_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_FILL_STYLE, AdgFillStyleClass))


typedef struct _AdgFillStyle        AdgFillStyle;
typedef struct _AdgFillStyleClass   AdgFillStyleClass;

struct _AdgFillStyle {
    /*< private >*/
    AdgStyle            parent;
};

struct _AdgFillStyleClass {
    /*< private >*/
    AdgStyleClass       parent_class;

    /*< public >*/
    /* Virtual table */
    void                (*set_extents)          (AdgFillStyle   *fill_style,
                                                 const CpmlExtents *extents);
};


GType              adg_fill_style_get_type      (void);
void               adg_fill_style_set_pattern   (AdgFillStyle       *fill_style,
                                                 cairo_pattern_t    *pattern);
cairo_pattern_t *  adg_fill_style_get_pattern   (AdgFillStyle       *fill_style);
void               adg_fill_style_set_extents   (AdgFillStyle       *fill_style,
                                                 const CpmlExtents  *extents);
const CpmlExtents *adg_fill_style_get_extents   (AdgFillStyle       *fill_style);

G_END_DECLS


#endif /* __ADG_FILL_STYLE_H__ */
