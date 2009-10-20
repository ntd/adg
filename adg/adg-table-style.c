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
 * SECTION:adg-table-style
 * @short_description: Customization of table rendering
 *
 * Contains parameters on how to build tables such as the lines to
 * use for borders and grids and the font dresses for titles or values.
 */

/**
 * AdgTableStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-table-style.h"
#include "adg-table-style-private.h"
#include "adg-dress-builtins.h"
#include "adg-font-style.h"
#include "adg-line-style.h"
#include "adg-intl.h"
#include "adg-util.h"


enum {
    PROP_0,
    PROP_COLOR_DRESS,
    PROP_BORDER_DRESS,
    PROP_GRID_DRESS,
    PROP_TITLE_DRESS,
    PROP_VALUE_DRESS,
    PROP_PADDING
};


static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             apply                   (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);


G_DEFINE_TYPE(AdgTableStyle, adg_table_style, ADG_TYPE_STYLE);


static void
adg_table_style_class_init(AdgTableStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTableStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    param = adg_param_spec_dress("color-dress",
                                  P_("Color Dress"),
                                  P_("Color dress for the whole tableension"),
                                  ADG_DRESS_COLOR,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COLOR_DRESS, param);

    param = adg_param_spec_dress("border-dress",
                                  P_("Border Dress"),
                                  P_("Line dress to use while drawing the table border"),
                                  ADG_DRESS_LINE_STROKE,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BORDER_DRESS, param);

    param = adg_param_spec_dress("grid-dress",
                                  P_("Grid Dress"),
                                  P_("Line dress to use while rendering the grid of the table"),
                                  ADG_DRESS_LINE_HATCH,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GRID_DRESS, param);

    param = adg_param_spec_dress("title-dress",
                                  P_("Title Dress"),
                                  P_("Font dress to use for titles"),
                                  ADG_DRESS_TEXT_VALUE,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TITLE_DRESS, param);

    param = adg_param_spec_dress("value-dress",
                                  P_("Value Dress"),
                                  P_("Font dress to use for values inside the cells"),
                                  ADG_DRESS_TEXT_LIMIT,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_DRESS, param);

    param = g_param_spec_boxed("padding",
                               P_("Padding"),
                               P_("How much space from the extents must be reserved (that is, left clear) inside the cell"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PADDING, param);
}

static void
adg_table_style_init(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(table_style,
                                                           ADG_TYPE_TABLE_STYLE,
                                                           AdgTableStylePrivate);

    data->color_dress = ADG_DRESS_COLOR;
    data->border_dress = ADG_DRESS_LINE_STROKE;
    data->grid_dress = ADG_DRESS_LINE_HATCH;
    data->title_dress = ADG_DRESS_TEXT_VALUE;
    data->value_dress = ADG_DRESS_TEXT_LIMIT;
    data->padding.x = 2;
    data->padding.y = 2;

    table_style->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        g_value_set_int(value, data->color_dress);
        break;
    case PROP_BORDER_DRESS:
        g_value_set_int(value, data->border_dress);
        break;
    case PROP_GRID_DRESS:
        g_value_set_int(value, data->grid_dress);
        break;
    case PROP_TITLE_DRESS:
        g_value_set_int(value, data->title_dress);
        break;
    case PROP_VALUE_DRESS:
        g_value_set_int(value, data->value_dress);
        break;
    case PROP_PADDING:
        g_value_set_boxed(value, &data->padding);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        adg_dress_set(&data->color_dress, g_value_get_int(value));
        break;
    case PROP_BORDER_DRESS:
        adg_dress_set(&data->border_dress, g_value_get_int(value));
        break;
    case PROP_GRID_DRESS:
        adg_dress_set(&data->grid_dress, g_value_get_int(value));
        break;
    case PROP_TITLE_DRESS:
        adg_dress_set(&data->title_dress, g_value_get_int(value));
        break;
    case PROP_VALUE_DRESS:
        adg_dress_set(&data->value_dress, g_value_get_int(value));
        break;
    case PROP_PADDING:
        cpml_pair_copy(&data->padding, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_table_style_new:
 *
 * Constructs a new tableension style initialized with default params.
 *
 * Returns: a new tableension style
 **/
