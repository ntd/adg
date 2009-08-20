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


/**
 * ADG_SLOT_DIM_STYLE:
 *
 * Gets the slot id for this style class.
 *
 * Returns: the requested slot id
 **/


#include "adg-dim-style.h"
#include "adg-dim-style-private.h"
#include "adg-font-style.h"
#include "adg-line-style.h"
#include "adg-arrow-style.h"
#include "adg-context.h"
#include "adg-intl.h"
#include "adg-util.h"


enum {
    PROP_0,
    PROP_VALUE_STYLE,
    PROP_TOLERANCE_STYLE,
    PROP_NOTE_STYLE,
    PROP_LINE_STYLE,
    PROP_ARROW_STYLE,
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
static GPtrArray *      get_pool                (void);
static void             set_value_style         (AdgDimStyle    *dim_style,
                                                 AdgFontStyle   *style);
static void             set_tolerance_style     (AdgDimStyle    *dim_style,
                                                 AdgFontStyle   *style);
static void             set_note_style          (AdgDimStyle    *dim_style,
                                                 AdgFontStyle   *style);
static void             set_line_style          (AdgDimStyle    *dim_style,
                                                 AdgLineStyle   *style);
static void             set_arrow_style         (AdgDimStyle    *dim_style,
                                                 AdgArrowStyle  *style);
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


G_DEFINE_TYPE(AdgDimStyle, adg_dim_style, ADG_TYPE_STYLE)


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

    style_class->get_pool = get_pool;

    param = g_param_spec_object("value-style",
                                P_("Value Style"),
                                P_("Font style for the basic value of the dimension"),
                                ADG_TYPE_STYLE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_STYLE,
                                    param);

    param = g_param_spec_object("tolerance-style",
                                P_("Tolerance Style"),
                                P_("Font style for the tolerances"),
                                ADG_TYPE_STYLE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_STYLE,
                                    param);

    param = g_param_spec_object("note-style",
                                P_("Note Style"),
                                P_("Font style for the note (the text after or under the basic value)"),
                                ADG_TYPE_STYLE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE_STYLE, param);

    param = g_param_spec_object("line-style",
                                P_("Line Style"),
                                P_("Line style for the baseline and the extension lines"),
                                ADG_TYPE_STYLE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE_STYLE, param);

    param = g_param_spec_object("arrow-style",
                                P_("Arrow Style"),
                                P_("Arrow style to use on the baseline"),
                                ADG_TYPE_STYLE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ARROW_STYLE,
                                    param);

    param = g_param_spec_double("from-offset",
                                P_("From Offset"),
                                P_("Offset (in global space) of the extension lines from the path to the quote"),
                                0, G_MAXDOUBLE, 5, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FROM_OFFSET,
                                    param);

    param = g_param_spec_double("to-offset",
                                P_("To Offset"),
                                P_("How many extend (in global space) the extension lines after hitting the baseline"),
                                0, G_MAXDOUBLE, 5, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TO_OFFSET, param);

    param = g_param_spec_double("beyond",
                                P_("Beyond Length"),
                                P_("How much the baseline should be extended (in global space) beyond the extension lines on dimensions with outside arrows: 0 means to automatically compute this value at run-time as 3*arrow-size (got from the binded array-style)"),
                                0, G_MAXDOUBLE, 0, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BEYOND, param);

    param = g_param_spec_double("baseline-spacing",
                                P_("Baseline Spacing"),
                                P_("Distance between two consecutive baselines while stacking dimensions"),
                                0, G_MAXDOUBLE, 30, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BASELINE_SPACING,
                                    param);

    param = g_param_spec_double("tolerance-spacing",
                                P_("Tolerance Spacing"),
                                P_("Distance between up and down tolerance text"),
                                0, G_MAXDOUBLE, 2, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_SPACING,
                                    param);

