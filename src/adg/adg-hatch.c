/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-hatch
 * @short_description: A hatched region
 *
 * The #AdgHatch object is used to fill a closed #AdgTrail model
 * with some sort of pattern.
 *
 * Since: 1.0
 **/

/**
 * AdgHatch:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-stroke.h"
#include "adg-style.h"
#include "adg-fill-style.h"
#include "adg-dress.h"
#include "adg-param-dress.h"

#include "adg-hatch.h"
#include "adg-hatch-private.h"


G_DEFINE_TYPE_WITH_PRIVATE(AdgHatch, adg_hatch, ADG_TYPE_STROKE)

enum {
    PROP_0,
    PROP_FILL_DRESS
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);


static void
adg_hatch_class_init(AdgHatchClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->render = _adg_render;

    param = adg_param_spec_dress("fill-dress",
                                 P_("Fill Dress"),
                                 P_("The dress to use for filling this entity"),
                                 ADG_DRESS_FILL_HATCH,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FILL_DRESS, param);
}

static void
adg_hatch_init(AdgHatch *hatch)
{
    AdgHatchPrivate *data = adg_hatch_get_instance_private(hatch);
    data->fill_dress = ADG_DRESS_FILL_HATCH;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgHatchPrivate *data = adg_hatch_get_instance_private((AdgHatch *) object);

    switch (prop_id) {
    case PROP_FILL_DRESS:
        g_value_set_enum(value, data->fill_dress);
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
    AdgHatch *hatch = (AdgHatch *) object;
    AdgHatchPrivate *data = adg_hatch_get_instance_private(hatch);

    switch (prop_id) {
    case PROP_FILL_DRESS:
        data->fill_dress = g_value_get_enum(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_hatch_new:
 * @trail: the #AdgTrail to hatch
 *
 * Creates a new hatch entity. @trail can be <constant>NULL</constant>,
 * in which case an empty hatch is created.
 *
 * Returns: the newly created hatch entity
 *
 * Since: 1.0
 **/
AdgHatch *
adg_hatch_new(AdgTrail *trail)
{
    return g_object_new(ADG_TYPE_HATCH, "trail", trail, NULL);
}

/**
 * adg_hatch_set_fill_dress:
 * @hatch: an #AdgHatch
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering @hatch. The new dress
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
adg_hatch_set_fill_dress(AdgHatch *hatch, AdgDress dress)
{
    g_return_if_fail(ADG_IS_HATCH(hatch));
    g_object_set(hatch, "fill-dress", dress, NULL);
}

/**
 * adg_hatch_get_fill_dress:
 * @hatch: an #AdgHatch
 *
 * Gets the line dress to be used in rendering @hatch.
 *
 * Returns: (transfer none): the current line dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_hatch_get_fill_dress(AdgHatch *hatch)
{
    AdgHatchPrivate *data;

    g_return_val_if_fail(ADG_IS_HATCH(hatch), ADG_DRESS_UNDEFINED);

    data = adg_hatch_get_instance_private(hatch);
    return data->fill_dress;
}


static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgHatch *hatch = (AdgHatch *) entity;
    AdgStroke *stroke = (AdgStroke *) entity;
    AdgTrail *trail = adg_stroke_get_trail(stroke);
    const cairo_path_t *cairo_path = adg_trail_get_cairo_path(trail);

    if (cairo_path != NULL) {
        AdgHatchPrivate *data = adg_hatch_get_instance_private(hatch);
        AdgFillStyle *fill_style =
            (AdgFillStyle *) adg_entity_style(entity, data->fill_dress);

        adg_fill_style_set_extents(fill_style, adg_entity_get_extents(entity));

        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_global_matrix(entity));
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        adg_style_apply((AdgStyle *) fill_style, entity, cr);
        cairo_fill(cr);
    }
}
