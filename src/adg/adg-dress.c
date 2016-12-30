/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-dress
 * @Section_Id:Dresses
 * @title: AdgDress
 * @short_description: The ADG way to associate styles to entities
 *
 * The dress is a virtualization of an #AdgStyle instance. #AdgEntity
 * objects do not directly refer to #AdgStyle but use #AdgDress values
 * instead. This allows some advanced operations, such as overriding
 * a dress only in a specific entity branch of the hierarchy or
 * customize multiple entities at once.
 *
 * Since: 1.0
 **/

/**
 * AdgDress:
 * @ADG_DRESS_UNDEFINED:             undefined dress, used for notifying invalid
 *                                   dresses.
 * @ADG_DRESS_COLOR:                 default built-in color. This is a
 *                                   pass-through dress, that is it does not
 *                                   change the cairo context when it is
 *                                   applied. This dress will be resolved to an
 *                                   #AdgColorStyle instance.
 * @ADG_DRESS_COLOR_BACKGROUND:      default built-in color to be used as the
 *                                   #AdgCanvas background. This dress will be
 *                                   resolved to an #AdgColorStyle instance.
 * @ADG_DRESS_COLOR_STROKE:          default built-in color for #AdgStroke
 *                                   entities. This dress will be resolved to
 *                                   an #AdgColorStyle instance.
 * @ADG_DRESS_COLOR_DIMENSION:       built-in color used by default in
 *                                   #AdgDimStyle. This dress will be resolved
 *                                   to an #AdgColorStyle instance.
 * @ADG_DRESS_COLOR_ANNOTATION:      built-in color used for rendering
 *                                   helper entities such as #AdgToyText,
 *                                   #AdgTable and #AdgTitleBlock. This dress
 *                                   will be resolved to an #AdgColorStyle
 *                                   instance.
 * @ADG_DRESS_COLOR_FILL:            built-in color used by default by
 *                                   #AdgFillStyle based styles. This dress
 *                                   will be resolved to an #AdgColorStyle
 *                                   instance.
 * @ADG_DRESS_COLOR_AXIS:            default built-in color for stroking
 *                                   #ADG_DRESS_LINE_AXIS lines. This dress
 *                                   will be resolved to an #AdgColorStyle
 *                                   instance.
 * @ADG_DRESS_COLOR_HIDDEN:          default built-in color for stroking
 *                                   #ADG_DRESS_LINE_HIDDEN lines. This dress
 *                                   will be resolved to an #AdgColorStyle
 *                                   instance.
 * @ADG_DRESS_LINE:                  default built-in line. This is a
 *                                   pass-through dress, that is it does not
 *                                   change the cairo context when it is
 *                                   applied. This dress will be resolved to
 *                                   an #AdgLineStyle instance.
 * @ADG_DRESS_LINE_STROKE:           built-in line type to be used by default
 *                                   for rendering #AdgStroke entities.
 *                                   This dress will be resolved to an
 *                                   #AdgLineStyle instance.
 * @ADG_DRESS_LINE_DIMENSION:        built-in line type used by default for
 *                                   rendering base and extension lines of
 *                                   dimensions. This dress will be resolved
 *                                   to an #AdgLineStyle instance.
 * @ADG_DRESS_LINE_FILL:             built-in line type used by #AdgFillStyle.
 *                                   This dress will be resolved to an
 *                                   #AdgLineStyle instance.
 * @ADG_DRESS_LINE_GRID:             built-in line type used for rendering
 *                                   the grid of #AdgTable entities, that is
 *                                   the frame of the cells. This dress will
 *                                   be resolved to an #AdgLineStyle instance.
 * @ADG_DRESS_LINE_FRAME:            built-in line type used for rendering the
 *                                   frame of #AdgTable entities, that is the
 *                                   frame around the whole table. This dress
 *                                   will be resolved to an #AdgLineStyle
 *                                   instance.
 * @ADG_DRESS_LINE_AXIS:             built-in line type used for rendering axis
 *                                   and centerlines. This dress will be
 *                                   resolved to an #AdgLineStyle instance.
 * @ADG_DRESS_LINE_HIDDEN:           built-in line type used for rendering
 *                                   hidden lines and edges. This dress will be
 *                                   resolved to an #AdgLineStyle instance.
 * @ADG_DRESS_FONT:                  default built-in font. This dress will be
 *                                   resolved to an #AdgFontStyle instance.
 * @ADG_DRESS_FONT_TEXT:             built-in font used by default for
 *                                   rendering common text such as #AdgToyText
*                                    or the value of #AdgTable entities. This
*                                    dress will be resolved to an #AdgFontStyle
*                                    instance.
 * @ADG_DRESS_FONT_ANNOTATION:       built-in font used for rendering auxiliary
 *                                   text, such as the titles on #AdgTable
 *                                   entities. This dress will be resolved to
 *                                   an #AdgFontStyle instance.
 * @ADG_DRESS_FONT_QUOTE_TEXT:       built-in font used for rendering regular
 *                                   text on dimension entities, such as the
 *                                   nominal value and the notes of a quote.
 *                                   This dress will be resolved to an
 *                                   #AdgFontStyle instance.
 * @ADG_DRESS_FONT_QUOTE_ANNOTATION: built-in font used for rendering auxiliary
 *                                   text on dimension entities, such as the
 *                                   min and max limits of a quote. This dress
 *                                   will be resolved to an #AdgFontStyle
 *                                   instance.
 * @ADG_DRESS_DIMENSION:             default built-in for dimensions. This
 *                                   dress will be resolved to an #AdgDimStyle
 *                                   instance.
 * @ADG_DRESS_DIMENSION_ANGULAR:     default built-in for angular dimensions.
 *                                   This dress will be resolved to an
 *                                   #AdgDimStyle instance.
 * @ADG_DRESS_DIMENSION_RADIUS:      default built-in for radius dimensions.
 *                                   This dress will be resolved to an
 *                                   #AdgDimStyle instance.
 * @ADG_DRESS_FILL:                  default built-in for filling. This is a
 *                                   pass-through dress, that is it does not
 *                                   change the cairo context when it is
 *                                   applied. This dress will be resolved to an
 *                                   #AdgFillStyle derived instance.
 * @ADG_DRESS_FILL_HATCH:            built-in dress used by default by
 *                                   #AdgHatch instances. This dress will be
 *                                   resolved to an #AdgFillStyle derived
 *                                   instance.
 * @ADG_DRESS_TABLE:                 default built-in for tables. This dress
 *                                   will be resolved to an #AdgTableStyle
 *                                   derived instance.
 *
 * An index representing a virtual #AdgStyle. The ADG comes equipped
 * with some built-in dress.
 *
 * Since: 1.0
 **/

