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


#if !defined (__ADG_H__)
#error "Only <adg/adg.h> can be included directly."
#endif


#ifndef __ADG_LINE_STYLE_H__
#define __ADG_LINE_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-dress.h>


G_BEGIN_DECLS

#define ADG_TYPE_LINE_STYLE             (adg_line_style_get_type())
#define ADG_LINE_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_LINE_STYLE, AdgLineStyle))
#define ADG_LINE_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_LINE_STYLE, AdgLineStyleClass))
#define ADG_IS_LINE_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_LINE_STYLE))
#define ADG_IS_LINE_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_LINE_STYLE))
#define ADG_LINE_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_LINE_STYLE, AdgLineStyleClass))


typedef struct _AdgLineStyle        AdgLineStyle;
typedef struct _AdgLineStyleClass   AdgLineStyleClass;

struct _AdgLineStyle {
    /*< private >*/
    AdgStyle             parent;
    gpointer             data;
};

struct _AdgLineStyleClass {
    /*< private >*/
    AdgStyleClass        parent_class;
};


GType           adg_line_style_get_type         (void) G_GNUC_CONST;
AdgLineStyle *  adg_line_style_new              (void);

void            adg_line_style_set_color_dress  (AdgLineStyle   *line_style,
                                                 AdgDress        dress);
AdgDress        adg_line_style_get_color_dress  (AdgLineStyle   *line_style);
void            adg_line_style_set_width        (AdgLineStyle   *line_style,
                                                 gdouble         width);
gdouble         adg_line_style_get_width        (AdgLineStyle   *line_style);
void            adg_line_style_set_cap          (AdgLineStyle   *line_style,
                                                 cairo_line_cap_t cap);
cairo_line_cap_t
                adg_line_style_get_cap          (AdgLineStyle   *line_style);
void            adg_line_style_set_join         (AdgLineStyle   *line_style,
                                                 cairo_line_join_t join);
cairo_line_join_t
                adg_line_style_get_join         (AdgLineStyle   *line_style);
void            adg_line_style_set_miter_limit  (AdgLineStyle   *line_style,
                                                 gdouble         miter_limit);
gdouble         adg_line_style_get_miter_limit  (AdgLineStyle   *line_style);
void            adg_line_style_set_antialias    (AdgLineStyle   *line_style,
                                                 cairo_antialias_t antialias);
cairo_antialias_t
                adg_line_style_get_antialias    (AdgLineStyle   *line_style);


G_END_DECLS


#endif /* __ADG_LINE_STYLE_H__ */
