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
 * SECTION:stroke
 * @title: AdgStroke
 * @short_description: A stroked entity
 *
 * The #AdgStroke object is a generic stroked entity: in the simplest
 * case, it is a stroked #cairo_path_t structure in model space.
 **/

#include "adg-stroke.h"
#include "adg-stroke-private.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_stroke_parent_class)


static void	finalize		(GObject	*object);
static void     invalidate              (AdgEntity      *entity);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static void     clear_stroke_cache      (AdgStroke      *stroke);


G_DEFINE_TYPE(AdgStroke, adg_stroke, ADG_TYPE_ENTITY)


static void
adg_stroke_class_init(AdgStrokeClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgStrokePrivate));

    gobject_class->finalize = finalize;

    entity_class->invalidate = invalidate;
    entity_class->render = render;
}

static void
adg_stroke_init(AdgStroke *stroke)
{
    AdgStrokePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(stroke,
                                                         ADG_TYPE_STROKE,
                                                         AdgStrokePrivate);

    priv->cp.x = priv->cp.y = 0.;
    priv->cairo_path = NULL;
    priv->callback = NULL;

    stroke->priv = priv;
}

static void
finalize(GObject *object)
{
    clear_stroke_cache((AdgStroke *) object);

    ((GObjectClass *) PARENT_CLASS)->finalize(object);
}

/**
 * adg_stroke_new:
 * @callback: an #AdgCallback callback
 * @user_data: user provided pointer to pass to the callback
 *
 * Creates a new stroke entity. The stroke must be constructed in the @callback
 * function: AdgStroke will cache and reuse the cairo_copy_path() returned by
 * the cairo context after the @callback call.
 *
 * Return value: the new entity
 **/
AdgEntity *
adg_stroke_new(AdgCallback callback, gpointer user_data)
{
    AdgStroke *stroke;

    stroke = (AdgStroke *) g_object_new(ADG_TYPE_STROKE, NULL);
    stroke->priv->callback = callback;
    stroke->priv->user_data = user_data;

    return (AdgEntity *) stroke;
}

/**
 * adg_stroke_get_cairo_path:
 * @stroke: an #AdgStroke
 *
 * Gets a pointer to the cairo path structure of @stroke. The return value
 * is owned by @stroke and must be considered read-only.
 *
 * Return value: a pointer to the internal #cairo_path_t structure
 **/
const cairo_path_t *
adg_stroke_get_cairo_path(AdgStroke *stroke)
{
    g_return_val_if_fail(ADG_IS_STROKE(stroke), NULL);

    return stroke->priv->cairo_path;
}

/**
 * adg_stroke_dump:
 * @stroke: an #AdgStroke
 *
 * Dumps the data content of @stroke to stdout in a human readable format.
 **/
void
adg_stroke_dump(AdgStroke *stroke)
{
    CpmlSegment segment;

    g_return_if_fail(ADG_IS_STROKE(stroke));

    if (!cpml_segment_from_cairo(&segment, stroke->priv->cairo_path)) {
        g_print("Invalid stroke data to dump!\n");
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}

static void
invalidate(AdgEntity *entity)
{
    clear_stroke_cache((AdgStroke *) entity);

    PARENT_CLASS->invalidate(entity);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgStroke *stroke = (AdgStroke *) entity;

    if (stroke->priv->cairo_path) {
        /* Use cached data */
        cairo_move_to(cr, stroke->priv->cp.x, stroke->priv->cp.y);
        cairo_append_path(cr, stroke->priv->cairo_path);
    } else {
        /* Construct and store the cache */
        cairo_get_current_point(cr, &stroke->priv->cp.x, &stroke->priv->cp.y);

        if (stroke->priv->callback)
            stroke->priv->callback(entity, cr, stroke->priv->user_data);

        stroke->priv->cairo_path = cairo_copy_path(cr);
    }

    adg_entity_apply(entity, ADG_SLOT_LINE_STYLE, cr);
    cairo_stroke(cr);

    PARENT_CLASS->render(entity, cr);
}

static void
clear_stroke_cache(AdgStroke *stroke)
{
    AdgStrokePrivate *priv = stroke->priv;

    if (priv->cairo_path != NULL) {
        cairo_path_destroy(priv->cairo_path);
        priv->cairo_path = NULL;
    }

    priv->cp.x = priv->cp.y = 0.;
}
