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


/**
 * SECTION:adg-table
 * @short_description: A tabled entity
 *
 * The #AdgTable is a simple table of evenly spaced rows. Every cell
 * can optionally contains a title and a value text.
 **/

/**
 * AdgTable:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-table.h"
#include "adg-table-private.h"
#include "adg-dress-builtins.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_table_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_table_parent_class)


enum {
    PROP_0,
    PROP_TABLE_DRESS
};

static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);


G_DEFINE_TYPE(AdgTable, adg_table, ADG_TYPE_ENTITY);


static void
adg_table_class_init(AdgTableClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTablePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->arrange = arrange;
    entity_class->render = render;

    param = adg_param_spec_dress("table-dress",
                                 P_("Table Dress"),
                                 P_("The dress to use for stroking this entity"),
                                 ADG_DRESS_TABLE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TABLE_DRESS, param);
}

static void
adg_table_init(AdgTable *table)
{
    AdgTablePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(table,
                                                         ADG_TYPE_TABLE,
                                                         AdgTablePrivate);

    data->table_dress = ADG_DRESS_TABLE;

    table->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgTablePrivate *data = ((AdgTable *) object)->data;

    switch (prop_id) {
    case PROP_TABLE_DRESS:
        g_value_set_int(value, data->table_dress);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgTable *table;
    AdgTablePrivate *data;

    table = (AdgTable *) object;
    data = table->data;

    switch (prop_id) {
    case PROP_TABLE_DRESS:
        adg_dress_set(&data->table_dress, g_value_get_int(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_table_new:
 *
 * Creates a new table entity.
 *
 * Returns: the newly created table entity
 **/
AdgTable *
adg_table_new(void)
{
    return g_object_new(ADG_TYPE_TABLE, NULL);
}

/**
 * adg_table_get_table_dress:
 * @table: an #AdgTable
 *
 * Gets the table dress to be used in rendering @table.
 *
 * Returns: the current table dress
 **/
AdgDress
adg_table_get_table_dress(AdgTable *table)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), ADG_DRESS_UNDEFINED);

    data = table->data;

    return data->table_dress;
}

/**
 * adg_table_set_table_dress:
 * @table: an #AdgTable
 * @dress: the new #AdgDress to use
 *
 * Sets a new table dress for rendering @table. The new dress
 * must be related to the original dress for this property:
 * you cannot set a dress used for line styles to a dress
 * managing fonts.
 *
 * The check is done by calling adg_dress_are_related() with
 * @dress and the previous dress as arguments. Check out its
 * documentation for details on what is a related dress.
 **/
void
adg_table_set_table_dress(AdgTable *table, AdgDress dress)
{
    AdgTablePrivate *data;

    g_return_if_fail(ADG_IS_TABLE(table));

    data = table->data;

    if (adg_dress_set(&data->table_dress, dress))
        g_object_notify((GObject *) table, "table-dress");
}


static void
arrange(AdgEntity *entity)
{
    /* TODO */
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgTable *table;
    AdgTablePrivate *data;

    table = (AdgTable *) entity;
    data = table->data;

    if (data->border != NULL)
        adg_entity_render((AdgEntity *) data->border, cr);

    if (data->grid != NULL)
        adg_entity_render((AdgEntity *) data->grid, cr);
}
