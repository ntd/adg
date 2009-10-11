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
#include "adg-dress-builtins.h"


static AdgDress         quark_to_dress          (GQuark          quark);
static void             dress_to_string         (const GValue   *src,
                                                 GValue         *dst);
static void             string_to_dress         (const GValue   *src,
                                                 GValue         *dst);
static void             param_class_init        (GParamSpecClass*klass);
static gboolean         value_validate          (GParamSpec     *spec,
                                                 GValue         *value);

static guint            array_append            (AdgDressPrivate*data);
static AdgDressPrivate *array_lookup            (guint           n);
static guint            array_len               (void);


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
 * @fallback: the fallback style
 *
 * Creates a new dress. It is a convenient wrapper of adg_dress_new_full()
 * that uses as ancestor the G_TYPE_FROM_INSTANCE() of @fallback.
 *
 * After a succesfull call, a new reference is added to @fallback.
 *
 * Returns: the new #AdgDress value or #ADG_DRESS_UNDEFINED on errors
 **/
AdgDress
adg_dress_new(const gchar *name, AdgStyle *fallback)
{
    g_return_val_if_fail(ADG_IS_STYLE(fallback), ADG_DRESS_UNDEFINED);

    return adg_dress_new_full(name, fallback, G_TYPE_FROM_INSTANCE(fallback));
}

/**
 * adg_dress_new_full:
 * @name: the dress name
 * @fallback: the fallback style
 * @ancestor_type: the common ancestor type
 *
 * Creates a new dress, explicitely setting the ancestor type.
 * If @fallback is not %NULL, @ancestor_type must be present in
 * its hierarchy: check out the adg_dress_style_is_compatible()
 * documentation to know what the ancestor type is used for.
 *
 * @fallback can be %NULL, in which case a "transparent" dress
 * is created. This kind of dress does not change the cairo
 * context because there is no style to apply. Any entity could
 * override it to change this behavior though.
 *
 * After a succesfull call, a new reference is added to @fallback
 * if needed.
 *
 * Returns: the new #AdgDress value or #ADG_DRESS_UNDEFINED on errors
 **/
AdgDress
adg_dress_new_full(const gchar *name, AdgStyle *fallback, GType ancestor_type)
{
    GQuark quark;
    AdgDress dress;
    AdgDressPrivate data;

    g_return_val_if_fail(name != NULL, ADG_DRESS_UNDEFINED);
    g_return_val_if_fail(g_type_is_a(ancestor_type, ADG_TYPE_STYLE),
                         ADG_DRESS_UNDEFINED);
    g_return_val_if_fail(fallback == NULL ||
                         G_TYPE_CHECK_INSTANCE_TYPE(fallback, ancestor_type),
                         ADG_DRESS_UNDEFINED);

    quark = g_quark_from_string(name);
    dress = quark_to_dress(quark);

    if (dress > 0) {
        g_warning("%s: `%s' name yet used by the `%d' dress",
                  G_STRLOC, name, dress);
        return ADG_DRESS_UNDEFINED;
    }

    data.quark = quark;
    data.fallback = fallback;
    data.ancestor_type = ancestor_type;

    g_object_ref(fallback);

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
 * adg_dress_get_fallback:
 * @dress: an #AdgDress
 *
 * Gets the fallback style associated to @dress. No warnings
 * are raised if the dress is not found.
 *
 * Returns: the requested #AdgStyle derived instance or %NULL if not set
 **/
AdgStyle *
adg_dress_get_fallback(AdgDress dress)
{
    AdgDressPrivate *data;

    if (dress <= 0 || dress >= array_len())
        return NULL;

    data = array_lookup(dress);

    return data->fallback;
}

/**
 * adg_dress_set_fallback:
 * @dress: an #AdgDress
 * @fallback: the new fallback style
 *
 * Associates a new @fallback style to @dress. If the dress does
 * not exist (it was not previously created by adg_dress_new()),
 * a warning message is raised and the function fails.
 *
 * @fallback is checked for compatibily with @dress. Any dress holds
 * an ancestor type: if this type is not found in the @fallback
 * hierarchy, a warning message is raised and the function fails.
 *
 * After a succesfull call, the reference to the previous fallback
 * (if any) is dropped while a new reference to @fallback is added.
 **/
void
adg_dress_set_fallback(AdgDress dress, AdgStyle *fallback)
{
    AdgDressPrivate *data;

    if (dress <= 0 || dress >= array_len()) {
        g_warning("%s: `%d' dress undefined", G_STRLOC, dress);
        return;
    }

    data = array_lookup(dress);

    if (data->fallback == fallback)
        return;

    /* Check if the new fallback style is compatible with this dress */
    if (fallback != NULL && !adg_dress_style_is_compatible(dress, fallback)) {
        g_warning("%s: `%s' is not compatible with `%s' for `%s' dress",
                  G_STRLOC, g_type_name(G_TYPE_FROM_INSTANCE(fallback)),
                  g_type_name(data->ancestor_type), adg_dress_name(dress));
        return;
    }

    if (data->fallback != NULL)
        g_object_unref(data->fallback);

    data->fallback = fallback;

    if (data->fallback != NULL)
        g_object_ref(data->fallback);
}

/**
 * adg_dress_style_is_compatible:
 * @dress: an #AdgDress
 * @style: the #AdgStyle to check
 *
 * Checks whether @style is compatible with @dress, that is if
 * @style has the ancestor style type (as returned by
 * adg_dress_get_ancestor_type()) in its hierarchy.
 *
 * Returns: %TRUE if @dress can accept @style, %FALSE otherwise
 **/
gboolean
adg_dress_style_is_compatible(AdgDress dress, AdgStyle *style)
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
