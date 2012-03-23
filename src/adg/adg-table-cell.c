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
 * SECTION:adg-table-cell
 * @short_description: A single cell of a table
 *
 * The #AdgTableCell is a boxed type, the basic component of an
 * #AdgTable entity. It must be added to an #AdgTableRow that,
 * in cascade, will be added to an #AdgTable entity.
 *
 * Any cell can be filled with a title and a value: the font to be
 * used will be picked up from the #AdgTableStyle got by resolving
 * the #AdgTable:table-dress property.
 *
 * The default title is placed at the upper left corner of the cell
 * while the value is centered up to the bottom edge of the cell.
 * Anyway the text positioning can be customized by using the
 * adg_table_cell_set_value_pos() method.
 *
 * Some convenient functions to easily create title and value entities
 * with plain text are provided: adg_table_cell_new_full(),
 * adg_table_cell_set_text_title() and adg_table_cell_set_text_value().
 * When using these methods keep in mind the underlying #AdgToyText
 * entities will be displaced accordingly to the
 * #AdgTableStyle:cell-padding value, not used when setting the
 * entities throught other APIs.
 *
 * Since: 1.0
 **/

/**
 * AdgTableCell:
 *
 * An opaque structure referring to the cell of an #AdgTableRow.
 * Any row can have an unlimited number of cells.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-text-internal.h"

#include "adg-container.h"
#include "adg-alignment.h"
#include "adg-textual.h"
#include "adg-style.h"
#include "adg-table-style.h"

#include "adg-table.h"
#include "adg-table-row.h"
#include "adg-table-cell.h"


struct _AdgTableCell {
    AdgTableRow   *row;
    gdouble        width;
    gboolean       has_frame;
    AdgEntity     *title;
    AdgEntity     *value;
    AdgPair        value_factor;
    CpmlExtents    extents;
};


static AdgTableCell *   _adg_cell_new           (void);
static void             _adg_cell_invalidate    (AdgTableCell   *table_cell);
static gboolean         _adg_cell_set_title     (AdgTableCell   *table_cell,
                                                 AdgEntity      *title);
static gboolean         _adg_cell_set_value     (AdgTableCell   *table_cell,
                                                 AdgEntity      *value);
static void             _adg_cell_set_value_pos (AdgTableCell   *table_cell,
                                                 const AdgPair  *from_factor,
                                                 const AdgPair  *to_factor);


GType
adg_table_cell_get_type(void)
{
    static GType cell_type = 0;

    if (G_UNLIKELY(cell_type == 0))
        cell_type = g_boxed_type_register_static("AdgTableCell",
                                                 (GBoxedCopyFunc) adg_table_cell_dup,
                                                 (GBoxedFreeFunc) adg_table_cell_free);

    return cell_type;
}


/**
 * adg_table_cell_dup:
 * @table_cell: an #AdgTableCell structure
 *
 * Duplicates @table_cell. The returned duplicate should be freed
 * with adg_table_cell_free() when no longer needed.
 *
 * Returns: (transfer full): a duplicate of @table_cell.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_cell_dup(const AdgTableCell *table_cell)
{
    return g_memdup(table_cell, sizeof(AdgTableCell));
}

/**
 * adg_table_cell_new:
 * @table_row: a valid #AdgTableRow
 *
 * Creates a new empty cell without a frame and appends it at the
 * end of the cells yet present in @table_row. You can add content
 * to the cell by using adg_table_cell_set_title() and
 * adg_table_cell_set_value() or enable the frame with
 * adg_table_cell_switch_frame().
 *
 * Returns: (transfer full): the newly created cell or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_cell_new(AdgTableRow *table_row)
{
    AdgTableCell *table_cell;

    g_return_val_if_fail(table_row != NULL, NULL);

    table_cell = _adg_cell_new();
    table_cell->row = table_row;

    adg_table_row_insert(table_row, table_cell, NULL);
    _adg_cell_invalidate(table_cell);

    return table_cell;
}

/**
 * adg_table_cell_new_before:
 * @before_cell: a valid #AdgTableCell
 *
 * Creates a new cell and inserts it rigthly before the @table_cell cell.
 *
 * Returns: (transfer full): the newly created cell or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_cell_new_before(AdgTableCell *before_cell)
{
    AdgTableRow *table_row;
    AdgTableCell *table_cell;

    g_return_val_if_fail(before_cell != NULL, NULL);

    table_row = before_cell->row;
    g_return_val_if_fail(table_row != NULL, NULL);

    table_cell = _adg_cell_new();
    table_cell->row = table_row;

    adg_table_row_insert(table_row, table_cell, NULL);
    _adg_cell_invalidate(table_cell);

    return table_cell;
}

/**
 * adg_table_cell_new_with_width:
 * @table_row: a valid #AdgTableRow
 * @width: the cell width
 *
 * A convenient wrapper to adg_table_cell_new() that allows to
 * specify the @width of @table_row all at once.
 *
 * Returns: (transfer full): the newly created cell or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_cell_new_with_width(AdgTableRow *table_row, gdouble width)
{
    AdgTableCell *table_cell = adg_table_cell_new(table_row);

    if (table_cell != NULL)
        adg_table_cell_set_width(table_cell, width);

    return table_cell;
}

/**
 * adg_table_cell_new_full:
 * @table_row: a valid #AdgTableRow
 * @width: the cell width
 * @name: (allow-none): the name to bound to this cell
 * @title: (allow-none): the title text
 * @has_frame: whether to draw or not the frame
 *
 * A convenient function to add a cell and specifies some common
 * used properties at once.
 *
 * If @name is %NULL, the created cell will not be a named cell.
 * Check adg_table_set_cell() for further details on what a named
 * cell is supposed to be..
 *
 * @title can be %NULL, in which case no title entity will be created.
 *
 * Returns: (transfer full): the newly created cell or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTableCell *
adg_table_cell_new_full(AdgTableRow *table_row, gdouble width,
                        const gchar *name, const gchar *title,
                        gboolean has_frame)
{
    AdgTableCell *table_cell = adg_table_cell_new(table_row);

    if (table_cell == NULL)
        return NULL;

    adg_table_cell_set_width(table_cell, width);
    adg_table_cell_switch_frame(table_cell, has_frame);

    if (title) {
        adg_table_cell_set_text_title(table_cell, title);
    }
    if (name) {
        AdgTable *table = adg_table_row_get_table(table_row);
        adg_table_set_cell(table, name, table_cell);
    }

    return table_cell;
}

/**
 * adg_table_cell_dispose:
 * @table_cell: a valid #AdgTableCell struct
 *
 * Disposes @table_cell.
 *
 * Since: 1.0
 **/