/**
 * ADG_TYPE_DRESS:
 *
 * The type used to express a dress index. It is defined only for GObject
 * internal and should not be used directly (at least, as far as I know).
 *
 * Since: 1.0
 **/

/**
 * ADG_VALUE_HOLDS_DRESS:
 * @value: a #GValue
 *
 * Checks whether a #GValue is actually holding an #AdgDress value or not.
 *
 * Returns: <constant>TRUE</constant> is @value is holding an #AdgDress, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-dash.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-marker.h"
#include "adg-arrow.h"
#include "adg-style.h"
#include "adg-color-style.h"
#include "adg-line-style.h"
#include "adg-text-internal.h"
#include "adg-dim-style.h"
#include "adg-fill-style.h"
#include "adg-ruled-fill.h"
#include "adg-table-style.h"

#include "adg-dress.h"
#include "adg-dress-private.h"

#define MM  *2.83464566927


static GArray *         _adg_data_array             (void);
static void             _adg_data_register          (AdgDress    dress,
                                                     AdgStyle   *fallback,
                                                     GType       ancestor_type);
static void             _adg_data_register_builtins (void);
static AdgDressPrivate *_adg_data_lookup            (AdgDress    dress);


/**
 * adg_dress_from_name:
 * @name: the name of a dress
 *
 * Gets the dress bound to a @name string. No warnings are raised
 * if the dress is not found.
 *
 * Returns: the #AdgDress value or #ADG_DRESS_UNDEFINED if not found.
 *
 * Since: 1.0
 **/
AdgDress
adg_dress_from_name(const gchar *name)
{
    GEnumClass *dress_class = g_type_class_ref(ADG_TYPE_DRESS);
    GEnumValue *enum_value = g_enum_get_value_by_name(dress_class, name);
    g_type_class_unref(dress_class);
    return enum_value != NULL ? enum_value->value : ADG_DRESS_UNDEFINED;
}

