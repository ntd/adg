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
 * SECTION:adg-dress
 * @Section_Id:AdgDress
 * @title: AdgDress
 * @short_description: The ADG way to associate styles to entities
 *
 * The dress is a virtualization of an #AdgStyle instance. #AdgEntity
 * objects do not directly refer to #AdgStyle but use #AdgDress values
 * instead. This allows some advanced operations, such as overriding
 * a dress only in a specific entity branch of the hierarchy or
 * customize multiple entities at once.
 **/

/**
 * AdgDress:
 *
 * An index representing a virtual #AdgStyle.
 **/

/**
 * ADG_TYPE_DRESS:
 *
 * The type used to express a dress index. It is defined only for GObject
 * internal and should not be used directly (at least, as far as I know).
 **/

/**
 * ADG_VALUE_HOLDS_DRESS:
 * @value: a #GValue
 *
 * Checks whether a #GValue is actually holding an #AdgDress value or not.
 *
 * Returns: %TRUE is @value is holding an #AdgDress, %FALSE otherwise
 **/


#include "adg-dress.h"
#include "adg-dress-private.h"
#include "adg-color-style.h"
#include "adg-line-style.h"
#include "adg-font-style.h"
#include "adg-dim-style.h"
#include "adg-arrow.h"


static AdgDress quark_to_dress          (GQuark                  quark);
static void     dress_to_string         (const GValue           *src,
                                         GValue                 *dst);
static void     string_to_dress         (const GValue           *src,
                                         GValue                 *dst);
static void     param_class_init        (GParamSpecClass        *klass);
static gboolean value_validate          (GParamSpec             *spec,
                                         GValue                 *value);

static guint    array_append            (AdgDressPrivate        *data);
static AdgDressPrivate *
                array_lookup            (guint                  n);
static guint    array_len               (void);


GType
adg_dress_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        const GTypeInfo info = { 0, };

        type = g_type_register_static(G_TYPE_INT, "AdgDress", &info, 0);

        g_value_register_transform_func(type, G_TYPE_STRING, dress_to_string);
        g_value_register_transform_func(G_TYPE_STRING, type, string_to_dress);
    }

    return type;
}

/**
 * adg_dress_new:
 * @name: the dress name
 * @style: the style to associate
 *
 * Creates a new dress. It is a convenient wrapper of adg_dress_new_full()
 * that uses as ancestor the G_TYPE_FROM_INSTANCE() of @style.
 *
 * After a succesfull call, a new reference is added to @style.
 *
 * Returns: the new #AdgDress value or #ADG_DRESS_UNDEFINED on errors
 **/
AdgDress
adg_dress_new(const gchar *name, AdgStyle *style)
{
    return adg_dress_new_full(name, style, G_TYPE_FROM_INSTANCE(style));
}

/**
 * adg_dress_new_full:
 * @name: the dress name
 * @style: the style to associate
 * @ancestor_type: the common ancestor type
 *
 * Creates a new dress, explicitely setting the ancestor type.
 * @ancestor_type must be present in the @style hierarchy: check
 * out the adg_dress_set_style() documentation to know what the
 * ancestor type is used for.
 *
 * If a dress with the same name exists, a warning is raised and
 * #ADG_DRESS_UNDEFINED is returned without further actions.
 *
 * After a succesfull call, a new reference is added to @style.
 *
 * Returns: the new #AdgDress value or #ADG_DRESS_UNDEFINED on errors
 **/