void
adg_table_cell_dispose(AdgTableCell *table_cell)
{
    _adg_cell_set_title(table_cell, NULL);
    _adg_cell_set_value(table_cell, NULL);
}

/**
 * adg_table_cell_free:
 * @table_cell: an #AdgTableCell structure
 *
 * Releases all the memory allocated by @table_cell, itself included.
 *
 * Since: 1.0
 **/
void
adg_table_cell_free(AdgTableCell *table_cell)
{
    AdgTableRow *table_row = table_cell->row;

    if (table_cell->row != NULL) {
        AdgTable *table = adg_table_row_get_table(table_row);
        adg_table_row_remove(table_row, table_cell);
        if (table)
            adg_table_set_cell(table, NULL, table_cell);
    }

    adg_table_cell_dispose(table_cell);
    g_free(table_cell);
}

/**
 * adg_table_cell_get_row:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the row container of @table_cell. The returned #AdgTableRow
 * is owned by @table_cell and must not be modified or freed.
 *
 * Returns: (transfer none): the container row.
 *
 * Since: 1.0
 **/
AdgTableRow *
adg_table_cell_get_row(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, NULL);
    return table_cell->row;
}

/**
 * adg_table_cell_get_table:
 * @table_cell: a valid #AdgTableCell
 *
 * A convenient function that gets the table that contains
 * @table_cell. The returned #AdgTable is owned by @table_cell
 * and must not be modified or freed.
 *
 * Returns: (transfer none): the container table.
 *
 * Since: 1.0
 **/