/**
 * adg_dress_are_related:
 * @dress1: an #AdgDress
 * @dress2: another #AdgDress
 *
 * Checks whether @dress1 and @dress2 are related, that is
 * if they have the same ancestor type as returned by
 * adg_dress_get_ancestor_type().
 *
 * Returns: <constant>TRUE</constant> if the dresses are related, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_dress_are_related(AdgDress dress1, AdgDress dress2)
{
    GType ancestor_type1, ancestor_type2;

    ancestor_type1 = adg_dress_get_ancestor_type(dress1);
    if (ancestor_type1 <= 0)
        return FALSE;

    ancestor_type2 = adg_dress_get_ancestor_type(dress2);
    if (ancestor_type2 <= 0)
        return FALSE;

    return ancestor_type1 == ancestor_type2;
}

/**
 * adg_dress_set:
 * @dress: a pointer to an #AdgDress
 * @src:   the source dress
 *
 * Copies @src in @dress. This operation can be successful only if
 * @dress is #ADG_DRESS_UNDEFINED or if it contains a dress related
 * to @src, i.e. adg_dress_are_related() returns <constant>TRUE</constant>.
 *
 * Returns: <constant>TRUE</constant> on copy done, <constant>FALSE</constant> on copy failed or not needed.
 *
 * Since: 1.0
 **/
gboolean
adg_dress_set(AdgDress *dress, AdgDress src)
{
    if (*dress == src)
        return FALSE;

    if (*dress != ADG_DRESS_UNDEFINED && !adg_dress_are_related(*dress, src))
        return FALSE;

    *dress = src;
    return TRUE;
}

/**
 * adg_dress_get_name:
 * @dress: an #AdgDress
 *
 * Gets the name associated to @dress. No warnings are raised if
 * @dress is not found.
 *
 * Returns: the requested name or <constant>NULL</constant> if not found.
 *
 * Since: 1.0
 **/
const gchar *
adg_dress_get_name(AdgDress dress)
{
    GEnumClass *dress_class = g_type_class_ref(ADG_TYPE_DRESS);
    GEnumValue *enum_value = g_enum_get_value(dress_class, dress);
    g_type_class_unref(dress_class);
    return enum_value != NULL ? enum_value->value_name : NULL;
}

/**
 * adg_dress_get_ancestor_type:
 * @dress: an #AdgDress
 *
 * Gets the base type that should be present in every #AdgStyle
 * acceptable by @dress.  No warnings are raised if @dress
 * is not found.
 *
 * Returns: the ancestor type or 0 on errors.
 *
 * Since: 1.0
 **/
GType
adg_dress_get_ancestor_type(AdgDress dress)
{
    AdgDressPrivate *data = _adg_data_lookup(dress);
    return data != NULL ? data->ancestor_type : 0;
}

/**
 * adg_dress_set_fallback:
 * @dress:                     an #AdgDress
 * @fallback: (transfer full): the new fallback style
 *
 * Associates a new @fallback style to @dress. If the dress is
 * not valid, a warning message is raised and the function fails.
 *
 * @fallback is checked for compatibily with @dress. Any dress holds
 * an ancestor type: if this type is not found in the @fallback
 * hierarchy, a warning message is raised and the function fails.
 *
 * After a succesfull call, the reference to the previous fallback
 * (if any) is dropped while a new reference to @fallback is added.
 *
 * Since: 1.0
 **/
void
adg_dress_set_fallback(AdgDress dress, AdgStyle *fallback)
{
    AdgDressPrivate *data = _adg_data_lookup(dress);

    g_return_if_fail(data != NULL);

    if (data->fallback == fallback)
        return;

    /* Check if the new fallback style is compatible with this dress */
    if (fallback != NULL && !adg_dress_style_is_compatible(dress, fallback)) {
        g_warning(_("%s: the fallback style of '%s' dress (%d) must be a '%s' derived type, but a '%s' has been provided"),
                  G_STRLOC, adg_dress_get_name(dress), dress,
                  g_type_name(data->ancestor_type),
                  g_type_name(G_TYPE_FROM_INSTANCE(fallback)));
        return;
    }

    if (data->fallback != NULL)
        g_object_unref(data->fallback);

    data->fallback = fallback;

    if (data->fallback != NULL)
        g_object_ref(data->fallback);
}

/**
 * adg_dress_get_fallback:
 * @dress: an #AdgDress
 *
 * Gets the fallback style associated to @dress. No warnings
 * are raised if the dress is not found. The returned style
 * is owned by dress and should not be freed or modified.
 *
 * Returns: (transfer none): the requested #AdgStyle derived instance or <constant>NULL</constant> if not set.
 *
 * Since: 1.0
 **/
