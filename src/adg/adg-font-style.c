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
 * SECTION:adg-font-style
 * @short_description: Font style related stuff
 *
 * Contains parameters on how to draw texts such as font family, slanting,
 * weight, hinting and so on.
 *
 * Since: 1.0
 */

/**
 * AdgFontStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-style.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"

#include "adg-font-style.h"
#include "adg-font-style-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_font_style_parent_class)


G_DEFINE_TYPE(AdgFontStyle, adg_font_style, ADG_TYPE_STYLE)

enum {
    PROP_0,
    PROP_COLOR_DRESS,
    PROP_FAMILY,
    PROP_SLANT,
    PROP_WEIGHT,
    PROP_SIZE,
    PROP_ANTIALIAS,
    PROP_SUBPIXEL_ORDER,
    PROP_HINT_STYLE,
    PROP_HINT_METRICS
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
adg_font_style_class_init(AdgFontStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgFontStylePrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->invalidate = _adg_invalidate;
    style_class->apply = _adg_apply;

    param = adg_param_spec_dress("color-dress",
                                 P_("Color Dress"),
                                 P_("The fallback color dress to bind to this style"),
                                 ADG_DRESS_COLOR,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COLOR_DRESS, param);

    param = g_param_spec_string("family",
                                P_("Font Family"),
                                P_("The font family name, encoded in UTF-8"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FAMILY, param);

    param = g_param_spec_int("slant",
                             P_("Font Slant"),
                             P_("Variant of a font face based on its slant"),
                             G_MININT, G_MAXINT, CAIRO_FONT_SLANT_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SLANT, param);

    param = g_param_spec_int("weight",
                             P_("Font Weight"),
                             P_("Variant of a font face based on its weight"),
                             G_MININT, G_MAXINT, CAIRO_FONT_WEIGHT_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_WEIGHT, param);

    param = g_param_spec_double("size",
                                P_("Font Size"),
                                P_("Font size in user space units"),
                                0, G_MAXDOUBLE, 10,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = g_param_spec_int("antialias",
                             P_("Font Antialiasing Mode"),
                             P_("Type of antialiasing to do when rendering text"),
                             G_MININT, G_MAXINT, CAIRO_ANTIALIAS_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANTIALIAS, param);

    param = g_param_spec_int("subpixel-order",
                             P_("Font Subpixel Order"),
                             P_("The order of color elements within each pixel on the display device when rendering with subpixel antialiasing mode"),
                             G_MININT, G_MAXINT,
                             CAIRO_SUBPIXEL_ORDER_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SUBPIXEL_ORDER, param);

    param = g_param_spec_int("hint-style",
                             P_("Type of Hinting"),
                             P_("How outlines must fit to the pixel grid in order to improve the glyph appearance"),
                             G_MININT, G_MAXINT, CAIRO_HINT_STYLE_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HINT_STYLE, param);

    param = g_param_spec_int("hint-metrics",
                             P_("Font Metric Hinting"),
                             P_("Whether to hint font metrics, that is align them to integer values in device space"),
                             G_MININT, G_MAXINT,
                             CAIRO_HINT_METRICS_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HINT_METRICS, param);
}

static void
adg_font_style_init(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(font_style,
                                                            ADG_TYPE_FONT_STYLE,
                                                            AdgFontStylePrivate);

    data->color_dress = ADG_DRESS_COLOR;
    data->family = NULL;
    data->slant = CAIRO_FONT_SLANT_NORMAL;
    data->weight = CAIRO_FONT_WEIGHT_NORMAL;
    data->size = 10;
    data->antialias = CAIRO_ANTIALIAS_DEFAULT;
    data->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
    data->hint_style = CAIRO_HINT_STYLE_DEFAULT;
    data->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;
    data->font = NULL;

    font_style->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgFontStylePrivate *data = ((AdgFontStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        g_value_set_int(value, data->color_dress);
        break;
    case PROP_FAMILY:
        g_value_set_string(value, data->family);
        break;
    case PROP_SLANT:
        g_value_set_int(value, data->slant);
        break;
    case PROP_WEIGHT:
        g_value_set_int(value, data->weight);
        break;
    case PROP_SIZE:
        g_value_set_double(value, data->size);
        break;
    case PROP_ANTIALIAS:
        g_value_set_int(value, data->antialias);
        break;
    case PROP_SUBPIXEL_ORDER:
        g_value_set_int(value, data->subpixel_order);
        break;
    case PROP_HINT_STYLE:
        g_value_set_int(value, data->hint_style);
        break;
    case PROP_HINT_METRICS:
        g_value_set_int(value, data->hint_metrics);
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
    AdgStyle *style;
    AdgFontStyle *font_style;
    AdgFontStylePrivate *data;

    style = (AdgStyle *) object;
    font_style = (AdgFontStyle *) object;
    data = font_style->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        data->color_dress = g_value_get_int(value);
        break;
    case PROP_FAMILY:
        g_free(data->family);
        data->family = g_value_dup_string(value);
        adg_style_invalidate(style);
        break;
    case PROP_SLANT:
        data->slant = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    case PROP_WEIGHT:
        data->weight = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    case PROP_SIZE:
        data->size = g_value_get_double(value);
        adg_style_invalidate(style);
        break;
    case PROP_ANTIALIAS:
        data->antialias = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    case PROP_SUBPIXEL_ORDER:
        data->subpixel_order = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    case PROP_HINT_STYLE:
        data->hint_style = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    case PROP_HINT_METRICS:
        data->hint_metrics = g_value_get_int(value);
        adg_style_invalidate(style);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_font_style_new:
 *
 * Constructs a new font style initialized with default params.
 *
 * Returns: a newly created font style
 *
 * Since: 1.0
 **/
