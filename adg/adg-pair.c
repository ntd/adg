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
 * SECTION:adg-pair
 * @title: AdgPair
 * @short_description: A wrapper for the #CpmlPair struct
 *
 * The AdgPair is a wrapper typedef in GType syntax of the #CpmlPair struct.
 **/

/**
 * AdgPair:
 *
 * Another name for the #CpmlPair type: check its documentation
 * for fields description.
 **/


#include "adg-pair.h"

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
 * adg_pair_dup:
 * @pair: an #AdgPair structure
 *
 * Duplicates @pair.
 *
 * Return value: the duplicate of @pair: must be freed with g_free()
 *               when no longer needed.
 **/
AdgPair *
adg_pair_dup(const AdgPair * pair)
{
    return g_memdup(pair, sizeof(AdgPair));
}
