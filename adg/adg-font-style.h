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


#ifndef __ADG_FONT_STYLE_H__
#define __ADG_FONT_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-matrix.h>


G_BEGIN_DECLS

#define ADG_TYPE_FONT_STYLE             (adg_font_style_get_type())
#define ADG_FONT_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_FONT_STYLE, AdgFontStyle))
#define ADG_FONT_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_FONT_STYLE, AdgFontStyleClass))
#define ADG_IS_FONT_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_FONT_STYLE))
#define ADG_IS_FONT_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_FONT_STYLE))
#define ADG_FONT_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_FONT_STYLE, AdgFontStyleClass))

typedef struct _AdgFontStyle        AdgFontStyle;
typedef struct _AdgFontStyleClass   AdgFontStyleClass;

struct _AdgFontStyle {
    /*< private >*/
    AdgStyle             parent;
    gpointer             data;
};

struct _AdgFontStyleClass {
    /*< private >*/
    AdgStyleClass        parent_class;
};


GType           adg_font_style_get_type         (void) G_GNUC_CONST;
AdgStyle *      adg_font_style_new              (void);

cairo_scaled_font_t *
                adg_font_style_get_scaled_font  (AdgFontStyle    *font_style,
                                                 const AdgMatrix *ctm);
void            adg_font_style_set_color_dress  (AdgFontStyle   *font_style,
                                                 AdgDress        dress);
AdgDress        adg_font_style_get_color_dress  (AdgFontStyle   *font_style);
void            adg_font_style_set_family       (AdgFontStyle    *font_style,
                                                 const gchar     *family);
const gchar *   adg_font_style_get_family       (AdgFontStyle    *font_style);
void            adg_font_style_set_slant        (AdgFontStyle    *font_style,
                                                 cairo_font_slant_t slant);
cairo_font_slant_t
                adg_font_style_get_slant        (AdgFontStyle    *font_style);
void            adg_font_style_set_weight       (AdgFontStyle    *font_style,
                                                 cairo_font_weight_t weight);
cairo_font_weight_t
                adg_font_style_get_weight       (AdgFontStyle    *font_style);
void            adg_font_style_set_size         (AdgFontStyle    *font_style,
                                                 gdouble          size);
gdouble         adg_font_style_get_size         (AdgFontStyle    *font_style);
void            adg_font_style_set_antialias    (AdgFontStyle    *font_style,
                                                 cairo_antialias_t antialias);
cairo_antialias_t
                adg_font_style_get_antialias    (AdgFontStyle    *font_style);
void            adg_font_style_set_subpixel_order
                                                (AdgFontStyle    *font_style,
                                                 cairo_subpixel_order_t subpixel_order);
cairo_subpixel_order_t
                adg_font_style_get_subpixel_order
                                                (AdgFontStyle    *font_style);
void            adg_font_style_set_hint_style   (AdgFontStyle    *font_style,
                                                 cairo_hint_style_t hint_style);
cairo_hint_style_t
                adg_font_style_get_hint_style   (AdgFontStyle    *font_style);
void            adg_font_style_set_hint_metrics (AdgFontStyle    *font_style,
                                                 cairo_hint_metrics_t hint_metrics);
cairo_hint_metrics_t
                adg_font_style_get_hint_metrics (AdgFontStyle    *font_style);

G_END_DECLS


#endif /* __ADG_FONT_STYLE_H__ */