AdgDress
adg_dress_new_full(const gchar *name, AdgStyle *style, GType ancestor_type)
{
    GQuark quark;
    AdgDress dress;
    AdgDressPrivate data;

    g_return_val_if_fail(name != NULL, 0);
    g_return_val_if_fail(style != NULL, 0);
    g_return_val_if_fail(G_TYPE_CHECK_INSTANCE_TYPE(style, ancestor_type), 0);

    quark = g_quark_from_string(name);
    dress = quark_to_dress(quark);

    if (dress > 0) {
        g_warning("%s: `%s' name yet used by the `%d' dress",
                  G_STRLOC, name, dress);
        return ADG_DRESS_UNDEFINED;
    }

    data.quark = quark;
    data.style = style;
    data.ancestor_type = ancestor_type;

    g_object_ref(style);

    return array_append(&data) - 1;
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
 * Returns: %TRUE if the dresses are related, %FALSE otherwise
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
 * @src: the source dress
 *
 * Copies @src in @dress. This operation can be succesful only if
 * @dress is #ADG_DRESS_UNDEFINED or if it contains a dress related
 * to @src, that is adg_dress_are_related() returns %TRUE.
 *
 * Returns: %TRUE on copy done, %FALSE on copy failed or not needed
 **/
gboolean
adg_dress_set(AdgDress *dress, AdgDress src)
{
    if (*dress == src)
        return FALSE;

    if (*dress != ADG_DRESS_UNDEFINED &&
        !adg_dress_are_related(*dress, src)) {
        const gchar *dress_name;
        const gchar *src_name;

        dress_name = adg_dress_name(*dress);
        if (dress_name == NULL)
            dress_name = "UNDEFINED";

        src_name = adg_dress_name(src);
        if (src_name == NULL)
            src_name = "UNDEFINED";

        g_warning("%s: `%d' (%s) and `%d' (%s) dresses are not related",
                  G_STRLOC, *dress, dress_name, src, src_name);

        return FALSE;
    }

    *dress = src;

    return TRUE;
}

/**
 * adg_dress_name:
 * @dress: an #AdgDress
 *
 * Gets the name associated to @dress. No warnings are raised if
 * @dress is not found.
 *
 * Returns: the requested name or %NULL if not found
 **/
const gchar *
adg_dress_name(AdgDress dress)
{
    if (dress <= 0 || dress >= array_len())
        return NULL;

    return g_quark_to_string(array_lookup(dress)->quark);
}

/**
 * adg_dress_from_name:
 * @name: the name of a dress
 *
 * Gets the dress bound to a @name string. No warnings are raised
 * if the dress is not found.
 *
 * Returns: the #AdgDress code or #ADG_DRESS_UNDEFINED if not found
 **/
AdgDress
adg_dress_from_name(const gchar *name)
{
    return quark_to_dress(g_quark_try_string(name));
}

/**
 * adg_dress_get_ancestor_type:
 * @dress: an #AdgDress
 *
 * Gets the base type that should be present in every #AdgStyle
 * acceptable by @dress.
 *
 * Returns: the ancestor type
 **/
GType
adg_dress_get_ancestor_type(AdgDress dress)
{
    AdgDressPrivate *data;

    if (dress <= 0 || dress >= array_len())
        return 0;

    data = array_lookup(dress);

    return data->ancestor_type;
}

/**
 * adg_dress_get_style:
 * @dress: an #AdgDress
 *
 * Gets the style associated to @dress. No warnings are raised
 * if the dress is not found.
 *
 * Returns: the requested #AdgStyle derived instance or %NULL if not found
 **/
AdgStyle *
adg_dress_get_style(AdgDress dress)
{
    AdgDressPrivate *data;

    if (dress <= 0 || dress >= array_len())
        return NULL;

    data = array_lookup(dress);

    return data->style;
}

/**
 * adg_dress_set_style:
 * @dress: an #AdgDress
 * @style: the new style
 *
 * Associates a new @style to @dress. If the dress does not exist
 * (it was not previously created by adg_dress_new()), a warning
 * message is raised and the function fails.
 *
 * @style is checked for compatibily with @dress. Any dress holds
 * an ancestor type: if this type is not found in the @style
 * hierarchy, a warning message is raised the function fails.
 *
 * After a succesfull call, the reference to the old style (if any)
 * is dropped while a new reference to @style is added.
 **/
void
adg_dress_set_style(AdgDress dress, AdgStyle *style)
{
    AdgDressPrivate *data;

    if (dress <= 0 || dress >= array_len()) {
        g_warning("%s: `%d' dress undefined", G_STRLOC, dress);
        return;
    }

    data = array_lookup(dress);

    if (data->style == style)
        return;

    /* Check if the new style is compatible with this dress */
    if (!G_TYPE_CHECK_INSTANCE_TYPE(style, data->ancestor_type)) {
        g_warning("%s: `%s' is not compatible with `%s' for `%s' dress",
                  G_STRLOC, g_type_name(G_TYPE_FROM_INSTANCE(style)),
                  g_type_name(data->ancestor_type), adg_dress_name(dress));
        return;
    }

    if (data->style != NULL)
        g_object_unref(data->style);

    data->style = style;

    if (data->style != NULL)
        g_object_ref(data->style);
}

/**
 * adg_dress_accept_style:
 * @dress: an #AdgDress
 * @style: the #AdgStyle to check
 *
 * Checks whether @style could be associated to @dress using
 * adg_dress_set_style().
 *
 * Returns: %TRUE if @dress can accept @style, %FALSE otherwise
 **/
gboolean
adg_dress_accept_style(AdgDress dress, AdgStyle *style)
{
    GType ancestor_type = adg_dress_get_ancestor_type(dress);

    g_return_val_if_fail(ancestor_type > 0, FALSE);
    g_return_val_if_fail(ADG_IS_STYLE(style), FALSE);

    return G_TYPE_CHECK_INSTANCE_TYPE(style, ancestor_type);
}


static AdgDress
quark_to_dress(GQuark quark)
{
    AdgDress dress;
    AdgDressPrivate *data;

    for (dress = 0; dress < array_len(); ++dress) {
        data = array_lookup(dress);

        if (data->quark == quark)
            return dress;
    }

    return ADG_DRESS_UNDEFINED;
}

static void
dress_to_string(const GValue *src, GValue *dst)
{
    g_value_set_string(dst, adg_dress_name(g_value_get_int(src)));
}

static void
string_to_dress(const GValue *src, GValue *dst)
{
    g_value_set_int(dst, adg_dress_from_name(g_value_get_string(src)));
}


typedef struct _AdgParamSpecDress AdgParamSpecDress;

struct _AdgParamSpecDress {
    GParamSpecInt parent;
    AdgDress     source_dress;
};


GType
_adg_param_spec_dress_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        const GTypeInfo info = {
            sizeof(GParamSpecClass),
            NULL,
            NULL,
            (GClassInitFunc) param_class_init,
            NULL,
            NULL,
            sizeof(AdgParamSpecDress),
            0,
        };

        type = g_type_register_static(G_TYPE_PARAM_INT,
                                      "AdgParamSpecDress", &info, 0);
    }

    return type;
}

