/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
 * Contains parameters on how to build dimensions such as the format of the
 * label, the different font styles (for value and limits), line style,
 * offsets of the various dimension components etc.
 *
 * A typical use case is to set the quote of angular dimensions in sexagesimal
 * units. To do this, use something similar to the following code:
 *
 * |[
 * AdgDimStyle *fallback, *dim_style;
 * AdgMarker   *marker;
 *
 * fallback = ADG_DIM_STYLE(adg_dress_get_fallback(ADG_DRESS_DIMENSION);
 * dim_style = adg_dim_style_new();
 *
 * // Use the same markers of the fallback style
 * marker = adg_dim_style_marker1_new(fallback);
 * adg_dim_style_set_marker1(dim_style, marker);
 * g_object_unref(marker);
 *
 * marker = adg_dim_style_marker2_new(fallback);
 * adg_dim_style_set_marker2(dim_style, marker);
 * g_object_unref(marker);
 *
 * // Set the decimals and rounding properties
 * adg_dim_style_set_decimals(dim_style, 0);
 * adg_dim_style_set_rounding(dim_style, 3);
 *
 * // Set the arguments and the format of the quote number
 * adg_dim_style_set_number_arguments(dim_style, "Dm");
 * adg_dim_style_set_number_format(dim_style, "%g°(%g')");
 * ]|
 *
 * After that you can apply the new style to the
 * <constant>ADG_DRESS_DIMENSION</constant> of the entity you want to change,
 * e.g.:
 *
 * |[
 * adg_entity_set_style(my_angular_dimension,
 *                      ADG_DRESS_DIMENSION,
 *                      ADG_STYLE(dim_style));
 * ]|
 *
 * Since: 1.0
 */

/**
 * AdgDimStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-style.h"
#include "adg-font-style.h"
#include "adg-dash.h"
#include "adg-line-style.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-marker.h"
#include "adg-dress.h"
#include "adg-param-dress.h"

#include "adg-dim-style.h"
#include "adg-dim-style-private.h"

#include <string.h>
#include <math.h>


#define VALID_FORMATS "aieDdMmSs"


G_DEFINE_TYPE(AdgDimStyle, adg_dim_style, ADG_TYPE_STYLE)

enum {
    PROP_0,
    PROP_MARKER1,
    PROP_MARKER2,
    PROP_COLOR_DRESS,
    PROP_VALUE_DRESS,
    PROP_MIN_DRESS,
    PROP_MAX_DRESS,
    PROP_LINE_DRESS,
    PROP_FROM_OFFSET,
    PROP_TO_OFFSET,
    PROP_BEYOND,
    PROP_BASELINE_SPACING,
    PROP_LIMITS_SPACING,
    PROP_QUOTE_SHIFT,
    PROP_LIMITS_SHIFT,
    PROP_NUMBER_FORMAT,
    PROP_NUMBER_ARGUMENTS,
    PROP_NUMBER_TAG,
    PROP_DECIMALS,
    PROP_ROUNDING,
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
static void             _adg_apply              (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);
static AdgMarker *      _adg_marker_new         (const AdgMarkerData
                                                                *marker_data);
static void             _adg_set_marker         (AdgMarkerData  *marker_data,
                                                 AdgMarker      *marker);
static void             _adg_free_marker        (AdgMarkerData  *marker_data);


static void
adg_dim_style_class_init(AdgDimStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgDimStylePrivate));

    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->apply = _adg_apply;

    param = g_param_spec_object("marker1",
                                P_("First Marker"),
                                P_("The template entity to use as first marker"),
                                ADG_TYPE_MARKER,
                                G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_MARKER1, param);

    param = g_param_spec_object("marker2",
                                P_("Second Marker"),
                                P_("The template entity to use as second marker"),
                                ADG_TYPE_MARKER,
                                G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_MARKER2, param);

    param = adg_param_spec_dress("color-dress",
                                 P_("Color Dress"),
                                 P_("Color dress for the whole dimension"),
                                 ADG_DRESS_COLOR_DIMENSION,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COLOR_DRESS, param);

    param = adg_param_spec_dress("value-dress",
                                 P_("Value Dress"),
                                 P_("Font dress for the nominal value of the dimension"),
                                 ADG_DRESS_FONT_QUOTE_TEXT,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_DRESS, param);

    param = adg_param_spec_dress("min-dress",
                                 P_("Minimum Limit Dress"),
                                 P_("Font dress for the lower limit value"),
                                 ADG_DRESS_FONT_QUOTE_ANNOTATION,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MIN_DRESS, param);

    param = adg_param_spec_dress("max-dress",
                                 P_("Maximum Limit Dress"),
                                 P_("Font dress for the upper limit value"),
                                 ADG_DRESS_FONT_QUOTE_ANNOTATION,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MAX_DRESS, param);

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
                                P_("How much the baseline should be extended (in global space) beyond the extension lines on dimensions with outside markers"),
                                0, G_MAXDOUBLE, 20,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BEYOND, param);

    param = g_param_spec_double("baseline-spacing",
                                P_("Baseline Spacing"),
                                P_("Distance between two consecutive baselines while stacking dimensions"),
                                0, G_MAXDOUBLE, 32,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BASELINE_SPACING, param);

    param = g_param_spec_double("limits-spacing",
                                P_("Limits Spacing"),
                                P_("Distance between limits/tolerances"),
                                0, G_MAXDOUBLE, 2,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LIMITS_SPACING, param);

    param = g_param_spec_boxed("quote-shift",
                               P_("Quote Shift"),
                               P_("Used to specify a smooth displacement (in global space) of the quote by taking as reference the perfect compact position (the middle of the baseline on common linear dimension, for instance)"),
                               CPML_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_QUOTE_SHIFT, param);

    param = g_param_spec_boxed("limits-shift",
                               P_("Limits Shift"),
                               P_("Used to specify a smooth displacement (in global space) for the limits/tolerances by taking as reference the perfect compact position"),
                               CPML_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LIMITS_SHIFT, param);

    param = g_param_spec_string("number-format",
                                P_("Number Format"),
                                P_("The format (in printf style) of the numeric component of the basic value"),
                                "%-.7g",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NUMBER_FORMAT, param);

    param = g_param_spec_string("number-arguments",
                                P_("Number Arguments"),
                                P_("The arguments to pass to the format function: see adg_dim_style_set_number_arguments() for further details"),
                                "d",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NUMBER_ARGUMENTS, param);

    param = g_param_spec_string("number-tag",
                                P_("Number Tag"),
                                P_("The tag to substitute inside the value template string"),
                                "<>",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NUMBER_TAG, param);

    param = g_param_spec_int("decimals",
                             P_("Rounded Value Decimals"),
                             P_("Number of significant decimals to use in the format string for rounded values (-1 to disable)"),
                             -1, G_MAXINT,
                             2,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DECIMALS, param);

    param = g_param_spec_int("rounding",
                             P_("Raw Value Decimals"),
                             P_("Number of significant decimals the raw value must be rounded to (-1 to disable)"),
                             -1, G_MAXINT,
                             6,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ROUNDING, param);
}

static void
adg_dim_style_init(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(dim_style,
                                                           ADG_TYPE_DIM_STYLE,
                                                           AdgDimStylePrivate);

    data->marker1.type = 0;
    data->marker1.n_parameters = 0;
    data->marker1.parameters = NULL;
    data->marker2.type = 0;
    data->marker2.n_parameters = 0;
    data->marker2.parameters = NULL;
    data->color_dress = ADG_DRESS_COLOR_DIMENSION;
    data->value_dress = ADG_DRESS_FONT_QUOTE_TEXT;
    data->min_dress = ADG_DRESS_FONT_QUOTE_ANNOTATION;
    data->max_dress = ADG_DRESS_FONT_QUOTE_ANNOTATION;
    data->line_dress = ADG_DRESS_LINE_DIMENSION;
    data->marker_dress = ADG_DRESS_UNDEFINED;
    data->from_offset = 6;
    data->to_offset = 6;
    data->beyond = 20;
    data->baseline_spacing = 32;
    data->limits_spacing = 0;
    data->quote_shift.x = 4;
    data->quote_shift.y = -1;
    data->limits_shift.x = +2;
    data->limits_shift.y = +2;
    data->number_format = g_strdup("%-.7g");
    data->number_arguments = g_strdup("d");
    data->number_tag = g_strdup("<>");
    data->decimals = 2;
    data->rounding = 6;

    dim_style->data = data;
}

static void
_adg_finalize(GObject *object)
{
    AdgDimStylePrivate *data = ((AdgDimStyle *) object)->data;

    _adg_free_marker(&data->marker1);
    _adg_free_marker(&data->marker2);

    g_free(data->number_format);
    data->number_format = NULL;

    g_free(data->number_arguments);
    data->number_arguments = NULL;

    g_free(data->number_tag);
    data->number_tag = NULL;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgDimStylePrivate *data = ((AdgDimStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        g_value_set_enum(value, data->color_dress);
        break;
    case PROP_VALUE_DRESS:
        g_value_set_enum(value, data->value_dress);
        break;
    case PROP_MIN_DRESS:
        g_value_set_enum(value, data->min_dress);
        break;
    case PROP_MAX_DRESS:
        g_value_set_enum(value, data->max_dress);
        break;
    case PROP_LINE_DRESS:
        g_value_set_enum(value, data->line_dress);
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
    case PROP_LIMITS_SPACING:
        g_value_set_double(value, data->limits_spacing);
        break;
    case PROP_QUOTE_SHIFT:
        g_value_set_boxed(value, &data->quote_shift);
        break;
    case PROP_LIMITS_SHIFT:
        g_value_set_boxed(value, &data->limits_shift);
        break;
    case PROP_NUMBER_FORMAT:
        g_value_set_string(value, data->number_format);
        break;
    case PROP_NUMBER_ARGUMENTS:
        g_value_set_string(value, data->number_arguments);
        break;
    case PROP_NUMBER_TAG:
        g_value_set_string(value, data->number_tag);
        break;
    case PROP_DECIMALS:
        g_value_set_int(value, data->decimals);
        break;
    case PROP_ROUNDING:
        g_value_set_int(value, data->rounding);
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
    AdgDimStyle *dim_style;
    AdgDimStylePrivate *data;

    dim_style = (AdgDimStyle *) object;
    data = dim_style->data;

    switch (prop_id) {
    case PROP_MARKER1:
        _adg_set_marker(&data->marker1, g_value_get_object(value));
        break;
    case PROP_MARKER2:
        _adg_set_marker(&data->marker2, g_value_get_object(value));
        break;
    case PROP_COLOR_DRESS:
        data->color_dress = g_value_get_enum(value);
        break;
    case PROP_VALUE_DRESS:
        data->value_dress = g_value_get_enum(value);
        break;
    case PROP_MIN_DRESS:
        data->min_dress = g_value_get_enum(value);
        break;
    case PROP_MAX_DRESS:
        data->max_dress = g_value_get_enum(value);
        break;
    case PROP_LINE_DRESS:
        data->line_dress = g_value_get_enum(value);
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
    case PROP_LIMITS_SPACING:
        data->limits_spacing = g_value_get_double(value);
        break;
    case PROP_QUOTE_SHIFT:
        cpml_pair_copy(&data->quote_shift, g_value_get_boxed(value));
        break;
    case PROP_LIMITS_SHIFT:
        cpml_pair_copy(&data->limits_shift, g_value_get_boxed(value));
        break;
    case PROP_NUMBER_FORMAT:
        g_free(data->number_format);
        data->number_format = g_value_dup_string(value);
        break;
    case PROP_NUMBER_ARGUMENTS: {
        const gchar *arguments = g_value_get_string(value);
        g_return_if_fail(arguments == NULL || strspn(arguments, VALID_FORMATS) == strlen(arguments));
        g_free(data->number_arguments);
        data->number_arguments = g_strdup(arguments);
        break;
    }
    case PROP_NUMBER_TAG:
        g_free(data->number_tag);
        data->number_tag = g_value_dup_string(value);
        break;
    case PROP_DECIMALS:
        data->decimals = g_value_get_int(value);
        break;
    case PROP_ROUNDING:
        data->rounding = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_dim_style_new:
 *
 * Constructs a new empty dimension style initialized with default params.
 *
 * Returns: (transfer full): a newly created dimension style.
 *
 * Since: 1.0
 **/
AdgDimStyle *
adg_dim_style_new(void)
{
    return g_object_new(ADG_TYPE_DIM_STYLE, NULL);
}

/**
 * adg_dim_style_set_marker1:
 * @dim_style: an #AdgStyle
 * @marker: an #AdgMarker derived entity
 *
 * Uses @marker as entity template to generate a new marker entity
 * when a call to adg_dim_style_marker1_new() is made. It is allowed
 * to pass <constant>NULL</constant> as @marker, in which case the
 * template data of the first marker are unset.
 *
 * This method duplicates internally the property values of @marker,
 * so any further change to @marker does not affect @dim_style anymore.
 * This also means @marker could be destroyed without problems after
 * this call because @dim_style uses only its property values and does
 * not add any references to @marker.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_marker1(AdgDimStyle *dim_style, AdgMarker *marker)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "marker1", marker, NULL);
}

/**
 * adg_dim_style_marker1_new:
 * @dim_style: an #AdgDimStyle
 *
 * Creates a new marker entity by cloning the #AdgDimStyle:marker1
 * object. The returned entity should be unreferenced with
 * g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a newly created marker or <constant>NULL</constant> if the #AdgDimStyle:marker1 property is not set or on errors.
 *
 * Since: 1.0
 **/
AdgMarker *
adg_dim_style_marker1_new(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return _adg_marker_new(&data->marker1);
}

/**
 * adg_dim_style_set_marker2:
 * @dim_style: an #AdgStyle
 * @marker: an #AdgMarker derived entity
 *
 * Uses @marker as entity template to generate a new marker entity
 * when a call to adg_dim_style_marker2_new() is made. It is allowed
 * to pass <constant>NULL</constant> as @marker, in which case the
 * template data of the second marker are unset.
 *
 * This method duplicates internally the property values of @marker,
 * so any further change to @marker does not affect @dim_style anymore.
 * This also means @marker could be destroyed without problems after
 * this call because @dim_style uses only its property values and does
 * not add any references to @marker.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_marker2(AdgDimStyle *dim_style, AdgMarker *marker)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "marker2", marker, NULL);
}

/**
 * adg_dim_style_marker2_new:
 * @dim_style: an #AdgDimStyle
 *
 * Creates a new marker entity by cloning the #AdgDimStyle:marker2
 * object. The returned entity should be unreferenced with
 * g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a newly created marker or <constant>NULL</constant> if the #AdgDimStyle:marker2 property is not set or on errors.
 *
 * Since: 1.0
 **/
AdgMarker *
adg_dim_style_marker2_new(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return _adg_marker_new(&data->marker2);
}

/**
 * adg_dim_style_set_color_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new color dress
 *
 * Sets a new color dress on @dim_style.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_color_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "color-dress", dress, NULL);
}

/**
 * adg_dim_style_get_color_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style color dress to be used. This dress should be
 * intended as a fallback color as it could be overriden by more
 * specific dresses, such as a color explicitely specified on the
 * #AdgDimStyle:value-dress.
 *
 * Returns: (transfer none): the color dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_dim_style_get_color_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->color_dress;
}

/**
 * adg_dim_style_set_value_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new basic value font style
 *
 * Sets a new dress on @dim_style for the basic value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_value_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "value-dress", dress, NULL);
}

/**
 * adg_dim_style_get_value_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the font dress to be used for the basic value of dimensions
 * with @dim_style.
 *
 * Returns: (transfer none): the font dress.
 *
 * Since: 1.0
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
 * adg_dim_style_set_min_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new lower limit dress
 *
 * Sets a new dress on @dim_style for the lower limit value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_min_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "min-dress", dress, NULL);
}

/**
 * adg_dim_style_get_min_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the lower limit.
 *
 * Returns: (transfer none): the lower limit dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_dim_style_get_min_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->min_dress;
}

/**
 * adg_dim_style_set_max_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new upper limit dress
 *
 * Sets a new dress on @dim_style for the upper limit value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_max_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "max-dress", dress, NULL);
}

/**
 * adg_dim_style_get_max_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the @dim_style dress to be used for the upper limit.
 *
 * Returns: (transfer none): the upper limit dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_dim_style_get_max_dress(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), ADG_DRESS_UNDEFINED);

    data = dim_style->data;

    return data->max_dress;
}

/**
 * adg_dim_style_set_line_dress:
 * @dim_style: an #AdgDimStyle object
 * @dress: the new line dress
 *
 * Sets a new line dress on @dim_style.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_line_dress(AdgDimStyle *dim_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "line-dress", dress, NULL);
}

/**
 * adg_dim_style_get_line_dress:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the line dress to be used for rendering the base and
 * the extension lines with @dim_style.
 *
 * Returns: (transfer none): the line dress.
 *
 * Since: 1.0
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
 * adg_dim_style_set_from_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new value in the #AdgDimStyle:from-offset property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_from_offset(AdgDimStyle *dim_style, gdouble offset)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "from-offset", offset, NULL);
}

/**
 * adg_dim_style_get_from_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in global space) the extension lines must keep from the
 * sensed points.
 *
 * Returns: the requested distance.
 *
 * Since: 1.0
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
 * adg_dim_style_set_to_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new value in the #AdgDimStyle:to-offset property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_to_offset(AdgDimStyle *dim_style, gdouble offset)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "to-offset", offset, NULL);
}

/**
 * adg_dim_style_get_to_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the extension lines must extend after
 * crossing the baseline.
 *
 * Returns: the requested distance.
 *
 * Since: 1.0
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
 * adg_dim_style_set_beyond:
 * @dim_style: an #AdgDimStyle object
 * @beyond: the new length
 *
 * Sets a new value in the #AdgDimStyle:beyond property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_beyond(AdgDimStyle *dim_style, gdouble beyond)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "beyond", beyond, NULL);
}

/**
 * adg_dim_style_get_beyond:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in global space) the baseline should extend beyond
 * the extension lines on dimension with outside markers.
 *
 * Returns: the requested beyond length.
 *
 * Since: 1.0
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
 * adg_dim_style_set_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new value in the #AdgDimStyle:baseline-spacing value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_baseline_spacing(AdgDimStyle *dim_style, gdouble spacing)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "baseline-spacing", spacing, NULL);
}

/**
 * adg_dim_style_get_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance between two consecutive baselines
 * while stacking dimensions.
 *
 * Returns: the requested spacing.
 *
 * Since: 1.0
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
 * adg_dim_style_set_limits_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new #AdgDimStyle:limits-spacing value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_limits_spacing(AdgDimStyle *dim_style, gdouble spacing)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "limits-spacing", spacing, NULL);
}

/**
 * adg_dim_style_get_limits_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in global space) between the limits/tolerances.
 *
 * Returns: the requested spacing.
 *
 * Since: 1.0
 **/
gdouble
adg_dim_style_get_limits_spacing(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), 0);

    data = dim_style->data;

    return data->limits_spacing;
}

