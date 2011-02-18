/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * Every row could be segmented with different cells by using
 * adg_table_cell_new() or adg_table_cell_new_before(). Any cell can
 * be filled with a title and a value: the font to be used will be
 * picked up from the #AdgTableStyle got by resolving the
 * #AdgTable:table-dress property.
 *
 * The default title is placed at the upper left corner of the cell
 * while the value is centered up to the bottom edge of the cell.
 * Anyway, the value position can be customized by using the
 * adg_table_cell_set_value_pos() method. Anyway, both entities react
 * to the common map displacements.
 *
 * Some convenient functions to easily create title and value entities
 * with plain text are provided: adg_table_cell_new_full(),
 * adg_table_cell_set_text_title() and adg_table_cell_set_text_value().
 * When using these methods keep in mind the underlying #AdgToyText
 * entities will be displaced accordingly to the
 * #AdgTableStyle:cell-padding value (not used when setting the
 * entities throught other APIs).
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


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"
#include "adg-style.h"
#include "adg-table-style.h"
#include "adg-path.h"
#include "adg-stroke.h"
#include "adg-textual.h"
#include "adg-toy-text.h"
#include "adg-container.h"
#include "adg-alignment.h"

#include "adg-table.h"
#include "adg-table-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_table_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_table_parent_class)


