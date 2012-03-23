/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-pair
 * @Section_Id:AdgPair
 * @title: AdgPair
 * @short_description: A wrapper for #CpmlPair
 *
 * AdgPair is a wrapper in #GType syntax of the #CpmlPair struct.
 *
 * Since: 1.0
 **/

/**
 * AdgPair:
 *
 * Another name for #CpmlPair: check its documentation for the
 * fields description and visibility details.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <string.h>


GType
adg_pair_get_type(void)
{
    static int pair_type = 0;

    if (G_UNLIKELY(pair_type == 0))
        pair_type = g_boxed_type_register_static("AdgPair",
                                                 (GBoxedCopyFunc) adg_pair_dup,
                                                 g_free);

    return pair_type;
}

/**
 * adg_pair_copy:
 * @pair: the destination #AdgPair
 * @src: the source #AdgPair
 *
 * Copies @src in @pair. It works in the same way as
 * cpml_pair_copy() but performing argument validation
 * before proceeding.
 *
 * Since: 1.0
 **/
void
adg_pair_copy(AdgPair *pair, const AdgPair *src)
{
    g_return_if_fail(pair != NULL);
    g_return_if_fail(src != NULL);

    memcpy(pair, src, sizeof(AdgPair));
}

/**
 * adg_pair_dup:
 * @pair: an #AdgPair structure
 *
 * Duplicates @pair.
 *
 * Returns: the duplicate of @pair: must be freed with g_free()
 *           when no longer needed.
 *
 * Since: 1.0
 **/
AdgPair *
adg_pair_dup(const AdgPair *pair)
{
    /* g_memdup() returns NULL if pair is NULL */
    return g_memdup(pair, sizeof(AdgPair));
}