AdgTable *
adg_table_cell_get_table(AdgTableCell *table_cell)
{
    AdgTableRow *table_row = adg_table_cell_get_row(table_cell);
    if (table_row == NULL)
        return NULL;

    return adg_table_row_get_table(table_row);
}

/**
 * adg_table_cell_set_title:
 * @table_cell: a valid #AdgTableCell
 * @title: the new title entity
 *
 * Sets @title as the new title entity of @table_cell. The top left
 * corner of the bounding box of @title will be cohincident to
 * the top left corner of the cell extents, taking into accounts
 * eventual padding spaces specified by the table style.
 *
 * The old internal entity is unrefenrenced while the @title (if
 * not %NULL) is refenenced with g_object_ref_sink().
 *
 * @title can be %NULL, in which case the old entity is removed.
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_title(AdgTableCell *table_cell, AdgEntity *title)
{
    g_return_if_fail(table_cell != NULL);
    g_return_if_fail(title == NULL || ADG_IS_ENTITY(title));

    if (_adg_cell_set_title(table_cell, title))
        _adg_cell_invalidate(table_cell);
}

/**
 * adg_table_cell_set_text_title:
 * @table_cell: a valid #AdgTableCell
 * @title: a text string
 *
 * Convenient function to set a the title of a cell using an #AdgToyText
 * entity with the font dress picked from #AdgTable:table-dress with
 * a call to adg_table_style_get_title_dress().
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_text_title(AdgTableCell *table_cell, const gchar *title)
{
    AdgTable *table;
    AdgEntity *entity;
    AdgTableStyle *table_style;
    const AdgPair *padding;
    AdgDress table_dress, font_dress;
    AdgMatrix map;

    g_return_if_fail(table_cell != NULL);

    if (title == NULL)
        adg_table_cell_set_title(table_cell, NULL);

    if (table_cell->title) {
        gchar *old_title;
        gboolean unchanged;

        if (ADG_IS_TEXTUAL(table_cell->title))
            old_title = adg_textual_dup_text((AdgTextual *) table_cell->title);
        else
            old_title = NULL;

        unchanged = g_strcmp0(title, old_title) == 0;
        g_free(old_title);

        if (unchanged)
            return;
    }

    table = adg_table_cell_get_table(table_cell);
    table_dress = adg_table_get_table_dress(table);
    table_style = (AdgTableStyle *) adg_entity_style((AdgEntity *) table,
                                                     table_dress);
    padding = adg_table_style_get_cell_padding(table_style);
    font_dress = adg_table_style_get_title_dress(table_style);
    entity = g_object_new(ADG_TYPE_BEST_TEXT, "text", title,
                          "font-dress", font_dress, NULL);

    cairo_matrix_init_translate(&map, padding->x, padding->y);
    adg_entity_set_global_map(entity, &map);

    adg_table_cell_set_title(table_cell, entity);
}

/**
 * adg_table_cell_title:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the current title of @table_cell. The returned string is owned
 * by @table_cell and must not be modified or freed.
 *
 * Returns: (transfer none): the title entity or %NULL for undefined title.
 *
 * Since: 1.0
 **/
AdgEntity *
adg_table_cell_title(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, NULL);

    return table_cell->title;
}

