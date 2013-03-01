/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-gobject
 * @Section_Id:GObject
 * @title: GObject wrappers
 * @short_description: Collection of #GBoxed wrappers for CPML structs.
 *
 * These wrappers are supposed to make bindings development easier.
 * This file defines the wrappers and the functions needed for
 * implementing a #GBoxed type, most notably the #GBoxedCopyFunc
 * requested by g_boxed_type_register_static() (the #GBoxedFreeFunc
 * will usually be g_free()).
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include <glib-object.h>
#include "cpml-gobject.h"


GType
cpml_pair_get_type(void)
{
    static GType pair_type = 0;

    if (G_UNLIKELY(pair_type == 0))
        pair_type = g_boxed_type_register_static("CpmlPair",
                                                 (GBoxedCopyFunc) cpml_pair_dup,
                                                 g_free);

    return pair_type;
}

/**
 * cpml_pair_dup:
 * @pair: an #CpmlPair structure
 *
 * Duplicates @pair.
 *
 * Returns: (transfer full): the duplicate of @pair: must be freed with g_free() when no longer needed.
 *
 * Since: 1.0
 **/
CpmlPair *
cpml_pair_dup(const CpmlPair *pair)
{
    /* g_memdup() returns NULL if pair is NULL */
    return g_memdup(pair, sizeof(CpmlPair));
}
