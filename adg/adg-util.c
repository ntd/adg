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
 * SECTION:adg-util
 * @Section_Id:utilities
 * @title: Utilities
 * @short_description: Assorted macros and functions
 *
 * Collection of macros and functions that do not fit inside any other topic.
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
 * ADG_MESSAGE:
 * @message: the message string
 *
 * A convenient function to output debug information. It is similar
 * to g_message() but prepends @message with some text specifying
 * the position where this function has been called.
 **/


#include "adg-util.h"
#include <string.h>
#include <limits.h>


/**
 * adg_strcmp:
 * @s1: first string to compare
 * @s2: second string to compare
 *
 * A strcmp() function guarded against %NULL values. It works in the
 * same way exept for the following situations:
 *
 * <itemizedlist>
 * <listitem><code>s1 == NULL && s2 == NULL</code>: returns %0;</listitem>
 * <listitem><code>s1 == NULL</code>: returns %INT_MIN;</listitem>
 * <listitem><code>s2 == NULL</code>: returns %INT_MAX.</listitem>
 * </itemizedlist>
 *
 * Returns: %0 if @s1 matches @s2, a value less than %0 if @s1 is less
 *          than @s2 or greather than %0 if @s1 is greather than @s2
 **/
gint
adg_strcmp(const gchar *s1, const gchar *s2)
{
    /* This will also catch the NULL == NULL case */
    if (s1 == s2)
        return 0;

    if (s1 == NULL)
        return INT_MIN;

    if (s2 == NULL)
        return INT_MAX;

    return strcmp(s1, s2);
}