/**
 * adg_table_cell_set_value:
 * @table_cell: a valid #AdgTableCell
 * @value: the new value entity
 *
 * Sets @value as the new value entity of @table_cell. The bottom middle
 * point of the bounding box of @value will be cohincident to the
 * bottom middle point of the cell extents, taking into accounts
 * eventual padding spaces specified by the table style.
 *
 * The old internal entity is unrefenrenced while the @value (if
 * not %NULL) is refenenced with g_object_ref_sink().
 *
 * @value can be %NULL, in which case the old entity is removed.
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_value(AdgTableCell *table_cell, AdgEntity *value)
{
    g_return_if_fail(table_cell != NULL);
    g_return_if_fail(value == NULL || ADG_IS_ENTITY(value));

    if (_adg_cell_set_value(table_cell, value))
        _adg_cell_invalidate(table_cell);
}

/**
 * adg_table_cell_set_text_value:
 * @table_cell: a valid #AdgTableCell
 * @value: a text string
 *
 * Convenient function to set a the value of a cell using an #AdgToyText
 * entity with a value font dress picked from #AdgTable:table-dress with
 * a call to adg_table_style_get_value_dress().
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_text_value(AdgTableCell *table_cell, const gchar *value)
{
    AdgTable *table;
    AdgEntity *entity;
    AdgTableStyle *table_style;
    const AdgPair *padding;
    AdgDress table_dress, font_dress;
    AdgMatrix map;

    g_return_if_fail(table_cell != NULL);

    if (value == NULL)
        adg_table_cell_set_value(table_cell, NULL);

    if (table_cell->value) {
        gchar *old_value;
        gboolean unchanged;

        if (ADG_IS_TEXTUAL(table_cell->value))
            old_value = adg_textual_dup_text((AdgTextual *) table_cell->value);
        else
            old_value = NULL;

        unchanged = g_strcmp0(value, old_value) == 0;
        g_free(old_value);
        if (unchanged)
            return;
    }

    table = adg_table_cell_get_table(table_cell);
    table_dress = adg_table_get_table_dress(table);
    table_style = (AdgTableStyle *) adg_entity_style((AdgEntity *) table,
                                                     table_dress);
    padding = adg_table_style_get_cell_padding(table_style);
    font_dress = adg_table_style_get_value_dress(table_style);
    entity = g_object_new(ADG_TYPE_BEST_TEXT, "text", value,
                          "font-dress", font_dress, NULL);

    cairo_matrix_init_translate(&map, 0, -padding->y);
    adg_entity_set_global_map(entity, &map);

    adg_table_cell_set_value(table_cell, entity);
}

/**
 * adg_table_cell_value:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the current value of @table_cell. The returned string is owned
 * by @table_cell and must not be modified or freed.
 *
 * Returns: (transfer none): the value entity or %NULL for undefined value.
 *
 * Since: 1.0
 **/
AdgEntity *
adg_table_cell_value(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, NULL);

    return table_cell->value;
}

/**
 * adg_table_cell_set_value_pos:
 * @table_cell: a valid #AdgTableCell
 * @from_factor: the alignment factor on the value entity
 * @to_factor: the alignment factor on the cell
 *
 * Sets a new custom position for the value entity of @table_cell. The
 * @from_factor specifies the source point (as a fraction of the
 * value extents) while the @to_factor is the destination point
 * (specified as a fraction of the cell extents) the source point
 * must be moved to.
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_value_pos(AdgTableCell *table_cell,
                             const AdgPair *from_factor,
                             const AdgPair *to_factor)
{
    g_return_if_fail(table_cell != NULL);
    _adg_cell_set_value_pos(table_cell, from_factor, to_factor);
}

/**
 * adg_table_cell_set_value_pos_explicit:
 * @table_cell: a valid #AdgTableCell
 * @from_x: the x alignment factor on the entity
 * @from_y: the y alignment factor on the entity
 * @to_x: the x alignment factor on the cell
 * @to_y: the y alignment factor on the cell
 *
 * A convenient wrapper around adg_table_cell_set_value_pos()
 * that uses explicit factors instead of #AdgPair.
 *
 * Since: 1.0
 **/
void
adg_table_cell_set_value_pos_explicit(AdgTableCell *table_cell,
                                      gdouble from_x, gdouble from_y,
                                      gdouble to_x, gdouble to_y)
{
    AdgPair from, to;

    from.x = from_x;
    from.y = from_y;
    to.x = to_x;
    to.y = to_y;

    adg_table_cell_set_value_pos(table_cell, &from, &to);
}

/**
 * adg_table_cell_set_width:
 * @table_cell: a valid #AdgTableCell
 * @width: the new width
 *
 * Sets a new width on @table_cell. The extents on the whole table
 * will be invalidated, so will be recomputed in the next
 * arrange() phase.
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
 * Since: 1.0
 **/