    param = g_param_spec_boxed("quote-shift",
                               P_("Quote Shift"),
                               P_("Used to specify a smooth displacement (in global space) of the quote by taking as reference the perfect compact position (the middle of the baseline on common linear dimension, for instance)"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_QUOTE_SHIFT,
                                    param);

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

    data->value_style = adg_style_from_id(ADG_TYPE_FONT_STYLE,
                                          ADG_FONT_STYLE_VALUE);
    data->tolerance_style = adg_style_from_id(ADG_TYPE_FONT_STYLE,
                                              ADG_FONT_STYLE_TOLERANCE);
    data->note_style = adg_style_from_id(ADG_TYPE_FONT_STYLE,
                                         ADG_FONT_STYLE_NOTE);
    data->line_style = adg_style_from_id(ADG_TYPE_LINE_STYLE,
                                         ADG_LINE_STYLE_DIM);
    data->arrow_style = adg_style_from_id(ADG_TYPE_ARROW_STYLE,
                                          ADG_ARROW_STYLE_ARROW);
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
    case PROP_VALUE_STYLE:
        g_value_set_object(value, data->value_style);
        break;
    case PROP_TOLERANCE_STYLE:
        g_value_set_object(value, data->tolerance_style);
        break;
    case PROP_NOTE_STYLE:
        g_value_set_object(value, data->note_style);
        break;
    case PROP_LINE_STYLE:
        g_value_set_object(value, data->line_style);
        break;
    case PROP_ARROW_STYLE:
        g_value_set_object(value, data->arrow_style);
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
    case PROP_VALUE_STYLE:
        set_value_style(dim_style, g_value_get_object(value));
        break;
    case PROP_TOLERANCE_STYLE:
        set_tolerance_style(dim_style, g_value_get_object(value));
        break;
    case PROP_NOTE_STYLE:
        set_note_style(dim_style, g_value_get_object(value));
        break;
    case PROP_LINE_STYLE:
        set_line_style(dim_style, g_value_get_object(value));
        break;
    case PROP_ARROW_STYLE:
        set_arrow_style(dim_style, g_value_get_object(value));
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


AdgStyleSlot
_adg_dim_style_get_slot(void)
{
    static AdgStyleSlot slot = -1;

    if (G_UNLIKELY(slot < 0))
        slot = adg_context_get_slot(ADG_TYPE_DIM_STYLE);

    return slot;
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
 * adg_dim_style_get_value_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the basic value font style of @dim_style. No reference will
 * be added to the returned style; it should not be unreferenced.
 *
 * Returns: the basic value style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_value_style(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->value_style;
}

/**
 * adg_dim_style_set_value_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new basic value font style
 *
 * Sets a new font style on @dim_style for basic values. The old
 * font style (if any) will be unreferenced while a new reference
 * will be added to @style.
 **/
void
adg_dim_style_set_value_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_return_if_fail(ADG_IS_FONT_STYLE(style));

    set_value_style(dim_style, style);
    g_object_notify((GObject *) dim_style, "value-style");
}

/**
 * adg_dim_style_get_tolerance_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the tolerance style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Returns: the tolerance style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_tolerance_style(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->tolerance_style;
}

/**
 * adg_dim_style_set_tolerance_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new tolerance style
 *
 * Sets a new tolerance style on @dim_style. The old tolerance style (if any)
 * will be unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_tolerance_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_return_if_fail(ADG_IS_FONT_STYLE(style));

    set_tolerance_style(dim_style, style);
    g_object_notify((GObject *) dim_style, "tolerance-style");
}

/**
 * adg_dim_style_get_note_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the note style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Returns: the note style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_note_style(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->note_style;
}

/**
 * adg_dim_style_set_note_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new note style
 *
 * Sets a new note style on @dim_style. The old note style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_note_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_return_if_fail(ADG_IS_FONT_STYLE(style));

    set_note_style(dim_style, style);
    g_object_notify((GObject *) dim_style, "note-style");
}

/**
 * adg_dim_style_get_line_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the line style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Returns: the line style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_line_style(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->line_style;
}

/**
 * adg_dim_style_set_line_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new line style
 *
 * Sets a new line style on @dim_style. The old line style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_line_style(AdgDimStyle *dim_style, AdgLineStyle *style)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_return_if_fail(ADG_IS_LINE_STYLE(style));

    set_line_style(dim_style, style);
    g_object_notify((GObject *) dim_style, "line-style");
}

/**
 * adg_dim_style_get_arrow_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the arrow style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Returns: the arrow style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_arrow_style(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->arrow_style;
}

/**
 * adg_dim_style_set_arrow_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new arrow style
 *
 * Sets a new arrow style on @dim_style. The old arrow style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_arrow_style(AdgDimStyle *dim_style, AdgArrowStyle *style)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_return_if_fail(ADG_IS_ARROW_STYLE(style));

    set_arrow_style(dim_style, style);
    g_object_notify((GObject *) dim_style, "arrow-style");
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
 * the extension lines when a dimension has outside arrows. If the
 * underlying AdgDimStyle:beyond property is %0, this function returns
 * the 3*"arrow-size", where "arrow-size" is the value returned by
 * adg_arrow_style_get_size() on #AdgArrowStyle binded to @dim_style.
 *
 * Returns: the requested lenght
 **/
gdouble
adg_dim_style_beyond(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;
    gdouble arrow_size;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    if (data->beyond > 0)
        return data->beyond;

    arrow_size = adg_arrow_style_get_size((AdgArrowStyle *) data->arrow_style);

    return arrow_size * 3;
}

/**
 * adg_dim_style_get_beyond:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the baseline should extend beyound
 * the extension lines on dimension with outside arrows. This is an
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


static GPtrArray *
get_pool(void)
{
    static GPtrArray *pool = NULL;

    if (G_UNLIKELY(pool == NULL)) {
        cairo_pattern_t *pattern;

        pool = g_ptr_array_sized_new(ADG_DIM_STYLE_LAST);

        /* No need to specify further params: the default is already ISO */
        pattern = cairo_pattern_create_rgb(1, 0, 0);
        pool->pdata[ADG_DIM_STYLE_ISO] = g_object_new(ADG_TYPE_DIM_STYLE,
                                                      "pattern", pattern,
                                                      NULL);
        cairo_pattern_destroy(pattern);

        pool->len = ADG_DIM_STYLE_LAST;
    }