AdgFontStyle *
adg_font_style_new(void)
{
    return g_object_new(ADG_TYPE_FONT_STYLE, NULL);
}

/**
 * adg_font_style_new_options:
 * @font_style: an #AdgFontStyle object
 *
 * Creates a new set of #cairo_font_options_t filled with the values
 * picked from @font_style. The returned value must be freed with
 * cairo_font_options_destroy().
 *
 * Returns: a newly allocated list of cairo font options
 *
 * Since: 1.0
 **/
cairo_font_options_t *
adg_font_style_new_options(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;
    cairo_font_options_t *options;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style), NULL);

    data = font_style->data;
    options = cairo_font_options_create();

    /* Check for cached font */
    if (data->font != NULL) {
        cairo_scaled_font_get_font_options(data->font, options);
    } else {
        cairo_font_options_set_antialias(options, data->antialias);
        cairo_font_options_set_subpixel_order(options, data->subpixel_order);
        cairo_font_options_set_hint_style(options, data->hint_style);
        cairo_font_options_set_hint_metrics(options, data->hint_metrics);
    }

    return options;
}

/**
 * adg_font_style_get_scaled_font:
 * @font_style: an #AdgFontStyle object
 * @ctm: the current transformation matrix
 *
 * Gets the scaled font of @font_style. The returned font is
 * owned by @font_style and must not be destroyed by the caller.
 *
 * Returns: the scaled font
 *
 * Since: 1.0
 **/
cairo_scaled_font_t *
adg_font_style_get_scaled_font(AdgFontStyle *font_style, const AdgMatrix *ctm)
{
    AdgFontStylePrivate *data;
    cairo_font_options_t *options;
    AdgMatrix matrix;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style), NULL);
    g_return_val_if_fail(ctm != NULL, NULL);

    data = font_style->data;

    /* Check for cached font */
    if (data->font != NULL) {
        AdgMatrix font_ctm;

        cairo_scaled_font_get_ctm(data->font, &font_ctm);

        /* The scaled font is valid only if the two ctm match */
        if (ctm->xx == font_ctm.xx && ctm->yy == font_ctm.yy &&
            ctm->xy == font_ctm.xy && ctm->yx == font_ctm.yx)
            return data->font;

        /* No valid cache found: rebuild the scaled font */
        adg_style_invalidate((AdgStyle *) font_style);
    }

    if (data->face == NULL) {
        const gchar *family = data->family != NULL ? data->family : "";

        data->face = cairo_toy_font_face_create(family, data->slant,
                                                data->weight);
    }

    cairo_matrix_init_scale(&matrix, data->size, data->size);
    options = adg_font_style_new_options(font_style);
    data->font = cairo_scaled_font_create(data->face, &matrix, ctm, options);
    cairo_font_options_destroy(options);

    return data->font;
}

