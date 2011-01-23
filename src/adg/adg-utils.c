/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 **/

/**
 * ADG_FORWARD_DECL:
 * @id: The name of a struct
 *
 * Forward declaration of struct @id. It is equivalent to a typical
 * struct forward declaration, for example:
 *
 * |[
 * ADG_FORWARD_DECL(test)
 * ]|
 *
 * will expand to:
 *
 * |[
 * typedef struct _test test
 * ]|
 *
 * This macro is needed to fake <command>gtk-doc</command>, because
 * up to now (v.1.12) it generates two conflicting links when using
 * forward declarations: the first in the source with the declaration
 * and the second where the type is defined. Using ADG_FORWARD_DECL()
 * instead of the usual typedef avoids the parsing of the declaration
 * in the first file (<command>gtk-doc</command> is not able to do C
 * preprocessing).
 *
 * The same principle can be applied in the definition file. Following
 * the previous example, you can use something like this where struct
 * _type is defined:
 *
 * |[
 * #if 0
 * // This is declared in another file
 * typedef struct _type type;
 * #endif
 * struct _type {
 * ...
 * };
 * ]|
 **/

/**
 * ADG_DIR_RIGHT:
 *
 * Symbolic constant for the right direction (in radians).
 **/

/**
 * ADG_DIR_DOWN:
 *
 * Symbolic constant for the down direction (in radians).
 **/

/**
 * ADG_DIR_LEFT:
 *
 * Symbolic constant for the left direction (in radians).
 **/

/**
 * ADG_DIR_UP:
 *
 * Symbolic constant for the up direction (in radians).
 **/

/**
 * ADG_UTF8_DIAMETER:
 *
 * String constant that embeds a UTF-8 encoded diameter (U+2300).
 * It can be used to prefix diameter quotes, such as:
 *
 * |[
 * adg_dim_set_value(dim, ADG_UTF8_DIAMETER "<>");
 * ]|
 **/

/**
 * ADG_UTF8_DEGREE:
 *
 * String constant that embeds a UTF-8 encoded degree symbol (U+00B0).
 * It is used to suffix by the default implementation of #AdgADim to
 * suffix the set value, but can be also used manually:
 *
 * |[
 * adg_dim_set_value(dim, "<>" ADG_UTF8_DEGREE);
 * ]|
 **/


#include "adg-internal.h"
#include "adg-utils.h"
#include <string.h>
#include <limits.h>


#if GLIB_CHECK_VERSION(2, 16, 0)
#else
/**
 * g_strcmp0:
 * @str1: a C string or %NULL
 * @str2: another C string or %NULL
 *
 * Compares @str1 and @str2 like strcmp(). Handles %NULL
 * gracefully by sorting it before non-%NULL strings.
 * This is a backward compatibility fallback for GLib
 * prior to 2.16.0
 *
 * Returns: -1, 0 or 1, if @str1 is <, == or > than @str2.
 */
int
g_strcmp0(const char *str1, const char *str2)
{
    if (!str1)
        return -(str1 != str2);

    if (!str2)
        return str1 != str2;

    return strcmp(str1, str2);
}
#endif

/**
 * adg_is_string_empty:
 * @str: the subject string
 *
 * Checks if @str is an empty string, that is if is %NULL or if
 * its first character is %'\0'.
 *
 * Returns: %TRUE if @str is an empty string, %FALSE otherwise
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
 * Returns: %TRUE if @value is a valid @enum_type, %FALSE otherwise
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
 * Checks if @value is a valid #gboolean value, that is if it is %TRUE
 * or %FALSE. No other values are accepted.
 *
 * Returns: %TRUE if @value is a valid #gboolean, %FALSE otherwise
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
 * %NULL, in which case an empty string ("") will be implied.
 *
 * Returns: a newly allocated string to be freed with g_free() or
 *          %NULL on errors
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
 * @domain: the translation domain to use, or %NULL to use
 *          the domain set with textdomain()
 * @msgid:  message to translate
 *
 * A variant of dgettext() (or of g_dgettext(), if available) that
 * initialize the ADG localization infrastructure.
 *
 * Returns: The translated string
 **/
G_CONST_RETURN gchar *
_adg_dgettext(const gchar *domain, const gchar *msgid)
{
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(!initialized)) {
        bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
        bindtextdomain(GETTEXT_PACKAGE "-properties", LOCALEDIR);
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
 * @domain:      the translation domain to use, or %NULL to use
 *               the domain set with textdomain()
 * @msgctxtid:   a combined message context and message id, separated
 *               by a \004 character
 * @msgidoffset: the offset of the message id in @msgctxid
 *
 * This function is basically a duplicate of g_dpgettext() but using
 * _adg_dgettext() internally instead of g_dgettext().
 *
 * Returns: The translated string
 **/
G_CONST_RETURN gchar *
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
