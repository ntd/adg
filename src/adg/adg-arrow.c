/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-arrow
 * @short_description: Arrow rendering related stuff
 *
 * Contains parameters on how to draw arrows, providing a way to register a
 * custom rendering callback.
 **/

/**
 * AdgArrow:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-path.h"
#include "adg-marker.h"

#include "adg-arrow.h"
#include "adg-arrow-private.h"


G_DEFINE_TYPE(AdgArrow, adg_arrow, ADG_TYPE_MARKER)

enum {
    PROP_0,
    PROP_ANGLE
};


static void             _adg_get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static AdgModel *       _adg_create_model       (AdgMarker      *marker);


static void
adg_arrow_class_init(AdgArrowClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    AdgMarkerClass *marker_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;
    marker_class = (AdgMarkerClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgArrowPrivate));

    gobject_class->set_property = _adg_set_property;
    gobject_class->get_property = _adg_get_property;

    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    marker_class->create_model = _adg_create_model;

    param = g_param_spec_double("angle",
                                P_("Arrow Angle"),
                                P_("The opening angle of the arrow"),
                                -G_PI, G_PI, G_PI / 6,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE, param);
}

static void
adg_arrow_init(AdgArrow *arrow)
{
    AdgArrowPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(arrow,
                                                        ADG_TYPE_ARROW,
                                                        AdgArrowPrivate);

    data->angle = G_PI/6;

    arrow->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgArrowPrivate *data = ((AdgArrow *) object)->data;

    switch (prop_id) {
    case PROP_ANGLE:
        g_value_set_double(value, data->angle);
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
    AdgArrowPrivate *data = ((AdgArrow *) object)->data;

    switch (prop_id) {
    case PROP_ANGLE:
        data->angle = cpml_angle(g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_arrow_new:
 * @trail: the #AdgTrail to arrow
 *
 * Creates a new undefined arrow entity. The position must be defined
 * by setting the #AdgMarker:trail and #AdgMarker:pos properties.
 * By default, an arrow as #AdgEntity:local-method set to #ADG_MIX_PARENT.
 *
 * Returns: the newly created arrow entity
 **/
AdgArrow *
adg_arrow_new(void)
{
    return g_object_new(ADG_TYPE_ARROW,
                        "local-method", ADG_MIX_PARENT,
                        NULL);
}

/**
 * adg_arrow_new_with_trail:
 * @trail: the #AdgTrail where the arrow should be added
 * @pos: the position ratio on @trail
 *
 * Creates a new arrow on the first segment on @trail at position
 * @pos, where @pos is a ratio of the @trail length (being %0 the
 * start point, %1 the end point, %0.5 the middle point and so on).
 * By default, an arrow as #AdgEntity:local-method set to #ADG_MIX_PARENT.
 *
 * Returns: the newly created arrow entity
 **/
AdgArrow *
adg_arrow_new_with_trail(AdgTrail *trail, gdouble pos)
{
    return g_object_new(ADG_TYPE_ARROW,
                        "local-method", ADG_MIX_PARENT,
                        "trail", trail,
                        "n-segment", 1,
                        "pos", pos,
                        NULL);
}

/**
 * adg_arrow_set_angle:
 * @arrow: an #AdgArrow
 * @angle: the new angle
 *
 * Sets a new angle: @angle will be the new opening angle of @arrow.
 * Changing the arrow angle will invalidate @arrow.
 **/
void
adg_arrow_set_angle(AdgArrow *arrow, gdouble angle)
{
    g_return_if_fail(ADG_IS_ARROW(arrow));
    g_object_set(arrow, "angle", angle, NULL);
}

/**
 * adg_arrow_get_angle:
 * @arrow: an #AdgArrow
 *
 * Gets the current angle of @arrow.
 *
 * Returns: the arrow angle, in radians
 **/
gdouble
adg_arrow_get_angle(AdgArrow *arrow)
{
    AdgArrowPrivate *data;

    g_return_val_if_fail(ADG_IS_ARROW(arrow), 0);

    data = arrow->data;

    return data->angle;
}


static void
_adg_arrange(AdgEntity *entity)
{
    AdgModel *model;
    const CpmlExtents *extents;
    CpmlExtents new_extents;

    model = adg_marker_model((AdgMarker *) entity);
    if (model == NULL)
        return;

    extents = adg_trail_get_extents((AdgTrail *) model);
    if (extents == NULL)
        return;

    cpml_extents_copy(&new_extents, extents);
    cpml_extents_transform(&new_extents, adg_entity_get_local_matrix(entity));
    adg_entity_set_extents(entity, &new_extents);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgModel *model;
    const cairo_path_t *cairo_path;

    model = adg_marker_model((AdgMarker *) entity);
    if (model == NULL)
        return;

    cairo_path = adg_trail_get_cairo_path((AdgTrail *) model);

    if (cairo_path != NULL) {
        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_global_matrix(entity));
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_fill(cr);
    }
}

static AdgModel *
_adg_create_model(AdgMarker *marker)
{
    AdgArrowPrivate *data;
    AdgPath *path;
    CpmlPair p1, p2;

    data = ((AdgArrow *) marker)->data;
    path = adg_path_new();
    cpml_vector_from_angle(&p1, data->angle / 2);
    p2.x = p1.x;
    p2.y = -p1.y;

    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to(path, &p1);
    adg_path_line_to(path, &p2);
    adg_path_close(path);

    return (AdgModel *) path;
}
