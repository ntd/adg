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
 * SECTION:adg-point
 * @Section_Id:AdgPoint
 * @title: AdgPoint
 * @short_description: A struct holding x, y coordinates
 *                     (either named or explicit)
 *
 * AdgPoint is an opaque structure that manages 2D coordinates, either
 * set explicitely throught adg_point_set() and adg_point_set_explicit()
 * or taken from a model with adg_point_set_from_model(). It can be
 * thought as an #AdgPair on steroid, because it adds named pair
 * support to a simple pairs, and thus enabling coordinates depending
 * on #AdgModel instances.
 **/

/**
 * ADG_POINT_PAIR:
 * @point: an #AdgPoint
 *
 * #AdgPoint is an evolution of the pair concept, but internally the
 * relevant data is still kept by an #AdgPair struct. This macro
 * gets this struct. Being declared at the start of the #AdgPoint
 * struct, it'd suffice to cast the point to an #AdgPair too.
 *
 * Returns: the pair of @point
 **/

/**
 * AdgPoint:
 *
 * This is an opaque struct: all its fields are privates.
 **/


#include "adg-point.h"

#include <string.h>


struct _AdgPoint {
    AdgPair      pair;
    AdgModel    *model;
    gchar       *name;
};

GType
adg_point_get_type(void)
{
    static int type = 0;

    if (G_UNLIKELY(type == 0))
        type = g_boxed_type_register_static("AdgPoint",
                                            (GBoxedCopyFunc) adg_point_dup,
                                            (GBoxedFreeFunc) adg_point_destroy);

    return type;
}

/**
 * adg_point_copy:
 * @point: an #AdgPoint
 * @src: the source point to copy
 *
 * Copies @src into @point.
 *
 * Returns: @point
 **/
AdgPoint *
adg_point_copy(AdgPoint *point, const AdgPoint *src)
{
    g_return_val_if_fail(point != NULL, NULL);
    g_return_val_if_fail(src != NULL, NULL);

    if (src->model != NULL)
        g_object_ref(src->model);

    if (point->model != NULL)
        g_object_unref(point->model);

    return memcpy(point, src, sizeof(AdgPoint));
}

/**
 * adg_point_dup:
 * @point: an #AdgPoint structure
 *
 * Duplicates @point.
 *
 * Returns: the duplicated #AdgPoint struct or %NULL on errors
 **/
AdgPoint *
adg_point_dup(const AdgPoint *point)
{
    g_return_val_if_fail(point != NULL, NULL);

    if (point->model != NULL)
        g_object_ref(point->model);

    return g_memdup(point, sizeof(AdgPoint));
}

/**
 * adg_point_new:
 *
 * Creates a new empty #AdgPoint. The returned pointer
 * should be freed with adg_point_destroy() when no longer needed.
 *
 * Returns: a newly created #AdgPoint
 **/
AdgPoint *
adg_point_new(void)
{
    return g_new0(AdgPoint, 1);
}

/**
 * adg_point_destroy:
 * @point: an #AdgPoint
 *
 * Destroys the @point instance, unreferencing the internal model if
 * @point is linked to a named pair.
 **/
void
adg_point_destroy(AdgPoint *point)
{
    g_return_if_fail(point != NULL);

    adg_point_set_from_model(point, NULL, NULL);

    g_free(point);
}

/**
 * adg_point_set:
 * @point: an #AdgPoint
 * @pair: the #AdgPair to use
 *
 * Sets an explicit pair in @point by using the given @pair. If
 * @point was linked to a named pair in a model, this link is
 * dropped before setting the pair.
 **/
void
adg_point_set(AdgPoint *point, const AdgPair *pair)
{
    g_return_if_fail(point != NULL);
    g_return_if_fail(pair != NULL);

    adg_point_set_explicit(point, pair->x, pair->y);
}

/**
 * adg_point_set_explicit:
 * @point: an #AdgPoint
 * @x: the x coordinate of the point
 * @y: the y coordinate of the point
 *
 * Works in the same way of adg_point_set() but accept direct numbers
 * instead of an #AdgPair structure.
 **/
void
adg_point_set_explicit(AdgPoint *point, gdouble x, gdouble y)
{
    g_return_if_fail(point != NULL);

    /* Unlink the named pair dependency, if any */
    adg_point_set_from_model(point, NULL, NULL);

    point->pair.x = x;
    point->pair.y = y;
}

/**
 * adg_point_set_from_model:
 * @point: an #AdgPair
 * @model: the #AdgModel
 * @name: the id of a named pair in @model
 *
 * Links the @name named pair of @model to @point, so any subsequent
 * call to adg_point_update() will read the named pair content. A
 * new reference is added to @model while the previous model (if any)
 * is unreferenced.
 *
 * It is allowed to use %NULL as @model, in which case the link
 * between @point and the named pair is dropped.
 **/
void
adg_point_set_from_model(AdgPoint *point, AdgModel *model, const gchar *name)
{
    g_return_if_fail(point != NULL);

    /* Check if unlinking a yet unlinked point */
    if (model == NULL && point->model == NULL)
        return;

    /* Ensure the name is not NULL if the model is specified */
    if (model != NULL) {
        g_return_if_fail(name != NULL);
    }

    /* Check if the named pair is different from the old one */
    if (model == point->model && strcmp(point->name, name) == 0)
        return;

    if (model != NULL)
        g_object_ref(model);

    if (point->model != NULL) {
        /* Remove the old named pair */
        g_object_unref(point->model);
        g_free(point->name);
        point->model = NULL;
        point->name = NULL;
    }

    if (model != NULL) {
        /* Set the new named pair */
        point->model = model;
        point->name = g_strdup(name);
    }

    adg_point_update(point);
}

/**
 * adg_point_update:
 * @point: an #AdgPoint
 *
 * Updates the pair of @point. If @point is an explicit pair (either
 * if set with adg_point_set() or adg_point_set_explicit()), no action
 * is taken. If @point is linked to a named pair, instead, the internal
 * pair is updated with the value returned by adg_model_named_pair().
 **/
void
adg_point_update(AdgPoint *point)
{
    const AdgPair *pair;

    g_return_if_fail(point != NULL);

    /* A point with explicit coordinates does not need to be updated */
    if (point->model == NULL)
        return;

    pair = adg_model_named_pair(point->model, point->name);
    if (pair == NULL) {
        g_warning("%s: `%s' named pair not found in `%s' model instance",
                  G_STRLOC, point->name,
                  g_type_name(G_TYPE_FROM_INSTANCE(point->model)));
        return;
    }

    cpml_pair_copy(&point->pair, pair);
}
