/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-title-block
 * @short_description: A title block entity
 *
 * Title blocks are commonly used in technical drawings to include
 * additional information not strictly related to physical dimensions,
 * such as title, material of the represented part, special treatments,
 * date and scale etc.
 *
 * Actually this entity is only a place-holder: it will be implemented
 * properly in a 0.6.x release, after having AdgToyTable in place.
 *
 * Since: 1.0
 **/

/**
 * AdgTitleBlock:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-table.h"
#include "adg-table-row.h"
#include "adg-table-cell.h"

#include "adg-title-block.h"
#include "adg-title-block-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_title_block_parent_class)


G_DEFINE_TYPE(AdgTitleBlock, adg_title_block, ADG_TYPE_TABLE);

enum {
    PROP_0,
    PROP_TITLE,
    PROP_DRAWING,
    PROP_SIZE,
    PROP_SCALE,
    PROP_AUTHOR,
    PROP_DATE,
    PROP_LOGO,
    PROP_PROJECTION
};


static void             _adg_finalize           (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);


static void
adg_title_block_class_init(AdgTitleBlockClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTitleBlockPrivate));

    gobject_class->finalize = _adg_finalize;
    gobject_class->set_property = _adg_set_property;
    gobject_class->get_property = _adg_get_property;

    param = g_param_spec_string("title",
                                P_("Title"),
                                P_("A descriptive title of the drawing"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TITLE, param);

    param = g_param_spec_string("drawing",
                                P_("Drawing Name"),
                                P_("The name of the drawing: the ADG canvas does not make any assumtpion on this text string"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DRAWING, param);

    param = g_param_spec_string("size",
                                P_("Media Size"),
                                P_("The media size to be used to print the drawing, usually something like \"A3\" or \"Letter\""),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = g_param_spec_string("scale",
                                P_("Scale"),
                                P_("The scale of the drawing, if it makes sense"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SCALE, param);

    param = g_param_spec_string("author",
                                P_("Author"),
                                P_("Name and last name of the author of the drawing"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_AUTHOR, param);

    param = g_param_spec_string("date",
                                P_("Date"),
                                P_("The date this drawing has been generated: setting it to an empty string will fallback to today in the preferred representation for the current locale"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DATE, param);

    param = g_param_spec_object("logo",
                                P_("Logo"),
                                P_("An entity to be displayed in the title block as the logo of the owner: the containing cell has a 1:1 ratio"),
                                ADG_TYPE_ENTITY,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOGO, param);

    param = g_param_spec_object("projection",
                                P_("Projection Scheme"),
                                P_("The entity usually reserved to identify the projection scheme adopted by this drawing"),
                                ADG_TYPE_ENTITY,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PROJECTION, param);
}

static void
adg_title_block_init(AdgTitleBlock *title_block)
{
    AdgTable *table = (AdgTable *) title_block;
    AdgTableRow *row;
    AdgTitleBlockPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(title_block,
                                                             ADG_TYPE_TITLE_BLOCK,
                                                             AdgTitleBlockPrivate);

    data->author = NULL;
    data->title = NULL;
    data->drawing = NULL;
    data->size = NULL;
    data->scale = NULL;
    data->author = NULL;
    data->date = NULL;
    data->projection = NULL;

    title_block->data = data;

    /* By default the title block should be floating */
    adg_entity_switch_floating((AdgEntity *) title_block, TRUE);

    /* Create the title block template*/

    /* First row */
    row = adg_table_row_new(table);
    adg_table_cell_new_with_width(row, 62);
    adg_table_cell_new_full(row, 200, "title", _("TITLE"), TRUE);

    /* Second row */
    row = adg_table_row_new(table);
    adg_table_cell_new_full(row, 62, "logo", NULL, FALSE);
    adg_table_cell_new_full(row, 40, "size", _("SIZE"), TRUE);
    adg_table_cell_new_full(row, 60, "scale", _("SCALE"), TRUE);
    adg_table_cell_new_full(row, 100, "drawing", _("DRAWING"), TRUE);

    /* Third row */
    row = adg_table_row_new(table);
    adg_table_cell_new_full(row, 62, "projection", NULL, TRUE);
    adg_table_cell_new_full(row, 100, "author", _("AUTHOR"), TRUE);
    adg_table_cell_new_full(row, 100, "date", _("DATE"), TRUE);
}