static void
param_class_init(GParamSpecClass *klass)
{
    klass->value_type = ADG_TYPE_DRESS;
    klass->value_validate = value_validate;
}

static gboolean
value_validate(GParamSpec *spec, GValue *value)
{
    AdgParamSpecDress *fspec;
    AdgDress dress, new_dress;

    fspec = (AdgParamSpecDress *) spec;
    dress = value->data[0].v_int;
    new_dress = ADG_DRESS_UNDEFINED;

    adg_dress_set(&new_dress, dress);
    value->data[0].v_int = new_dress;

    return dress != new_dress;
}


/**
 * adg_param_spec_dress:
 * @name: canonical name
 * @nick: nickname of the param
 * @blurb: brief desciption
 * @dress: the #AdgDress dress
 * @flags: a combination of #GParamFlags
 *
 * Creates a param spec to hold a dress value.
 *
 * Returns: the newly allocated #GParamSpec
 **/
GParamSpec *
adg_param_spec_dress(const gchar *name, const gchar *nick, const gchar *blurb,
                     AdgDress dress, GParamFlags flags)
{
    AdgParamSpecDress *fspec;

    if (dress <= 0 || dress >= array_len()) {
        g_warning("%s: `%d' dress undefined", G_STRLOC, dress);
        return NULL;
    }

    fspec = g_param_spec_internal(ADG_TYPE_PARAM_SPEC_DRESS,
                                  name, nick, blurb, flags);
    fspec->source_dress = dress;

    return (GParamSpec *) fspec;
}


