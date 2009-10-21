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
 * @short_description: A tabular entity
 *
 * The #AdgTable is the entity to be used for rendering data arranged
 * in tabular evironments.
 *
 * To define a table, you should add to its internal model any number
 * of row using adg_table_row_new() or adg_table_row_new_before().
 *
 * Every row should be segmented with different cells by using
 * adg_table_cell_new() or adg_table_cell_new_before(). Any cell can
 * be filled with a title and a value: the font to be used will be
 * picked up from the #AdgTableStyle got by resolving the
 * #AdgTable:table-dress property.
 **/

/**
 * AdgTable:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

/**
 * AdgTableRow:
 *
 * An opaque structure referring to a row of an #AdgTable. Any
 * table can have an unlimited number of rows.
 **/

/**
 * AdgTableCell:
 *
 * An opaque structure referring to the cell of an #AdgTableRow.
 * Any row can have an unlimited number of cells.
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

static void             dispose                 (GObject        *object);
static void             finalize                (GObject        *object);
static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             invalidate              (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             row_arrange_size        (AdgTableRow    *row);
static void             row_arrange_org         (AdgTableRow    *row);
static void             row_free                (AdgTableRow    *row);
static AdgTableCell *   cell_new                (AdgTableRow    *row,
                                                 AdgTableCell   *before_cell,
                                                 gdouble         padding,
                                                 gdouble         width,
                                                 const gchar    *name,
                                                 const gchar    *title,
                                                 const gchar    *value);
static void             cell_free               (AdgTableCell   *cell);
static gboolean         value_match             (gpointer        key,
                                                 gpointer        value,
                                                 gpointer        user_data);


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

    gobject_class->dispose = dispose;
    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->invalidate = invalidate;
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
    data->border = NULL;
    data->grid = NULL;
    data->rows = NULL;
    data->cell_names = NULL;

    table->data = data;
}

static void
dispose(GObject *object)
{
    invalidate((AdgEntity *) object);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
finalize(GObject *object)
{
    AdgTable *table;
    AdgTablePrivate *data;

    table = (AdgTable *) object;
    data = table->data;

    if (data->rows != NULL) {
        g_slist_foreach(data->rows, (GFunc) row_free, NULL);
        g_slist_free(data->rows);
    }

    if (data->cell_names != NULL)
        g_hash_table_destroy(data->cell_names);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
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

/**
 * adg_table_get_n_rows:
 * @table: an #AdgTable
 *
 * Gets the number of rows stored in @table.
 *
 * Returns: the number of rows or %0 on empty @table or errors
 **/
guint
adg_table_get_n_rows(AdgTable *table)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), 0);

    data = table->data;

    if (data->rows == NULL)
        return 0;

    return g_slist_length(data->rows);
}

/**
 * adg_table_row_new:
 * @table: an #AdgTable
 * @height: height of the row
 *
 * Creates a new empty row and appends it at the end of the rows
 * yet present in @table.
 *
 * Returns: the newly created row or %NULL on errors
 **/
AdgTableRow *
adg_table_row_new(AdgTable *table, gdouble height)
{
    AdgTablePrivate *data;
    AdgTableRow *new_row;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;
    new_row = g_new(AdgTableRow, 1);
    new_row->table = table;
    new_row->cells = NULL;
    new_row->extents.is_defined = FALSE;
    new_row->extents.size.y = height;

    data->rows = g_slist_append(data->rows, new_row);

    if (height != 0)
        invalidate((AdgEntity *) table);

    return new_row;
}

/**
 * adg_table_row_new_before:
 * @row: a valid #AdgTableRow
 * @height: height of the row
 *
 * Creates a new empty row and inserts it just before @row.
 *
 * Returns: the newly created row or %NULL on errors
 **/
AdgTableRow *
adg_table_row_new_before(AdgTableRow *row, gdouble height)
{
    AdgTable *table;
    AdgTablePrivate *data;
    GSList *row_node;
    AdgTableRow *new_row;

    g_return_val_if_fail(row != NULL, NULL);

    table = row->table;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;
    row_node = g_slist_find(data->rows, row);

    /* This MUST be present, otherwise something really bad happened */
    g_assert(row_node != NULL);

    new_row = g_new(AdgTableRow, 1);
    new_row->table = table;
    new_row->cells = NULL;
    new_row->extents.is_defined = FALSE;
    new_row->extents.size.y = height;

    data->rows = g_slist_insert_before(data->rows, row_node, new_row);

    if (height != 0)
        invalidate((AdgEntity *) table);

    return new_row;
}

/**
 * adg_table_row_delete:
 * @row: a valid #AdgTableRow
 *
 * Removes @row from its owner table and frees every resources allocated
 * by it. This means also the eventual cells owned by @row will be freed.
 **/
void
adg_table_row_delete(AdgTableRow *row)
{
    AdgTable *table;
    AdgTablePrivate *data;

    g_return_if_fail(row != NULL);

    table = row->table;

    g_return_if_fail(ADG_IS_TABLE(table));

    data = table->data;

    g_slist_foreach(row->cells, (GFunc) cell_free, NULL);
    g_slist_free(row->cells);
    data->rows = g_slist_remove(data->rows, row);

    g_free(row);
}