G_DEFINE_TYPE(AdgTable, adg_table, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_TABLE_DRESS,
    PROP_HAS_FRAME
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_finalize           (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_global_changed     (AdgEntity      *entity);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_invalidate         (AdgEntity      *entity);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_arrange_grid       (AdgEntity      *entity);
static void             _adg_arrange_frame      (AdgEntity      *entity,
                                                 const CpmlExtents *extents);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_propagate          (AdgTable       *table,
                                                 const gchar    *detailed_signal,
                                                 ...);
static AdgTableRow *    _adg_row_new            (AdgTable       *table,
                                                 AdgTableRow    *before_row);
static void             _adg_row_arrange        (AdgTableRow    *row);
static void             _adg_row_arrange_size   (AdgTableRow    *row);
static void             _adg_row_dispose        (AdgTableRow    *row);
static void             _adg_row_free           (AdgTableRow    *row);
static AdgTableCell *   _adg_cell_new           (AdgTableRow    *row,
                                                 AdgTableCell   *before_cell,
                                                 gdouble         width,
                                                 gboolean        has_frame,
                                                 const gchar    *name,
                                                 AdgEntity      *title,
                                                 AdgEntity      *value);
static void             _adg_cell_set_name           (AdgTableCell   *cell,
                                                 const gchar    *name);
static gboolean         _adg_cell_set_title     (AdgTableCell   *cell,
                                                 AdgEntity      *title);
static gboolean         _adg_cell_set_value     (AdgTableCell   *cell,
                                                 AdgEntity      *value);
static void             _adg_cell_set_value_pos (AdgTableCell   *cell,
                                                 const AdgPair  *from_factor,
                                                 const AdgPair  *to_factor);
static void             _adg_cell_arrange       (AdgTableCell   *cell);
static void             _adg_cell_arrange_size  (AdgTableCell   *cell);
static void             _adg_cell_dispose       (AdgTableCell   *cell);
static void             _adg_cell_free          (AdgTableCell   *cell);
static gboolean         _adg_value_match        (gpointer        key,
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

    data->table_dress = ADG_DRESS_TABLE;
    data->has_frame = TRUE;

    data->table_style = NULL;
    data->grid = NULL;
    data->frame = NULL;
    data->rows = NULL;
    data->cell_names = NULL;

    table->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgTablePrivate *data = ((AdgTable *) object)->data;

    if (data->grid) {
        g_object_unref(data->grid);
        data->grid = NULL;
    }

    if (data->frame) {
        g_object_unref(data->frame);
        data->frame = NULL;
    }

    /* The rows finalization will happen in the finalize() method */
    if (data->rows)
        g_slist_foreach(data->rows, (GFunc) _adg_row_dispose, NULL);

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
        g_slist_foreach(data->rows, (GFunc) _adg_row_free, NULL);
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
 * Creates a new empty table entity. The #AdgEntity:local-method
 * property is set by default to #ADG_MIX_DISABLED, that is the
 * table is not subject to any local transformations.
 *
 * Returns: the newly created table entity
 **/
AdgTable *
adg_table_new(void)
{
    return g_object_new(ADG_TYPE_TABLE,
                        "local-method", ADG_MIX_DISABLED,
                        NULL);
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
    g_return_if_fail(ADG_IS_TABLE(table));
    g_object_set(table, "table-dress", dress, NULL);
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
 * adg_table_switch_frame:
 * @table: an #AdgTable
 * @new_state: the new state of the frame
 *
 * Sets the #AdgTable:has-frame property: %TRUE will draw a
 * frame around the whole table using the #AdgTableStyle:frame-dress
 * dress of the table style.
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
 * Returns: the current state
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
 *
 * Creates a new empty row and appends it at the end of the rows
 * yet present in @table. By default, the height of this new
 * row will be the fallback value provided by the table style:
 * you can override it by using adg_table_row_set_height().
 *
 * Returns: the newly created row or %NULL on errors
 **/
AdgTableRow *
adg_table_row_new(AdgTable *table)
{
    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    return _adg_row_new(table, NULL);
}

/**
 * adg_table_row_new_before:
 * @row: a valid #AdgTableRow
 *
 * Creates a new empty row with default height and inserts it
 * just before @row.
 *
 * Returns: the newly created row or %NULL on errors
 **/
AdgTableRow *
adg_table_row_new_before(AdgTableRow *row)
{
    g_return_val_if_fail(row != NULL, NULL);
    g_return_val_if_fail(ADG_IS_TABLE(row->table), NULL);

    return _adg_row_new(row->table, row);
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

    g_slist_foreach(row->cells, (GFunc) _adg_cell_free, NULL);
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
 * Sets a new height on @row. The extents will be invalidated to
 * recompute the whole layout of the table. Specifying %0 in
 * @height will use the default height set in the table style.
 **/
void
adg_table_row_set_height(AdgTableRow *row, gdouble height)
{
    g_return_if_fail(row != NULL);

    row->height = height;

    adg_entity_invalidate((AdgEntity *) row->table);
}

/**
 * adg_table_row_get_height:
 * @row: a valid #AdgTableRow
 *
 * Gets the height of @row.
 *
 * Returns: the requested height or %0 on errors
 **/
gdouble
adg_table_row_get_height(AdgTableRow *row)
{
    g_return_val_if_fail(row != NULL, 0.);

    return row->height;
}

/**
 * adg_table_row_get_extents:
 * @row: a valid #AdgTableRow
 *
 * Gets the extents of @row. This function is useful only after the
 * arrange() phase as in the other situation the extents will likely
 * be not up to date.
 *
 * Returns: the extents of @row or %NULL on errors
 **/
const CpmlExtents *
adg_table_row_get_extents(AdgTableRow *row)
{
    g_return_val_if_fail(row != NULL, NULL);

    return &row->extents;
}

/**
 * adg_table_cell_new:
 * @row: a valid #AdgTableRow
 * @width: width of the cell
 *
 * Creates a new empty cell without a frame and appends it at the
 * end of the cells yet present in @row. You can add content to the
 * cell by using adg_table_cell_set_title() and
 * adg_table_cell_set_value() or enable the frame with
 * adg_table_cell_switch_frame().
 *
 * A positive @width value specifies the width of this cell in global
 * space: if the width of its content (that is, either the title or the
 * value entity) will be greater than @width, it will be rendered
 * outside the cell boundary box, luckely overwriting the adiacent
 * cells.
 *
 * Using %0 as @width means the width of the cell will be automatically
 * adjusted to the maximum width of its content.
 *
 * Negative width values are not allowed: this condition will raise
 * a warning without any further processing.
 *
 * Returns: the newly created cell or %NULL on errors
 **/
AdgTableCell *
adg_table_cell_new(AdgTableRow *row, gdouble width)
{
    g_return_val_if_fail(row != NULL, NULL);
    g_return_val_if_fail(width >= 0, NULL);

    return _adg_cell_new(row, NULL, width, FALSE, NULL, NULL, NULL);
}

/**
 * adg_table_cell_new_before:
 * @cell: a valid #AdgTableCell
 * @width: width of the cell
 *
 * Creates a new cell and inserts it rigthly before the @cell cell.
 * This works similarily and accepts the same parameters as the
 * adg_table_cell_new() function.
 *
 * Returns: the newly created cell or %NULL on errors
 **/
AdgTableCell *
adg_table_cell_new_before(AdgTableCell *cell, gdouble width)
{
    g_return_val_if_fail(cell != NULL, NULL);
    g_return_val_if_fail(cell->row != NULL, NULL);
    g_return_val_if_fail(width >= 0, NULL);

    return _adg_cell_new(cell->row, cell, width, FALSE, NULL, NULL, NULL);
}

/**
 * adg_table_cell_new_full:
 * @row: a valid #AdgTableRow
 * @width: width of the cell
 * @name: name to associate
 * @title: title to render
 * @value: value to render
 *
 * A convenient function to append a framed cell to @row with a
 * specific title and value text. The font to use for rendering
 * @title and @value will be picked up from the table style, so
 * be sure to have the correct table dress set before calling
 * this function.
 *
 * @row and @width have the same meanings as in adg_table_cell_new():
 * check its documentation for details.
 *
 * @name is an optional identifier to univoquely access this cell
 * by using adg_table_cell(). The identifier must be univoque:
 * if there is yet a cell with the same name a warning message will
 * be raised and the function will fail.
 *
 * Returns: the newly created cell or %NULL on errors
 **/
AdgTableCell *
adg_table_cell_new_full(AdgTableRow *row, gdouble width, const gchar *name,
                        const gchar *title, const gchar *value)
{
    AdgTableCell *cell;

    g_return_val_if_fail(row != NULL, NULL);

    cell = _adg_cell_new(row, NULL, width, TRUE, name, NULL, NULL);

    if (title)
        adg_table_cell_set_text_title(cell, title);

    if (value)
        adg_table_cell_set_text_value(cell, value);

    return cell;
}

/**
 * adg_table_cell:
 * @table: an #AdgTable
 * @name: the name of a cell
 *
 * Gets the cell named @name inside @table. Only named cells can be
 * retrieved by this method.
 *
 * Returns: the requested cell or %NULL if not found
 **/
AdgTableCell *
adg_table_cell(AdgTable *table, const gchar *name)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    data = table->data;

    if (data->cell_names == NULL)
        return NULL;

    return g_hash_table_lookup(data->cell_names, name);
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

    _adg_cell_free(cell);
    row->cells = g_slist_remove(row->cells, cell);
}

/**
 * adg_table_cell_set_name:
 * @cell: a valid #AdgTableCell
 * @name: the new name of @cell
 *
 * Sets a new name on @cell: this will allow to access @cell by
 * name at a later time using the adg_table_cell() API.
 **/
void
adg_table_cell_set_name(AdgTableCell *cell, const gchar *name)
{
    AdgTablePrivate *data;

    g_return_if_fail(cell != NULL);

    data = cell->row->table->data;

    _adg_cell_set_name(cell, NULL);
    _adg_cell_set_name(cell, name);
}

/**
 * adg_table_cell_get_name:
 * @cell: a valid #AdgTableCell
 *
 * Gets the name assigned to @cell. This function is highly inefficient
 * as the cell names are stored in a hash table optimized to get a cell
 * from a name. Getting the name from a cell involves a full hash table
 * inspection.
 *
 * Returns: the name bound of @cell or %NULL on no name or errors
 **/
const gchar *
adg_table_cell_get_name(AdgTableCell *cell)
{
    AdgTablePrivate *data;

    g_return_val_if_fail(cell != NULL, NULL);

    data = cell->row->table->data;

    return g_hash_table_find(data->cell_names, _adg_value_match, cell);
}

/**
 * adg_table_cell_set_title:
 * @cell: a valid #AdgTableCell
 * @title: the new title entity
 *
 * Sets @title as the new title entity of @cell. The top left
 * corner of the bounding box of @title will be cohincident to
 * the top left corner of the cell extents, taking into accounts
 * eventual padding spaces specified by the table style.
 *
 * The old internal entity is unrefenrenced while the @title (if
 * not %NULL) is refenenced with g_object_ref_sink().
 *
 * @title can be %NULL, in which case the old entity is removed.
 **/
void
adg_table_cell_set_title(AdgTableCell *cell, AdgEntity *title)
{
    g_return_if_fail(cell != NULL);
    g_return_if_fail(title == NULL || ADG_IS_ENTITY(title));

    if (_adg_cell_set_title(cell, title))
        adg_entity_invalidate((AdgEntity *) cell->row->table);
}

/**
 * adg_table_cell_set_text_title:
 * @cell: a valid #AdgTableCell
 * @title: a text string
 *
 * Convenient function to set a the title of a cell using an #AdgToyText
 * entity with the font dress picked from #AdgTable:table-dress with
 * a call to adg_table_style_get_title_dress().
 **/
void
adg_table_cell_set_text_title(AdgTableCell *cell, const gchar *title)
{
    AdgTable *table;
    AdgEntity *entity;
    AdgTableStyle *table_style;
    const AdgPair *padding;
    AdgDress table_dress, font_dress;
    AdgMatrix map;

    g_return_if_fail(cell != NULL);

    if (title == NULL)
        adg_table_cell_set_title(cell, NULL);

    if (cell->title) {
        gchar *old_title;
        gboolean unchanged;

        if (ADG_IS_TEXTUAL(cell->title))
            old_title = adg_textual_dup_text((AdgTextual *) cell->title);
        else
            old_title = NULL;

        unchanged = g_strcmp0(title, old_title) == 0;
        g_free(old_title);

        if (unchanged)
            return;
    }

    table = cell->row->table;
    table_dress = adg_table_get_table_dress(table);
    table_style = (AdgTableStyle *) adg_entity_style((AdgEntity *) table,
                                                     table_dress);
    padding = adg_table_style_get_cell_padding(table_style);
    font_dress = adg_table_style_get_title_dress(table_style);
    entity = g_object_new(ADG_TYPE_TOY_TEXT, "text", title,
                          "font-dress", font_dress, NULL);

    cairo_matrix_init_translate(&map, padding->x, padding->y);
    adg_entity_set_global_map(entity, &map);

    adg_table_cell_set_title(cell, entity);
}

/**
 * adg_table_cell_title:
 * @cell: a valid #AdgTableCell
 *
 * Gets the current title of @cell. The returned string is owned
 * by @cell and must not be modified or freed.
 *
 * Returns: the title entity or %NULL for undefined title
 **/
AdgEntity *
adg_table_cell_title(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return cell->title;
}

/**
 * adg_table_cell_set_value:
 * @cell: a valid #AdgTableCell
 * @value: the new value entity
 *
 * Sets @value as the new value entity of @cell. The bottom middle
 * point of the bounding box of @value will be cohincident to the
 * bottom middle point of the cell extents, taking into accounts
 * eventual padding spaces specified by the table style.
 *
 * The old internal entity is unrefenrenced while the @value (if
 * not %NULL) is refenenced with g_object_ref_sink().
 *
 * @value can be %NULL, in which case the old entity is removed.
 **/
void
adg_table_cell_set_value(AdgTableCell *cell, AdgEntity *value)
{
    g_return_if_fail(cell != NULL);
    g_return_if_fail(value == NULL || ADG_IS_ENTITY(value));

    if (_adg_cell_set_value(cell, value))
        adg_entity_invalidate((AdgEntity *) cell->row->table);
}

/**
 * adg_table_cell_set_text_value:
 * @cell: a valid #AdgTableCell
 * @value: a text string
 *
 * Convenient function to set a the value of a cell using an #AdgToyText
 * entity with a value font dress picked from #AdgTable:table-dress with
 * a call to adg_table_style_get_value_dress().
 **/
void
adg_table_cell_set_text_value(AdgTableCell *cell, const gchar *value)
{
    AdgTable *table;
    AdgEntity *entity;
    AdgTableStyle *table_style;
    const AdgPair *padding;
    AdgDress table_dress, font_dress;
    AdgMatrix map;

    g_return_if_fail(cell != NULL);

    if (value == NULL)
        adg_table_cell_set_value(cell, NULL);

    if (cell->value) {
        gchar *old_value;
        gboolean unchanged;

        if (ADG_IS_TEXTUAL(cell->value))
            old_value = adg_textual_dup_text((AdgTextual *) cell->value);
        else
            old_value = NULL;

        unchanged = g_strcmp0(value, old_value) == 0;
        g_free(old_value);
    }

    table = cell->row->table;
    table_dress = adg_table_get_table_dress(table);
    table_style = (AdgTableStyle *) adg_entity_style((AdgEntity *) table,
                                                     table_dress);
    padding = adg_table_style_get_cell_padding(table_style);
    font_dress = adg_table_style_get_value_dress(table_style);
    entity = g_object_new(ADG_TYPE_TOY_TEXT, "text", value,
                          "font-dress", font_dress, NULL);

    cairo_matrix_init_translate(&map, 0, -padding->y);
    adg_entity_set_global_map(entity, &map);

    adg_table_cell_set_value(cell, entity);
}

/**
 * adg_table_cell_value:
 * @cell: a valid #AdgTableCell
 *
 * Gets the current value of @cell. The returned string is owned
 * by @cell and must not be modified or freed.
 *
 * Returns: the value entity or %NULL for undefined value
 **/
AdgEntity *
adg_table_cell_value(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return cell->value;
}

/**
 * adg_table_cell_set_value_pos:
 * @cell: a valid #AdgTableCell
 * @from_factor: the alignment factor on the value entity
 * @to_factor: the alignment factor on the cell
 *
 * Sets a new custom position for the value entity of @cell. The
 * @from_factor specifies the source point (as a fraction of the
 * value extents) while the @to_factor is the destination point
 * (specified as a fraction of the cell extents) the source point
 * must be moved to.
 **/
void
adg_table_cell_set_value_pos(AdgTableCell *cell, const AdgPair *from_factor,
                             const AdgPair *to_factor)
{
    g_return_if_fail(cell != NULL);
    _adg_cell_set_value_pos(cell, from_factor, to_factor);
}

/**
 * adg_table_cell_set_value_pos_explicit:
 * @cell: a valid #AdgTableCell
 * @from_x: the x alignment factor on the entity
 * @from_y: the y alignment factor on the entity
 * @to_x: the x alignment factor on the cell
 * @to_y: the y alignment factor on the cell
 *
 * A convenient wrapper around adg_table_cell_set_value_pos()
 * that uses explicit factors instead of #AdgPair.
 **/
void
adg_table_cell_set_value_pos_explicit(AdgTableCell *cell,
                                      gdouble from_x, gdouble from_y,
                                      gdouble to_x, gdouble to_y)
{
    AdgPair from, to;

    from.x = from_x;
    from.y = from_y;
    to.x = to_x;
    to.y = to_y;

    adg_table_cell_set_value_pos(cell, &from, &to);
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

    cell->width = width;

    adg_entity_invalidate((AdgEntity *) cell->row->table);
}

/**
 * adg_table_cell_get_width:
 * @cell: a valid #AdgTableCell
 *
 * Gets the width of @cell.
 *
 * Returns: the requested width or %0 on errors
 **/
gdouble
adg_table_cell_get_width(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, 0.);

    return cell->width;
}

/**
 * adg_table_cell_switch_frame:
 * @cell: a valid #AdgTableCell
 * @new_state: the new frame state
 *
 * Sets the frame flag of @cell: if @new_state is %TRUE, a frame around
 * @cell will be rendered using the #AdgTableStyle:cell-dress dress
 * of the table style.
 **/
void
adg_table_cell_switch_frame(AdgTableCell *cell, gboolean new_state)
{
    AdgTablePrivate *data;

    g_return_if_fail(cell != NULL);

    if (cell->has_frame == new_state)
        return;

    data = cell->row->table->data;
    cell->has_frame = new_state;

    if (data->grid) {
        g_object_unref(data->grid);
        data->grid = NULL;
    }
}

/**
 * adg_table_cell_has_frame:
 * @cell: a valid #AdgTableCell
 *
 * Gets the frame flag of @cell.
 *
 * Returns: the frame flag
 **/
gboolean
adg_table_cell_has_frame(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, FALSE);

    return cell->has_frame;
}

/**
 * adg_table_cell_get_extents:
 * @cell: a valid #AdgTableCell
 *
 * Gets the extents of @cell. This function is useful only after the
 * arrange() phase as in the other situation the extents will likely
 * be not up to date.
 *
 * Returns: the extents of @cell or %NULL on errors
 **/
const CpmlExtents *
adg_table_cell_get_extents(AdgTableCell *cell)
{
    g_return_val_if_fail(cell != NULL, NULL);

    return &cell->extents;
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
    const AdgPair *spacing;
    GSList *row_node;
    AdgTableRow *row;
    gdouble y;

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

        _adg_row_arrange_size(row);

        if (row->extents.size.x > extents.size.x)
            extents.size.x = row->extents.size.x;
        extents.size.y += row->extents.size.y;
    }

    /* Arrange the layout of the table components */
    y = extents.org.y + spacing->y;
    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;

        row->extents.org.x = extents.org.x;
        row->extents.org.y = y;
        row->extents.size.x = extents.size.x;

        _adg_row_arrange(row);

        y += row->extents.size.y + spacing->y;
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
    AdgTablePrivate *data;
    AdgPath *path;
    AdgTrail *trail;
    GSList *row_node, *cell_node;
    AdgTableRow *row;
    AdgTableCell *cell;
    AdgPair pair;
    AdgDress dress;

    data = ((AdgTable *) entity)->data;

    if (data->grid)
        return;

    path = adg_path_new();
    trail = (AdgTrail *) path;

    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;

        for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
            cell = cell_node->data;

            if (!cell->has_frame)
                continue;

            cpml_pair_copy(&pair, &cell->extents.org);
            adg_path_move_to(path, &pair);
            pair.x += cell->extents.size.x;
            adg_path_line_to(path, &pair);
            pair.y += cell->extents.size.y;
            adg_path_line_to(path, &pair);
            pair.x -= cell->extents.size.x;
            adg_path_line_to(path, &pair);
            adg_path_close(path);
        }
    }

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
    AdgPair pair;
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
    guint signal_id;
    GQuark detail = 0;
    va_list var_args, var_copy;
    AdgTablePrivate *data;
    GSList *row_node;
    AdgTableRow *row;
    GSList *cell_node;
    AdgTableCell *cell;
    AdgAlignment *alignment;

    if (!g_signal_parse_name(detailed_signal, G_TYPE_FROM_INSTANCE(table),
                             &signal_id, &detail, FALSE)) {
        g_return_if_reached();
    }

    va_start(var_args, detailed_signal);
    data = table->data;

    if (data->frame) {
        G_VA_COPY(var_copy, var_args);
        g_signal_emit_valist(data->frame, signal_id, detail, var_copy);
    }

    if (data->grid) {
        G_VA_COPY(var_copy, var_args);
        g_signal_emit_valist(data->grid, signal_id, detail, var_copy);
    }

    for (row_node = data->rows; row_node; row_node = row_node->next) {
        row = row_node->data;

        for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
            cell = cell_node->data;

            if (cell->title) {
                alignment = (AdgAlignment *) adg_entity_get_parent(cell->title);
                G_VA_COPY(var_copy, var_args);
                g_signal_emit_valist(alignment, signal_id, detail, var_copy);
            }

            if (cell->value) {
                alignment = (AdgAlignment *) adg_entity_get_parent(cell->value);
                G_VA_COPY(var_copy, var_args);
                g_signal_emit_valist(alignment, signal_id, detail, var_copy);
            }
        }
    }

    va_end(var_args);
}

