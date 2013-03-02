/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_TABLE_ROW_H__
#define __ADG_TABLE_ROW_H__


G_BEGIN_DECLS

#define ADG_TYPE_TABLE_ROW         (adg_table_row_get_type())


GType           adg_table_row_get_type          (void) G_GNUC_CONST;

AdgTableRow *   adg_table_row_dup               (const AdgTableRow  *table_row);
AdgTableRow *   adg_table_row_new               (AdgTable       *table);
AdgTableRow *   adg_table_row_new_before        (AdgTableRow    *before_row);
void            adg_table_row_free              (AdgTableRow    *table_row);
void            adg_table_row_insert            (AdgTableRow    *table_row,
                                                 AdgTableCell   *table_cell,
                                                 AdgTableCell   *before_cell);
void            adg_table_row_remove            (AdgTableRow    *table_row,
                                                 AdgTableCell   *table_cell);
void            adg_table_row_foreach           (AdgTableRow    *table_row,
                                                 GCallback       callback,
                                                 gpointer        user_data);
AdgTable *      adg_table_row_get_table         (AdgTableRow    *table_row);
void            adg_table_row_set_height        (AdgTableRow    *table_row,
                                                 gdouble         height);
gdouble         adg_table_row_get_height        (AdgTableRow    *table_row);
const CpmlExtents *
                adg_table_row_get_extents       (AdgTableRow    *table_row);
const CpmlPair *adg_table_row_size_request      (AdgTableRow    *table_row);
const CpmlExtents *
                adg_table_row_arrange           (AdgTableRow    *table_row,
                                                 const CpmlExtents *layout);

G_END_DECLS


#endif /* __ADG_TABLE_ROW_H__ */
