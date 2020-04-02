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
 * SECTION:adg-utils
 * @Section_Id:utilities
 * @title: Utilities
 * @short_description: Assorted macros and functions
 *
 * Collection of macros and functions that do not fit inside any other topic.
 *
 * Since: 1.0
 **/

/**
 * ADG_DIR_RIGHT:
 *
 * Symbolic constant for the right direction (in radians).
 *
 * Since: 1.0
 **/

/**
 * ADG_DIR_DOWN:
 *
 * Symbolic constant for the down direction (in radians).
 *
 * Since: 1.0
 **/

/**
 * ADG_DIR_LEFT:
 *
 * Symbolic constant for the left direction (in radians).
 *
 * Since: 1.0
 **/

/**
 * ADG_DIR_UP:
 *
 * Symbolic constant for the up direction (in radians).
 *
 * Since: 1.0
 **/

/**
 * ADG_UTF8_DIAMETER:
 *
 * String constant that embeds a UTF-8 encoded diameter (U+2300).
 * It can be used to prefix diameter quotes, such as:
 *
 * <informalexample><programlisting language="C">
 * adg_dim_set_value(dim, ADG_UTF8_DIAMETER "<>");
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/

/**
 * ADG_UTF8_DEGREE:
 *
 * String constant that embeds a UTF-8 encoded degree symbol (U+00B0).
 * It is used to suffix by the default implementation of #AdgADim to
 * suffix the set value, but can be also used manually:
 *
 * <informalexample><programlisting language="C">
 * adg_dim_set_value(dim, "<>" ADG_UTF8_DEGREE);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <string.h>
#include <limits.h>
#include <math.h>


#if GLIB_CHECK_VERSION(2, 54, 0)
#else

/**
 * g_object_new_with_properties: (skip)
 * @object_type: the object type to instantiate
 * @n_properties: the number of properties
 * @names: (array length=n_properties): the names of each property to be set
 * @values: (array length=n_properties): the values of each property to be set
 *
 * Creates a new instance of a #GObject subtype and sets its properties using
 * the provided arrays. Both arrays must have exactly @n_properties elements,
 * and the names and values correspond by index.
 *
 * Construction parameters (see %G_PARAM_CONSTRUCT, %G_PARAM_CONSTRUCT_ONLY)
 * which are not explicitly specified are set to their default values.
 *
 * Returns: (type GObject.Object) (transfer full): a new instance of
 * @object_type
 *
 * Since: 1.0
 */
GObject *
g_object_new_with_properties(GType object_type, guint n_properties,
                             const char *names[], const GValue values[])
{
    GParameter *params = g_newa(GParameter, n_properties);
    guint n;

    for (n = 0; n < n_properties; ++n) {
        params[n].name = names[n];
        memcpy(&params[n].value,  &values[n], sizeof(GValue));
    }

    return g_object_newv(object_type, n_properties, params);
}

#endif

/**
 * adg_is_string_empty:
 * @str: the subject string
 *
 * Checks if @str is an empty string, that is if is
 * <constant>NULL</constant> or if its first character
 * is <constant>'\0'</constant>.
 *
 * Returns: <constant>TRUE</constant> if @str is an empty string, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_is_string_empty(const gchar *str)
{
    return str == NULL || str[0] == '\0';
}

/**
 * adg_is_enum_value:
 * @value: the enum value to check
 * @enum_type: a #GEnum based type
 *
 * Checks if @value is a valid @enum_type value.
 *
 * Returns: <constant>TRUE</constant> if @value is a valid @enum_type, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_is_enum_value(int value, GType enum_type)
{
    GEnumClass *enum_class;
    gboolean found;

    enum_class = g_type_class_ref(enum_type);
    g_return_val_if_fail(enum_class != NULL, FALSE);

    found = FALSE;

    if (value >= enum_class->minimum && value <= enum_class->maximum) {
        GEnumValue *enum_value;
        guint n;

        for (n = 0; !found && n < enum_class->n_values; ++n) {
            enum_value = enum_class->values + n;
            found = value == enum_value->value;
        }
    }

    g_type_class_unref(enum_class);

    return found;
}

/**
 * adg_is_boolean_value:
 * @value: the gboolean value to check
 *
 * Checks if @value is a valid #gboolean value, that is if it is
 * <constant>TRUE</constant> or <constant>FALSE</constant>.
 * No other values are accepted.
 *
 * Returns: <constant>TRUE</constant> if @value is a valid #gboolean, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_is_boolean_value(gboolean value)
{
    return value == TRUE || value == FALSE;
}

/**
 * adg_string_replace:
 * @str: the original string
 * @from: the substring to replace
 * @to: the replacement string
 *
 * Replaces @from with @to inside @str and returns the result as a
 * newly allocated string.
 *
 * @str and @from must be non-null valid C strings while @to can be
 * <constant>NULL</constant>, in which case an empty string
 * (<constant>""</constant>) will be implied.
 *
 * Returns: a newly allocated string to be freed with g_free() or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
gchar *
adg_string_replace(const gchar *str, const gchar *from, const gchar *to)
{
    gchar *result;
    int from_len;
    gchar *ptr, *old_result;

    g_return_val_if_fail(str != NULL, NULL);
    g_return_val_if_fail(from != NULL, NULL);

    from_len = strlen(from);

    g_return_val_if_fail(from_len > 0, NULL);

    if (to == NULL)
        to = "";

    result = g_strdup(str);

    while ((ptr = strstr(result, from)) != NULL) {
        *ptr = '\0';
        old_result = result;
        result = g_strconcat(old_result, to, ptr + from_len, NULL);
        g_free(old_result);
    }

    return result;
}

/**
 * _adg_dgettext:
 * @domain: the translation domain to use, or
 *          <constant>NULL</constant> to use the domain set
 *          with <function>textdomain</function>
 * @msgid:  message to translate
 *
 * A variant of dgettext() (or of g_dgettext(), if available) that
 * initialize the ADG localization infrastructure.
 *
 * Returns: The translated string
 *
 * Since: 1.0
 **/