/**
 * adg_dim_style_set_quote_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new #AdgDimStyle:quote-shift value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_quote_shift(AdgDimStyle *dim_style, const CpmlPair *shift)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "quote-shift", shift, NULL);
}

/**
 * adg_dim_style_get_quote_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the quote. The returned pointer refers
 * to an internal allocated struct and must not be modified or freed.
 *
 * Returns: (transfer none): the requested shift.
 *
 * Since: 1.0
 **/
const CpmlPair *
adg_dim_style_get_quote_shift(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return &data->quote_shift;
}

/**
 * adg_dim_style_set_limits_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new #AdgDimStyle:limits-shift value.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_limits_shift(AdgDimStyle *dim_style, const CpmlPair *shift)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "limits-shift", shift, NULL);
}

/**
 * adg_dim_style_get_limits_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement for the limits. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Returns: (transfer none): the requested shift.
 *
 * Since: 1.0
 **/
const CpmlPair *
adg_dim_style_get_limits_shift(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return &data->limits_shift;
}

/**
 * adg_dim_style_set_number_format:
 * @dim_style: an #AdgDimStyle object
 * @format: the new format to adopt
 *
 * Sets a new value in the #AdgDimStyle:number-format property.
 *
 * @format is similar to a printf() style format string but only 'e', 'E',
 * 'f', 'F', 'g' and 'G' specifiers are allowed. For reference, the
 * implementation leverages the g_ascii_formatd() GLib function. If you want
 * to use special characters (%"(", %")" and %"%") as literals you need to
 * escape them with a backslash.
 *
 * You can use round parenthesis to group together one or more % directives
 * with other related literals. In that case, when the value bound to the
 * directive will be 0, the whole group will disappear. Some example:
 *
 * |[
 * AdgDim *dim;
 * AdgDimStyle *dim_style;
 * gchar *text;
 *
 * dim = ADG_DIM(adg_ldim_new());
 * dim_style = ADG_DIM_STYLE(adg_entity_style(ADG_ENTITY(dim), ADG_DRESS_DIMENSION));
 *
 * adg_dim_style_set_number_arguments(dim_style, "dD");
 * adg_dim_style_set_number_format(dim_style, "(%g)( truncated to %g)");
 *
 * text = adg_dim_get_text(dim, 1.2);
 * g_print("%s\n", text); // Prints "1.2 truncated to 1"
 * g_free(text);
 *
 * text = adg_dim_get_text(dim, 0.2);
 * g_print("%s\n", text); // Prints "0.2"
 * g_free(text);
 *
 * text = adg_dim_get_text(dim, 0);
 * g_print("%s\n", text); // Prints ""
 * g_free(text);
 * ]|
 *
 * Groups can be nested and can have more than one value, in which case all
 * the values contained in a group must be 0 in order to let it disappear.
 * This comes in handy for removing trailing 0 fields. A typical example is
 * the sexagesimal representation of angles:
 *
 * |[
 * adg_dim_style_set_number_arguments(dim_style, "DMs");
 * adg_dim_style_set_number_format(dim_style, "%g°(%g'(%g\"))");
 *
 * text = adg_dim_get_text(dim, 1.5);
 * g_print("%s\n", text); // Prints "1°30'"
 * g_free(text);
 *
 * text = adg_dim_get_text(dim, 2.002777);
 * g_print("%s\n", text); // Prints "2°0'10\""
 * g_free(text);
 * ]|
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_number_format(AdgDimStyle *dim_style, const gchar *format)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "number-format", format, NULL);
}

/**
 * adg_dim_style_get_number_format:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number format (in printf style) of this quoting style. The
 * returned pointer refers to internally managed text that must not be
 * modified or freed.
 *
 * Returns: (transfer none): the requested format.
 *
 * Since: 1.0
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
 * adg_dim_style_set_number_arguments:
 * @dim_style: an #AdgDimStyle object
 * @arguments: the arguments to pass to the formatting function
 *
 * A string that identifies the arguments to pass to the formatting function.
 * See adg_dim_style_convert() to know the allowed character that can be
 * included into this string.
 *
 * The number of arguments (i.e. the @arguments length) must match the
 * #AdgDimStyle:number-format property (i.e. the number of % directives
 * included in that property). See adg_dim_style_set_number_format() for more
 * technical details and some examples.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_number_arguments(AdgDimStyle *dim_style, const gchar *arguments)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "number-arguments", arguments, NULL);
}

/**
 * adg_dim_style_get_number_arguments:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the arguments used by the formatting function. See
 * adg_dim_style_set_number_arguments() for details on what this means.
 *
 * Returns: (transfer none): the requested arguments.
 *
 * Since: 1.0
 **/
