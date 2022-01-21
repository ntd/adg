/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-pango-style
 * @short_description: A font style with pango support
 *
 * Adds pango support to the #AdgFontStyle class.
 *
 * Since: 1.0
 */

/**
 * AdgPangoStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <pango/pango.h>

#include "adg-style.h"
#include "adg-dress.h"
#include "adg-font-style.h"

#include "adg-pango-style.h"
#include "adg-pango-style-private.h"


#define _ADG_OLD_STYLE_CLASS  ((AdgStyleClass *) adg_pango_style_parent_class)


G_DEFINE_TYPE_WITH_PRIVATE(AdgPangoStyle, adg_pango_style, ADG_TYPE_FONT_STYLE)

enum {
    PROP_0,
    PROP_SPACING
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_invalidate         (AdgStyle       *style);
static void             _adg_apply              (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);


static void
adg_pango_style_class_init(AdgPangoStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->invalidate = _adg_invalidate;
    style_class->apply = _adg_apply;

    param = g_param_spec_int("spacing",
                             P_("Spacing"),
                             P_("Amount of spacing between lines on multiline text"),
                             G_MININT, G_MAXINT, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SPACING, param);
}

static void
adg_pango_style_init(AdgPangoStyle *pango_style)
{
    AdgPangoStylePrivate *data = adg_pango_style_get_instance_private(pango_style);
    data->font_description = NULL;
    data->spacing = 0;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgPangoStylePrivate *data = adg_pango_style_get_instance_private((AdgPangoStyle *) object);

    switch (prop_id) {
    case PROP_SPACING:
        g_value_set_int(value, data->spacing);
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
    AdgPangoStylePrivate *data = adg_pango_style_get_instance_private((AdgPangoStyle *) object);

    switch (prop_id) {
    case PROP_SPACING:
        data->spacing = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_pango_style_new:
 *
 * Constructs a new pango style initialized with default params.
 *
 * Returns: a newly created pango style
 *
 * Since: 1.0
 **/
AdgPangoStyle *
adg_pango_style_new(void)
{
    return g_object_new(ADG_TYPE_PANGO_STYLE, NULL);
}

/**
 * adg_pango_style_get_description:
 * @pango_style: an #AdgPangoStyle object
 *
 * Gets the #PangoFontDescription of @pango_style. The returned font is
 * owned by @pango_style and must not be destroyed by the caller.
 *
 * Returns: the font description
 *
 * Since: 1.0
 **/
PangoFontDescription *
adg_pango_style_get_description(AdgPangoStyle *pango_style)
{
    AdgPangoStylePrivate *data;

    g_return_val_if_fail(ADG_IS_PANGO_STYLE(pango_style), NULL);

    data = adg_pango_style_get_instance_private(pango_style);

    if (data->font_description == NULL) {
        AdgFontStyle *font_style;
        const gchar *family;
        cairo_font_slant_t slant;
        cairo_font_weight_t weight;
        gdouble size;

        font_style = (AdgFontStyle *) pango_style;
        family = adg_font_style_get_family(font_style);
        slant = adg_font_style_get_slant(font_style);
        weight = adg_font_style_get_weight(font_style);
        size = adg_font_style_get_size(font_style);

        data->font_description = pango_font_description_new();

        pango_font_description_set_family(data->font_description, family);

        switch (slant) {
        case CAIRO_FONT_SLANT_NORMAL:
            pango_font_description_set_style(data->font_description,
                                             PANGO_STYLE_NORMAL);
            break;
        case CAIRO_FONT_SLANT_ITALIC:
            pango_font_description_set_style(data->font_description,
                                             PANGO_STYLE_ITALIC);
            break;
        case CAIRO_FONT_SLANT_OBLIQUE:
            pango_font_description_set_style(data->font_description,
                                             PANGO_STYLE_OBLIQUE);
            break;
        default:
            g_warning(_("Unhandled slant value (%d)"), slant);
        }

        switch (weight) {
        case CAIRO_FONT_WEIGHT_NORMAL:
            pango_font_description_set_weight(data->font_description,
                                              PANGO_WEIGHT_NORMAL);
            break;
        case CAIRO_FONT_WEIGHT_BOLD:
            pango_font_description_set_weight(data->font_description,
                                              PANGO_WEIGHT_BOLD);
            break;
        default:
            g_warning(_("Unhandled weight value (%d)"), weight);
        }

        pango_font_description_set_size(data->font_description,
                                        size * PANGO_SCALE);
    }

    return data->font_description;
}

/**
 * adg_pango_style_set_spacing:
 * @pango_style: an #AdgPangoStyle object
 * @spacing: the new spacing
 *
 * Sets the new spacing to @spacing on @pango_style.
 *
 * Since: 1.0
 **/
void
adg_pango_style_set_spacing(AdgPangoStyle *pango_style, gint spacing)
{
    g_return_if_fail(ADG_IS_PANGO_STYLE(pango_style));
    g_object_set(pango_style, "spacing", spacing, NULL);
}

/**
 * adg_pango_style_get_spacing:
 * @pango_style: an #AdgPangoStyle object
 *
 * Gets the spacing of @pango_style.
 *
 * Returns: (type gint): the current spacing value.
 *
 * Since: 1.0
 **/
gint
adg_pango_style_get_spacing(AdgPangoStyle *pango_style)
{
    AdgPangoStylePrivate *data;

    g_return_val_if_fail(ADG_IS_PANGO_STYLE(pango_style), 0);

    data = adg_pango_style_get_instance_private(pango_style);
    return data->spacing;
}


static void
_adg_invalidate(AdgStyle *style)
{
    AdgPangoStyle *pango_style = (AdgPangoStyle *) style;
    AdgPangoStylePrivate *data = adg_pango_style_get_instance_private(pango_style);

    if (data->font_description != NULL) {
        pango_font_description_free(data->font_description);
        data->font_description = NULL;
    }

    if (_ADG_OLD_STYLE_CLASS->invalidate != NULL)
        _ADG_OLD_STYLE_CLASS->invalidate(style);
}

static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgFontStyle *font_style;
    AdgDress color_dress;

    font_style = (AdgFontStyle *) style;
    color_dress = adg_font_style_get_color_dress(font_style);

    adg_entity_apply_dress(entity, color_dress, cr);
}
