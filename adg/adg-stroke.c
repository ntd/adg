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
#include "adg-dress-builtins.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_stroke_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_stroke_parent_class)


enum {
    PROP_0,
    PROP_LINE_DRESS,
    PROP_TRAIL
};

static void             dispose                 (GObject        *object);
static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             local_changed           (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static gboolean         set_trail               (AdgStroke      *stroke,
                                                 AdgTrail       *trail);
static void             unset_trail             (AdgStroke      *stroke);


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

    gobject_class->dispose = dispose;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->local_changed = local_changed;
    entity_class->arrange = arrange;
    entity_class->render = render;

    param = adg_param_spec_dress("line-dress",
                                 P_("Line Dress"),
                                 P_("The dress to use for stroking this entity"),
                                 ADG_DRESS_LINE_MEDIUM,
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

    data->line_dress = ADG_DRESS_LINE_MEDIUM;
    data->trail = NULL;

    stroke->data = data;
}

static void
dispose(GObject *object)
{
    AdgStroke *stroke = (AdgStroke *) object;

    adg_stroke_set_trail(stroke, NULL);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgStrokePrivate *data = ((AdgStroke *) object)->data;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        g_value_set_int(value, data->line_dress);
        break;
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
    AdgStroke *stroke;
    AdgStrokePrivate *data;

    stroke = (AdgStroke *) object;
    data = stroke->data;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        adg_dress_set(&data->line_dress, g_value_get_int(value));
        break;
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
 * Returns: the newly created stroke entity
 **/
AdgStroke *
adg_stroke_new(AdgTrail *trail)
{
    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    return g_object_new(ADG_TYPE_STROKE, "trail", trail, NULL);
}

/**
 * adg_stroke_get_line_dress:
 * @stroke: an #AdgStroke
 *
 * Gets the line dress to be used in rendering @stroke.
 *
 * Returns: the current line dress
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
 **/
void
adg_stroke_set_line_dress(AdgStroke *stroke, AdgDress dress)
{
    AdgStrokePrivate *data;

    g_return_if_fail(ADG_IS_STROKE(stroke));

    data = stroke->data;

    if (adg_dress_set(&data->line_dress, dress))
        g_object_notify((GObject *) stroke, "line-dress");
}

/**
 * adg_stroke_get_trail:
 * @stroke: an #AdgStroke
 *
 * Gets the #AdgTrail bound to this @stroke entity.
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


static void
local_changed(AdgEntity *entity)
{
    AdgMatrix old;
    const AdgMatrix *new;

    adg_matrix_copy(&old, adg_entity_local_matrix(entity));

    if (PARENT_ENTITY_CLASS->local_changed != NULL)
        PARENT_ENTITY_CLASS->local_changed(entity);

    new = adg_entity_local_matrix(entity);

    /* For simple translation, avoid the entity invalidation:
     * translate the extents of the same vector instead */
    if (old.xx != new->xx || old.yy != new->yy ||
        old.xy != new->xy || old.yx != new->yx) {
        adg_entity_invalidate(entity);
    } else {
        CpmlExtents *extents = (CpmlExtents *) adg_entity_extents(entity);

        extents->org.x += new->x0 - old.x0;
        extents->org.y += new->y0 - old.y0;
    }
}

static void
arrange(AdgEntity *entity)
{
    CpmlExtents *extents = (CpmlExtents *) adg_entity_extents(entity);

    if (!extents->is_defined) {
        AdgStroke *stroke;
        AdgStrokePrivate *data;
        const AdgMatrix *local;

        stroke = (AdgStroke *) entity;
        data = stroke->data;
        local = adg_entity_local_matrix(entity);

        cpml_extents_copy(extents, adg_trail_extents(data->trail));

        /* Apply the local matrix to the extents */
        cpml_pair_transform(&extents->org, local);
        cpml_vector_transform(&extents->size, local);
    }
}

static void
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
        cairo_set_matrix(cr, adg_entity_ctm(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        adg_entity_apply_dress(entity, data->line_dress, cr);
        cairo_stroke(cr);
    }
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

    if (data->trail != NULL)
        set_trail(stroke, NULL);
}
