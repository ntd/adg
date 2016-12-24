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


#if GLIB_CHECK_VERSION(2, 16, 0)
#else
/**
 * g_strcmp0:
 * @s1: a C string or <constant>NULL</constant>
 * @s2: another C string or <constant>NULL</constant>
 *
 * Compares @s1 and @s2 like strcmp(). Handles
 * <constant>NULL</constant> gracefully by sorting it
 * before non-<constant>NULL</constant> strings.
 * Comparing two <constant>NULL</constant> pointers
 * returns 0.
 *
 * This is a backward compatibility fallback for GLib
 * prior to 2.16.0.
 *
 * Returns: an integer less than, equal to, or greater than zero, if @s1 is less than, equal to or greater than @s2.
 *
 * Since: 1.0
 */
int
g_strcmp0(const char *s1, const char *s2)
{
    if (!s1)
        return -(s1 != s2);

    if (!s2)
        return s1 != s2;

    return strcmp(s1, s2);
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

#if GLIB_CHECK_VERSION(2, 18, 0)
    return g_dgettext(domain, msgid);
#else
    return dgettext(domain, msgid);
#endif
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
 * x and y are converted by using atoi(), so refer to your C library
 * documentation for details on the algorithm used.
 *
 * Returns: the (possibly approximated) double conversion of @scale or 0 on errors.
 *
 * Since: 1.0
 **/
gdouble
adg_scale_factor(const gchar *scale)
{
    gint numerator, denominator;
    const gchar *ptr;

    g_return_val_if_fail(scale != NULL, 0);

    numerator = atoi(scale);
    if (numerator <= 0)
        return 0;

    ptr = strchr(scale, ':');
    if (ptr == NULL)
        return numerator;

    denominator = atoi(ptr + 1);
    if (denominator <= 0)
        return 0;

    return (gdouble) numerator / (gdouble) denominator;
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
 * @decimals: the number of significald decimals to consider
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
