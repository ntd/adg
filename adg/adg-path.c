/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

/**
 * SECTION:path
 * @title: AdgPath
 * @short_description: A stroked path entity
 *
 * The #AdgPath object is probably the simplest entity: it represents a path
 * (as intended by the #cairo_path_t structure) in model space.
 **/

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_path_parent_class)


static void	finalize		(GObject	*object);
static void     invalidate              (AdgEntity      *entity);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static void     clear_path_cache        (AdgPath        *path);


G_DEFINE_TYPE(AdgPath, adg_path, ADG_TYPE_ENTITY)


static void
adg_path_class_init(AdgPathClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPathPrivate));

    gobject_class->finalize = finalize;

    entity_class->invalidate = invalidate;
    entity_class->render = render;
}

static void
adg_path_init(AdgPath *path)
{
    AdgPathPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
						       AdgPathPrivate);

    priv->cp.x = priv->cp.y = 0.;
    priv->cairo_path = NULL;
    priv->callback = NULL;

    path->priv = priv;
}

static void
finalize(GObject *object)
{
    clear_path_cache((AdgPath *) object);

    ((GObjectClass *) PARENT_CLASS)->finalize(object);
}

/**
 * adg_path_new:
 * @callback: an #AdgCallback callback
 *
 * Creates a new path entity. The path must be constructed in the @callback
 * function: AdgPath will cache and reuse the cairo_copy_path() returned by
 * the cairo context after the @callback call.
 *
 * Return value: the new entity
 **/
AdgEntity *
adg_path_new(AdgCallback callback)
{
    AdgEntity *entity;

    entity = (AdgEntity *) g_object_new(ADG_TYPE_PATH, NULL);
    ((AdgPath *) entity)->priv->callback = callback;

    return entity;
}

/**
 * adg_path_get_cairo_path:
 * @path: an #AdgPath
 *
 * Gets a pointer to the cairo path structure of @path. The return value
 * is owned by @path and must be considered read-only.
 *
 * Return value: a pointer to the internal #cairo_path_t structure
 **/
const cairo_path_t *
adg_path_get_cairo_path(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    return path->priv->cairo_path;
}

/**
 * adg_path_dump:
 * @path: an #AdgPath
 *
 * Dumps the data content of @path to stdout in a human readable format.
 **/
void
adg_path_dump(AdgPath *path)
{
    CpmlPath cpml_path;

    g_return_if_fail(ADG_IS_PATH(path));

    if (!cpml_path_from_cairo(&cpml_path, path->priv->cairo_path, NULL)) {
        g_print("No path data to dump!\n");
    } else if (!cpml_path_dump(&cpml_path)) {
        g_print("Invalid path data!\n");
    }
}

static void
invalidate(AdgEntity *entity)
{
    clear_path_cache((AdgPath *) entity);

    PARENT_CLASS->invalidate(entity);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgPath *path = (AdgPath *) entity;

    if (path->priv->cairo_path) {
        /* Use cached data */
        cairo_move_to(cr, path->priv->cp.x, path->priv->cp.y);
        cairo_append_path(cr, path->priv->cairo_path);
    } else {
        /* Construct and store the cache */
        cairo_get_current_point(cr, &path->priv->cp.x, &path->priv->cp.y);

        if (path->priv->callback)
            path->priv->callback(entity, cr);

        path->priv->cairo_path = cairo_copy_path(cr);
    }

    adg_entity_apply(entity, ADG_SLOT_LINE_STYLE, cr);
    cairo_stroke(cr);

    PARENT_CLASS->render(entity, cr);
}

static void
clear_path_cache(AdgPath *path)
{
    AdgPathPrivate *priv = path->priv;

    if (priv->cairo_path != NULL) {
        cairo_path_destroy(priv->cairo_path);
        priv->cairo_path = NULL;
    }

    priv->cp.x = priv->cp.y = 0.;
}
