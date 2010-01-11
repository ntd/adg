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


#ifndef __ADG_TABLE_STYLE_H__
#define __ADG_TABLE_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-dress.h>
#include <adg/adg-marker.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_TABLE_STYLE             (adg_table_style_get_type())
#define ADG_TABLE_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TABLE_STYLE, AdgTableStyle))
#define ADG_TABLE_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TABLE_STYLE, AdgTableStyleClass))
#define ADG_IS_TABLE_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TABLE_STYLE))
#define ADG_IS_TABLE_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TABLE_STYLE))
#define ADG_TABLE_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TABLE_STYLE, AdgTableStyleClass))


typedef struct _AdgTableStyle        AdgTableStyle;
typedef struct _AdgTableStyleClass   AdgTableStyleClass;

struct _AdgTableStyle {
    /*< private >*/
    AdgStyle             parent;
    gpointer             data;
};

struct _AdgTableStyleClass {
    /*< private >*/
    AdgStyleClass        parent_class;
};


GType           adg_table_style_get_type          (void) G_GNUC_CONST;
AdgStyle *      adg_table_style_new               (void);

void            adg_table_style_set_color_dress (AdgTableStyle  *table_style,
                                                 AdgDress        dress);
AdgDress        adg_table_style_get_color_dress (AdgTableStyle  *table_style);
void            adg_table_style_set_frame_dress (AdgTableStyle  *table_style,
                                                 AdgDress        dress);
AdgDress        adg_table_style_get_frame_dress (AdgTableStyle  *table_style);
void            adg_table_style_set_grid_dress  (AdgTableStyle  *table_style,
                                                 AdgDress        dress);
AdgDress        adg_table_style_get_grid_dress  (AdgTableStyle  *table_style);
void            adg_table_style_set_title_dress (AdgTableStyle  *table_style,
                                                 AdgDress        dress);
AdgDress        adg_table_style_get_title_dress (AdgTableStyle  *table_style);
void            adg_table_style_set_value_dress (AdgTableStyle  *table_style,
                                                 AdgDress        dress);
AdgDress        adg_table_style_get_value_dress (AdgTableStyle  *table_style);
void            adg_table_style_set_row_height  (AdgTableStyle  *table_style,
                                                 gdouble         row_height);
gdouble         adg_table_style_get_row_height  (AdgTableStyle  *table_style);
void            adg_table_style_set_cell_padding(AdgTableStyle  *table_style,
                                                 const AdgPair  *cell_padding);
const AdgPair * adg_table_style_get_cell_padding(AdgTableStyle  *table_style);
void            adg_table_style_set_cell_spacing(AdgTableStyle  *table_style,
                                                 const AdgPair  *cell_spacing);
const AdgPair * adg_table_style_get_cell_spacing(AdgTableStyle  *table_style);

G_END_DECLS


#endif /* __ADG_TABLE_STYLE_H__ */
