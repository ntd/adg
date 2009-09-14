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
 * SECTION:adg-dim-style
 * @short_description: Dimension style related stuff
 *
 * Contains parameters on how to build dimensions such as the different font
 * styles (for value, tolerance and note), line style, offsets of the various
 * dimension components etc...
 */

/**
 * AdgDimStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-dim-style.h"
#include "adg-dim-style-private.h"
#include "adg-font-style.h"
#include "adg-line-style.h"
#include "adg-intl.h"
#include "adg-util.h"


enum {
    PROP_0,
    PROP_VALUE_DRESS,
    PROP_UP_DRESS,
    PROP_DOWN_DRESS,
    PROP_NOTE_DRESS,
    PROP_LINE_DRESS,
    PROP_FROM_OFFSET,
    PROP_TO_OFFSET,
    PROP_BEYOND,
    PROP_BASELINE_SPACING,
    PROP_TOLERANCE_SPACING,
    PROP_QUOTE_SHIFT,
    PROP_TOLERANCE_SHIFT,
    PROP_NOTE_SHIFT,
    PROP_NUMBER_FORMAT,
    PROP_NUMBER_TAG
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
                                                 cairo_t        *cr);
static void             set_quote_shift         (AdgDimStyle    *dim_style,
                                                 const AdgPair  *shift);
static void             set_tolerance_shift     (AdgDimStyle    *dim_style,
                                                 const AdgPair  *shift);
static void             set_note_shift          (AdgDimStyle    *dim_style,
                                                 const AdgPair  *shift);
static void             set_number_format       (AdgDimStyle    *dim_style,
                                                 const gchar    *format);
static void             set_number_tag          (AdgDimStyle    *dim_style,
                                                 const gchar    *tag);


G_DEFINE_TYPE(AdgDimStyle, adg_dim_style, ADG_TYPE_STYLE);


static void
adg_dim_style_class_init(AdgDimStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgDimStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    param = adg_param_spec_dress("value-dress",
                                  P_("Value Dress"),
                                  P_("Font dress for the nominal value of the dimension"),
                                  ADG_DRESS_TEXT_VALUE,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_DRESS, param);

    param = adg_param_spec_dress("up-dress",
                                  P_("Up-limit Dress"),
                                  P_("Font dress for the upper limit value"),
                                  ADG_DRESS_TEXT_LIMIT,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_UP_DRESS, param);

    param = adg_param_spec_dress("down-dress",
                                  P_("Down-limit Dress"),
                                  P_("Font dress for the lower limit value"),
                                  ADG_DRESS_TEXT_LIMIT,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DOWN_DRESS, param);

    param = adg_param_spec_dress("note-dress",
                                  P_("Note Dress"),
                                  P_("Font dress for the note"),
                                  ADG_DRESS_TEXT_VALUE,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE_DRESS, param);

    param = adg_param_spec_dress("line-dress",
                                  P_("Line Dress"),
                                  P_("Line dress for the baseline and the extension lines"),
                                  ADG_DRESS_LINE_DIMENSION,
                                  G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE_DRESS, param);

    param = g_param_spec_double("from-offset",
                                P_("From Offset"),
                                P_("Offset (in global space) of the extension lines from the path to the quote"),
                                0, G_MAXDOUBLE, 5,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FROM_OFFSET, param);

    param = g_param_spec_double("to-offset",
                                P_("To Offset"),
                                P_("How many extend (in global space) the extension lines after hitting the baseline"),
                                0, G_MAXDOUBLE, 5,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TO_OFFSET, param);

    param = g_param_spec_double("beyond",
                                P_("Beyond Length"),
                                P_("How much the baseline should be extended (in global space) beyond the extension lines on dimensions with outside markers: 0 means to automatically compute this value at run-time as 3*marker-size (got from the binded array-style)"),
                                0, G_MAXDOUBLE, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BEYOND, param);

    param = g_param_spec_double("baseline-spacing",
                                P_("Baseline Spacing"),
                                P_("Distance between two consecutive baselines while stacking dimensions"),
                                0, G_MAXDOUBLE, 30,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BASELINE_SPACING, param);

    param = g_param_spec_double("tolerance-spacing",
                                P_("Tolerance Spacing"),
                                P_("Distance between up and down tolerance text"),
                                0, G_MAXDOUBLE, 2,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_SPACING, param);

    param = g_param_spec_boxed("quote-shift",
                               P_("Quote Shift"),
                               P_("Used to specify a smooth displacement (in global space) of the quote by taking as reference the perfect compact position (the middle of the baseline on common linear dimension, for instance)"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_QUOTE_SHIFT, param);

    param = g_param_spec_boxed("tolerance-shift",
                               P_("Tolerance Shift"),
                               P_("Used to specify a smooth displacement (in global space) for the tolerance text by taking as reference the perfect compact position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_SHIFT,
                                    param);

    param = g_param_spec_boxed("note-shift",
                               P_("Note Shift"),
                               P_("Used to specify a smooth displacement (in global space) for the note text by taking as reference the perfect compact position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE_SHIFT, param);

    param = g_param_spec_string("number-format",
                                P_("Number Format"),
                                P_("The format (in printf style) of the numeric component of the basic value"),
                                "%-.7g", G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NUMBER_FORMAT,
                                    param);

    param = g_param_spec_string("number-tag",
                                P_("Number Tag"),
                                P_("The tag to substitute inside the basic value pattern"),
                                "<>", G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NUMBER_TAG, param);
}

static void
adg_dim_style_init(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(dim_style,
                                                           ADG_TYPE_DIM_STYLE,
                                                           AdgDimStylePrivate);

    data->value_dress = ADG_DRESS_TEXT_VALUE;
    data->up_dress = ADG_DRESS_TEXT_LIMIT;
    data->down_dress = ADG_DRESS_TEXT_LIMIT;
    data->note_dress = ADG_DRESS_TEXT_VALUE;
    data->line_dress = ADG_DRESS_LINE_DIMENSION;
    data->marker_dress = ADG_DRESS_UNDEFINED;
    data->from_offset = 6;
    data->to_offset = 6;
    data->beyond = 0;
    data->baseline_spacing = 30;
    data->tolerance_spacing = 1;
    data->quote_shift.x = 0;
    data->quote_shift.y = -4;
    data->tolerance_shift.x = +2;
    data->tolerance_shift.y = -2;
    data->note_shift.x = +4;
    data->note_shift.y = 0;
    data->number_format = g_strdup("%-.7g");
    data->number_tag = g_strdup("<>");

    dim_style->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgDimStylePrivate *data = ((AdgDimStyle *) object)->data;

    switch (prop_id) {
    case PROP_VALUE_DRESS:
        g_value_set_int(value, data->value_dress);
        break;
    case PROP_UP_DRESS:
        g_value_set_int(value, data->up_dress);
        break;
    case PROP_DOWN_DRESS:
        g_value_set_int(value, data->down_dress);
        break;
    case PROP_NOTE_DRESS:
        g_value_set_int(value, data->note_dress);
        break;
    case PROP_LINE_DRESS:
        g_value_set_int(value, data->line_dress);
        break;
    case PROP_FROM_OFFSET:
        g_value_set_double(value, data->from_offset);
        break;
    case PROP_TO_OFFSET:
        g_value_set_double(value, data->to_offset);
        break;
    case PROP_BEYOND:
        g_value_set_double(value, data->beyond);
        break;
    case PROP_BASELINE_SPACING:
        g_value_set_double(value, data->baseline_spacing);
        break;
    case PROP_TOLERANCE_SPACING:
        g_value_set_double(value, data->tolerance_spacing);
        break;
    case PROP_QUOTE_SHIFT:
        g_value_set_boxed(value, &data->quote_shift);
        break;
    case PROP_TOLERANCE_SHIFT:
        g_value_set_boxed(value, &data->tolerance_shift);
        break;
    case PROP_NOTE_SHIFT:
        g_value_set_boxed(value, &data->note_shift);
        break;
    case PROP_NUMBER_FORMAT:
        g_value_set_string(value, data->number_format);
        break;
    case PROP_NUMBER_TAG:
        g_value_set_string(value, data->number_tag);
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
    AdgDimStyle *dim_style;
    AdgDimStylePrivate *data;

    dim_style = (AdgDimStyle *) object;
    data = dim_style->data;

    switch (prop_id) {
    case PROP_VALUE_DRESS:
        adg_dress_set(&data->value_dress, g_value_get_int(value));
        break;
    case PROP_UP_DRESS:
        adg_dress_set(&data->up_dress, g_value_get_int(value));
        break;
    case PROP_DOWN_DRESS:
        adg_dress_set(&data->down_dress, g_value_get_int(value));
        break;
    case PROP_NOTE_DRESS:
        adg_dress_set(&data->note_dress, g_value_get_int(value));
        break;
    case PROP_LINE_DRESS:
        adg_dress_set(&data->line_dress, g_value_get_int(value));
        break;
    case PROP_FROM_OFFSET:
        data->from_offset = g_value_get_double(value);
        break;
    case PROP_TO_OFFSET:
        data->to_offset = g_value_get_double(value);
        break;
    case PROP_BEYOND:
        data->beyond = g_value_get_double(value);
        break;
    case PROP_BASELINE_SPACING:
        data->baseline_spacing = g_value_get_double(value);
        break;
    case PROP_TOLERANCE_SPACING:
        data->tolerance_spacing = g_value_get_double(value);
        break;
    case PROP_QUOTE_SHIFT:
        set_quote_shift(dim_style, g_value_get_boxed(value));
        break;
    case PROP_TOLERANCE_SHIFT:
        set_tolerance_shift(dim_style, g_value_get_boxed(value));
        break;
    case PROP_NOTE_SHIFT:
        set_note_shift(dim_style, g_value_get_boxed(value));
        break;
    case PROP_NUMBER_FORMAT:
        set_number_format(dim_style, g_value_get_string(value));
        break;
    case PROP_NUMBER_TAG:
        set_number_tag(dim_style, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_dim_style_new:
 *
 * Constructs a new dimension style initialized with default params.
 *
 * Returns: a new dimension style
 **/
