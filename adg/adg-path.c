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
 * SECTION:path
 * @title: AdgPath
 * @short_description: A path model
 *
 * The #AdgPath object is a virtual path: in the simplest
 * case, it is a stroked #cairo_path_t structure in model space.
 **/

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgModelClass *) adg_path_parent_class)


static void	finalize		(GObject	*object);
static void     changed                 (AdgModel       *model);
static void     clear_path_cache        (AdgPath        *path);


G_DEFINE_TYPE(AdgPath, adg_path, ADG_TYPE_MODEL)


static void
adg_path_class_init(AdgPathClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPathPrivate));

    gobject_class->finalize = finalize;

    model_class->changed = changed;
}

static void
adg_path_init(AdgPath *path)
{
    AdgPathPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
                                                       AdgPathPrivate);

    priv->cp.x = priv->cp.y = 0.;
    priv->cairo_path = NULL;

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
 *
 * Creates a new path model. The path must be constructed in the @callback
 * function: AdgPath will cache and reuse the cairo_copy_path() returned by
 * the cairo context after the @callback call.
 *
 * Return value: the new model
 **/
AdgModel *
adg_path_new(void)
{
    return (AdgModel *) g_object_new(ADG_TYPE_PATH, NULL);
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
    CpmlSegment segment;

    g_return_if_fail(ADG_IS_PATH(path));

    if (!cpml_segment_from_cairo(&segment, path->priv->cairo_path)) {
        g_print("Invalid path data to dump!\n");
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}

static void
changed(AdgModel *model)
{
    clear_path_cache((AdgPath *) model);

    PARENT_CLASS->changed(model);
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