static AdgTableRow *
_adg_row_new(AdgTable *table, AdgTableRow *before_row)
{
    AdgTablePrivate *data;
    AdgTableRow *new_row;

    data = table->data;
    new_row = g_new(AdgTableRow, 1);
    new_row->table = table;
    new_row->cells = NULL;
    new_row->height = 0;
    new_row->extents.is_defined = FALSE;

    if (before_row == NULL) {
        data->rows = g_slist_append(data->rows, new_row);
    } else {
        GSList *before_node = g_slist_find(data->rows, before_row);

        /* This MUST be present, otherwise something really bad happened */
        g_return_val_if_fail(before_node != NULL, NULL);

        data->rows = g_slist_insert_before(data->rows, before_node, new_row);
    }

    _adg_invalidate((AdgEntity *) table);

    return new_row;
}

/* Before calling this function, row->extents should be updated */
static void
_adg_row_arrange(AdgTableRow *row)
{
    AdgTableStyle *table_style;
    const AdgPair *spacing;
    const AdgPair *org;
    AdgTableCell *cell;
    GSList *cell_node;
    gdouble x;

    table_style = GET_TABLE_STYLE(row->table);
    spacing = adg_table_style_get_cell_spacing(table_style);
    org = &row->extents.org;
    x = org->x + spacing->x;

    for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;

        cell->extents.org.x = x;
        cell->extents.org.y = org->y;

        _adg_cell_arrange(cell);

        x += cell->extents.size.x + spacing->x;
    }

    row->extents.is_defined = TRUE;
}