const gchar *
adg_dim_style_get_number_arguments(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), NULL);

    data = dim_style->data;

    return data->number_arguments;
}

/**
 * adg_dim_style_set_number_tag:
 * @dim_style: an #AdgDimStyle object
 * @tag: the new tag
 *
 * Sets a new tag in the #AdgDimStyle:number-tag property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_number_tag(AdgDimStyle *dim_style, const gchar *tag)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "number-tag", tag, NULL);
}

/**
 * adg_dim_style_get_number_tag:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number tag of @dim_style. This tag will be used while
 * generating the set values of the dimensions bound to this style:
 * check the #AdgDim:value documentation for further details.
 *
 * The returned pointer refers to internally managed text that
 * must not be modified or freed.
 *
 * Returns: (transfer none): the requested tag.
 *
 * Since: 1.0
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
 * adg_dim_style_set_decimals:
 * @dim_style: an #AdgDimStyle object
 * @decimals: number of significant decimals
 *
 * Sets a new value in the #AdgDimStyle:decimals property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_decimals(AdgDimStyle *dim_style, gint decimals)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "decimals", decimals, NULL);
}

/**
 * adg_dim_style_get_decimals:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the decimals the value of a dimension will be rounded to before the
 * rendering.
 *
 * Returns: the number of significant decimals or -2 on errors.
 *
 * Since: 1.0
 **/