void
adg_table_cell_set_width(AdgTableCell *table_cell, gdouble width)
{
    g_return_if_fail(table_cell != NULL);
    g_return_if_fail(width >= 0);

    if (table_cell->width != width) {
        table_cell->width = width;
        _adg_cell_invalidate(table_cell);
    }
}

/**
 * adg_table_cell_get_width:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the width of @table_cell.
 *
 * Returns: the requested width or %0 on errors
 *
 * Since: 1.0
 **/
gdouble
adg_table_cell_get_width(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, 0.);

    return table_cell->width;
}

/**
 * adg_table_cell_switch_frame:
 * @table_cell: a valid #AdgTableCell
 * @has_frame: whether to draw or not the frame
 *
 * Sets the frame flag of @table_cell: if @has_frame is %TRUE, a frame around
 * @table_cell will be rendered using the #AdgTableStyle:cell-dress dress
 * of the table style.
 *
 * Since: 1.0
 **/
void
adg_table_cell_switch_frame(AdgTableCell *table_cell, gboolean has_frame)
{
    g_return_if_fail(table_cell != NULL);

    if (table_cell->has_frame != has_frame) {
        AdgTable *table = adg_table_cell_get_table(table_cell);
        table_cell->has_frame = has_frame;
        adg_table_invalidate_grid(table);
    }
}

/**
 * adg_table_cell_has_frame:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the frame flag of @table_cell.
 *
 * Returns: the frame flag.
 *
 * Since: 1.0
 **/
gboolean
adg_table_cell_has_frame(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, FALSE);

    return table_cell->has_frame;
}

/**
 * adg_table_cell_get_extents:
 * @table_cell: a valid #AdgTableCell
 *
 * Gets the extents of @table_cell. This function is useful only after the
 * arrange() phase as in the other situation the extents will likely
 * be not up to date.
 *
 * Returns: the extents of @table_cell or %NULL on errors
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_table_cell_get_extents(AdgTableCell *table_cell)
{
    g_return_val_if_fail(table_cell != NULL, NULL);

    return &table_cell->extents;
}

/**
 * adg_table_cell_size_request:
 * @table_cell: a valid #AdgTableCell
 * @row_extents: the extents of the container #AdgTableRow
 *
 * Computes the minimum space needed to properly render @table_cell
 * and updates the size component of the internal #CpmlExtents struct,
 * returning it to the caller. The returned #AdgPair is owned by
 * @table_cell and should not be modified or freed.
 *
 * Returns: (transfer none): the minimum size required.
 *
 * Since: 1.0
 **/
const AdgPair *
adg_table_cell_size_request(AdgTableCell *table_cell,
                            const CpmlExtents *row_extents)
{
    CpmlVector *size;
    AdgAlignment *title_alignment;
    AdgAlignment *value_alignment;
    AdgTable *table;

    size = &table_cell->extents.size;

    if (table_cell->title) {
        title_alignment = (AdgAlignment *) adg_entity_get_parent(table_cell->title);
        adg_entity_arrange((AdgEntity *) title_alignment);
    } else {
        title_alignment = NULL;
    }

    if (table_cell->value) {
        value_alignment = (AdgAlignment *) adg_entity_get_parent(table_cell->value);
        adg_entity_arrange((AdgEntity *) value_alignment);
    } else {
        value_alignment = NULL;
    }

    table = adg_table_cell_get_table(table_cell);
    size->y = row_extents->size.y;

    if (table_cell->width == 0) {
        AdgTableStyle *table_style = (AdgTableStyle *) adg_table_get_table_style(table);
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
        size->x = table_cell->width;
    }

    return size;
}

