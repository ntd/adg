/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * SECTION:point
 * @title: AdgPoint
 * @short_description: A generic point with model and paper space components
 *
 * The AdgPoint is quite different from what usually expected: to get the
 * common (x, y) struct take a look to the #AdgPair object.
 *
 * In ADG, points have usually two components, being the component an usual
 * (x, y) struct (an AdgPair object in this implementation). The model
 * matrix is applied only to the model component while the paper matrix
 * affects only the paper component.
 **/

#include "adg-point.h"

#include <string.h>


GType
adg_point_get_type(void)
{
    static int point_type = 0;

    if (G_UNLIKELY(point_type == 0))
        point_type = g_boxed_type_register_static("AdgPoint",
                                                  (GBoxedCopyFunc) adg_point_dup,
                                                  g_free);

    return point_type;
}

/**
 * adg_point_dup:
 * @point: an #AdgPoint structure
 *
 * Duplicates @point.
 *
 * Return value: the duplicate of @point: must be freed with g_free()
 *               when no longer needed.
 **/
AdgPoint *
adg_point_dup(const AdgPoint *point)
{
    return g_memdup(point, sizeof(AdgPoint));
}

/**
 * adg_point_copy:
 * @point: an #AdgPoint structure
 * @dest: the destination
 *
 * Shortcut to copy @point to @dest.
 **/
void
adg_point_copy(const AdgPoint *point, AdgPoint *dest)
{
    memcpy(dest, point, sizeof(AdgPoint));
}

/**
 * adg_point_set:
 * @point: an #AdgPoint structure
 * @model: the model component
 * @paper: the paper component
 *
 * Fills the component of @point using the provided ones.
 **/
void
adg_point_set(AdgPoint *point, const AdgPair *model, const AdgPair *paper)
{
    memcpy(&point->model, model, sizeof(AdgPair));
    memcpy(&point->paper, paper, sizeof(AdgPair));
}
