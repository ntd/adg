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


#ifndef __ADG_PANGO_STYLE__
#define __ADG_PANGO_STYLE__


G_BEGIN_DECLS

#define ADG_TYPE_PANGO_STYLE             (adg_pango_style_get_type())
#define ADG_PANGO_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_PANGO_STYLE, AdgPangoStyle))
#define ADG_PANGO_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_PANGO_STYLE, AdgPangoStyleClass))
#define ADG_IS_PANGO_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_PANGO_STYLE))
#define ADG_IS_PANGO_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_PANGO_STYLE))
#define ADG_PANGO_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_PANGO_STYLE, AdgPangoStyleClass))


typedef struct _AdgPangoStyle        AdgPangoStyle;
typedef struct _AdgPangoStyleClass   AdgPangoStyleClass;

struct _AdgPangoStyle {
    /*< private >*/
    AdgFontStyle        parent;
};

struct _AdgPangoStyleClass {
    /*< private >*/
    AdgFontStyleClass   parent_class;
};


GType           adg_pango_style_get_type        (void);
AdgPangoStyle * adg_pango_style_new             (void);
PangoFontDescription *
                adg_pango_style_get_description (AdgPangoStyle  *pango_style);
gint            adg_pango_style_get_spacing     (AdgPangoStyle  *pango_style);
void            adg_pango_style_set_spacing     (AdgPangoStyle  *pango_style,
                                                 gint            spacing);


G_END_DECLS


#endif /* __ADG_PANGO_STYLE_H__ */