/**
 * ADG_DRESS_UNDEFINED:
 *
 * A value reperesenting an undefined #AdgDress.
 **/

/**
 * ADG_DRESS_COLOR_REGULAR:
 *
 * The default builtin #AdgDress color. This dress will be resolved
 * to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color_regular(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_COLOR_STYLE, NULL);

        dress = adg_dress_new("color-regular", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_DIMENSION:
 *
 * The builtin #AdgDress color used by dimensioning items. This dress
 * will be resolved to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_COLOR_STYLE,
                                       "red", 0.75, NULL);

        dress = adg_dress_new("color-dimension", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_REGULAR:
 *
 * The default builtin #AdgDress line. This dress will be resolved
 * to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_regular(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_LINE_STYLE,
                                       "width", 2., NULL);

        dress = adg_dress_new("line-regular", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_DIMENSION:
 *
 * The builtin #AdgDress line type used by base and extension lines in
 * dimensioning. This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_LINE_STYLE,
                                       "antialias", CAIRO_ANTIALIAS_NONE,
                                       "width", 1., NULL);

        dress = adg_dress_new("line-dimension", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT_REGULAR:
 *
 * The default builtin #AdgDress font. This dress will be resolved
 * to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text_regular(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_FONT_STYLE,
                                       "family", "Serif",
                                       "size", 14., NULL);

        dress = adg_dress_new("text-regular", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT_VALUE:
 *
 * The builtin #AdgDress font used to render the nominal value of a
 * dimension. This dress will be resolved to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text_value(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_FONT_STYLE,
                                       "family", "Sans",
                                       "weight", CAIRO_FONT_WEIGHT_BOLD,
                                       "size", 12., NULL);

        dress = adg_dress_new("text-value", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT_LIMIT:
 *
 * The builtin #AdgDress font used to render the limits of either
 * the min and max values of a dimension. This dress will be
 * resolved to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text_limit(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *style = g_object_new(ADG_TYPE_FONT_STYLE,
                                       "family", "Sans",
                                       "size", 8., NULL);

        dress = adg_dress_new("text-limit", style);
        g_object_unref(style);
    }

    return dress;
}

/**
 * ADG_DRESS_DIMENSION_REGULAR:
 *
 * The default builtin #AdgDress dimensioning style. This dress
 * will be resolved to an #AdgDimStyle instance.
 **/
AdgDress
_adg_dress_dimension_regular(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgMarker *arrow = g_object_new(ADG_TYPE_ARROW, NULL);
        AdgStyle *style = g_object_new(ADG_TYPE_DIM_STYLE, NULL);

        adg_dim_style_use_marker1((AdgDimStyle *) style, arrow);
        adg_marker_set_pos(arrow, 1);
        adg_dim_style_use_marker2((AdgDimStyle *) style, arrow);

        dress = adg_dress_new("dimension-regular", style);
        g_object_unref(style);
        g_object_unref(arrow);
    }

    return dress;
}


static GArray * array_singleton         (void) G_GNUC_CONST;

static GArray *
array_singleton(void)
{
    static GArray *array = NULL;

    if (array == NULL) {
        const AdgDressPrivate data = { 0, };

        array = g_array_new(FALSE, FALSE, sizeof(AdgDressPrivate));

        /* Reserve the first item for the undefined dress */
        g_array_append_val(array, data);
    }

    return array;
}

static guint
array_append(AdgDressPrivate *data)
{
    return g_array_append_val(array_singleton(), *data)->len;
}

static AdgDressPrivate *
array_lookup(guint n)
{
    return &g_array_index(array_singleton(), AdgDressPrivate, n);
}

static guint
array_len(void)
{
    return array_singleton()->len;
}