/**
 * adg_font_style_set_color_dress:
 * @font_style: an #AdgFontStyle
 * @dress: the new color dress to use
 *
 * Sets a new color dress on @font_style. The new dress
 * should be related to the original dress: you cannot
 * set a dress used for font styles to a dress managing
 * fonts.
 *
 * The validation of the new dress is done by calling
 * adg_dress_are_related() with @dress and the previous
 * dress as arguments: check out its documentation for
 * details on what is a related dress.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_color_dress(AdgFontStyle *font_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "color-dress", dress, NULL);
}

/**
 * adg_font_style_get_color_dress:
 * @font_style: an #AdgFontStyle
 *
 * Gets the color dress used by @font_style.
 *
 * Returns: the current color dress
 *
 * Since: 1.0
 **/
AdgDress
adg_font_style_get_color_dress(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style), ADG_DRESS_UNDEFINED);

    data = font_style->data;

    return data->color_dress;
}

/**
 * adg_font_style_set_family:
 * @font_style: an #AdgFontStyle object
 * @family: the new family
 *
 * Sets a new family.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_family(AdgFontStyle *font_style, const gchar *family)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "family", family, NULL);
}

/**
 * adg_font_style_get_family:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the family of @font_style. The returned pointer refers to
 * internally managed text that must not be modified or freed.
 *
 * Returns: the requested family
 *
 * Since: 1.0
 **/
const gchar *
adg_font_style_get_family(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style), NULL);

    data = font_style->data;

    return data->family;
}

/**
 * adg_font_style_set_slant:
 * @font_style: an #AdgFontStyle object
 * @slant: the new slant
 *
 * Sets a new slant variant on @font_style.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_slant(AdgFontStyle *font_style,
                         cairo_font_slant_t slant)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "slant", slant, NULL);
}

/**
 * adg_font_style_get_slant:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the slant variant of @font_style.
 *
 * Returns: the slant variant
 *
 * Since: 1.0
 **/
cairo_font_slant_t
adg_font_style_get_slant(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_FONT_SLANT_NORMAL);

    data = font_style->data;

    return data->slant;
}

/**
 * adg_font_style_set_weight:
 * @font_style: an #AdgFontStyle object
 * @weight: the new weight
 *
 * Sets a new weight variant on @font_style.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_weight(AdgFontStyle *font_style,
                          cairo_font_weight_t weight)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "weight", weight, NULL);
}

/**
 * adg_font_style_get_weight:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the weight variant of @font_style.
 *
 * Returns: the weight variant
 *
 * Since: 1.0
 **/
cairo_font_weight_t
adg_font_style_get_weight(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_FONT_WEIGHT_NORMAL);

    data = font_style->data;

    return data->weight;
}

/**
 * adg_font_style_set_size:
 * @font_style: an #AdgFontStyle object
 * @size: the new size
 *
 * Sets a new size (in global space) on @font_style.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_size(AdgFontStyle *font_style, gdouble size)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "size", size, NULL);
}

/**
 * adg_font_style_get_size:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the size (in global space) of @font_style.
 *
 * Returns: the size variant
 *
 * Since: 1.0
 **/
gdouble
adg_font_style_get_size(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style), 0.);

    data = font_style->data;

    return data->size;
}