gint
adg_dim_style_get_decimals(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), -2);

    data = dim_style->data;

    return data->decimals;
}

/**
 * adg_dim_style_set_rounding:
 * @dim_style: an #AdgDimStyle object
 * @rounding: number of significant decimals of the raw value
 *
 * Sets a new value in the #AdgDimStyle:rounding property.
 *
 * Since: 1.0
 **/
void
adg_dim_style_set_rounding(AdgDimStyle *dim_style, gint rounding)
{
    g_return_if_fail(ADG_IS_DIM_STYLE(dim_style));
    g_object_set(dim_style, "rounding", rounding, NULL);
}

/**
 * adg_dim_style_get_rounding:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number of decimals the raw value must be rounded to.
 *
 * Returns: the number of significant decimals or -2 on errors.
 *
 * Since: 1.0
 **/
gint
adg_dim_style_get_rounding(AdgDimStyle *dim_style)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), -2);

    data = dim_style->data;

    return data->rounding;
}

/**
 * adg_dim_style_convert:
 * @dim_style: an #AdgDimStyle object
 * @value: (inout): the value to convert
 * @format: the convertion to apply
 *
 * Converts @value using the specific @format algorithm and store the
 * result in @value itself. If @value is %NULL, nothing is performed.
 *
 * In the following the allowed values for @format:
 * - 'a': the raw @value, i.e. no conversion is performed;
 * - 'i': the raw number of minutes, i.e. the fractional part of @value x 60
 * - 'e': the raw number of seconds, i.e. the fractional part of the raw
 *        number of minutes x 60
 * - 'D': the truncated value of the raw value ('a');
 * - 'd': the rounded value of the raw value ('a');
 * - 'M': the truncated value of the raw number of minutes ('i');
 * - 'm': the rounded value of the raw number of minutes ('i');
 * - 'S': the truncated value of the raw number of seconds ('e');
 * - 's': the rounded value of the raw number of seconds ('e');
 *
 * The rounding is done according to the number of decimal specified in
 * #AdgDimStyle:decimals.
 *
 * Returns: %TRUE if @value has been converted, %FALSE on errors.
 *
 * Since: 1.0
 **/