AdgStyle *
adg_dress_get_fallback(AdgDress dress)
{
    AdgDressPrivate *data = _adg_data_lookup(dress);
    return data != NULL ? data->fallback : NULL;
}

/**
 * adg_dress_style_is_compatible:
 * @dress:                  an #AdgDress
 * @style: (transfer none): the #AdgStyle to check
 *
 * Checks whether @style is compatible with @dress, that is if
 * @style has the ancestor style type (as returned by
 * adg_dress_get_ancestor_type()) in its hierarchy.
 *
 * Returns: <constant>TRUE</constant> if @dress can accept @style, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_dress_style_is_compatible(AdgDress dress, AdgStyle *style)
{
    GType ancestor_type = adg_dress_get_ancestor_type(dress);

    g_return_val_if_fail(ancestor_type > 0, FALSE);
    g_return_val_if_fail(ADG_IS_STYLE(style), FALSE);

    return G_TYPE_CHECK_INSTANCE_TYPE(style, ancestor_type);
}


static GArray *
_adg_data_array(void)
{
    /* The following register keeps track of the metadata bound to every
     * #AdgDress value, such as the fallback style and the ancestor type.
     *
     * The AdgDress value is cohincident with the index of its metadata
     * inside this register, that is if %ADG_DRESS_COLOR_BACKGROUND is 2,
     * array->data[2] will contain its metadata.
     */
    static GArray *array = NULL;

    if (G_UNLIKELY(array == NULL)) {
        array = g_array_new(FALSE, FALSE, sizeof(AdgDressPrivate));
        _adg_data_register_builtins();
    }

    return array;
}

static void
_adg_data_register(AdgDress dress, AdgStyle *fallback, GType ancestor_type)
{
    GArray         *array = _adg_data_array();
    AdgDressPrivate data;

    data.fallback = fallback;
    data.ancestor_type = ancestor_type;

    g_array_insert_vals(array, dress, g_memdup(&data, sizeof(data)), 1);
}

