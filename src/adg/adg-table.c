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


/**
 * SECTION:adg-table
 * @short_description: A tabular entity
 *
 * The #AdgTable is the entity to be used for rendering data arranged
 * in tabular evironments.
 *
 * To define a table, you should add to it a serie of one or more
 * #AdgTableRow by using the #AdgTableRow specific APIs.
 *
 * <note><para>
 * By default, the #AdgText:local-mix property is set to
 * #ADG_MIX_DISABLED on #AdgTable entities.
 * </para></note>
 *
 * Since: 1.0
 **/

/**
 * AdgTable:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-model.h"
#include "adg-trail.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"
#include "adg-style.h"
#include "adg-table-style.h"
#include "adg-path.h"
#include "adg-stroke.h"
#include "adg-container.h"
#include "adg-alignment.h"
#include "adg-entity-private.h"

#include "adg-table.h"
#include "adg-table-private.h"
#include "adg-table-row.h"
#include "adg-table-cell.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_table_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_table_parent_class)


G_DEFINE_TYPE(AdgTable, adg_table, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_TABLE_DRESS,
    PROP_HAS_FRAME
};

typedef struct {
    GCallback       callback;
    gpointer        user_data;
} AdgClosure;



static void         _adg_dispose            (GObject        *object);
static void         _adg_finalize           (GObject        *object);
static void         _adg_get_property       (GObject        *object,
                                             guint           param_id,
                                             GValue         *value,
                                             GParamSpec     *pspec);
static void         _adg_set_property       (GObject        *object,
                                             guint           param_id,
                                             const GValue   *value,
                                             GParamSpec     *pspec);
static void         _adg_destroy            (AdgEntity      *entity);
static void         _adg_global_changed     (AdgEntity      *entity);
static void         _adg_local_changed      (AdgEntity      *entity);
static void         _adg_invalidate         (AdgEntity      *entity);
static void         _adg_arrange            (AdgEntity      *entity);
static void         _adg_arrange_grid       (AdgEntity      *entity);
static void         _adg_arrange_frame      (AdgEntity      *entity,
                                             const CpmlExtents *extents);
static void         _adg_render             (AdgEntity      *entity,
                                             cairo_t        *cr);
static void         _adg_propagate          (AdgTable       *table,
                                             const gchar    *detailed_signal,
                                             ...);
static void         _adg_foreach_row        (AdgTableRow    *table_row,
                                             const AdgClosure *closure);
static void         _adg_append_frame       (AdgTableCell   *table_cell,
                                             AdgPath        *path);
static void         _adg_proxy_signal       (AdgTableCell   *table_cell,
                                             AdgProxyData   *proxy_data);
static gboolean     _adg_value_match        (gpointer        key,
                                             gpointer        value,
                                             gpointer        user_data);


static void
adg_table_class_init(AdgTableClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTablePrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->destroy = _adg_destroy;
    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    param = adg_param_spec_dress("table-dress",
                                 P_("Table Dress"),
                                 P_("The dress to use for stroking this entity"),
                                 ADG_DRESS_TABLE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TABLE_DRESS, param);

    param = g_param_spec_boolean("has-frame",
                                 P_("Has Frame Flag"),
                                 P_("If enabled, a frame using the proper dress found in this table style will be drawn around the table extents"),
                                 TRUE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HAS_FRAME, param);
}

static void
adg_table_init(AdgTable *table)
{
    AdgTablePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(table,
                                                        ADG_TYPE_TABLE,
                                                        AdgTablePrivate);
    AdgEntityPrivate *entity_data = ((AdgEntity *) table)->data;

    data->table_dress = ADG_DRESS_TABLE;
    data->has_frame = TRUE;

    data->table_style = NULL;
    data->grid = NULL;
    data->frame = NULL;
    data->rows = NULL;
    data->cell_names = NULL;

    table->data = data;

    /* Initialize to custom default some AdgEntity field by directly
     * accessing the private struct to avoid notify signal emissions
     */
    entity_data->local_mix = ADG_MIX_DISABLED;
}