gboolean
adg_dim_style_convert(AdgDimStyle *dim_style, gdouble *value, gchar format)
{
    AdgDimStylePrivate *data;

    g_return_val_if_fail(ADG_IS_DIM_STYLE(dim_style), FALSE);

    /* Inout parameter not provided: just return FALSE */
    if (value == NULL) {
        return FALSE;
    }

    data = dim_style->data;

    /* Round the raw value, if requested */
    if (data->rounding > -1) {
        gdouble coefficient = pow(10, data->rounding);
        *value = round(*value * coefficient) / coefficient;
    }

    switch (format) {

    case 'a':                           /* Raw value */
        break;

    case 'i':                           /* Raw minutes */
        *value = (*value - (gint) *value) * 60;
        break;

    case 'e':                           /* Raw seconds */
        adg_dim_style_convert(dim_style, value, 'i');
        *value = (*value - (gint) *value) * 60;
        break;

    case 'D':                           /* Truncated value */
        *value = (gint) *value;
        break;

    case 'd':                           /* Rounded value */
        *value = adg_round(*value, data->decimals);
        break;

    case 'M':                           /* Truncated minutes */
        adg_dim_style_convert(dim_style, value, 'i');
        *value = (gint) *value;
        break;

    case 'm':                           /* Rounded minutes */
        adg_dim_style_convert(dim_style, value, 'i');
        *value = adg_round(*value, data->decimals);
        break;

    case 'S':                           /* Truncated seconds */
        adg_dim_style_convert(dim_style, value, 'e');
        *value = (gint) *value;
        break;

    case 's':                           /* Rounded seconds */
        adg_dim_style_convert(dim_style, value, 'e');
        *value = adg_round(*value, data->decimals);
        break;

    default:
        g_return_val_if_reached(FALSE);
        return FALSE;
    }

    return TRUE;
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgDimStylePrivate *data = ((AdgDimStyle *) style)->data;
    adg_entity_apply_dress(entity, data->color_dress, cr);
}