static void
_adg_finalize(GObject *object)
{
    AdgTitleBlockPrivate *data = ((AdgTitleBlock *) object)->data;

    g_free(data->title);
    g_free(data->drawing);
    g_free(data->size);
    g_free(data->scale);
    g_free(data->author);
    g_free(data->date);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgTitleBlockPrivate *data = ((AdgTitleBlock *) object)->data;

    switch (prop_id) {
    case PROP_TITLE:
        g_value_set_string(value, data->title);
        break;
    case PROP_DRAWING:
        g_value_set_string(value, data->drawing);
        break;
    case PROP_SIZE:
        g_value_set_string(value, data->size);
        break;
    case PROP_SCALE:
        g_value_set_string(value, data->scale);
        break;
    case PROP_AUTHOR:
        g_value_set_string(value, data->author);
        break;
    case PROP_DATE:
        g_value_set_string(value, data->date);
        break;
    case PROP_LOGO:
        g_value_set_object(value, data->logo);
        break;
    case PROP_PROJECTION:
        g_value_set_object(value, data->projection);
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
    AdgTitleBlock *title_block;
    AdgTitleBlockPrivate *data;
    AdgTable *table;
    AdgTableCell *cell;

    title_block = (AdgTitleBlock *) object;
    data = title_block->data;
    table = (AdgTable *) object;

    switch (prop_id) {
    case PROP_TITLE:
        g_free(data->title);
        data->title = g_value_dup_string(value);
        cell = adg_table_get_cell(table, "title");
        adg_table_cell_set_text_value(cell, data->title);
        break;
    case PROP_DRAWING:
        g_free(data->drawing);
        data->drawing = g_value_dup_string(value);
        cell = adg_table_get_cell(table, "drawing");
        adg_table_cell_set_text_value(cell, data->drawing);
        break;
    case PROP_SIZE:
        g_free(data->size);
        data->size = g_value_dup_string(value);
        cell = adg_table_get_cell(table, "size");
        adg_table_cell_set_text_value(cell, data->size);
        break;
    case PROP_SCALE:
        g_free(data->scale);
        data->scale = g_value_dup_string(value);
        cell = adg_table_get_cell(table, "scale");
        adg_table_cell_set_text_value(cell, data->scale);
        break;
    case PROP_AUTHOR:
        g_free(data->author);
        data->author = g_value_dup_string(value);
        cell = adg_table_get_cell(table, "author");
        adg_table_cell_set_text_value(cell, data->author);
        break;
    case PROP_DATE:
        g_free(data->date);
        if (g_value_get_string(value) == NULL) {
            /* NULL means the date must be automatically updated */
            GDate *gdate;
            char buffer[100] = { 0 };

            gdate = g_date_new();
            g_date_set_time_t(gdate, time (NULL));
            g_date_strftime(buffer, sizeof(buffer), "%x", gdate);
            g_date_free(gdate);

            data->date = g_strdup(buffer);
        } else {
            data->date = g_value_dup_string(value);
        }
        cell = adg_table_get_cell(table, "date");
        adg_table_cell_set_text_value(cell, data->date);
        break;
    case PROP_LOGO:
        data->logo = g_value_get_object(value);
        cell = adg_table_get_cell(table, "logo");
        adg_table_cell_set_value(cell, data->logo);
        adg_table_cell_set_value_pos_explicit(cell, 0.5, 1, 0.5, 0.5);
        break;
    case PROP_PROJECTION:
        data->projection = g_value_get_object(value);
        cell = adg_table_get_cell(table, "projection");
        adg_table_cell_set_value(cell, data->projection);
        adg_table_cell_set_value_pos_explicit(cell, 0.5, 0.5, 0.5, 0.5);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_title_block_new:
 *
 * Creates a new empty title block entity.
 *
 * Returns: (transfer full): the newly created title block entity.
 *
 * Since: 1.0
 **/
AdgTitleBlock *
adg_title_block_new(void)
{
    return g_object_new(ADG_TYPE_TITLE_BLOCK, NULL);
}

/**
 * adg_title_block_set_title:
 * @title_block: an #AdgTitleBlock entity
 * @title: the new title
 *
 * Sets a new title on the title block.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_title(AdgTitleBlock *title_block, const gchar *title)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "title", title, NULL);
}

/**
 * adg_title_block_get_title:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the descriptive title associated to this title block.
 * The returned string is owned by @title_block and should not
 * be modifed or freed.
 *
 * Returns: (transfer none): the title or <constant>NULL</constant> on no title or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_title(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->title;
}

/**
 * adg_title_block_set_drawing:
 * @title_block: an #AdgTitleBlock entity
 * @drawing: the new drawing name
 *
 * Sets a new drawing name on the title block.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_drawing(AdgTitleBlock *title_block, const gchar *drawing)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "drawing", drawing, NULL);
}

/**
 * adg_title_block_get_drawing:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the drawing name, commonly used to specify the file name.
 * The returned string is owned by @title_block and should not
 * be modifed or freed.
 *
 * Returns: (transfer none): the drawing name or <constant>NULL</constant> on no name or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_drawing(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->drawing;
}

/**
 * adg_title_block_set_size:
 * @title_block: an #AdgTitleBlock entity
 * @size: the new size
 *
 * Sets a new size on the title block.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_size(AdgTitleBlock *title_block, const gchar *size)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "size", size, NULL);
}

/**
 * adg_title_block_get_size:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the media size (a descriptive name) where this drawing will
 * be printed. Usually contains something like <constant>"A4"</constant>
 * or <constant>"Letter"</constant>.
 * The returned string is owned by @title_block and should not
 * be modifed or freed.
 *
 * Returns: (transfer none): the size or <constant>NULL</constant> on no size or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_size(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->size;
}

/**
 * adg_title_block_set_scale:
 * @title_block: an #AdgTitleBlock entity
 * @scale: the new scale
 *
 * Sets a new scale on the title block.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_scale(AdgTitleBlock *title_block, const gchar *scale)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "scale", scale, NULL);
}

/**
 * adg_title_block_get_scale:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the scale descriptive name of the drawing.
 *
 * Returns: (transfer none): the scale text or <constant>NULL</constant> on no scale or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_scale(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->scale;
}

/**
 * adg_title_block_set_author:
 * @title_block: an #AdgTitleBlock entity
 * @author: the new author
 *
 * Sets a new author on the title block.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_author(AdgTitleBlock *title_block, const gchar *author)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "author", author, NULL);
}

/**
 * adg_title_block_get_author:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the author's name of the drawing.
 *
 * Returns: (transfer none): the author or <constant>NULL</constant> on no author or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_author(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->author;
}

/**
 * adg_title_block_set_date:
 * @title_block: an #AdgTitleBlock entity
 * @date: the new date
 *
 * Sets a new date on the title block. By default the date is
 * set to <constant>NULL</constant> and it will be implicitely
 * rendered using the*preferred representation for the current
 * local of the actual date. This is roughly equivalent to:
 *
 * <informalexample><programlisting language="C">
 * strftime(buffer, sizeof(buffer), "%x", now);
 * adg_title_block_set_date(title_block, buffer);
 * </programlisting></informalexample>
 *
 * To not render any value, use an empty string as @date.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_date(AdgTitleBlock *title_block, const gchar *date)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "date", date, NULL);
}

/**
 * adg_title_block_get_date:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the date of the rendering set on @title_block.
 *
 * Returns: (transfer none): the date or <constant>NULL</constant> on no date or errors.
 *
 * Since: 1.0
 **/
const gchar *
adg_title_block_get_date(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->date;
}

/**
 * adg_title_block_set_logo:
 * @title_block: an #AdgTitleBlock entity
 * @logo: the new logo
 *
 * Sets a new logo on the title block. This function will add
 * a reference to @logo, removing the eventual reference held
 * to the old logo, hence possibly destroying the old endity.
 *
 * The space reserved for the logo is 56x56, so try to keep the
 * new logo near this size or scale it accordingly.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_logo(AdgTitleBlock *title_block, AdgEntity *logo)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "logo", logo, NULL);
}

/**
 * adg_title_block_logo:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the logo bound to this title block.
 * The returned object is owned by @title_block and should not
 * be unreferenced although can be freely modified.
 *
 * Returns: (transfer none): the logo or <constant>NULL</constant> on no logo or errors.
 *
 * Since: 1.0
 **/
AdgEntity *
adg_title_block_logo(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->logo;
}

/**
 * adg_title_block_set_projection:
 * @title_block: an #AdgTitleBlock entity
 * @projection: the new projection
 *
 * Sets a new projection symbol on the title block. This function
 * will add a reference to @projection, removing the eventual
 * reference held to the old symbol, hence possibly destroying
 * the old endity.
 *
 * The space reserved for the projection is 56x56, so try to keep the
 * new projection near this size or scale it accordingly.
 *
 * Since: 1.0
 **/
void
adg_title_block_set_projection(AdgTitleBlock *title_block,
                               AdgEntity *projection)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(title_block, "projection", projection, NULL);
}

/**
 * adg_title_block_projection:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the projection bound to this title block.
 * The returned object is owned by @title_block and should not
 * be unreferenced although can be freely modified.
 *
 * Returns: (transfer none): the projection or <constant>NULL</constant> on no projection or errors.
 *
 * Since: 1.0
 **/
AdgEntity *
adg_title_block_projection(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return data->projection;
}
