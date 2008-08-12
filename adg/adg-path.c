/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * The #AdgPath object is peraphs the simplest entity.
 *
 * It contains a pointer to the desired #cairo_path_t structure.
 */

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-line-style.h"
#include "adg-util.h"
#include "adg-canvas.h"
#include "adg-intl.h"
#include <math.h>

#define PARENT_CLASS ((AdgEntityClass *) adg_path_parent_class)
#define ARC_TOLERANCE   0.1


typedef enum _Direction Direction;
enum _Direction {
    DIRECTION_FORWARD,
    DIRECTION_REVERSE
};


static void	finalize		(GObject	*object);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static void     clear                   (AdgPath        *path);


G_DEFINE_TYPE(AdgPath, adg_path, ADG_TYPE_ENTITY);


static void
adg_path_class_init(AdgPathClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPathPrivate));

    gobject_class->finalize = finalize;

    entity_class->render = render;

    klass->clear = clear;
}

static void
adg_path_init(AdgPath *path)
{
    AdgPathPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
						       AdgPathPrivate);

    priv->cp.x = 0.;
    priv->cp.y = 0.;
    priv->cairo_path = NULL;
    priv->callback = NULL;

    path->priv = priv;
}

static void
finalize(GObject *object)
{
    ADG_PATH_GET_CLASS(object)->clear((AdgPath *) object);
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
 */
AdgEntity *
adg_path_new(AdgCallback callback)
{
    AdgEntity *entity;

    entity = (AdgEntity *) g_object_new(ADG_TYPE_PATH, NULL);
    ((AdgPath *) entity)->priv->callback = callback;

    return entity;
}

void
adg_path_clear(AdgPath *path)
{
    g_return_if_fail(ADG_IS_PATH(path));
    ADG_PATH_GET_CLASS(path)->clear(path);
}


const cairo_path_t *
adg_path_get_cairo_path(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    return path->priv->cairo_path;
}

void
adg_path_dump(AdgPath *path)
{
    cairo_path_data_t *data;
    gint n_data, n_point;

    g_return_if_fail(ADG_IS_PATH(path));

    if (path->priv->cairo_path->data == NULL)
        return;

    for (n_data = 0; n_data < path->priv->cairo_path->num_data; ++n_data) {
	data = path->priv->cairo_path->data + n_data;

	switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
	    g_print("Move to ");
	    break;
	case CAIRO_PATH_LINE_TO:
	    g_print("Line to ");
	    break;
	case CAIRO_PATH_CURVE_TO:
	    g_print("Curve to ");
	    break;
	case CAIRO_PATH_CLOSE_PATH:
	    g_print("Path close");
	    break;
	default:
	    g_print("Unknown entity (%d)", data->header.type);
	    break;
	}

	for (n_point = 1; n_point < data->header.length; ++n_point)
	    g_print("(%lf, %lf) ", data[n_point].point.x,
		    data[n_point].point.y);

	n_data += n_point - 1;
	g_print("\n");
    }
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
clear(AdgPath *path)
{
    if (path->priv->cairo_path != NULL) {
        cairo_path_destroy(path->priv->cairo_path);
        path->priv->cairo_path = NULL;
    }

    path->priv->cp.x = 0.;
    path->priv->cp.y = 0.;
}
