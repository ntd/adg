/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * @base_iface:		the base interface
 * @org:		the origin point accessor
 *
 * The virtual methods @org must be defined by all the types which
 * implement this interface.
 **/

#include "adg-positionable.h"
#include "adg-intl.h"


enum {
    ORG_MOVED,
    LAST_SIGNAL
};

static void      iface_base	(AdgPositionableIface   *iface);
static void      iface_init	(AdgPositionableIface   *iface);
static AdgPoint *org    	(AdgPositionable        *positionable);


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

    param = g_param_spec_object("org",
                                P_("Org"),
                                P_("Origin point"),
                                ADG_TYPE_POINT,
                                G_PARAM_READWRITE);
    g_object_interface_install_property(iface, param);

    /**
     * AdgPositionable::org-moved:
     * @positionable: an entity implementing #AdgPositionable
     *
     * Emitted whenever the origin has changed.
     **/
    param_types[0] = ADG_TYPE_POINT;
    signals[ORG_MOVED] = g_signal_newv("org-moved", ADG_TYPE_POSITIONABLE,
                                       G_SIGNAL_RUN_FIRST,
                                       NULL, NULL, NULL,
                                       g_cclosure_marshal_VOID__OBJECT,
                                       G_TYPE_NONE, 1, param_types);
}

static void
iface_init (AdgPositionableIface *iface)
{
    iface->org = org;
}


static AdgPoint *
org(AdgPositionable *positionable)
{
    g_warning("AdgPositionable::org not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(positionable)));
    return NULL;
}

/**
 * adg_positionable_get_org:
 * @positionable: an entity implementing AdgPositionable
 *
 * Gets the origin point of @positionable.
 *
 * Return value: A pointer to the internal origin point
 **/
const AdgPoint *
adg_positionable_get_org(AdgPositionable *positionable)
{
    g_return_val_if_fail(ADG_IS_POSITIONABLE(positionable), NULL);

    return ADG_POSITIONABLE_GET_IFACE(positionable)->org(positionable);
}

/**
 * adg_positionable_set_org:
 * @positionable: an entity implementing AdgPositionable
 * @org: the new origin
 *
 * Sets the origin of @positionable to @org. An "org-moved" signal is emitted.
 **/
void
adg_positionable_set_org(AdgPositionable *positionable, const AdgPoint *org)
{
    AdgPoint *current_org;
    AdgPoint old_org;

    g_return_if_fail(ADG_IS_POSITIONABLE(positionable));

    current_org = ADG_POSITIONABLE_GET_IFACE(positionable)->org(positionable);

    adg_point_copy(&old_org, current_org);
    adg_point_copy(current_org, org);
    g_signal_emit(positionable, signals[ORG_MOVED], 0, &old_org);
}

/**
 * adg_positionable_set_org_explicit:
 * @positionable: an entity implementing AdgPositionable
 * @model_x: the new x position in model space
 * @model_y: the new y position in model space
 * @paper_x: the new x position in paper space
 * @paper_y: the new y position in paper space
 *
 * Sets the origin of @positionable to the new coordinates. It calls
 * adg_positionable_set_org() internally.
 **/
void
adg_positionable_set_org_explicit(AdgPositionable *positionable,
                                  gdouble model_x, gdouble model_y,
                                  gdouble paper_x, gdouble paper_y)
{
    AdgPoint org;

    org.model.x = model_x;
    org.model.y = model_y;
    org.paper.x = paper_x;
    org.paper.y = paper_y;

    adg_positionable_set_org(positionable, &org);
}