AdgStyle *
adg_dim_style_new(void)
{
    return g_object_new(ADG_TYPE_DIM_STYLE, NULL);
}

/**
 * adg_dim_style_get_value_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the basic value.
 *
 * Returns: the basic value dress
 **/
AdgDress
adg_dim_style_get_value_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->value_dress;
}

/**
 * adg_dim_style_set_value_dress:
 * @dim_style: an #AdgDimStyle object
 * @style: the new basic value font style
 *
 * Sets a new dress on @dim_style for the basic value.
 **/
void
adg_dim_style_set_value_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;

    if (adg_dress_set(&data->value_dress, dress))
        g_object_notify((GObject *) dim_style, "value-dress");
}

/**
 * adg_dim_style_get_up_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the upper limit.
 *
 * Returns: the upper limit dress
 **/
AdgDress
adg_dim_style_get_up_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->up_dress;
}

/**
 * adg_dim_style_set_up_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new upper limit dress
 *
 * Sets a new dress on @dim_style for the upper limit value.
 **/
void
adg_dim_style_set_up_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;

    if (adg_dress_set(&data->up_dress, dress))
        g_object_notify((GObject *) dim_style, "up-dress");
}

/**
 * adg_dim_style_get_down_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the lower limit.
 *
 * Returns: the lower limit dress
 **/
