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


#ifndef __ADG_DIM_STYLE_H__
#define __ADG_DIM_STYLE_H__


G_BEGIN_DECLS

#define ADG_TYPE_DIM_STYLE             (adg_dim_style_get_type())
#define ADG_DIM_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_DIM_STYLE, AdgDimStyle))
#define ADG_DIM_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_DIM_STYLE, AdgDimStyleClass))
#define ADG_IS_DIM_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_DIM_STYLE))
#define ADG_IS_DIM_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_DIM_STYLE))
#define ADG_DIM_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_DIM_STYLE, AdgDimStyleClass))


typedef struct _AdgDimStyle        AdgDimStyle;
typedef struct _AdgDimStyleClass   AdgDimStyleClass;

struct _AdgDimStyle {
    /*< private >*/
    AdgStyle            parent;
};

struct _AdgDimStyleClass {
    /*< private >*/
    AdgStyleClass       parent_class;
};


GType           adg_dim_style_get_type          (void);
AdgDimStyle *   adg_dim_style_new               (void);
void            adg_dim_style_set_marker1       (AdgDimStyle    *dim_style,
                                                 AdgMarker      *marker);
AdgMarker *     adg_dim_style_marker1_new       (AdgDimStyle    *dim_style);
void            adg_dim_style_set_marker2       (AdgDimStyle    *dim_style,
                                                 AdgMarker      *marker);
AdgMarker *     adg_dim_style_marker2_new       (AdgDimStyle    *dim_style);
void            adg_dim_style_set_color_dress   (AdgDimStyle    *dim_style,
                                                 AdgDress        dress);
AdgDress        adg_dim_style_get_color_dress   (AdgDimStyle    *dim_style);
void            adg_dim_style_set_value_dress   (AdgDimStyle    *dim_style,
                                                 AdgDress        dress);
AdgDress        adg_dim_style_get_value_dress   (AdgDimStyle    *dim_style);
void            adg_dim_style_set_min_dress     (AdgDimStyle    *dim_style,
                                                 AdgDress        dress);
AdgDress        adg_dim_style_get_min_dress     (AdgDimStyle    *dim_style);
void            adg_dim_style_set_max_dress     (AdgDimStyle    *dim_style,
                                                 AdgDress        dress);
AdgDress        adg_dim_style_get_max_dress     (AdgDimStyle    *dim_style);
void            adg_dim_style_set_line_dress    (AdgDimStyle    *dim_style,
                                                 AdgDress        dress);
AdgDress        adg_dim_style_get_line_dress    (AdgDimStyle    *dim_style);
void            adg_dim_style_set_from_offset   (AdgDimStyle    *dim_style,
                                                 gdouble         offset);
gdouble         adg_dim_style_get_from_offset   (AdgDimStyle    *dim_style);
void            adg_dim_style_set_to_offset     (AdgDimStyle    *dim_style,
                                                 gdouble         offset);
gdouble         adg_dim_style_get_to_offset     (AdgDimStyle    *dim_style);
void            adg_dim_style_set_beyond        (AdgDimStyle    *dim_style,
                                                 gdouble         beyond);
gdouble         adg_dim_style_get_beyond        (AdgDimStyle    *dim_style);
void            adg_dim_style_set_baseline_spacing
                                                (AdgDimStyle    *dim_style,
                                                 gdouble         spacing);
gdouble         adg_dim_style_get_baseline_spacing
                                                (AdgDimStyle    *dim_style);
void            adg_dim_style_set_limits_spacing(AdgDimStyle    *dim_style,
                                                 gdouble         spacing);
gdouble         adg_dim_style_get_limits_spacing(AdgDimStyle    *dim_style);
void            adg_dim_style_set_quote_shift   (AdgDimStyle    *dim_style,
                                                 const CpmlPair *shift);
const CpmlPair *adg_dim_style_get_quote_shift   (AdgDimStyle    *dim_style);
void            adg_dim_style_set_limits_shift  (AdgDimStyle    *dim_style,
                                                 const CpmlPair *shift);
const CpmlPair *adg_dim_style_get_limits_shift  (AdgDimStyle    *dim_style);
void            adg_dim_style_set_number_format (AdgDimStyle    *dim_style,
                                                 const gchar    *format);
const gchar *   adg_dim_style_get_number_format (AdgDimStyle    *dim_style);
void            adg_dim_style_set_number_arguments
                                                (AdgDimStyle    *dim_style,
                                                 const gchar    *arguments);
const gchar *   adg_dim_style_get_number_arguments
                                                (AdgDimStyle    *dim_style);
void            adg_dim_style_set_number_tag    (AdgDimStyle    *dim_style,
                                                 const gchar    *tag);
const gchar *   adg_dim_style_get_number_tag    (AdgDimStyle    *dim_style);
void            adg_dim_style_set_decimals      (AdgDimStyle    *dim_style,
                                                 gint            decimals);
gint            adg_dim_style_get_decimals      (AdgDimStyle    *dim_style);
void            adg_dim_style_set_rounding      (AdgDimStyle    *dim_style,
                                                 gint            rounding);
gint            adg_dim_style_get_rounding      (AdgDimStyle    *dim_style);
gboolean        adg_dim_style_convert           (AdgDimStyle    *dim_style,
                                                 gdouble        *value,
                                                 gchar           format);

G_END_DECLS


#endif /* __ADG_DIM_STYLE_H__ */