/**
 * adg_table_cell_arrange:
 * @table_cell: an #AdgTableCell
 * @layout: the new extents to use
 *
 * Rearranges the underlying #AdgTableCell owned by @table_cell using
 * the new extents provided in @layout. If the x or y size component
 * of @layout is negative, the value holded by the internal extents
 * struct is not overriden.
 *
 * <note><para>
 * table_cell->extents must be up to date if @layout->size.x or
 * @layout->size.y is negative in order to have a valid size.
 * </para></note>
 *
 * Returns: the extents of @table_cell or %NULL on errors
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_table_cell_arrange(AdgTableCell *table_cell, const CpmlExtents *layout)
{
    CpmlExtents *extents;
    AdgAlignment *alignment;
    AdgMatrix map;

    /* Set the new extents */
    extents = &table_cell->extents;
    extents->org = layout->org;
    if (layout->size.x > 0)
        extents->size.x = layout->size.x;
    if (layout->size.y > 0)
        extents->size.y = layout->size.y;
    extents->is_defined = TRUE;

    if (table_cell->title) {
        alignment = (AdgAlignment *) adg_entity_get_parent(table_cell->title);

        cairo_matrix_init_translate(&map, extents->org.x, extents->org.y);
        adg_entity_set_global_map((AdgEntity *) alignment, &map);
    }

    if (table_cell->value) {
        AdgPair to;

        alignment = (AdgAlignment *) adg_entity_get_parent(table_cell->value);
        to.x = extents->size.x * table_cell->value_factor.x + extents->org.x;
        to.y = extents->size.y * table_cell->value_factor.y + extents->org.y;

        cairo_matrix_init_translate(&map, to.x, to.y);
        adg_entity_set_global_map((AdgEntity *) alignment, &map);
    }

    return extents;
}


static AdgTableCell *
_adg_cell_new(void)
{
    AdgTableCell *table_cell;

    table_cell = g_new(AdgTableCell, 1);
    table_cell->row = NULL;
    table_cell->width = 0.;
    table_cell->has_frame = FALSE;
    table_cell->title = NULL;
    table_cell->value = NULL;
    table_cell->extents.is_defined = FALSE;
    table_cell->value_factor.x = 0.5;
    table_cell->value_factor.y = 1;

    return table_cell;
}

static void
_adg_cell_invalidate(AdgTableCell *table_cell)
{
    AdgTable *table = adg_table_cell_get_table(table_cell);

    if (table)
        adg_entity_invalidate((AdgEntity *) table);
}

static gboolean
_adg_cell_set_title(AdgTableCell *table_cell, AdgEntity *title)
{
    AdgEntity *alignment;

    if (table_cell->title == title)
        return FALSE;

    if (table_cell->title) {
        alignment = adg_entity_get_parent(table_cell->title);
        g_object_unref(alignment);
    }

    table_cell->title = title;

    if (title) {
        AdgEntity *table = (AdgEntity *) adg_table_cell_get_table(table_cell);
        alignment = (AdgEntity *) adg_alignment_new_explicit(0, -1);
        g_object_ref_sink(alignment);
        adg_entity_set_parent(alignment, table);
        adg_container_add((AdgContainer *) alignment, title);
    }

    return TRUE;
}

static gboolean
_adg_cell_set_value(AdgTableCell *table_cell, AdgEntity *value)
{
    AdgEntity *alignment;

    if (table_cell->value == value)
        return FALSE;

    if (table_cell->value) {
        alignment = adg_entity_get_parent(table_cell->value);
        g_object_unref(alignment);
    }

    table_cell->value = value;

    if (value) {
        AdgEntity *table = (AdgEntity *) adg_table_cell_get_table(table_cell);
        alignment = (AdgEntity *) adg_alignment_new_explicit(0.5, 0);
        g_object_ref_sink(alignment);
        adg_entity_set_parent(alignment, table);
        adg_container_add((AdgContainer *) alignment, value);
    }

    return TRUE;
}

static void
_adg_cell_set_value_pos(AdgTableCell *table_cell,
                        const AdgPair *from_factor, const AdgPair *to_factor)
{
    AdgAlignment *alignment;

    alignment = (AdgAlignment *) adg_entity_get_parent(table_cell->value);

    if (from_factor)
        adg_alignment_set_factor(alignment, from_factor);

    if (to_factor)
        table_cell->value_factor = *to_factor;
}
