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


#ifndef __ADG_TABLE_CELL_H__
#define __ADG_TABLE_CELL_H__


G_BEGIN_DECLS

#define ADG_TYPE_TABLE_CELL         (adg_table_cell_get_type())


GType           adg_table_cell_get_type         (void);

AdgTableCell *  adg_table_cell_dup              (const AdgTableCell *src);
AdgTableCell *  adg_table_cell_new              (AdgTableRow    *table_row);
AdgTableCell *  adg_table_cell_new_before       (AdgTableCell   *before_cell);
AdgTableCell *  adg_table_cell_new_with_width   (AdgTableRow    *table_row,
                                                 gdouble         width);
AdgTableCell *  adg_table_cell_new_full         (AdgTableRow    *table_row,
                                                 gdouble         width,
                                                 const gchar    *name,
                                                 const gchar    *title,
                                                 gboolean        has_frame);
void            adg_table_cell_dispose          (AdgTableCell   *table_cell);
void            adg_table_cell_free             (AdgTableCell   *table_cell);

AdgTableRow *   adg_table_cell_get_row          (AdgTableCell   *table_cell);
AdgTable *      adg_table_cell_get_table        (AdgTableCell   *table_cell);
void            adg_table_cell_set_title        (AdgTableCell   *table_cell,
                                                 AdgEntity      *title);
void            adg_table_cell_set_text_title   (AdgTableCell   *table_cell,
                                                 const gchar    *title);
AdgEntity *     adg_table_cell_title            (AdgTableCell   *table_cell);
void            adg_table_cell_set_value        (AdgTableCell   *table_cell,
                                                 AdgEntity      *value);
void            adg_table_cell_set_text_value   (AdgTableCell   *table_cell,
                                                 const gchar    *value);
AdgEntity *     adg_table_cell_value            (AdgTableCell   *table_cell);
void            adg_table_cell_set_value_pos    (AdgTableCell   *table_cell,
                                                 const CpmlPair *from_factor,
                                                 const CpmlPair *to_factor);
void            adg_table_cell_set_value_pos_explicit
                                                (AdgTableCell   *table_cell,
                                                 gdouble         from_x,
                                                 gdouble         from_y,
                                                 gdouble         to_x,
                                                 gdouble         to_y);
void            adg_table_cell_set_width        (AdgTableCell   *table_cell,
                                                 gdouble         width);
gdouble         adg_table_cell_get_width        (AdgTableCell   *table_cell);
void            adg_table_cell_switch_frame     (AdgTableCell   *table_cell,
                                                 gboolean        has_frame);
gboolean        adg_table_cell_has_frame        (AdgTableCell   *table_cell);
const CpmlExtents *
                adg_table_cell_get_extents      (AdgTableCell   *table_cell);
const CpmlPair *adg_table_cell_size_request     (AdgTableCell   *table_cell,
                                                 const CpmlExtents *row_extents);
const CpmlExtents *
                adg_table_cell_arrange          (AdgTableCell   *table_cell,
                                                 const CpmlExtents *layout);

G_END_DECLS


#endif /* __ADG_TABLE_CELL_H__ */