static void
_adg_dispose(GObject *object)
{
    AdgTable *table = (AdgTable *) object;
    AdgTablePrivate *data = table->data;

    adg_table_invalidate_grid(table);

    if (data->frame) {
        g_object_unref(data->frame);
        data->frame = NULL;
    }

    if (data->rows) {
        adg_table_foreach_cell(table,
                               (GCallback) adg_table_cell_dispose, NULL);
        data->rows = NULL;
    }

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_finalize(GObject *object)
{
    AdgTable *table;
    AdgTablePrivate *data;

    table = (AdgTable *) object;
    data = table->data;

    if (data->rows) {
        g_slist_foreach(data->rows, (GFunc) adg_table_row_free, NULL);
        g_slist_free(data->rows);
    }

    if (data->cell_names)
        g_hash_table_destroy(data->cell_names);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgTablePrivate *data = ((AdgTable *) object)->data;

    switch (prop_id) {
    case PROP_TABLE_DRESS:
        g_value_set_int(value, data->table_dress);
        break;
    case PROP_HAS_FRAME:
        g_value_set_boolean(value, data->has_frame);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgTablePrivate *data = ((AdgTable *) object)->data;

    switch (prop_id) {
    case PROP_TABLE_DRESS:
        data->table_dress = g_value_get_int(value);
        break;
    case PROP_HAS_FRAME:
        data->has_frame = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_table_new:
 *
 * Creates a new empty table entity.
 *
 * Returns: the newly created table entity
 *
 * Since: 1.0
 **/
AdgTable *
adg_table_new(void)
{
    return g_object_new(ADG_TYPE_TABLE, NULL);
}

/**
 * adg_table_insert:
 * @table: an #AdgTable
 * @table_row: a valid #AdgTableRow
 * @before_row: (allow-none): an #AdgTableRow or %NULL
 *
 * Inserts @table_row inside the rows list of @table. If @before_row
 * is specified, @table_row is inserted before it.
 *
 * Since: 1.0
 **/
void
adg_table_insert(AdgTable *table, AdgTableRow *table_row,
                 AdgTableRow *before_row)
{
    AdgTablePrivate *data;

    g_return_if_fail(ADG_IS_TABLE(table));
    g_return_if_fail(table_row != NULL);

    data = table->data;

    if (before_row == NULL) {
        data->rows = g_slist_append(data->rows, table_row);
    } else {
        GSList *before = g_slist_find(data->rows, before_row);

        /* This MUST be present, otherwise something really bad happened */
        g_return_if_fail(before != NULL);

        data->rows = g_slist_insert_before(data->rows, before, table_row);
    }
}

/**
 * adg_table_remove:
 * @table: an #AdgTable
 * @table_row: a valid #AdgTableRow
 *
 * Removes @table_row from list of rows of @table.
 *
 * Since: 1.0
 **/
void
adg_table_remove(AdgTable *table, AdgTableRow *table_row)
{
    AdgTablePrivate *data;

    g_return_if_fail(ADG_IS_TABLE(table));
    g_return_if_fail(table_row != NULL);

    data = table->data;
    data->rows = g_slist_remove(data->rows, table_row);
}

/**
 * adg_table_foreach:
 * @table: an #AdgTable
 * @callback: (scope call): a callback
 * @user_data: callback user data
 *
 * Invokes @callback on each row of @table.
 * The callback should be declared as:
 *
 * |[
 * void callback(AdgTableRow *table_row, gpointer user_data);
 * ]|
 *
 * Since: 1.0
 **/
void
adg_table_foreach(AdgTable *table, GCallback callback, gpointer user_data)
{
    AdgTablePrivate *data = table->data;

    g_return_if_fail(table != NULL);
    g_return_if_fail(callback != NULL);

    g_slist_foreach(data->rows, (GFunc) callback, user_data);
}

/**
 * adg_table_foreach_cell:
 * @table: an #AdgTable
 * @callback: (scope call): a callback
 * @user_data: callback user data
 *
 * Invokes @callback on each cell of @table.
 * The callback should be declared as:
 *
 * |[
 * void callback(AdgTableCell *table_cell, gpointer user_data);
 * ]|
 *
 * Since: 1.0
 **/
void
adg_table_foreach_cell(AdgTable *table,
                       GCallback callback, gpointer user_data)
{
    AdgClosure closure = { callback, user_data };
    adg_table_foreach(table, (GCallback) _adg_foreach_row, &closure);
}

/**
 * adg_table_set_cell:
 * @table: an #AdgTable
 * @name: the name of the cell
 * @table_cell: the named cell
 *
 * Binds @table_cell to @name, so it can be accessed by name later
 * with adg_table_get_cell(). Internally the binding is handled with
 * an hash table, so accessing the cell this way is O(1).
 *
 * If @name is %NULL, any binding to @ŧable_cell will be removed.
 * This is quite inefficient because the whole hash table must be scanned.
 *
 * If @table_cell is %NULL, the key with @name in the hash table will
 * be removed.
 *
 * Both @name and @table_cell cannot be %NULL at the same time.
 *
 * Since: 1.0
 **/
void
adg_table_set_cell(AdgTable *table, const gchar *name,
                   AdgTableCell *table_cell)
{
    AdgTablePrivate *data;

    g_return_if_fail(ADG_IS_TABLE(table));
    g_return_if_fail(name != NULL || table_cell != NULL);

    data = table->data;

    if (data->cell_names == NULL) {
        /* Check if trying to remove from an empty hash table */
        if (name == NULL || table_cell == NULL)
            return;

        data->cell_names = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                 g_free, NULL);
    }

    if (name == NULL) {
        /* _adg_value_match() will return the key in user_data[1] */
        gpointer user_data[] = { table_cell, NULL };
        g_hash_table_find(data->cell_names, _adg_value_match, user_data);
        g_hash_table_remove(data->cell_names, user_data[1]);
    } else if (table_cell == NULL) {
        g_hash_table_remove(data->cell_names, name);
    } else {
        g_hash_table_insert(data->cell_names, g_strdup(name), table_cell);
    }
}

/**
 * adg_table_get_table_style:
 * @table: an #AdgTable
 *
 * Gets the #AdgTableStyle explicitely set on @table. This is a kind
 * of accessor function: for rendering purpose use adg_entity_style()
 * instead. The returned object is owned by @table and should not be
 * freed or modified.
 *
 * Returns: (transfer none): the requested style or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgStyle *
adg_table_get_table_style(AdgTable *table)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;
    return (AdgStyle *) data->table_style;
}

/**
 * adg_table_get_cell:
 * @table: an #AdgTable
 * @name: the name of a cell
 *
 * Gets the cell named @name inside @table. Only named cells
 * can be retrieved by this method.
 *
 * The returned cell is owned by @ŧable and must not be
 * modified or freed.
 *
 * Returns: (transfer none): the requested cell or %NULL if not found.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_get_cell(AdgTable *table, const gchar *name)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;

    if (data->cell_names == NULL)
        return NULL;

    return g_hash_table_lookup(data->cell_names, name);
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
 *
 * Since: 1.0
 **/
void
adg_table_set_table_dress(AdgTable *table, AdgDress dress)
{
    g_return_if_fail(ADG_IS_TABLE(table));
    g_object_set(table, "table-dress", dress, NULL);
}

/**
 * adg_table_get_table_dress:
 * @table: an #AdgTable
 *
 * Gets the table dress to be used in rendering @table.
 *
 * Returns: (transfer none): the current table dress.
 *
 * Since: 1.0
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
 * adg_table_switch_frame:
 * @table: an #AdgTable
 * @new_state: the new state of the frame
 *
 * Sets the #AdgTable:has-frame property: %TRUE will draw a
 * frame around the whole table using the #AdgTableStyle:frame-dress
 * dress of the table style.
 *
 * Since: 1.0
 **/
void
adg_table_switch_frame(AdgTable *table, gboolean new_state)
{
    g_return_if_fail(ADG_IS_TABLE(table));
    g_object_set(table, "has-frame", new_state, NULL);
}

/**
 * adg_table_has_frame:
 * @table: an #AdgTable
 *
 * Returns the state of the #AdgTable:has-frame property.
 *
 * Returns: the current state.
 *
 * Since: 1.0
 **/
gboolean
adg_table_has_frame(AdgTable *table)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), FALSE);

    data = table->data;

    return data->has_frame;
}

