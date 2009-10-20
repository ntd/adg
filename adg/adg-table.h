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


GType           adg_table_get_type              (void) G_GNUC_CONST;

AdgTable *      adg_table_new                   (void);

AdgDress        adg_table_get_table_dress       (AdgTable       *table);
void            adg_table_set_table_dress       (AdgTable       *table,
                                                 AdgDress        dress);

G_END_DECLS


#endif /* __ADG_TABLE_H__ */