AdgStyle *
adg_table_style_new(void)
{
    return g_object_new(ADG_TYPE_TABLE_STYLE, NULL);
}

/**
 * adg_table_style_get_color_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the @table_style color dress to be used. This dress should be
 * intended as a fallback color as it could be overriden by more
 * specific dresses, such as a color explicitely specified on the
 * #AdgTableStyle:value-dress.
 *
 * Returns: the color dress
 **/
AdgDress
adg_table_style_get_color_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->color_dress;
}

/**
 * adg_table_style_set_color_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new color dress
 *
 * Sets a new color dress on @table_style.
 **/
void
adg_table_style_set_color_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->color_dress, dress))
        g_object_notify((GObject *) table_style, "color-dress");
}

/**
 * adg_table_style_get_border_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the line dress to be used for rendering the borders with
 * @table_style.
 *
 * Returns: the line dress
 **/
AdgDress
adg_table_style_get_border_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->border_dress;
}

/**
 * adg_table_style_set_border_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @table_style for rendering the borders.
 **/
void
adg_table_style_set_border_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->border_dress, dress))
        g_object_notify((GObject *) table_style, "border-dress");
}

/**
 * adg_table_style_get_grid_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the line dress to be used for rendering the grids with
 * @table_style.
 *
 * Returns: the line dress
 **/
AdgDress
adg_table_style_get_grid_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->grid_dress;
}

/**
 * adg_table_style_set_grid_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @table_style for rendering the grids.
 **/
void
adg_table_style_set_grid_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->grid_dress, dress))
        g_object_notify((GObject *) table_style, "grid-dress");
}

/**
 * adg_table_style_get_title_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the font dress to be used for rendering cell titles
 * with @table_style.
 *
 * Returns: the font dress
 **/
AdgDress
adg_table_style_get_title_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->title_dress;
}

/**
 * adg_table_style_set_title_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new font dress
 *
 * Sets a new font dress on @table_style for rendering cell titles.
 **/
void
adg_table_style_set_title_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->title_dress, dress))
        g_object_notify((GObject *) table_style, "title-dress");
}

/**
 * adg_table_style_get_value_dress:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the font dress to be used for rendering cell values
 * with @table_style.
 *
 * Returns: the font dress
 **/
AdgDress
adg_table_style_get_value_dress(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), ADG_DRESS_UNDEFINED);

    data = table_style->data;

    return data->value_dress;
}

/**
 * adg_table_style_set_value_dress:
 * @table_style: an #AdgTableStyle object
 * @dress: the new font dress
 *
 * Sets a new font dress on @table_style for rendering cell values.
 **/
void
adg_table_style_set_value_dress(AdgTableStyle *table_style, AdgDress dress)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));

    data = table_style->data;

    if (adg_dress_set(&data->value_dress, dress))
        g_object_notify((GObject *) table_style, "value-dress");
}

/**
 * adg_table_style_get_padding:
 * @table_style: an #AdgTableStyle object
 *
 * Gets the padding values in x and y to be left clear inside the cells.
 * The returned pointer refers to an internal allocated struct and
 * must not be modified or freed.
 *
 * Returns: the internal padding values
 **/
const AdgPair *
adg_table_style_get_padding(AdgTableStyle *table_style)
{
    AdgTableStylePrivate *data;

    g_return_val_if_fail(ADG_IS_TABLE_STYLE(table_style), NULL);

    data = table_style->data;

    return &data->padding;
}

/**
 * adg_table_style_set_quote_shift:
 * @table_style: an #AdgTableStyle object
 * @padding: the new padding values
 *
 * Sets new #AdgTableStyle:padding values.
 **/
void
adg_table_style_set_padding(AdgTableStyle *table_style, const AdgPair *padding)
{
    AdgTableStylePrivate *data;

    g_return_if_fail(ADG_IS_TABLE_STYLE(table_style));
    g_return_if_fail(padding != NULL);

    data = table_style->data;
    cpml_pair_copy(&data->padding, padding);

    g_object_notify((GObject *) table_style, "padding");
}


static void
apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgTableStylePrivate *data = ((AdgTableStyle *) style)->data;

    adg_entity_apply_dress(entity, data->color_dress, cr);
}
