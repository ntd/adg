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
 * SECTION:adg-stroke
 * @short_description: A stroked entity
 *
 * The #AdgStroke object is a stroked representation of an #AdgTrail model.
 **/

/**
 * AdgStroke:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-stroke.h"
#include "adg-stroke-private.h"
#include "adg-line-style.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_TRAIL
};

static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static gboolean set_trail               (AdgStroke      *stroke,
                                         AdgTrail       *trail);
static void     unset_trail             (AdgStroke      *stroke);
static gboolean render                  (AdgEntity      *entity,
                                         cairo_t        *cr);


G_DEFINE_TYPE(AdgStroke, adg_stroke, ADG_TYPE_ENTITY);


static void
adg_stroke_class_init(AdgStrokeClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgStrokePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->render = render;

    param = g_param_spec_object("trail",
                                P_("Trail"),
                                P_("The trail to be stroked"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_CONSTRUCT|G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TRAIL, param);
}

static void
adg_stroke_init(AdgStroke *stroke)
{
    AdgStrokePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(stroke,
                                                         ADG_TYPE_STROKE,
                                                         AdgStrokePrivate);

    data->trail = NULL;

    stroke->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgStrokePrivate *data = ((AdgStroke *) object)->data;

    switch (prop_id) {
    case PROP_TRAIL:
        g_value_set_object(value, &data->trail);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgStroke *stroke = (AdgStroke *) object;

    switch (prop_id) {
    case PROP_TRAIL:
        set_trail(stroke, (AdgTrail *) g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_stroke_new:
 * @trail: the #AdgTrail to stroke
 *
 * Creates a new stroke entity.
 *
 * Returns: the newly created entity
 **/
AdgEntity *
adg_stroke_new(AdgTrail *trail)
{
    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    return (AdgEntity *) g_object_new(ADG_TYPE_STROKE, "trail", trail, NULL);
}


/**
 * adg_stroke_get_trail:
 * @stroke: an #AdgStroke
 *
 * Gets the #AdgTrail binded to this @stroke entity.
 *
 * Returns: the requested #AdgTrail or %NULL on errors
 **/
AdgTrail *
adg_stroke_get_trail(AdgStroke *stroke)
{
    AdgStrokePrivate *data;

    g_return_val_if_fail(ADG_IS_STROKE(stroke), NULL);

    data = stroke->data;

    return data->trail;
}

/**
 * adg_stroke_set_trail:
 * @stroke: an #AdgStroke
 * @trail: the new #AdgTrail to bind
 *
 * Sets @trail as the new trail to be stroked by @stroke.
 **/
void
adg_stroke_set_trail(AdgStroke *stroke, AdgTrail *trail)
{
    g_return_if_fail(ADG_IS_STROKE(stroke));

    if (set_trail(stroke, trail))
        g_object_notify((GObject *) stroke, "trail");
}


static gboolean
set_trail(AdgStroke *stroke, AdgTrail *trail)
{
    AdgEntity *entity;
    AdgStrokePrivate *data;

    entity = (AdgEntity *) stroke;
    data = stroke->data;

    if (trail == data->trail)
        return FALSE;

    if (data->trail != NULL) {
        g_object_weak_unref((GObject *) data->trail,
                            (GWeakNotify) unset_trail, stroke);
        adg_model_remove_dependency((AdgModel *) data->trail, entity);
    }

    data->trail = trail;

    if (data->trail != NULL) {
        g_object_weak_ref((GObject *) data->trail,
                          (GWeakNotify) unset_trail, stroke);
        adg_model_add_dependency((AdgModel *) data->trail, entity);
    }

    return TRUE;
}

static void
unset_trail(AdgStroke *stroke)
{
    AdgStrokePrivate *data = stroke->data;

    if (data->trail != NULL) {
        data->trail = NULL;
        adg_entity_invalidate((AdgEntity *) stroke);
    }
}

static gboolean
render(AdgEntity *entity, cairo_t *cr)
{
    AdgStroke *stroke;
    AdgStrokePrivate *data;
    const cairo_path_t *cairo_path;

    stroke = (AdgStroke *) entity;
    data = stroke->data;
    cairo_path = adg_trail_get_cairo_path(data->trail);

    if (cairo_path != NULL) {
        cairo_save(cr);
        adg_entity_apply_local_matrix(entity, cr);
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        adg_entity_apply(entity, ADG_SLOT_LINE_STYLE, cr);
        cairo_stroke(cr);
    }

    return TRUE;
}
