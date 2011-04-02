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
#include "adg-dress-builtins.h"
#include "adg-font-style.h"

#include "adg-pango-style.h"
#include "adg-pango-style-private.h"


#define _ADG_OLD_STYLE_CLASS  ((AdgStyleClass *) adg_pango_style_parent_class)


G_DEFINE_TYPE(AdgPangoStyle, adg_pango_style, ADG_TYPE_FONT_STYLE)


static void             _adg_invalidate         (AdgStyle       *style);
static void             _adg_apply              (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);


static void
adg_pango_style_class_init(AdgPangoStyleClass *klass)
{
    AdgStyleClass *style_class;

    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPangoStylePrivate));

    style_class->invalidate = _adg_invalidate;
    style_class->apply = _adg_apply;
}

static void
adg_pango_style_init(AdgPangoStyle *pango_style)
{
    AdgPangoStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(pango_style,
                                                             ADG_TYPE_PANGO_STYLE,
                                                             AdgPangoStylePrivate);

    data->font_description = NULL;

    pango_style->data = data;
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

    data = pango_style->data;

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


static void
_adg_invalidate(AdgStyle *style)
{
    AdgPangoStyle *pango_style;
    AdgPangoStylePrivate *data;

    pango_style = (AdgPangoStyle *) style;
    data = pango_style->data;

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