/**
 * adg_table_invalidate_grid:
 * @table: an #AdgTable
 *
 * <note><para>
 * This method is only useful in table children implementation.
 * </para></note>
 *
 * Clears the internal grid cache, effectively forcing its
 * regeneration next time the #AdgEntity::arrange signal is emitted.
 **/
void
adg_table_invalidate_grid(AdgTable *table)
{
    AdgTablePrivate *data;

    g_return_if_fail(ADG_IS_TABLE(table));

    data = table->data;

    if (data->grid) {
        g_object_unref(data->grid);
        data->grid = NULL;
    }
}


static void
_adg_destroy(AdgEntity *entity)
{
    _adg_propagate((AdgTable *) entity, "destroy");

    if (_ADG_OLD_ENTITY_CLASS->destroy)
        _ADG_OLD_ENTITY_CLASS->destroy(entity);
}

static void
_adg_global_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    _adg_propagate((AdgTable *) entity, "global-changed");
}

static void
_adg_local_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    _adg_propagate((AdgTable *) entity, "local-changed");
}

static void
_adg_invalidate(AdgEntity *entity)
{
    _adg_propagate((AdgTable *) entity, "invalidate");
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgTable *table;
    AdgTablePrivate *data;
    CpmlExtents extents = { 0 };
    CpmlExtents row_layout = { 0 };
    const CpmlExtents *row_extents;
    const CpmlPair *spacing;
    const CpmlPair *size;
    GSList *row_node;
    AdgTableRow *row;

    table = (AdgTable *) entity;
    data = table->data;

    /* Resolve the table style */
    if (data->table_style == NULL)
        data->table_style = (AdgTableStyle *)
            adg_entity_style(entity, data->table_dress);

    spacing = adg_table_style_get_cell_spacing(data->table_style);

    /* Compute the size of the table */
    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;
        size = adg_table_row_size_request(row);

        if (size->x > extents.size.x)
            extents.size.x = size->x;
        extents.size.y += size->y;
    }

    /* Arrange the layout of the table components */
    row_layout.org.x = extents.org.x;
    row_layout.org.y = extents.org.y + spacing->y;
    row_layout.size.x = extents.size.x;
    row_layout.size.y = -1;
    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;
        row_extents = adg_table_row_arrange(row, &row_layout);
        row_layout.org.y += row_extents->size.y + spacing->y;
    }

    _adg_arrange_grid(entity);
    _adg_arrange_frame(entity, &extents);

    extents.is_defined = TRUE;
    cpml_extents_transform(&extents, adg_entity_get_global_matrix(entity));
    cpml_extents_transform(&extents, adg_entity_get_local_matrix(entity));
    adg_entity_set_extents(entity, &extents);
}