/**
 * adg_table_row_get_n_cells:
 * @row: a valid #AdgTableRow
 *
 * Gets the number of cells stored in @row.
 *
 * Returns: the number of cells or %0 on empty row or errors
 **/
guint
adg_table_row_get_n_cells(const AdgTableRow *row)
{
    g_return_val_if_fail(row != NULL, 0);

    if (row->cells == NULL)
        return 0;

    return g_slist_length(row->cells);
}

/**
 * adg_table_row_set_height:
 * @row: a valid #AdgTableRow
 * @height: the new height
 *
 * Sets a new height on @row. The extents will be invalidated, so
 * will be recomputed in the next arrange() phase.
 **/
void
adg_table_row_set_height(AdgTableRow *row, gdouble height)
{
    g_return_if_fail(row != NULL);

    row->extents.size.y = height;

    adg_entity_invalidate((AdgEntity *) row->table);
}

/**
 * adg_table_row_extents:
 * @row: a valid #AdgTableRow
 *
 * Gets the extents of @row. This function is useful only after the
 * arrange() phase as in the other situation the extents will likely
 * be not up to date.
 *
 * Returns: the extents of @row or %NULL on errors
 **/
const CpmlExtents *
adg_table_row_extents(AdgTableRow *row)
{
    g_return_val_if_fail(row != NULL, NULL);

    return &row->extents;
}

/**
 * adg_table_get_cell_by_name:
 * @table: an #AdgTable
 * @name: the name of a cell
 *
 * Gets the cell named @name inside @table.
 *
 * Returns: the requested cell or %NULL if not found
 **/
AdgTableCell *
adg_table_get_cell_by_name(AdgTable *table, const gchar *name)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;

    if (data->cell_names == NULL)
        return NULL;

    return g_hash_table_lookup(data->cell_names, name);
}

/**
 * adg_table_cell_new:
 * @row: a valid #AdgTableRow
 * @padding: empty space to add before the cell
 * @width: width of the cell
 * @name: name to associate
 * @title: title to render
 * @value: value to render
 *
 * Creates a new cell and appends it at the end of the cells
 * yet present in @row.
 *
 * @name is an optional identifier to univoquely access this cell
 * by using adg_table_get_cell_by_name(). The identifier must be
 * univoque, so it there is yet a cell with the same name this
 * function will fail.
 *
 * @title and @value could be %NULL, in which case nothing will
 * be rendered. There is still the possibility to change these
 * properties programmaticaly at a later time by using
 * adg_table_cell_set_title() and adg_table_cell_set_value().
 *
 * Returns: the newly created cell or %NULL on errors
 **/
AdgTableCell *
adg_table_cell_new(AdgTableRow *row, gdouble padding, gdouble width,
                   const gchar *name, const gchar *title, const gchar *value)
{
    g_return_val_if_fail(row != NULL, NULL);

    return cell_new(row, NULL, padding, width, name, title, value);
}

/**
 * adg_table_cell_new_before:
 * @cell: a valid #AdgTableCell
 * @padding: empty space to add before the cell
 * @width: width of the cell
 * @name: name to associate
 * @title: title to render
 * @value: value to render
 *
 * Creates a new cell and inserts it rigthly before the @cell cell.
 * This works similarily and accepts the same parameters as the
 * adg_table_cell_new() function.
 *
 * Returns: the newly created cell or %NULL on errors
 **/
AdgTableCell *
adg_table_cell_new_before(AdgTableCell *cell, gdouble padding,
                          gdouble width, const gchar *name,
                          const gchar *title, const gchar *value)
{
    g_return_val_if_fail(cell != NULL, NULL);
    g_return_val_if_fail(cell->row != NULL, NULL);

    return cell_new(cell->row, cell, padding, width, name, title, value);
}

/**
 * adg_table_cell_delete:
 * @cell: a valid #AdgTableCell
 *
 * Deletes @cell removing it from the container row and freeing
 * any resource associated to it.
 **/
void
adg_table_cell_delete(AdgTableCell *cell)
{
    AdgTableRow *row;

    g_return_if_fail(cell != NULL);

    row = cell->row;

    g_return_if_fail(row != NULL);

    cell_free(cell);
    row->cells = g_slist_remove(row->cells, cell);
}

/**
 * adg_table_cell_get_title:
 * @cell: a valid #AdgTableCell
 *
 * Gets the current title of @cell. The returned string is owned
 * by @cell and must not be modified or freed.
 *
 * Returns: the title text or %NULL on errors
 **/
const gchar *
adg_table_cell_get_title(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return cell->title;
}

/**
 * adg_table_cell_set_title:
 * @cell: a valid #AdgTableCell
 * @title: the new title to use
 *
 * Sets a new title on @cell.
 **/
void
adg_table_cell_set_title(AdgTableCell *cell, const gchar *title)
{
    g_return_if_fail(cell != NULL);

    g_free(cell->title);
    cell->title = g_strdup(title);
}

