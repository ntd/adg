/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-table-row
 * @short_description: A tabular row
 *
 * The #AdgTableRow is a boxed type containing a single row of cells
 * of an #AdgTable object.
 *
 * Every row is segmented into different cells. It must be populated
 * by using the #AdgCell APIs, such as adg_table_cell_new() or
 * adg_table_cell_new_before().
 *
 * Since: 1.0
 **/



/**
 * AdgTableRow:
 *
 * An opaque structure referring to a row of an #AdgTable. Any
 * table can have an unlimited number of rows.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-style.h"
#include "adg-table-style.h"

#include "adg-table.h"
#include "adg-table-row.h"
#include "adg-table-cell.h"


struct _AdgTableRow {
    AdgTable      *table;
    GSList        *cells;
    gdouble        height;
    CpmlExtents    extents;
};


static AdgTableRow *    _adg_row_new        (AdgTable       *table);


GType
adg_table_row_get_type(void)
{
    static GType row_type = 0;

    if (G_UNLIKELY(row_type == 0))
        row_type = g_boxed_type_register_static("AdgTableRow",
                                                (GBoxedCopyFunc) adg_table_row_dup,
                                                (GBoxedFreeFunc) adg_table_row_free);

    return row_type;
}


/**
 * adg_table_row_dup:
 * @table_row: an #AdgTableRow structure
 *
 * Duplicates @table_row. The returned duplicate should be freed
 * with adg_table_row_free() when no longer needed.
 *
 * Returns: (transfer full): a duplicate of @table_row.
 *
 * Since: 1.0
 **/