const gchar *
_adg_dgettext(const gchar *domain, const gchar *msgid)
{
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(!initialized)) {
#ifdef G_OS_UNIX
        bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
#else
        /* On windows, LOCALEDIR is relative to the installation path */
        gchar *path = g_build_filename(g_win32_get_package_installation_directory_of_module(NULL),
                                       LOCALEDIR, NULL);
        bindtextdomain(GETTEXT_PACKAGE, path);
        g_free(path);
#endif
        bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
        initialized = TRUE;
    }

    return g_dgettext(domain, msgid);
}

/**
 * _adg_dpgettext:
 * @domain:      the translation domain to use, or
 *               <constant>NULL</constant> to use the domain set with
 *               <function>textdomain</function>
 * @msgctxtid:   a combined message context and message id, separated
 *               by a \004 character
 * @msgidoffset: the offset of the message id in @msgctxid
 *
 * This function is basically a duplicate of g_dpgettext() but using
 * _adg_dgettext() internally instead of g_dgettext().
 *
 * Returns: The translated string
 *
 * Since: 1.0
 **/
const gchar *
_adg_dpgettext(const gchar *domain, const gchar *msgctxtid, gsize msgidoffset)
{
    const gchar *translation;
    gchar *sep;

    translation = _adg_dgettext(domain, msgctxtid);

    if (translation == msgctxtid) {
        if (msgidoffset > 0)
            return msgctxtid + msgidoffset;

        sep = strchr(msgctxtid, '|');

        if (sep) {
            /* try with '\004' instead of '|', in case
             * xgettext -kQ_:1g was used
             */
            gchar *tmp = g_alloca(strlen(msgctxtid) + 1);
            strcpy(tmp, msgctxtid);
            tmp[sep - msgctxtid] = '\004';

            translation = _adg_dgettext(domain, tmp);

            if (translation == tmp)
                return sep + 1;
        }
    }

    return translation;
}

/**
 * adg_find_file:
 * @file: the file to search
 * @...:  a NULL terminated list of paths where to look for
 *        file existence.
 *
 * Searches @file in the provided paths and returns the full
 * path to the first existing match. The check is performed
 * using g_file_test() with the G_FILE_TEST_EXISTS test.
 *
 * The result should be freed with g_free() when no longer needed.
 *
 * Returns: a newly allocated string containing the path or <constant>NULL</constant> if not found or on errors.
 *
 * Since: 1.0
 **/
gchar *
adg_find_file(const gchar *file, ...)
{
    va_list var_args;
    gchar *path;
    const gchar *base;

    g_return_val_if_fail(file != NULL, NULL);

    va_start(var_args, file);

    while ((base = va_arg(var_args, const gchar *)) != NULL) {
        path = g_build_filename(base, file, NULL);
        if (g_file_test(path, G_FILE_TEST_EXISTS))
            return path;
        g_free(path);
    }

    return NULL;
}

/**
 * adg_scale_factor:
 * @scale: a string identifying the scale
 *
 * Converts a scale in the form x:y (where x and y are respectively
 * two positive integers representing the numerator and denominator
 * of a fraction) into its approximate double representation. Any
 * garbage following x or y will be silently ignored, meaning that
 * x+garbage:y+garbage is equivalent to x:y. Furthermore, the postfix
 * :y can be omitted, in which case (double) x will be returned.
 *
 * x and y are converted by using atof(), so refer to your C library
 * documentation for details on the algorithm used.
 *
 * Returns: the (possibly approximated) double conversion of @scale or 0 on errors.
 *
 * Since: 1.0
 **/
gdouble
adg_scale_factor(const gchar *scale)
{
    gdouble numerator, denominator;
    const gchar *ptr;
    gchar *orig;

    g_return_val_if_fail(scale != NULL, 0);

    orig = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");

    numerator = atof(scale);

    ptr = strchr(scale, ':');
    denominator = ptr == NULL ? 1 : atof(ptr + 1);

    setlocale(LC_NUMERIC, orig);

    if (denominator == 0)
        return 0;

    return numerator / denominator;
}