AdgDress
adg_dim_style_get_down_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->down_dress;
}

/**
 * adg_dim_style_set_down_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new lower limit dress
 *
 * Sets a new dress on @dim_style for the lower limit value.
 **/
void
adg_dim_style_set_down_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;

    if (adg_dress_set(&data->down_dress, dress))
        g_object_notify((GObject *) dim_style, "down-dress");
}

/**
 * adg_dim_style_get_note_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the note text.
 *
 * Returns: the note dress
 **/
AdgDress
adg_dim_style_get_note_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->note_dress;
}

/**
 * adg_dim_style_set_note_dress:
 * @dim_style: an #AdgDimStyle object
 * @style: the new note style
 *
 * Sets a new dress on @dim_style for the note text.
 **/
void
adg_dim_style_set_note_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;

    if (adg_dress_set(&data->note_dress, dress))
        g_object_notify((GObject *) dim_style, "note-dress");
}

/**
 * adg_dim_style_get_line_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for rendering the baseline and
 * the extension lines.
 *
 * Returns: the line dress
 **/
AdgDress
adg_dim_style_get_line_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->line_dress;
}

/**
 * adg_dim_style_set_line_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @dim_style.
 **/
void
adg_dim_style_set_line_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;

    if (adg_dress_set(&data->line_dress, dress))
        g_object_notify((GObject *) dim_style, "line-dress");
}