    return pool;
}

static void
set_value_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    AdgDimStylePrivate *data = dim_style->data;

    if (data->value_style)
        g_object_unref(data->value_style);

    g_object_ref(style);
    data->value_style = (AdgStyle *) style;
}

static void
set_tolerance_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    AdgDimStylePrivate *data = dim_style->data;

    if (data->tolerance_style)
        g_object_unref(data->tolerance_style);

    g_object_ref(style);
    data->tolerance_style = (AdgStyle *) style;
}

static void
set_note_style(AdgDimStyle *dim_style, AdgFontStyle *style)
{
    AdgDimStylePrivate *data = dim_style->data;

    if (data->note_style)
        g_object_unref(data->note_style);

    g_object_ref(style);
    data->note_style = (AdgStyle *) style;
}

static void
set_line_style(AdgDimStyle *dim_style, AdgLineStyle *style)
{
    AdgDimStylePrivate *data = dim_style->data;

    if (data->line_style)
        g_object_unref(data->line_style);

    g_object_ref(style);
    data->line_style = (AdgStyle *) style;
}

static void
set_arrow_style(AdgDimStyle *dim_style, AdgArrowStyle *style)
{
    AdgDimStylePrivate *data = dim_style->data;

    if (data->arrow_style)
        g_object_unref(data->arrow_style);

    g_object_ref(style);
    data->arrow_style = (AdgStyle *) style;
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
