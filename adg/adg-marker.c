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
 * SECTION:adg-marker
 * @short_description: Base entity for markers
 *
 * A marker is an entity to be applied at the start or end of a segment.
 * Typical examples include arrows, ticks, dots and so on.
 **/

/**
 * AdgMarker:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-marker.h"
#include "adg-marker-private.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_SIZE,
    PROP_ANGLE
};


static void     get_property            (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static gboolean set_size                (AdgMarker      *marker,
                                         gdouble         size);
static gboolean set_angle               (AdgMarker      *marker,
                                         gdouble         angle);


G_DEFINE_ABSTRACT_TYPE(AdgMarker, adg_marker, ADG_TYPE_ENTITY);


static void
adg_marker_class_init(AdgMarkerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgMarkerPrivate));

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    param = g_param_spec_double("size",
                                P_("Marker Size"),
                                P_("The size (in global space) of the marker"),
                                0, G_MAXDOUBLE, 10,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = g_param_spec_double("angle",
                                P_("Marker Angle"),
                                P_("The angle of the marker, an implementation dependend value"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE, param);
}

static void
adg_marker_init(AdgMarker *marker)
{
    AdgMarkerPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(marker,
                                                         ADG_TYPE_MARKER,
                                                         AdgMarkerPrivate);
    data->size = 10;
    data->angle = 0;

    marker->data = data;
}


static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgMarkerPrivate *data = ((AdgMarker *) object)->data;

    switch (prop_id) {
    case PROP_SIZE:
        g_value_set_double(value, data->size);
        break;
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
    AdgMarker *marker = (AdgMarker *) object;

    switch (prop_id) {
    case PROP_SIZE:
        set_size(marker, g_value_get_double(value));
        break;
    case PROP_ANGLE:
        set_angle(marker, g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_marker_get_size:
 * @marker: an #AdgMarker
 *
 * Gets the current size of @marker.
 *
 * Returns: the marker size in global space
 **/
gdouble
adg_marker_get_size(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), 0);

    data = marker->data;

    return data->size;
}

/**
 * adg_marker_set_size:
 * @marker: an #AdgMarker
 * @size: the new size
 *
 * Sets a new size on @marker. The @size is an implementation-dependent
 * property: it has meaning only when used by an #AdgMarker derived type.
 **/
void
adg_marker_set_size(AdgMarker *marker, gdouble size)
{
    g_return_if_fail(ADG_IS_MARKER(marker));

    if (set_size(marker, size))
        g_object_notify((GObject *) marker, "size");
}

/**
 * adg_marker_get_angle:
 * @marker: an #AdgMarker
 *
 * Gets the current angle of @marker.
 *
 * Returns: the marker angle, in radians
 **/
gdouble
adg_marker_get_angle(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), 0);

    data = marker->data;

    return data->angle;
}

/**
 * adg_marker_set_angle:
 * @marker: an #AdgMarker
 * @angle: the new angle
 *
 * Sets a new angle on @marker. The @angle is an implementation-dependent
 * property: it has meaning only when used by an #AdgMarker derived type.
 **/
void
adg_marker_set_angle(AdgMarker *marker, gdouble angle)
{
    g_return_if_fail(ADG_IS_MARKER(marker));

    if (set_angle(marker, angle))
        g_object_notify((GObject *) marker, "angle");
}


static gboolean
set_size(AdgMarker *marker, gdouble size)
{
    AdgMarkerPrivate *data = marker->data;

    if (size == data->size)
        return FALSE;

    data->size = size;

    return TRUE;
}

static gboolean
set_angle(AdgMarker *marker, gdouble angle)
{
    AdgMarkerPrivate *data = marker->data;

    if (angle == data->angle)
        return FALSE;

    data->angle = angle;

    return TRUE;
}