static void
_adg_row_arrange_size(AdgTableRow *row)
{
    AdgTableStyle *table_style;
    const AdgPair *spacing;
    CpmlVector *size;
    AdgTableCell *cell;
    GSList *cell_node;

    table_style = GET_TABLE_STYLE(row->table);
    spacing = adg_table_style_get_cell_spacing(table_style);
    size = &row->extents.size;

    size->x = 0;
    if (row->height == 0)
        size->y = adg_table_style_get_row_height(table_style);
    else
        size->y = row->height;

    /* Compute the row width by summing every cell width */
    for (cell_node = row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;

        _adg_cell_arrange_size(cell);

        size->x += cell->extents.size.x + spacing->x;
    }

    if (size->x > 0)
        size->x += spacing->x;
}

static void
_adg_row_dispose(AdgTableRow *row)
{
    g_slist_foreach(row->cells, (GFunc) _adg_cell_dispose, NULL);
}

static void
_adg_row_free(AdgTableRow *row)
{
    g_slist_foreach(row->cells, (GFunc) _adg_cell_free, NULL);
    g_slist_free(row->cells);

    g_free(row);
}

static AdgTableCell *
_adg_cell_new(AdgTableRow *row, AdgTableCell *before_cell,
              gdouble width, gboolean has_frame,
              const gchar *name, AdgEntity *title, AdgEntity *value)
{
    AdgTablePrivate *data;
    AdgTableCell *new_cell;

    data = row->table->data;

    if (name && data->cell_names &&
        g_hash_table_lookup(data->cell_names, name)) {
        g_warning(_("%s: `%s' cell name is yet used"), G_STRLOC, name);
        return NULL;
    }

    new_cell = g_new(AdgTableCell, 1);
    new_cell->row = row;
    new_cell->width = width;
    new_cell->has_frame = has_frame;
    new_cell->title = NULL;
    new_cell->value = NULL;
    new_cell->extents.is_defined = FALSE;
    new_cell->value_factor.x = 0.5;
    new_cell->value_factor.y = 1;

    _adg_cell_set_title(new_cell, title);
    _adg_cell_set_value(new_cell, value);

    if (before_cell == NULL) {
        row->cells = g_slist_append(row->cells, new_cell);
    } else {
        GSList *before_node = g_slist_find(row->cells, before_cell);

        /* This MUST be not null, otherwise something really bad happened */
        g_return_val_if_fail(before_node != NULL, NULL);

        row->cells = g_slist_insert_before(row->cells, before_node, new_cell);
    }

    if (name)
        _adg_cell_set_name(new_cell, name);

    return new_cell;
}

