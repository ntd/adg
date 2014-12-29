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


#ifndef __ADG_TABLE_H__
#define __ADG_TABLE_H__


G_BEGIN_DECLS

#define ADG_TYPE_TABLE             (adg_table_get_type())
#define ADG_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TABLE, AdgTable))
#define ADG_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TABLE, AdgTableClass))
#define ADG_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TABLE))
#define ADG_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TABLE))
#define ADG_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TABLE, AdgTableClass))


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


GType           adg_table_get_type              (void);

AdgTable *      adg_table_new                   (void);

void            adg_table_insert                (AdgTable       *table,
                                                 AdgTableRow    *table_row,
                                                 AdgTableRow    *before_row);
void            adg_table_remove                (AdgTable       *table,
                                                 AdgTableRow    *table_row);
void            adg_table_foreach               (AdgTable       *table,
                                                 GCallback       callback,
                                                 gpointer        user_data);
void            adg_table_foreach_cell          (AdgTable       *table,
                                                 GCallback       callback,
                                                 gpointer        user_data);
void            adg_table_set_cell              (AdgTable       *table,
                                                 const gchar    *name,
                                                 AdgTableCell   *table_cell);
AdgTableCell *  adg_table_get_cell              (AdgTable       *table,
                                                 const gchar    *name);
AdgStyle *      adg_table_get_table_style       (AdgTable       *table);
void            adg_table_set_table_dress       (AdgTable       *table,
                                                 AdgDress        dress);
AdgDress        adg_table_get_table_dress       (AdgTable       *table);
void            adg_table_switch_frame          (AdgTable       *table,
                                                 gboolean        new_state);
gboolean        adg_table_has_frame             (AdgTable       *table);
void            adg_table_invalidate_grid       (AdgTable       *table);

G_END_DECLS


#endif /* __ADG_TABLE_H__ */