static void
_adg_data_register_builtins(void)
{
    AdgDash *dash;
    AdgMarker *arrow1, *arrow2;

    _adg_data_register(ADG_DRESS_UNDEFINED,
                       NULL,
                       G_TYPE_INVALID);


    /* Predefined colors */

    _adg_data_register(ADG_DRESS_COLOR,
                       NULL,
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_BACKGROUND,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "blue",  1.,
                                    "green", 1.,
                                    "red",   1.,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_STROKE,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_DIMENSION,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "red",   0.,
                                    "green", 0.4,
                                    "blue",  0.6,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_ANNOTATION,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "red",   0.4,
                                    "green", 0.4,
                                    "blue",  0.2,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_FILL,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "red",   0.25,
                                    "green", 0.25,
                                    "blue",  0.25,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_AXIS,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "red",   0.,
                                    "green", 0.75,
                                    "blue",  0.25,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);

    _adg_data_register(ADG_DRESS_COLOR_HIDDEN,
                       g_object_new(ADG_TYPE_COLOR_STYLE,
                                    "red",   0.5,
                                    "green", 0.5,
                                    "blue",  0.5,
                                    NULL),
                       ADG_TYPE_COLOR_STYLE);


    /* Predefined lines */

    _adg_data_register(ADG_DRESS_LINE,
                       NULL,
                       ADG_TYPE_LINE_STYLE);

    _adg_data_register(ADG_DRESS_LINE_STROKE,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_STROKE,
                                    "width",       1.5,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);

    _adg_data_register(ADG_DRESS_LINE_DIMENSION,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "width", 0.5,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);

    _adg_data_register(ADG_DRESS_LINE_FILL,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_FILL,
                                    "width",       0.5,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);

    _adg_data_register(ADG_DRESS_LINE_GRID,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "antialias", CAIRO_ANTIALIAS_NONE,
                                    "width",     1.,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);

    _adg_data_register(ADG_DRESS_LINE_FRAME,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                    "antialias",   CAIRO_ANTIALIAS_NONE,
                                    "width",       2.,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);


    dash = adg_dash_new_with_dashes(4, 2 MM, 2 MM, 10 MM, 2 MM);
    _adg_data_register(ADG_DRESS_LINE_AXIS,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_AXIS,
                                    "width",       0.25 MM,
                                    "dash",        dash,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);
    adg_dash_destroy(dash);

    dash = adg_dash_new_with_dashes(2, 6 MM, 6 MM);
    _adg_data_register(ADG_DRESS_LINE_HIDDEN,
                       g_object_new(ADG_TYPE_LINE_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_HIDDEN,
                                    "width",       0.25 MM,
                                    "dash",        dash,
                                    NULL),
                       ADG_TYPE_LINE_STYLE);
    adg_dash_destroy(dash);


    /* Predefined fonts */

    _adg_data_register(ADG_DRESS_FONT,
                       g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                    "family", "Serif",
                                    "size",   14.,
                                    NULL),
                       ADG_TYPE_BEST_FONT_STYLE);

    _adg_data_register(ADG_DRESS_FONT_TEXT,
                       g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                    "family",      "Sans",
                                    "weight",      CAIRO_FONT_WEIGHT_BOLD,
                                    "size",        12.,
                                    NULL),
                       ADG_TYPE_BEST_FONT_STYLE);

    _adg_data_register(ADG_DRESS_FONT_ANNOTATION,
                       g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                    "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                    "family",      "Sans",
                                    "size",        8.,
                                    NULL),
                       ADG_TYPE_BEST_FONT_STYLE);

    _adg_data_register(ADG_DRESS_FONT_QUOTE_TEXT,
                       g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                    "family", "Sans",
                                    "weight", CAIRO_FONT_WEIGHT_BOLD,
                                    "size",   12.,
                                    NULL),
                       ADG_TYPE_BEST_FONT_STYLE);

    _adg_data_register(ADG_DRESS_FONT_QUOTE_ANNOTATION,
                       g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                    "family",      "Sans",
                                    "size",        8.,
                                    NULL),
                       ADG_TYPE_BEST_FONT_STYLE);


    /* Predefined dimension styles */

    arrow1 = (AdgMarker *) adg_arrow_new();
    arrow2 = (AdgMarker *) adg_arrow_new();
    adg_marker_set_pos(arrow2, 1);
    _adg_data_register(ADG_DRESS_DIMENSION,
                       g_object_new(ADG_TYPE_DIM_STYLE,
                                    "marker1", arrow1,
                                    "marker2", arrow2,
                                    NULL),
                       ADG_TYPE_DIM_STYLE);
    g_object_unref(arrow1);
    g_object_unref(arrow2);


    /* Predefined angular dimension styles */

    arrow1 = (AdgMarker *) adg_arrow_new();
    arrow2 = (AdgMarker *) adg_arrow_new();
    adg_marker_set_pos(arrow2, 1);
    _adg_data_register(ADG_DRESS_DIMENSION_ANGULAR,
                       g_object_new(ADG_TYPE_DIM_STYLE,
                                    "marker1", arrow1,
                                    "marker2", arrow2,
                                    "number-arguments", "d",
                                    "number-format", "%g°",
                                    NULL),
                       ADG_TYPE_DIM_STYLE);
    g_object_unref(arrow1);
    g_object_unref(arrow2);


    /* Predefined radius dimension styles */

    arrow1 = (AdgMarker *) adg_arrow_new();
    arrow2 = (AdgMarker *) adg_arrow_new();
    adg_marker_set_pos(arrow2, 1);
    _adg_data_register(ADG_DRESS_DIMENSION_RADIUS,
                       g_object_new(ADG_TYPE_DIM_STYLE,
                                    "marker1", arrow1,
                                    "marker2", arrow2,
                                    "number-arguments", "d",
                                    "number-format", "R %g",
                                    NULL),
                       ADG_TYPE_DIM_STYLE);
    g_object_unref(arrow1);
    g_object_unref(arrow2);


    /* Predefined fill styles */

    _adg_data_register(ADG_DRESS_FILL,
                       NULL,
                       ADG_TYPE_FILL_STYLE);

    _adg_data_register(ADG_DRESS_FILL_HATCH,
                       g_object_new(ADG_TYPE_RULED_FILL,
                                    "line-dress", ADG_DRESS_LINE_FILL,
                                    NULL),
                       ADG_TYPE_FILL_STYLE);


    /* Predefined table styles */

    _adg_data_register(ADG_DRESS_TABLE,
                       g_object_new(ADG_TYPE_TABLE_STYLE, NULL),
                       ADG_TYPE_TABLE_STYLE);
}

static AdgDressPrivate *
_adg_data_lookup(AdgDress dress)
{
    GArray *array = _adg_data_array();

    if (dress >= array->len)
        return NULL;

    return ((AdgDressPrivate *) array->data) + dress;
}