static void
_adg_cell_set_name(AdgTableCell *cell, const gchar *name)
{
    AdgTablePrivate *data = cell->row->table->data;

    if (data->cell_names == NULL && name == NULL)
        return;

    if (data->cell_names == NULL)
        data->cell_names = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                 g_free, NULL);

    if (name == NULL)
        g_hash_table_foreach_remove(data->cell_names, _adg_value_match, cell);
    else
        g_hash_table_insert(data->cell_names, g_strdup(name), cell);
}

static gboolean
_adg_cell_set_title(AdgTableCell *cell, AdgEntity *title)
{
    AdgAlignment *alignment;

    if (cell->title == title)
        return FALSE;

    if (cell->title) {
        alignment = (AdgAlignment *) adg_entity_get_parent(cell->title);
        g_object_unref(alignment);
    }

    cell->title = title;

    if (title) {
        alignment = adg_alignment_new_explicit(0, -1);
        g_object_ref_sink(alignment);
        adg_entity_set_parent((AdgEntity *) alignment,
                              (AdgEntity *) cell->row->table);

        adg_container_add((AdgContainer *) alignment, title);
    }

    return TRUE;
}

static gboolean
_adg_cell_set_value(AdgTableCell *cell, AdgEntity *value)
{
    AdgAlignment *alignment;

    if (cell->value == value)
        return FALSE;

    if (cell->value) {
        alignment = (AdgAlignment *) adg_entity_get_parent(cell->value);
        g_object_unref(alignment);
    }

    cell->value = value;

    if (value) {
        alignment = adg_alignment_new_explicit(0.5, 0);
        g_object_ref_sink(alignment);
        adg_entity_set_parent((AdgEntity *) alignment,
                              (AdgEntity *) cell->row->table);

        adg_container_add((AdgContainer *) alignment, value);
    }

    return TRUE;
}