static void
_adg_arrange_grid(AdgEntity *entity)
{
    AdgTable *table;
    AdgTablePrivate *data;
    AdgPath *path;
    AdgTrail *trail;
    AdgDress dress;

    table = (AdgTable *) entity;
    data = table->data;

    if (data->grid)
        return;

    path = adg_path_new();
    trail = (AdgTrail *) path;

    adg_table_foreach_cell(table, (GCallback) _adg_append_frame, path);

    if (!adg_trail_get_extents(trail)->is_defined)
        return;

    dress = adg_table_style_get_grid_dress(data->table_style);
    data->grid = g_object_new(ADG_TYPE_STROKE,
                              "line-dress", dress,
                              "trail", trail,
                              "parent", entity,
                              NULL);
    adg_entity_arrange((AdgEntity *) data->grid);
}

static void
_adg_arrange_frame(AdgEntity *entity, const CpmlExtents *extents)
{
    AdgTablePrivate *data;
    AdgPath *path;
    AdgTrail *trail;
    CpmlPair pair;
    AdgDress dress;

    data = ((AdgTable *) entity)->data;

    if (data->frame || !data->has_frame)
        return;

    path = adg_path_new();
    trail = (AdgTrail *) path;

    cpml_pair_copy(&pair, &extents->org);
    adg_path_move_to(path, &pair);
    pair.x += extents->size.x;
    adg_path_line_to(path, &pair);
    pair.y += extents->size.y;
    adg_path_line_to(path, &pair);
    pair.x -= extents->size.x;
    adg_path_line_to(path, &pair);
    adg_path_close(path);

    dress = adg_table_style_get_frame_dress(data->table_style);
    data->frame = g_object_new(ADG_TYPE_STROKE,
                               "line-dress", dress,
                               "trail", trail,
                               "parent", entity,
                               NULL);
    adg_entity_arrange((AdgEntity *) data->frame);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgTablePrivate *data = ((AdgTable *) entity)->data;

    adg_style_apply((AdgStyle *) data->table_style, entity, cr);

    _adg_propagate((AdgTable *) entity, "render", cr);
}

