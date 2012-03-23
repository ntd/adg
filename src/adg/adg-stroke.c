/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 *
 * Since: 1.0
 **/

/**
 * AdgStroke:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"
#include "adg-style.h"
#include "adg-line-style.h"
#include "adg-model.h"
#include "adg-trail.h"

#include "adg-stroke.h"
#include "adg-stroke-private.h"

#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_stroke_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_stroke_parent_class)


G_DEFINE_TYPE(AdgStroke, adg_stroke, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_LINE_DRESS,
    PROP_TRAIL
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_global_changed     (AdgEntity      *entity);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_unset_trail        (AdgStroke      *stroke);


static void
adg_stroke_class_init(AdgStrokeClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgStrokePrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    param = adg_param_spec_dress("line-dress",
                                 P_("Line Dress"),
                                 P_("The dress to use for stroking this entity"),
                                 ADG_DRESS_LINE_STROKE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE_DRESS, param);

    param = g_param_spec_object("trail",
                                P_("Trail"),
                                P_("The trail to be stroked"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_TRAIL, param);
}

static void
adg_stroke_init(AdgStroke *stroke)
{
    AdgStrokePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(stroke,
                                                         ADG_TYPE_STROKE,
                                                         AdgStrokePrivate);

    data->line_dress = ADG_DRESS_LINE_STROKE;
    data->trail = NULL;

    stroke->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgStroke *stroke = (AdgStroke *) object;

    adg_stroke_set_trail(stroke, NULL);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgStrokePrivate *data = ((AdgStroke *) object)->data;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        g_value_set_int(value, data->line_dress);
        break;
    case PROP_TRAIL:
        g_value_set_object(value, data->trail);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgStrokePrivate *data = ((AdgStroke *) object)->data;
    AdgTrail *old_trail;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        data->line_dress = g_value_get_int(value);
        break;
    case PROP_TRAIL:
        old_trail = data->trail;
        data->trail = g_value_get_object(value);

        if (data->trail != old_trail) {
            if (data->trail) {
                g_object_weak_ref((GObject *) data->trail,
                                  (GWeakNotify) _adg_unset_trail, object);
                adg_model_add_dependency((AdgModel *) data->trail,
                                         (AdgEntity *) object);
            }
            if (old_trail) {
                g_object_weak_unref((GObject *) old_trail,
                                    (GWeakNotify) _adg_unset_trail, object);
                adg_model_remove_dependency((AdgModel *) old_trail,
                                            (AdgEntity *) object);
            }
        }
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
 * Creates a new stroke entity based on the @trail model.
 * @trail can be %NULL, in which case an empty stroke is created.
 *
 * Returns: the newly created stroke entity
 *
 * Since: 1.0
 **/
AdgStroke *
adg_stroke_new(AdgTrail *trail)
{
    return g_object_new(ADG_TYPE_STROKE, "trail", trail, NULL);
}

/**
 * adg_stroke_set_line_dress:
 * @stroke: an #AdgStroke
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering @stroke. The new dress
 * must be related to the original dress for this property:
 * you cannot set a dress used for line styles to a dress
 * managing fonts.
 *
 * The check is done by calling adg_dress_are_related() with
 * @dress and the previous dress as arguments. Check out its
 * documentation for details on what is a related dress.
 *
 * Since: 1.0
 **/
void
adg_stroke_set_line_dress(AdgStroke *stroke, AdgDress dress)
{
    g_return_if_fail(ADG_IS_STROKE(stroke));
    g_object_set(stroke, "line-dress", dress, NULL);
}

/**
 * adg_stroke_get_line_dress:
 * @stroke: an #AdgStroke
 *
 * Gets the line dress to be used in rendering @stroke.
 *
 * Returns: (transfer none): the current line dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_stroke_get_line_dress(AdgStroke *stroke)
{
    AdgStrokePrivate *data;

    g_return_val_if_fail(ADG_IS_STROKE(stroke), ADG_DRESS_UNDEFINED);

    data = stroke->data;

    return data->line_dress;
}

/**
 * adg_stroke_set_trail:
 * @stroke: an #AdgStroke
 * @trail: the new #AdgTrail to bind
 *
 * Sets @trail as the new trail to be stroked by @stroke.
 *
 * Since: 1.0
 **/
void
adg_stroke_set_trail(AdgStroke *stroke, AdgTrail *trail)
{
    g_return_if_fail(ADG_IS_STROKE(stroke));
    g_object_set(stroke, "trail", trail, NULL);
}

/**
 * adg_stroke_get_trail:
 * @stroke: an #AdgStroke
 *
 * Gets the #AdgTrail bound to this @stroke entity.
 * The returned trail is owned by @stroke and should not
 * be freed or modified.
 *
 * Returns: (transfer none): the requested #AdgTrail or %NULL on errors.
 *
 * Since: 1.0
 **/
AdgTrail *
adg_stroke_get_trail(AdgStroke *stroke)
{
    AdgStrokePrivate *data;

    g_return_val_if_fail(ADG_IS_STROKE(stroke), NULL);

    data = stroke->data;

    return data->trail;
}


static void
_adg_global_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    adg_entity_invalidate(entity);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    adg_entity_invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgStroke *stroke;
    AdgStrokePrivate *data;
    CpmlExtents extents;

    /* Check for cached result */
    if (adg_entity_get_extents(entity)->is_defined)
        return;

    stroke = (AdgStroke *) entity;
    data = stroke->data;

    cpml_extents_copy(&extents, adg_trail_get_extents(data->trail));
    cpml_extents_transform(&extents, adg_entity_get_local_matrix(entity));
    cpml_extents_transform(&extents, adg_entity_get_global_matrix(entity));

    adg_entity_set_extents(entity, &extents);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgStroke *stroke;
    AdgStrokePrivate *data;
    const cairo_path_t *cairo_path;

    stroke = (AdgStroke *) entity;
    data = stroke->data;
    cairo_path = adg_trail_get_cairo_path(data->trail);

    if (cairo_path != NULL) {
        cairo_transform(cr, adg_entity_get_global_matrix(entity));

        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        adg_entity_apply_dress(entity, data->line_dress, cr);
        cairo_stroke(cr);
    }
}

static void
_adg_unset_trail(AdgStroke *stroke)
{
    g_object_set(stroke, "trail", NULL, NULL);
}