static void
_adg_cell_set_value_pos(AdgTableCell *cell,
                        const AdgPair *from_factor, const AdgPair *to_factor)
{
    AdgAlignment *alignment;

    alignment = (AdgAlignment *) adg_entity_get_parent(cell->value);

    if (from_factor)
        adg_alignment_set_factor(alignment, from_factor);

    if (to_factor)
        cell->value_factor = *to_factor;
}

/* Before calling this function, cell->extents should be updated */
static void
_adg_cell_arrange(AdgTableCell *cell)
{
    CpmlExtents *extents;
    AdgAlignment *alignment;
    AdgMatrix map;

    extents = &cell->extents;

    if (cell->title) {
        alignment = (AdgAlignment *) adg_entity_get_parent(cell->title);

        cairo_matrix_init_translate(&map, extents->org.x, extents->org.y);
        adg_entity_set_global_map((AdgEntity *) alignment, &map);
    }

    if (cell->value) {
        AdgPair to;

        alignment = (AdgAlignment *) adg_entity_get_parent(cell->value);
        to.x = extents->size.x * cell->value_factor.x + extents->org.x;
        to.y = extents->size.y * cell->value_factor.y + extents->org.y;

        cairo_matrix_init_translate(&map, to.x, to.y);
        adg_entity_set_global_map((AdgEntity *) alignment, &map);
    }

    extents->is_defined = TRUE;
}