static void
_adg_propagate(AdgTable *table, const gchar *detailed_signal, ...)
{
    va_list var_copy;
    AdgTablePrivate *data;
    AdgProxyData proxy_data;

    if (!g_signal_parse_name(detailed_signal, G_TYPE_FROM_INSTANCE(table),
                             &proxy_data.signal_id, &proxy_data.detail, FALSE)) {
        g_return_if_reached();
    }

    va_start(proxy_data.var_args, detailed_signal);
    data = table->data;

    if (data->frame) {
        G_VA_COPY(var_copy, proxy_data.var_args);
        g_signal_emit_valist(data->frame, proxy_data.signal_id,
                             proxy_data.detail, var_copy);
    }

    if (data->grid) {
        G_VA_COPY(var_copy, proxy_data.var_args);
        g_signal_emit_valist(data->grid, proxy_data.signal_id,
                             proxy_data.detail, var_copy);
    }

    adg_table_foreach_cell(table, (GCallback) _adg_proxy_signal, &proxy_data);
    va_end(proxy_data.var_args);
}

static void
_adg_foreach_row(AdgTableRow *table_row, const AdgClosure *closure)
{
    adg_table_row_foreach(table_row, closure->callback, closure->user_data);
}

static void
_adg_append_frame(AdgTableCell *table_cell, AdgPath *path)
{
    CpmlPair pair;
    const CpmlExtents *extents;

    if (! adg_table_cell_has_frame(table_cell))
        return;

    extents = adg_table_cell_get_extents(table_cell);

    cpml_pair_copy(&pair, &extents->org);
    adg_path_move_to(path, &pair);
    pair.x += extents->size.x;
    adg_path_line_to(path, &pair);
    pair.y += extents->size.y;
    adg_path_line_to(path, &pair);
    pair.x -= extents->size.x;
    adg_path_line_to(path, &pair);
    adg_path_close(path);
}

static void
_adg_proxy_signal(AdgTableCell *table_cell, AdgProxyData *proxy_data)
{
    AdgEntity *entity;
    AdgAlignment *alignment;
    va_list var_copy;

    entity = adg_table_cell_title(table_cell);
    if (entity) {
        alignment = (AdgAlignment *) adg_entity_get_parent(entity);
        G_VA_COPY(var_copy, proxy_data->var_args);
        g_signal_emit_valist(alignment, proxy_data->signal_id,
                             proxy_data->detail, var_copy);
    }

    entity = adg_table_cell_value(table_cell);
    if (entity) {
        alignment = (AdgAlignment *) adg_entity_get_parent(entity);
        G_VA_COPY(var_copy, proxy_data->var_args);
        g_signal_emit_valist(alignment, proxy_data->signal_id,
                             proxy_data->detail, var_copy);
    }
}

static gboolean
_adg_value_match(gpointer key, gpointer value, gpointer user_data)
{
    gpointer *array = user_data;

    if (value == array[0]) {
        array[1] = key;
        return TRUE;
    }
    return FALSE;
}