/**
 * adg_dim_style_get_from_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in global space) the extension lines must keep from the
 * sensed points.
 *
 * Returns: the requested distance
 **/
gdouble
adg_dim_style_get_from_offset(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->from_offset;
}

/**
 * adg_dim_style_set_from_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new "from-offset" value.
 **/
void
adg_dim_style_set_from_offset(AdgDimStyle *dim_style, gdouble offset)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;
    data->from_offset = offset;

    g_object_notify((GObject *) dim_style, "from-offset");
}

/**
 * adg_dim_style_get_to_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the extension lines must extend after
 * crossing the baseline.
 *
 * Returns: the requested distance
 **/
gdouble
adg_dim_style_get_to_offset(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->to_offset;
}

/**
 * adg_dim_style_set_to_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new "to-offset" value.
 **/
void
adg_dim_style_set_to_offset(AdgDimStyle *dim_style, gdouble offset)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;
    data->to_offset = offset;

    g_object_notify((GObject *) dim_style, "to-offset");
}

/**
 * adg_dim_style_beyond:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the baseline should extend beyound
 * the extension lines when a dimension has outside markers. If the
 * underlying AdgDimStyle:beyond property is %0, this function returns
 * the 3*"marker-size", where "marker-size" is the value returned by
 * adg_marker_style_get_size() on #AdgMarkerStyle binded to @dim_style.
 *
 * Returns: the requested lenght
 **/
gdouble
adg_dim_style_beyond(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;
    gdouble marker_size;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;
    marker_size = 10;

    if (data->beyond > 0)
        return data->beyond;

    return marker_size * 3;
}

/**
 * adg_dim_style_get_beyond:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the baseline should extend beyound
 * the extension lines on dimension with outside markers. This is an
 * accessor method: if you need AdgDimStyle:beyond for rendering purpose,
 * use adg_dim_style_beyond() instead.
 *
 * Returns: the requested lenght or %0 for automatic computation
 **/
gdouble
adg_dim_style_get_beyond(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->beyond;
}

/**
 * adg_dim_style_set_beyond:
 * @dim_style: an #AdgDimStyle object
 * @length: the new length
 *
 * Sets a new "beyond" value.
 **/
void
adg_dim_style_set_beyond(AdgDimStyle *dim_style, gdouble length)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;
    data->beyond = length;

    g_object_notify((GObject *) dim_style, "beyond");
}

/**
 * adg_dim_style_get_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance between two consecutive baselines
 * while stacking dimensions.
 *
 * Returns: the requested spacing
 **/
gdouble
adg_dim_style_get_baseline_spacing(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->baseline_spacing;
}

/**
 * adg_dim_style_set_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new "baseline-spacing" value.
 **/
void
adg_dim_style_set_baseline_spacing(AdgDimStyle *dim_style, gdouble spacing)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;
    data->baseline_spacing = spacing;

    g_object_notify((GObject *) dim_style, "baseline-spacing");
}

/**
 * adg_dim_style_get_tolerance_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in global space) between up and down tolerances.
 *
 * Returns: the requested spacing
 **/
gdouble
adg_dim_style_get_tolerance_spacing(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->tolerance_spacing;
}

/**
 * adg_dim_style_set_tolerance_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new "tolerance-spacing" value.
 **/