static AdgMarker *
_adg_marker_new(const AdgMarkerData *marker_data)
{
    if (marker_data->type == 0)
        return NULL;

    return g_object_newv(marker_data->type,
                         marker_data->n_parameters,
                         marker_data->parameters);
}

static void
_adg_set_marker(AdgMarkerData *marker_data, AdgMarker *marker)
{
    g_return_if_fail(marker == NULL || ADG_IS_MARKER(marker));

    /* Free the previous marker data, if any */
    _adg_free_marker(marker_data);

    if (marker) {
        GObject *object;
        GParamSpec **specs;
        GParamSpec *spec;
        GParameter *parameter;
        guint n;

        object = (GObject *) marker;
        specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(marker),
                                               &marker_data->n_parameters);

        marker_data->type = G_TYPE_FROM_INSTANCE(marker);
        marker_data->parameters = g_new0(GParameter, marker_data->n_parameters);

        for (n = 0; n < marker_data->n_parameters; ++n) {
            spec = specs[n];
            parameter = &marker_data->parameters[n];

            /* Using intern strings because GParameter:name is const.
             * GObject properties are internally managed using non-static
             * GQuark, so g_intern_string() is the way to go */
            parameter->name = g_intern_string(spec->name);

            g_value_init(&parameter->value, spec->value_type);
            g_object_get_property(object, spec->name, &parameter->value);
        }

        g_free(specs);
    }
}

static void
_adg_free_marker(AdgMarkerData *marker_data)
{
    guint n;

    for (n = 0; n < marker_data->n_parameters; ++n)
        g_value_unset(&marker_data->parameters[n].value);

    marker_data->type = 0;
    marker_data->n_parameters = 0;
    marker_data->parameters = NULL;
}