/**
 * adg_font_style_set_antialias:
 * @font_style: an #AdgFontStyle object
 * @antialias: the new antialias mode
 *
 * Sets a new antialias mode.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_antialias(AdgFontStyle *font_style,
                             cairo_antialias_t antialias)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "antialias", antialias, NULL);
}

/**
 * adg_font_style_get_antialias:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the antialias mode used.
 *
 * Returns: the requested antialias mode
 *
 * Since: 1.0
 **/
cairo_antialias_t
adg_font_style_get_antialias(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_ANTIALIAS_DEFAULT);

    data = font_style->data;

    return data->antialias;
}

/**
 * adg_font_style_set_subpixel_order:
 * @font_style: an #AdgFontStyle object
 * @subpixel_order: the new subpixel order mode
 *
 * Sets a new subpixel order mode.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_subpixel_order(AdgFontStyle *font_style,
                                  cairo_subpixel_order_t subpixel_order)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "subpixel-order", subpixel_order, NULL);
}

/**
 * adg_font_style_get_subpixel_order:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the subpixel order mode used, that is the order of color elements
 * within each pixel on the display device when rendering with an
 * antialiasing mode of %CAIRO_ANTIALIAS_SUBPIXEL.
 *
 * Returns: the requested subpixel order mode
 *
 * Since: 1.0
 **/
cairo_subpixel_order_t
adg_font_style_get_subpixel_order(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_SUBPIXEL_ORDER_DEFAULT);

    data = font_style->data;

    return data->subpixel_order;
}

/**
 * adg_font_style_set_hint_style:
 * @font_style: an #AdgFontStyle object
 * @hint_style: the new hint style mode
 *
 * Sets a new hint style mode.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_hint_style(AdgFontStyle *font_style,
                              cairo_hint_style_t hint_style)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "hint-style", hint_style, NULL);
}

/**
 * adg_font_style_get_hint_style:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the hint style mode used, that is how to fit outlines
 * to the pixel grid in order to improve the appearance of the result.
 *
 * Returns: the requested hint style mode
 *
 * Since: 1.0
 **/
cairo_hint_style_t
adg_font_style_get_hint_style(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_HINT_STYLE_DEFAULT);

    data = font_style->data;

    return data->hint_style;
}

/**
 * adg_font_style_set_hint_metrics:
 * @font_style: an #AdgFontStyle object
 * @hint_metrics: the new hint metrics state
 *
 * Sets a new hint metrics state.
 *
 * Since: 1.0
 **/
void
adg_font_style_set_hint_metrics(AdgFontStyle *font_style,
                                cairo_hint_metrics_t hint_metrics)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));
    g_object_set(font_style, "hint-metrics", hint_metrics, NULL);
}

/**
 * adg_font_style_get_hint_metrics:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the state on whether to hint font metrics.
 *
 * Returns: the requested hint metrics state
 *
 * Since: 1.0
 **/
cairo_hint_metrics_t
adg_font_style_get_hint_metrics(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FONT_STYLE(font_style),
                         CAIRO_HINT_METRICS_DEFAULT);

    data = font_style->data;

    return data->hint_metrics;
}


static void
_adg_invalidate(AdgStyle *style)
{
    AdgFontStyle *font_style;
    AdgFontStylePrivate *data;

    font_style = (AdgFontStyle *) style;
    data = font_style->data;

    if (data->font != NULL) {
        cairo_scaled_font_destroy(data->font);
        data->font = NULL;
    }

    if (data->face == NULL) {
        cairo_font_face_destroy(data->face);
        data->face = NULL;
    }
}

static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgFontStyle *font_style;
    AdgFontStylePrivate *data;
    AdgMatrix ctm;
    cairo_scaled_font_t *font;

    font_style = (AdgFontStyle *) style;
    data = font_style->data;

    adg_entity_apply_dress(entity, data->color_dress, cr);

    cairo_get_matrix(cr, &ctm);
    font = adg_font_style_get_scaled_font((AdgFontStyle *) style, &ctm);

    cairo_set_scaled_font(cr, font);
}