void
adg_dim_style_set_tolerance_spacing(AdgDimStyle *dim_style, gdouble spacing)
{
    AdgDimStylePrivate *data;

    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    data = dim_style->data;
    data->tolerance_spacing = spacing;

    g_object_notify((GObject *) dim_style, "tolerance-spacing");
}

/**
 * adg_dim_style_get_quote_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the quote. The returned pointer refers
 * to an internal allocated struct and must not be modified or freed.
 *
 * Returns: the requested shift
 **/
const AdgPair *
adg_dim_style_get_quote_shift(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return &data->quote_shift;
}

/**
 * adg_dim_style_set_quote_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "quote-shift" value.
 **/
void
adg_dim_style_set_quote_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    set_quote_shift(dim_style, shift);
    g_object_notify((GObject *) dim_style, "quote-shift");
}

/**
 * adg_dim_style_get_tolerance_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the tolerance text. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Returns: the requested shift
 **/
const AdgPair *
adg_dim_style_get_tolerance_shift(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return &data->tolerance_shift;
}

/**
 * adg_dim_style_set_tolerance_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "tolerance-shift" value.
 **/
void
adg_dim_style_set_tolerance_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    set_tolerance_shift(dim_style, shift);
    g_object_notify((GObject *) dim_style, "tolerance-shift");
}

/**
 * adg_dim_style_get_note_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the note text. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Returns: the requested shift
 **/
const AdgPair *
adg_dim_style_get_note_shift(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return &data->note_shift;
}

/**
 * adg_dim_style_set_note_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "note-shift" value.
 **/
void
adg_dim_style_set_note_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    set_note_shift(dim_style, shift);
    g_object_notify((GObject *) dim_style, "note-shift");
}

/**
 * adg_dim_style_get_number_format:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number format (in printf style) of this quoting style. The
 * returned pointer refers to internally managed text that must not be
 * modified or freed.
 *
 * Returns: the requested format
 **/
const gchar *
adg_dim_style_get_number_format(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->number_format;
}

/**
 * adg_dim_style_set_number_format:
 * @dim_style: an #AdgDimStyle object
 * @format: the new format to adopt
 *
 * Sets a new "number-format" value.
 **/
void
adg_dim_style_set_number_format(AdgDimStyle *dim_style, const gchar *format)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    set_number_format(dim_style, format);
    g_object_notify((GObject *) dim_style, "number-format");
}

/**
 * adg_dim_style_get_number_tag:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number tag to substitute while building the basic value. The
 * returned pointer refers to internally managed text that must not be
 * modified or freed.
 *
 * Returns: the requested tag
 **/
const gchar *
adg_dim_style_get_number_tag(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->number_tag;
}

/**
 * adg_dim_style_set_number_tag:
 * @dim_style: an #AdgDimStyle object
 * @tag: the new tag
 *
 * Sets a new "number-tag" value.
 **/
void
adg_dim_style_set_number_tag(AdgDimStyle *dim_style, const gchar *tag)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));

    set_number_tag(dim_style, tag);
    g_object_notify((GObject *) dim_style, "number-tag");
}


static void
apply(AdgStyle *style, cairo_t *cr)
{
}

static void
set_quote_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    AdgDimStylePrivate *data = dim_style->data;

    cpml_pair_copy(&data->quote_shift, shift);
}

static void
set_tolerance_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    AdgDimStylePrivate *data = dim_style->data;

    cpml_pair_copy(&data->tolerance_shift, shift);
}

static void
set_note_shift(AdgDimStyle *dim_style, const AdgPair *shift)
{
    AdgDimStylePrivate *data = dim_style->data;

    cpml_pair_copy(&data->note_shift, shift);
}

static void
set_number_format(AdgDimStyle *dim_style, const gchar *format)
{
    AdgDimStylePrivate *data = dim_style->data;

    g_free(data->number_format);
    data->number_format = g_strdup(format);
}

static void
set_number_tag(AdgDimStyle *dim_style, const gchar *tag)
{
    AdgDimStylePrivate *data = dim_style->data;

    g_free(data->number_tag);
    data->number_tag = g_strdup(tag);
}
