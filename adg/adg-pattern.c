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
 * SECTION:adg-pattern
 * @Section_Id:AdgPattern-GBoxed
 * @title: AdgPattern
 * @short_description: A wrapper for #cairo_pattern_t
 *
 * AdgPattern is a wrapper in #GType syntax of the #cairo_pattern_t struct.
 **/

/**
 * AdgPattern:
 *
 * Another name for #cairo_pattern_t: check its documentation for the
 * fields description and visibility details.
 **/


#include "adg-pattern.h"


GType
adg_pattern_get_type(void)
{
    static int pattern_type = 0;

    if (G_UNLIKELY(pattern_type == 0))
        pattern_type = g_boxed_type_register_static("AdgPattern",
                                                    (GBoxedCopyFunc) cairo_pattern_reference,
                                                    (GBoxedFreeFunc) cairo_pattern_destroy);

    return pattern_type;
}
