/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_COLOR_STYLE_H__
#define __ADG_COLOR_STYLE_H__


G_BEGIN_DECLS

#define ADG_TYPE_COLOR_STYLE             (adg_color_style_get_type())
#define ADG_COLOR_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_COLOR_STYLE, AdgColorStyle))
#define ADG_COLOR_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_COLOR_STYLE, AdgColorStyleClass))
#define ADG_IS_COLOR_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_COLOR_STYLE))
#define ADG_IS_COLOR_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_COLOR_STYLE))
#define ADG_COLOR_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_COLOR_STYLE, AdgColorStyleClass))


typedef struct _AdgColorStyle        AdgColorStyle;
typedef struct _AdgColorStyleClass   AdgColorStyleClass;

struct _AdgColorStyle {
    /*< private >*/
    AdgStyle             parent;
};

struct _AdgColorStyleClass {
    /*< private >*/
    AdgStyleClass        parent_class;
};


GType           adg_color_style_get_type        (void);
AdgColorStyle * adg_color_style_new             (void);

void            adg_color_style_set_red         (AdgColorStyle  *color_style,
                                                 gdouble         red);
gdouble         adg_color_style_get_red         (AdgColorStyle  *color_style);
void            adg_color_style_set_green       (AdgColorStyle  *color_style,
                                                 gdouble         green);
gdouble         adg_color_style_get_green       (AdgColorStyle  *color_style);
void            adg_color_style_set_blue        (AdgColorStyle  *color_style,
                                                 gdouble         blue);
gdouble         adg_color_style_get_blue        (AdgColorStyle  *color_style);
void            adg_color_style_set_rgb         (AdgColorStyle  *color_style,
                                                 gdouble         red,
                                                 gdouble         green,
                                                 gdouble         blue);
void            adg_color_style_put_rgb         (AdgColorStyle  *color_style,
                                                 gdouble        *red,
                                                 gdouble        *green,
                                                 gdouble        *blue);
void            adg_color_style_set_alpha       (AdgColorStyle  *color_style,
                                                 gdouble         alpha);
gdouble         adg_color_style_get_alpha       (AdgColorStyle  *color_style);

G_END_DECLS


#endif /* __ADG_COLOR_STYLE_H__ */