AdgTableRow *
adg_table_row_dup(const AdgTableRow *table_row)
{
    return g_memdup(table_row, sizeof(AdgTableRow));
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
 * Returns: (transfer full): the newly created row or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableRow *
adg_table_row_new(AdgTable *table)
{
    AdgTableRow *table_row;

    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    table_row = _adg_row_new(table);
    adg_table_insert(table, table_row, NULL);
    adg_entity_invalidate((AdgEntity *) table);

    return table_row;
}

/**
 * adg_table_row_new_before:
 * @before_row: a valid #AdgTableRow
 *
 * Creates a new empty row with default height and inserts it
 * just before @before_row.
 *
 * Returns: (transfer full): the newly created row or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableRow *
adg_table_row_new_before(AdgTableRow *before_row)
{
    AdgTableRow *table_row;
    AdgTable *table;

    g_return_val_if_fail(before_row != NULL, NULL);

    table = (AdgTable *) before_row->table;
    g_return_val_if_fail(ADG_IS_TABLE(table), NULL);

    table_row = _adg_row_new(table);
    adg_table_insert(table, table_row, before_row);
    adg_entity_invalidate((AdgEntity *) before_row->table);

    return table_row;
}


/**
 * adg_table_row_free:
 * @table_row: an #AdgTableRow structure
 *
 * Releases all the memory allocated by @table_row, itself included.
 *
 * Since: 1.0
 **/
void
adg_table_row_free(AdgTableRow *table_row)
{
    AdgTable *table;

    g_slist_foreach(table_row->cells, (GFunc) adg_table_cell_free, NULL);
    g_slist_free(table_row->cells);

    table = table_row->table;
    if (table != NULL)
        adg_table_remove(table, table_row);

    g_free(table_row);
}

/**
 * adg_table_row_insert:
 * @table_row: a valid #AdgTableRow
 * @table_cell: the #AdgTableCell to insert
 * @before_cell: (allow-none): an #AdgTableRow or %NULL
 *
 * Inserts @table_cell inside @table_row. If @before_cell
 * is specified, @table_cell is inserted before it.
 *
 * Since: 1.0
 **/
void
adg_table_row_insert(AdgTableRow *table_row, AdgTableCell *table_cell,
                     AdgTableCell *before_cell)
{
    g_return_if_fail(table_row != NULL);
    g_return_if_fail(table_cell != NULL);

    if (before_cell == NULL) {
        table_row->cells = g_slist_append(table_row->cells, table_cell);
    } else {
        GSList *before = g_slist_find(table_row->cells, before_cell);

        /* This MUST be present, otherwise something really bad happened */
        g_return_if_fail(before != NULL);

        table_row->cells = g_slist_insert_before(table_row->cells,
                                                 before, table_cell);
    }
}

/**
 * adg_table_row_remove:
 * @table_row: a valid #AdgTableRow
 * @table_cell: the #AdgTableCell to remove
 *
 * Removes @table_cell from list of cells of @table_row.
 *
 * Since: 1.0
 **/
void
adg_table_row_remove(AdgTableRow *table_row, AdgTableCell *table_cell)
{
    g_return_if_fail(table_row != NULL);
    g_return_if_fail(table_cell != NULL);

    table_row->cells = g_slist_remove(table_row->cells, table_cell);
}

/**
 * adg_table_row_foreach:
 * @table_row: an #AdgTableRow
 * @callback: (scope call): a callback
 * @user_data: callback user data
 *
 * Invokes @callback on each cell of @table_row.
 * The callback should be declared as:
 *
 * |[
 * void callback(AdgTableCell *table_cell, gpointer user_data);
 * ]|
 *
 * Since: 1.0
 **/
void
adg_table_row_foreach(AdgTableRow *table_row,
                      GCallback callback, gpointer user_data)
{
    g_return_if_fail(table_row != NULL);
    g_return_if_fail(callback != NULL);

    g_slist_foreach(table_row->cells, (GFunc) callback, user_data);
}

/**
 * adg_table_row_get_table:
 * @table_row: a valid #AdgTableRow
 *
 * Returns the container table of @table_row. The returned table
 * is owned by @table_row and must not be modified or freed.
 *
 * Returns: (transfer none): the requested table or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTable *
adg_table_row_get_table(AdgTableRow *table_row)
{
    g_return_val_if_fail(table_row != NULL, NULL);

    return table_row->table;
}

/**
 * adg_table_row_set_height:
 * @table_row: a valid #AdgTableRow
 * @height: the new height
 *
 * Sets a new height on @table_row. The extents will be invalidated to
 * recompute the whole layout of the table. Specifying %0 in
 * @height will use the default height set in the table style.
 *
 * Since: 1.0
 **/
void
adg_table_row_set_height(AdgTableRow *table_row, gdouble height)
{
    g_return_if_fail(table_row != NULL);

    table_row->height = height;

    adg_entity_invalidate((AdgEntity *) table_row->table);
}

/**
 * adg_table_row_get_height:
 * @table_row: a valid #AdgTableRow
 *
 * Gets the height of @table_row.
 *
 * Returns: the requested height or %0 on errors
 *
 * Since: 1.0
 **/
gdouble
adg_table_row_get_height(AdgTableRow *table_row)
{
    g_return_val_if_fail(table_row != NULL, 0.);

    return table_row->height;
}

/**
 * adg_table_row_get_extents:
 * @table_row: a valid #AdgTableRow
 *
 * Gets the extents of @table_row. This function is useful only after
 * the arrange() phase as in the other situation the extents will
 * likely be not up to date.
 *
 * Returns: (transfer none): the extents of @table_row or %NULL on errors.
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_table_row_get_extents(AdgTableRow *table_row)
{
    g_return_val_if_fail(table_row != NULL, NULL);

    return &table_row->extents;
}

/**
 * adg_table_row_size_request:
 * @table_row: a valid #AdgTableRow
 *
 * Computes the minimum space needed to properly render @table_row
 * and updates the size component of the internal #CpmlExtents struct,
 * returning it to the caller. The returned #AdgPair is owned by
 * @table_row and should not be modified or freed.
 *
 * Returns: (transfer none): the minimum size required.
 *
 * Since: 1.0
 **/
const AdgPair *
adg_table_row_size_request(AdgTableRow *table_row)
{
    AdgTableStyle *table_style;
    const AdgPair *spacing;
    CpmlExtents *extents;
    CpmlVector *size;
    AdgTableCell *cell;
    GSList *cell_node;
    const AdgPair *cell_size;

    g_return_val_if_fail(table_row != NULL, NULL);

    table_style = (AdgTableStyle *) adg_table_get_table_style(table_row->table);
    spacing = adg_table_style_get_cell_spacing(table_style);
    extents = &table_row->extents;
    size = &extents->size;

    size->x = 0;
    if (table_row->height == 0)
        size->y = adg_table_style_get_row_height(table_style);
    else
        size->y = table_row->height;

    /* Compute the row width by summing every cell width */
    for (cell_node = table_row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;
        cell_size = adg_table_cell_size_request(cell, extents);
        size->x += cell_size->x + spacing->x;
    }

    if (size->x > 0)
        size->x += spacing->x;

    return size;
}

/**
 * adg_table_row_arrange:
 * @table_row: an #AdgTableRow
 * @layout: the new extents to use
 *
 * Rearranges the underlying #AdgTableCell owned by @table_row using
 * the new extents provided in @layout. If the x or y size component
 * of @layout is negative, the value holded by the internal extents
 * struct is not overriden.
 *
 * <note><para>
 * table_row->extents must be up to date if @layout->size.x or
 * @layout->size.y is negative in order to have a valid size.
 * </para></note>
 *
 * Returns: (transfer none): the extents of @table_row or %NULL on errors.
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_table_row_arrange(AdgTableRow *table_row, const CpmlExtents *layout)
{
    CpmlExtents *extents;
    CpmlExtents cell_layout;
    const CpmlExtents *cell_extents;
    AdgTableStyle *table_style;
    const AdgPair *spacing;
    AdgTableCell *cell;
    GSList *cell_node;

    g_return_val_if_fail(table_row != NULL, NULL);

    /* Set the new extents */
    extents = &table_row->extents;
    extents->org = layout->org;
    if (layout->size.x > 0)
        extents->size.x = layout->size.x;
    if (layout->size.y > 0)
        extents->size.y = layout->size.y;
    extents->is_defined = TRUE;

    table_style = (AdgTableStyle *) adg_table_get_table_style(table_row->table);
    spacing = adg_table_style_get_cell_spacing(table_style);

    /* Propagate the arrange to the table cells */
    cell_layout.org.x = extents->org.x + spacing->x;
    cell_layout.org.y = extents->org.y;
    cell_layout.size.x = -1;
    cell_layout.size.y = extents->size.y;

    for (cell_node = table_row->cells; cell_node; cell_node = cell_node->next) {
        cell = cell_node->data;
        cell_extents = adg_table_cell_arrange(cell, &cell_layout);
        cell_layout.org.x += cell_extents->size.x + spacing->x;
    }

    return extents;
}


static AdgTableRow *
_adg_row_new(AdgTable *table)
{
    AdgTableRow *table_row = g_new(AdgTableRow, 1);

    table_row->table = table;
    table_row->cells = NULL;
    table_row->height = 0;
    table_row->extents.is_defined = FALSE;

    return table_row;
}
