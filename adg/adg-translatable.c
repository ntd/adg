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
 * SECTION:positionable
 * @title: AdgPositionable
 * @short_description: Interface for positionable entities
 *
 * The #AdgPositionableIface interface gives a common way to manage entities
 * with an origin point.
 **/

/**
 * AdgPositionableIface:
 * @base_iface: the base interface
 * @get_origin: returns the current origin
 * @set_origin: sets a new origin
 *
 * The virtual methods @get_origin and @set_origin must be defined
 * by all the types which implement this interface.
 **/

#include "adg-positionable.h"
#include "adg-intl.h"


enum {
    ORIGIN_MOVED,
    LAST_SIGNAL
};

static void     iface_base      (AdgPositionableIface   *iface);
static void     iface_init      (AdgPositionableIface   *iface);
static void     get_origin    	(AdgPositionable        *positionable,
                                 AdgPoint               *dest);
static void     set_origin      (AdgPositionable        *positionable,
                                 const AdgPoint         *origin);

static guint    signals[LAST_SIGNAL] = { 0 };


GType
adg_positionable_get_type(void)
{
    static GType positionable_type = 0;

    if (G_UNLIKELY(positionable_type == 0)) {
        static const GTypeInfo positionable_info = {
            sizeof(AdgPositionableIface),
            (GBaseInitFunc) iface_base,
            NULL,
            (GClassInitFunc) iface_init,
            NULL,
        };

        positionable_type = g_type_register_static(G_TYPE_INTERFACE,
                                                   "AdgPositionable",
                                                   &positionable_info, 0);
    }

    return positionable_type;
}

static void
iface_base(AdgPositionableIface *iface)
{
    static gboolean initialized = FALSE;
    GParamSpec *param;
    GType param_types[1];

    if (initialized) {
        return;
    }
    initialized = TRUE;

    param = g_param_spec_object("origin",
                                P_("Origin"),
                                P_("Origin point"),
                                ADG_TYPE_POINT,
                                G_PARAM_READWRITE);
    g_object_interface_install_property(iface, param);

    /**
     * AdgPositionable::origin-moved:
     * @positionable: an entity implementing #AdgPositionable
     * @old_origin: an #AdgPoint with the previous origin
     *
     * Emitted whenever the origin has changed.
     **/
    param_types[0] = ADG_TYPE_POINT;
    signals[ORIGIN_MOVED] = g_signal_newv("origin-moved", ADG_TYPE_POSITIONABLE,
                                          G_SIGNAL_RUN_FIRST,
                                          NULL, NULL, NULL,
                                          g_cclosure_marshal_VOID__OBJECT,
                                          G_TYPE_NONE, 1, param_types);
}

static void
iface_init (AdgPositionableIface *iface)
{
    iface->get_origin = get_origin;
    iface->set_origin = set_origin;
}


static void
get_origin(AdgPositionable *positionable, AdgPoint *dest)
{
    g_warning("AdgPositionable::get_origin not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(positionable)));
}

static void
set_origin(AdgPositionable *positionable, const AdgPoint *point)
{
    g_warning("AdgPositionable::set_origin not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(positionable)));
}

/**
 * adg_positionable_get_origin:
 * @positionable: an entity implementing AdgPositionable
 * @dest: the destination #AdgPoint struct
 *
 * Gets the origin point of @positionable.
 **/
void
adg_positionable_get_origin(AdgPositionable *positionable, AdgPoint *dest)
{
    g_return_if_fail(ADG_IS_POSITIONABLE(positionable));
    g_return_if_fail(dest != NULL);

    ADG_POSITIONABLE_GET_IFACE(positionable)->get_origin(positionable, dest);
}

/**
 * adg_positionable_set_origin:
 * @positionable: an entity implementing AdgPositionable
 * @origin: the new origin
 *
 * Sets the origin of @positionable to @origin.
 * An "origin-moved" signal is emitted.
 **/
void
adg_positionable_set_origin(AdgPositionable *positionable,
                            const AdgPoint *origin)
{
    AdgPositionableIface *iface;
    AdgPoint old_origin;

    g_return_if_fail(ADG_IS_POSITIONABLE(positionable));
    g_return_if_fail(origin != NULL);

    iface = ADG_POSITIONABLE_GET_IFACE(positionable);

    iface->get_origin(positionable, &old_origin);
    iface->set_origin(positionable, origin);

    g_signal_emit(positionable, signals[ORIGIN_MOVED], 0, &old_origin);
}

/**
 * adg_positionable_set_origin_explicit:
 * @positionable: an entity implementing AdgPositionable
 * @model_x: the new x position in model space
 * @model_y: the new y position in model space
 * @paper_x: the new x position in paper space
 * @paper_y: the new y position in paper space
 *
 * Sets the origin of @positionable to the new coordinates. It calls
 * adg_positionable_set_origin() internally.
 **/
void
adg_positionable_set_origin_explicit(AdgPositionable *positionable,
                                     gdouble model_x, gdouble model_y,
                                     gdouble paper_x, gdouble paper_y)
{
    AdgPoint origin;

    origin.model.x = model_x;
    origin.model.y = model_y;
    origin.paper.x = paper_x;
    origin.paper.y = paper_y;

    adg_positionable_set_origin(positionable, &origin);
}
