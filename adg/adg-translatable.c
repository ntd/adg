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
 * SECTION:adg-translatable
 * @title: AdgTranslatable
 * @short_description: Interface for translatable entities
 *
 * The #AdgTranslatableIface interface gives a common way to manage entities
 * with an origin point.
 **/

/**
 * AdgTranslatableIface:
 * @base_iface: the base interface
 * @get_origin: returns the current origin
 * @set_origin: sets a new origin
 *
 * The virtual methods @get_origin and @set_origin must be defined
 * by all the types which implement this interface.
 **/

#include "adg-translatable.h"
#include "adg-intl.h"


enum {
    ORIGIN_MOVED,
    LAST_SIGNAL
};

static void     iface_base      (AdgTranslatableIface   *iface);
static void     iface_init      (AdgTranslatableIface   *iface);
static void     get_origin    	(AdgTranslatable        *translatable,
                                 AdgPoint               *dest);
static void     set_origin      (AdgTranslatable        *translatable,
                                 const AdgPoint         *origin);

static guint    signals[LAST_SIGNAL] = { 0 };


GType
adg_translatable_get_type(void)
{
    static GType translatable_type = 0;

    if (G_UNLIKELY(translatable_type == 0)) {
        static const GTypeInfo translatable_info = {
            sizeof(AdgTranslatableIface),
            (GBaseInitFunc) iface_base,
            NULL,
            (GClassInitFunc) iface_init,
            NULL,
        };

        translatable_type = g_type_register_static(G_TYPE_INTERFACE,
                                                   "AdgTranslatable",
                                                   &translatable_info, 0);
    }

    return translatable_type;
}

static void
iface_base(AdgTranslatableIface *iface)
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
     * AdgTranslatable::origin-moved:
     * @translatable: an entity implementing #AdgTranslatable
     * @old_origin: an #AdgPoint with the previous origin
     *
     * Emitted whenever the origin has changed.
     **/
    param_types[0] = ADG_TYPE_POINT;
    signals[ORIGIN_MOVED] = g_signal_newv("origin-moved", ADG_TYPE_TRANSLATABLE,
                                          G_SIGNAL_RUN_FIRST,
                                          NULL, NULL, NULL,
                                          g_cclosure_marshal_VOID__OBJECT,
                                          G_TYPE_NONE, 1, param_types);
}

static void
iface_init (AdgTranslatableIface *iface)
{
    iface->get_origin = get_origin;
    iface->set_origin = set_origin;
}


static void
get_origin(AdgTranslatable *translatable, AdgPoint *dest)
{
    g_warning("AdgTranslatable::get_origin not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(translatable)));
}

static void
set_origin(AdgTranslatable *translatable, const AdgPoint *point)
{
    g_warning("AdgTranslatable::set_origin not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(translatable)));
}

/**
 * adg_translatable_get_origin:
 * @translatable: an entity implementing AdgTranslatable
 * @dest: the destination #AdgPoint struct
 *
 * Gets the origin point of @translatable.
 **/
void
adg_translatable_get_origin(AdgTranslatable *translatable, AdgPoint *dest)
{
    g_return_if_fail(ADG_IS_TRANSLATABLE(translatable));
    g_return_if_fail(dest != NULL);

    ADG_TRANSLATABLE_GET_IFACE(translatable)->get_origin(translatable, dest);
}

/**
 * adg_translatable_set_origin:
 * @translatable: an entity implementing AdgTranslatable
 * @origin: the new origin
 *
 * Sets the origin of @translatable to @origin.
 * An "origin-moved" signal is emitted.
 **/
void
adg_translatable_set_origin(AdgTranslatable *translatable,
                            const AdgPoint *origin)
{
    AdgTranslatableIface *iface;
    AdgPoint old_origin;

    g_return_if_fail(ADG_IS_TRANSLATABLE(translatable));
    g_return_if_fail(origin != NULL);

    iface = ADG_TRANSLATABLE_GET_IFACE(translatable);

    iface->get_origin(translatable, &old_origin);
    iface->set_origin(translatable, origin);

    g_signal_emit(translatable, signals[ORIGIN_MOVED], 0, &old_origin);
}

/**
 * adg_translatable_set_origin_explicit:
 * @translatable: an entity implementing AdgTranslatable
 * @model_x: the new x position in model space
 * @model_y: the new y position in model space
 * @paper_x: the new x position in paper space
 * @paper_y: the new y position in paper space
 *
 * Sets the origin of @translatable to the new coordinates. It calls
 * adg_translatable_set_origin() internally.
 **/
void
adg_translatable_set_origin_explicit(AdgTranslatable *translatable,
                                     gdouble model_x, gdouble model_y,
                                     gdouble paper_x, gdouble paper_y)
{
    AdgPoint origin;

    origin.model.x = model_x;
    origin.model.y = model_y;
    origin.paper.x = paper_x;
    origin.paper.y = paper_y;

    adg_translatable_set_origin(translatable, &origin);
}