/**
 * adg_table_cell_get_value:
 * @cell: a valid #AdgTableCell
 *
 * Gets the current value of @cell. The returned string is owned
 * by @cell and must not be modified or freed.
 *
 * Returns: the value text or %NULL on errors
 **/
const gchar *
adg_table_cell_get_value(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return cell->value;
}

/**
 * adg_table_cell_set_value:
 * @cell: a valid #AdgTableCell
 * @value: the new value to use
 *
 * Sets a new value on @cell.
 **/
void
adg_table_cell_set_value(AdgTableCell *cell, const gchar *value)
{
    g_return_if_fail(cell != NULL);

    g_free(cell->value);
    cell->value = g_strdup(value);
}

/**
 * adg_table_cell_set_width:
 * @cell: a valid #AdgTableCell
 * @width: the new width
 *
 * Sets a new width on @cell. The extents on the whole table
 * will be invalidated, so will be recomputed in the next
 * arrange() phase.
 **/
void
adg_table_cell_set_width(AdgTableCell *cell, gdouble width)
{
    g_return_if_fail(cell != NULL);

    cell->extents.size.x = width;

    adg_entity_invalidate((AdgEntity *) cell->row->table);
}

/**
 * adg_table_cell_extents:
 * @cell: a valid #AdgTableCell
 *
 * Gets the extents of @cell. This function is useful only after the
 * arrange() phase as in the other situation the extents will likely
 * be not up to date.
 *
 * Returns: the extents of @cell or %NULL on errors
 **/
const CpmlExtents *
adg_table_cell_extents(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return &cell->extents;
}


static void
invalidate(AdgEntity *entity)
{
    AdgTablePrivate *data = ((AdgTable *) entity)->data;

    if (data->border != NULL) {
        g_object_unref(data->border);
        data->border = NULL;
    }

    if (data->grid != NULL) {
        g_object_unref(data->grid);
        data->grid = NULL;
    }
}

static void
arrange(AdgEntity *entity)
{
    AdgTable *table;
    AdgTablePrivate *data;
    GSList *row_node;
    AdgTableRow *row;

    table = (AdgTable *) entity;
    data = table->data;

    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;
        row_arrange_size(row);
    }

    /* TODO: update the org according to the table alignments */

    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;
        row_arrange_org(row);
    }

    data = table->data;

    /* TODO: update border, grid and build the toy-text */
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

static void
row_arrange_size(AdgTableRow *row)
{
    AdgTableCell *cell;
    GSList *cell_node;

    row->extents.size.x = 0;

    /* Force all the cells of this row to the same height and
     * compute the row width by summing every cell width */
    for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;

        cell->extents.size.y = row->extents.size.y;

        row->extents.size.x += cell->extents.size.x;
    }
}

static void
row_arrange_org(AdgTableRow *row)
{
    AdgTableCell *cell;
    GSList *cell_node;

    for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;

        cell->extents.org.x = row->extents.org.x + row->extents.size.x;
        cell->extents.org.y = row->extents.org.y;
        cell->extents.is_defined = TRUE;
    }

    row->extents.is_defined = TRUE;
}

static void
row_free(AdgTableRow *row)
{
    g_slist_foreach(row->cells, (GFunc) cell_free, NULL);
    g_slist_free(row->cells);

    g_free(row);
}

static AdgTableCell *
cell_new(AdgTableRow *row, AdgTableCell *before_cell,
         gdouble padding, gdouble width,
         const gchar *name, const gchar *title, const gchar *value)
{
    AdgTablePrivate *data;
    AdgTableCell *new_cell;

    data = row->table->data;

    if (name != NULL) {
        if (data->cell_names == NULL) {
            data->cell_names = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                     g_free, NULL);
        } else if (g_hash_table_lookup(data->cell_names, name) != NULL) {
            g_warning(_("%s: `%s' cell name is yet used"),
                      G_STRLOC, name);
            return NULL;
        }
    }

    new_cell = g_new(AdgTableCell, 1);
    new_cell->row = row;
    new_cell->title = g_strdup(title);
    new_cell->value = g_strdup(value);
    new_cell->extents.is_defined = FALSE;
    new_cell->extents.size.x = width;

    if (before_cell == NULL) {
        row->cells = g_slist_append(row->cells, new_cell);
    } else {
        GSList *before_node = g_slist_find(row->cells, before_cell);

        row->cells = g_slist_insert_before(row->cells, before_node, new_cell);
    }

    if (name != NULL)
        g_hash_table_insert(data->cell_names, g_strdup(name), new_cell);

    return new_cell;
}

static void
cell_free(AdgTableCell *cell)
{
    AdgTablePrivate *data = cell->row->table->data;

    g_hash_table_foreach_remove(data->cell_names, value_match, cell);

    g_free(cell->title);
    g_free(cell->value);
    g_free(cell);
}

static gboolean
value_match(gpointer key, gpointer value, gpointer user_data)
{
    return value == user_data;
}
