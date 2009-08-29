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


#include "adg-arrow.h"
#include "adg-arrow-private.h"
#include "adg-path.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_ANGLE
};


static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static gboolean         render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static AdgModel *       create_model            (AdgMarker      *marker);
static gboolean         set_angle               (AdgArrow       *arrow,
                                                 gdouble         angle);


G_DEFINE_TYPE(AdgArrow, adg_arrow, ADG_TYPE_MARKER);


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

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    entity_class->render = render;

    marker_class->create_model = create_model;

    param = g_param_spec_double("angle",
                                P_("Arrow Angle"),
                                P_("The opening angle of the arrow"),
                                0, G_PI*2, G_PI/6,
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
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
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
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgArrow *arrow = (AdgArrow *) object;

    switch (prop_id) {
    case PROP_ANGLE:
        set_angle(arrow, g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
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

    if (set_angle(arrow, angle))
        g_object_notify((GObject *) arrow, "angle");
}


static gboolean
render(AdgEntity *entity, cairo_t *cr)
{
    AdgModel *model;
    const cairo_path_t *cairo_path;

    model = adg_marker_get_model((AdgMarker *) entity);
    cairo_path = adg_path_get_cairo_path((AdgPath *) model);

    if (cairo_path != NULL) {
        AdgMatrix local, ctm;

        adg_entity_get_local_matrix(entity, &local);
        cairo_get_matrix(cr, &ctm);
        cairo_matrix_multiply(&ctm, &ctm, &local);

        cairo_save(cr);
        cairo_set_matrix(cr, &ctm);
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_fill(cr);
    }

    return TRUE;
}

static AdgModel *
create_model(AdgMarker *marker)
{
    AdgArrowPrivate *data;
    AdgPath *path;
    CpmlVector vector;

    data = ((AdgArrow *) marker)->data;
    path = (AdgPath *) adg_path_new();
    cpml_vector_from_angle(&vector, data->angle / 2, 1);

    adg_path_move_to(path, 0, 0);
    adg_path_line_to(path, vector.x, vector.y);
    adg_path_line_to(path, vector.x, -vector.y);
    adg_path_close(path);

    return (AdgModel *) path;
}

static gboolean
set_angle(AdgArrow *arrow, gdouble angle)
{
    AdgArrowPrivate *data = arrow->data;

    if (angle == data->angle)
        return FALSE;

    data->angle = angle;
    adg_entity_invalidate((AdgEntity *) arrow);

    return TRUE;
}
