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


#ifndef __ADG_TABLE_H__
#define __ADG_TABLE_H__

#include <adg/adg-entity.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_TABLE             (adg_table_get_type())
#define ADG_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TABLE, AdgTable))
#define ADG_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TABLE, AdgTableClass))
#define ADG_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TABLE))
#define ADG_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TABLE))
#define ADG_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TABLE, AdgTableClass))

typedef struct _AdgTableCell    AdgTableCell;
typedef struct _AdgTableRow     AdgTableRow;
typedef struct _AdgTable        AdgTable;
typedef struct _AdgTableClass   AdgTableClass;

struct _AdgTable {
    /*< private >*/
    AdgEntity           parent;
    gpointer            data;
};

struct _AdgTableClass {
    /*< private >*/
    AdgEntityClass      parent_class;
};


GType           adg_table_get_type              (void) G_GNUC_CONST;

AdgTable *      adg_table_new                   (void);

AdgDress        adg_table_get_table_dress       (AdgTable       *table);
void            adg_table_set_table_dress       (AdgTable       *table,
                                                 AdgDress        dress);
gboolean        adg_table_has_frame             (AdgTable       *table);
void            adg_table_switch_frame          (AdgTable       *table,
                                                 gboolean        new_state);
guint           adg_table_get_n_rows            (AdgTable       *table);

AdgTableRow *   adg_table_row_new               (AdgTable       *table);
AdgTableRow *   adg_table_row_new_before        (AdgTableRow    *row);
void            adg_table_row_delete            (AdgTableRow    *row);
guint           adg_table_row_get_n_cells       (const AdgTableRow *row);
void            adg_table_row_set_height        (AdgTableRow    *row,
                                                 gdouble         height);
gdouble         adg_table_row_get_height        (AdgTableRow    *row);
const CpmlExtents *
                adg_table_row_extents           (AdgTableRow    *row);

AdgTableCell *  adg_table_cell                  (AdgTable       *table,
                                                 const gchar    *name);
AdgTableCell *  adg_table_cell_new              (AdgTableRow    *row,
                                                 gdouble         width);
AdgTableCell *  adg_table_cell_new_before       (AdgTableCell   *cell,
                                                 gdouble         width);
AdgTableCell *  adg_table_cell_new_full         (AdgTableRow    *row,
                                                 gdouble         width,
                                                 const gchar    *name,
                                                 const gchar    *title,
                                                 const gchar    *value);
void            adg_table_cell_delete           (AdgTableCell   *cell);
const gchar *   adg_table_cell_get_name         (AdgTableCell   *cell);
void            adg_table_cell_set_name         (AdgTableCell   *cell,
                                                 const gchar    *name);
AdgEntity *     adg_table_cell_get_title        (AdgTableCell   *cell);
void            adg_table_cell_set_title        (AdgTableCell   *cell,
                                                 AdgEntity      *title);
void            adg_table_cell_set_text_title   (AdgTableCell   *cell,
                                                 const gchar    *title);
AdgEntity *     adg_table_cell_get_value        (AdgTableCell   *cell);
void            adg_table_cell_set_value        (AdgTableCell   *cell,
                                                 AdgEntity      *value);
void            adg_table_cell_set_value_pos    (AdgTableCell   *cell,
                                                 const AdgPair  *from_factor,
                                                 const AdgPair  *to_factor);
void            adg_table_cell_set_text_value   (AdgTableCell   *cell,
                                                 const gchar    *value);
void            adg_table_cell_set_width        (AdgTableCell   *cell,
                                                 gdouble         width);
gdouble         adg_table_cell_get_width        (AdgTableCell   *cell);
gboolean        adg_table_cell_has_frame        (AdgTableCell   *cell);
void            adg_table_cell_switch_frame     (AdgTableCell   *cell,
                                                 gboolean        new_state);
const CpmlExtents *
                adg_table_cell_extents          (AdgTableCell   *cell);

G_END_DECLS


#endif /* __ADG_TABLE_H__ */
