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
 * SECTION:rotable
 * @title: AdgRotable
 * @short_description: Interface for rotable entities
 *
 * The #AdgRotableIface interface gives a common way to manage entities
 * that needs a custom rotation on the origin point.
 **/

/**
 * AdgRotableIface:
 * @base_iface: the base interface
 * @get_angle: returns the current angle
 * @set_angle: sets a new angle
 *
 * The virtual methods @get_angle and @set_angle must be defined
 * by all the types which implement this interface.
 **/

#include "adg-rotable.h"
#include "adg-intl.h"


enum {
    ANGLE_CHANGED,
    LAST_SIGNAL
};

static void     iface_base      (AdgRotableIface*iface);
static void     iface_init      (AdgRotableIface*iface);
static gdouble  get_angle       (AdgRotable     *rotable);
static void     set_angle       (AdgRotable     *rotable,
                                 gdouble         angle);

static guint    signals[LAST_SIGNAL] = { 0 };


GType
adg_rotable_get_type(void)
{
    static GType rotable_type = 0;

    if (G_UNLIKELY(rotable_type == 0)) {
        static const GTypeInfo rotable_info = {
            sizeof(AdgRotableIface),
            (GBaseInitFunc) iface_base,
            NULL,
            (GClassInitFunc) iface_init,
            NULL,
        };

        rotable_type = g_type_register_static(G_TYPE_INTERFACE,
                                                   "AdgRotable",
                                                   &rotable_info, 0);
    }

    return rotable_type;
}

static void
iface_base(AdgRotableIface *iface)
{
    static gboolean initialized = FALSE;
    GParamSpec *param;
    GType param_types[1];

    if (initialized) {
        return;
    }
    initialized = TRUE;

    param = g_param_spec_object("angle",
                                P_("Rotation Angle"),
                                P_("The angle (in degree) the entity should rotate around the origin point"),
                                G_TYPE_DOUBLE,
                                G_PARAM_READWRITE);
    g_object_interface_install_property(iface, param);

    /**
     * AdgRotable::angle-changed:
     * @rotable: an entity implementing #AdgRotable
     * @old_angle: the previous rotation angle
     *
     * Emitted whenever the angle has changed.
     **/
    param_types[0] = G_TYPE_DOUBLE;
    signals[ANGLE_CHANGED] = g_signal_newv("angle-changed", ADG_TYPE_ROTABLE,
                                           G_SIGNAL_RUN_FIRST,
                                           NULL, NULL, NULL,
                                           g_cclosure_marshal_VOID__DOUBLE,
                                           G_TYPE_NONE, 1, param_types);
}

static void
iface_init (AdgRotableIface *iface)
{
    iface->get_angle = get_angle;
    iface->set_angle = set_angle;
}


static gdouble
get_angle(AdgRotable *rotable)
{
    g_warning("AdgRotable::get_angle not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(rotable)));
    return 0.;
}

static void
set_angle(AdgRotable *rotable, gdouble angle)
{
    g_warning("AdgRotable::set_angle not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(rotable)));
}

/**
 * adg_rotable_get_angle:
 * @rotable: an entity implementing AdgRotable
 *
 * Gets the rotation angle of @rotable.
 *
 * Return value: The rotation angle (in degree)
 **/
gdouble
adg_rotable_get_angle(AdgRotable *rotable)
{
    g_return_if_fail(ADG_IS_ROTABLE(rotable));

    return ADG_ROTABLE_GET_IFACE(rotable)->get_angle(rotable);
}

/**
 * adg_rotable_set_angle:
 * @rotable: an entity implementing AdgRotable
 * @angle: the new angle
 *
 * Sets the rotation angle of @rotable to @angle.
 * An "angle-changed" signal is emitted.
 **/
void
adg_rotable_set_angle(AdgRotable *rotable, gdouble angle)
{
    AdgRotableIface *iface;
    gdouble old_angle;

    g_return_if_fail(ADG_IS_ROTABLE(rotable));

    iface = ADG_ROTABLE_GET_IFACE(rotable);

    old_angle = iface->get_angle(rotable);
    iface->set_angle(rotable, angle);

    g_signal_emit(rotable, signals[ANGLE_CHANGED], 0, &old_angle);
}