/**
 * adg_type_from_filename:
 * @file: the full path to the file
 *
 * Gets the surface type from @file. The algorithm simply looks to the
 * file name extension and tries to guess the correct surface type. If the
 * guess fails, e.g. the extension does not exist or it is not usual, the
 * function returns <constant>CAIRO_SURFACE_TYPE_XLIB</constant>. This is
 * the value conventionally used to signal unrecognized file names.
 *
 * Returns: (type gint): the surface type of @file
 *          or <constant>CAIRO_SURFACE_TYPE_XLIB</constant>.
 *
 * Since: 1.0
 **/
cairo_surface_type_t
adg_type_from_filename(const gchar *file)
{
    const gchar *p_suffix;
    gchar *suffix;
    cairo_surface_type_t type;

    g_return_val_if_fail(file != NULL, CAIRO_SURFACE_TYPE_XLIB);

    p_suffix = strrchr(file, '.');
    if (p_suffix == NULL)
        return CAIRO_SURFACE_TYPE_XLIB;

    /* Put in suffix the lowercase extension without the leading dot */
    suffix = g_ascii_strdown(p_suffix + 1, -1);

    if (strcmp(suffix, "png") == 0) {
        type = CAIRO_SURFACE_TYPE_IMAGE;
    } else if (strcmp(suffix, "svg") == 0) {
        type = CAIRO_SURFACE_TYPE_SVG;
    } else if (strcmp(suffix, "pdf") == 0) {
        type = CAIRO_SURFACE_TYPE_PDF;
    } else if (strcmp(suffix, "ps") == 0) {
        type = CAIRO_SURFACE_TYPE_PS;
    } else {
        type = CAIRO_SURFACE_TYPE_XLIB;
    }

    g_free(suffix);
    return type;
}

/**
 * adg_object_clone:
 * @src: (transfer none): the source #GObject to clone
 *
 * A helper method that clones a generic #GObject instance. The implementation
 * leverages the g_object_get_property() method on @src to get all the
 * properties and uses g_object_newv() to create the destination clone.
 *
 * The code is not as sophisticated as one might expect, so apart from what
 * described there is no other magic involved. It is internally used by ADG to
 * clone #AdgStyle instances in adg_style_clone().
 *
 * Returns: (transfer full): the clone of @src.
 *
 * Since: 1.0
 **/
GObject *
adg_object_clone(GObject *src)
{
    GObject     *dst;
    GParamSpec **specs;
    const char **names;
    GValue      *values;
    const gchar *name;
    GValue      *value;
    guint        n, n_specs, n_properties;

    g_return_val_if_fail(G_IS_OBJECT(src), NULL);
    specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(src), &n_specs);
    names = g_new0(const char *, n_specs);
    values = g_new0(GValue, n_specs);
    n_properties = 0;

    for (n = 0; n < n_specs; ++n) {
        if ((specs[n]->flags & G_PARAM_READWRITE) == G_PARAM_READWRITE) {
            name = g_intern_string(specs[n]->name);
            names[n_properties] = name;
            value = &values[n_properties];
            g_value_init(value, specs[n]->value_type);
            g_object_get_property(src, name, value);
            ++ n_properties;
        }
    }

    dst = g_object_new_with_properties(G_TYPE_FROM_INSTANCE(src),
                                       n_properties, names, values);
    g_free(specs);
    g_free(names);
    g_free(values);

    return dst;
}

/**
 * adg_nop:
 *
 * A function that does nothing. It can be used as
 * <constant>/dev/null</constant> when callback are required, e.g. with
 * g_log_set_default_handler().
 *
 * Since: 1.0
 **/
void
adg_nop(void)
{
}

/**
 * adg_round:
 * @value: the value to round
 * @decimals: the number of significant decimals to consider
 *
 * Rounds the @value floating number to a specific number of digits. Be aware
 * a binary floating point is unable to represent all decimal numbers, i.e.
 * (WARNING: pure theoretical example ahead) rounding 3.3333 to the second
 * decimal can return in 3.32999999.
 *
 * Returns: (type gdouble): the rounded number.
 *
 * Since: 1.0
 **/
gdouble
adg_round(gdouble value, gint decimals)
{
    return decimals > 0 ? adg_round(value*10, decimals-1) / 10 : round(value);
}

/**
 * adg_unescaped_strchr:
 * @string: a string
 * @ch: a character to find
 *
 * Similar to the standard strchr(), this function returns a pointer to the to
 * the matched character that *is not* preceded by a backslash.
 *
 * Returns: (transfer none): a pointer to the matched @ch inside @string or
 *          %NULL if not found.
 *
 * Since: 1.0
 **/
const gchar *
adg_unescaped_strchr(const gchar *string, gint ch)
{
    const gchar *ptr;

    g_return_val_if_fail(string != NULL, NULL);

    for (ptr = string; (ptr = strchr(ptr, ch)) != NULL; ++ ptr) {
        if (ptr == string || *(ptr-1) != '\\') {
            break;
        }
    }

    return ptr;
}