static void
_adg_cell_arrange_size(AdgTableCell *cell)
{
    CpmlVector *size;
    AdgAlignment *title_alignment;
    AdgAlignment *value_alignment;

    size = &cell->extents.size;

    if (cell->title) {
        title_alignment = (AdgAlignment *) adg_entity_get_parent(cell->title);
        adg_entity_arrange((AdgEntity *) title_alignment);
    } else {
        title_alignment = NULL;
    }

    if (cell->value) {
        value_alignment = (AdgAlignment *) adg_entity_get_parent(cell->value);
        adg_entity_arrange((AdgEntity *) value_alignment);
    } else {
        value_alignment = NULL;
    }

    size->y = cell->row->extents.size.y;

    if (cell->width == 0) {
        AdgTableStyle *table_style = GET_TABLE_STYLE(cell->row->table);
        const CpmlExtents *extents;

        /* The width depends on the cell content (default = 0) */
        size->x = 0;

        if (title_alignment) {
            extents = adg_entity_get_extents((AdgEntity *) title_alignment);
            size->x = extents->size.x;
        }

        if (value_alignment) {
            extents = adg_entity_get_extents((AdgEntity *) value_alignment);
            if (extents->size.x > size->x)
                size->x = extents->size.x;
        }

        size->x += adg_table_style_get_cell_spacing(table_style)->x * 2;
    } else {
        size->x = cell->width;
    }
}

static void
_adg_cell_dispose(AdgTableCell *cell)
{
    _adg_cell_set_title(cell, NULL);
    _adg_cell_set_value(cell, NULL);
}

static void
_adg_cell_free(AdgTableCell *cell)
{
    _adg_cell_set_name(cell, NULL);
    _adg_cell_dispose(cell);
    g_free(cell);
}

static gboolean
_adg_value_match(gpointer key, gpointer value, gpointer user_data)
{
    return value == user_data;
}
