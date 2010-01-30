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
 * SECTION:adg-font-style
 * @short_description: Font style related stuff
 *
 * Contains parameters on how to draw texts such as font family, slanting,
 * weight, hinting and so on.
 */

/**
 * AdgFontStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-font-style.h"
#include "adg-font-style-private.h"
#include "adg-dress-builtins.h"
#include "adg-util.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_font_style_parent_class)


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


static void             dispose                 (GObject        *object);
static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             apply                   (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);
static gboolean         set_family              (AdgFontStyle   *font_style,
                                                 const gchar    *family);
static gboolean         set_slant               (AdgFontStyle   *font_style,
                                                 cairo_font_slant_t slant);
static gboolean         set_weight              (AdgFontStyle   *font_style,
                                                 cairo_font_weight_t weight);
static gboolean         set_size                (AdgFontStyle   *font_style,
                                                 gdouble         size);
static gboolean         set_antialias           (AdgFontStyle   *font_style,
                                                 cairo_antialias_t antialias);
static gboolean         set_subpixel_order      (AdgFontStyle   *font_style,
                                                 cairo_subpixel_order_t subpixel_order);
static gboolean         set_hint_style          (AdgFontStyle   *font_style,
                                                 cairo_hint_style_t hint_style);
static gboolean         set_hint_metrics        (AdgFontStyle   *font_style,
                                                 cairo_hint_metrics_t hint_metrics);
static void             unset_face              (AdgFontStyle   *font_style);
static void             unset_font              (AdgFontStyle   *font_style);


G_DEFINE_TYPE(AdgFontStyle, adg_font_style, ADG_TYPE_STYLE);


static void
adg_font_style_class_init(AdgFontStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgFontStylePrivate));

    gobject_class->dispose = dispose;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    param = adg_param_spec_dress("color-dress",
                                 P_("Color Dress"),
                                 P_("The color dress to bind to this font style"),
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
dispose(GObject *object)
{
    AdgFontStyle *font_style = (AdgFontStyle *) object;

    unset_font(font_style);
    unset_face(font_style);

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
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
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgFontStyle *font_style;
    AdgFontStylePrivate *data;

    font_style = (AdgFontStyle *) object;
    data = font_style->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        adg_dress_set(&data->color_dress, g_value_get_int(value));
        break;
    case PROP_FAMILY:
        set_family(font_style, g_value_get_string(value));
        break;
    case PROP_SLANT:
        set_slant(font_style, g_value_get_int(value));
        break;
    case PROP_WEIGHT:
        set_weight(font_style, g_value_get_int(value));
        break;
    case PROP_SIZE:
        set_size(font_style, g_value_get_double(value));
        break;
    case PROP_ANTIALIAS:
        set_antialias(font_style, g_value_get_int(value));
        break;
    case PROP_SUBPIXEL_ORDER:
        set_subpixel_order(font_style, g_value_get_int(value));
        break;
    case PROP_HINT_STYLE:
        set_hint_style(font_style, g_value_get_int(value));
        break;
    case PROP_HINT_METRICS:
        set_hint_metrics(font_style, g_value_get_int(value));
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
 * Returns: a new font style
 **/
AdgStyle *
adg_font_style_new(void)
{
    return g_object_new(ADG_TYPE_FONT_STYLE, NULL);
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
        unset_font(font_style);
    }

    if (data->face == NULL) {
        const gchar *family = data->family != NULL ? data->family : "";

        data->face = cairo_toy_font_face_create(family, data->slant,
                                                data->weight);
    }

    cairo_matrix_init_scale(&matrix, data->size, data->size);
    options = cairo_font_options_create();

    cairo_font_options_set_antialias(options, data->antialias);
    cairo_font_options_set_subpixel_order(options, data->subpixel_order);
    cairo_font_options_set_hint_style(options, data->hint_style);
    cairo_font_options_set_hint_metrics(options, data->hint_metrics);

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
 **/
void
adg_font_style_set_color_dress(AdgFontStyle *font_style, AdgDress dress)
{
    AdgFontStylePrivate *data;

    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    data = font_style->data;

    if (adg_dress_set(&data->color_dress, dress))
        g_object_notify((GObject *) font_style, "color-dress");
}

/**
 * adg_font_style_get_color_dress:
 * @font_style: an #AdgFontStyle
 *
 * Gets the color dress used by @font_style.
 *
 * Returns: the current color dress
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
 **/
void
adg_font_style_set_family(AdgFontStyle *font_style, const gchar *family)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_family(font_style, family))
        g_object_notify((GObject *) font_style, "family");
}

/**
 * adg_font_style_get_family:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the family of @font_style. The returned pointer refers to
 * internally managed text that must not be modified or freed.
 *
 * Returns: the requested family
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
 **/
void
adg_font_style_set_slant(AdgFontStyle *font_style,
                         cairo_font_slant_t slant)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_slant(font_style, slant))
        g_object_notify((GObject *) font_style, "slant");
}

/**
 * adg_font_style_get_slant:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the slant variant of @font_style.
 *
 * Returns: the slant variant
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
 **/
void
adg_font_style_set_weight(AdgFontStyle *font_style,
                          cairo_font_weight_t weight)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_weight(font_style, weight))
        g_object_notify((GObject *) font_style, "weight");
}

/**
 * adg_font_style_get_weight:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the weight variant of @font_style.
 *
 * Returns: the weight variant
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
 **/
void
adg_font_style_set_size(AdgFontStyle *font_style, gdouble size)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_size(font_style, size))
        g_object_notify((GObject *) font_style, "size");
}

/**
 * adg_font_style_get_size:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the size (in global space) of @font_style.
 *
 * Returns: the size variant
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
 **/
void
adg_font_style_set_antialias(AdgFontStyle *font_style,
                             cairo_antialias_t antialias)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_antialias(font_style, antialias))
        g_object_notify((GObject *) font_style, "antialias");
}

/**
 * adg_font_style_get_antialias:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the antialias mode used.
 *
 * Returns: the requested antialias mode
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
 **/
void
adg_font_style_set_subpixel_order(AdgFontStyle *font_style,
                                  cairo_subpixel_order_t subpixel_order)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_subpixel_order(font_style, subpixel_order))
        g_object_notify((GObject *) font_style, "subpixel-order");
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
 **/
void
adg_font_style_set_hint_style(AdgFontStyle *font_style,
                              cairo_hint_style_t hint_style)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_hint_style(font_style, hint_style))
        g_object_notify((GObject *) font_style, "hint-style");
}

/**
 * adg_font_style_get_hint_style:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the hint style mode used, that is how to fit outlines
 * to the pixel grid in order to improve the appearance of the result.
 *
 * Returns: the requested hint style mode
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
 **/
void
adg_font_style_set_hint_metrics(AdgFontStyle *font_style,
                                cairo_hint_metrics_t hint_metrics)
{
    g_return_if_fail(ADG_IS_FONT_STYLE(font_style));

    if (set_hint_metrics(font_style, hint_metrics))
        g_object_notify((GObject *) font_style, "hint-metrics");
}

/**
 * adg_font_style_get_hint_metrics:
 * @font_style: an #AdgFontStyle object
 *
 * Gets the state on whether to hint font metrics.
 *
 * Returns: the requested hint metrics state
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
apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
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

static gboolean
set_family(AdgFontStyle *font_style, const gchar *family)
{
    AdgFontStylePrivate *data = font_style->data;

    if (adg_strcmp(family, data->family) == 0)
        return FALSE;

    g_free(data->family);
    data->family = g_strdup(family);
    unset_face(font_style);

    return TRUE;
}

static gboolean
set_slant(AdgFontStyle *font_style, cairo_font_slant_t slant)
{
    AdgFontStylePrivate *data = font_style->data;

    if (slant == data->slant)
        return FALSE;

    data->slant = slant;
    unset_face(font_style);

    return TRUE;
}

static gboolean
set_weight(AdgFontStyle *font_style, cairo_font_weight_t weight)
{
    AdgFontStylePrivate *data = font_style->data;

    if (weight == data->weight)
        return FALSE;

    data->weight = weight;
    unset_face(font_style);

    return TRUE;
}

static gboolean
set_size(AdgFontStyle *font_style, gdouble size)
{
    AdgFontStylePrivate *data = font_style->data;

    if (size == data->size)
        return FALSE;

    data->size = size;
    unset_font(font_style);

    return TRUE;
}

static gboolean
set_antialias(AdgFontStyle *font_style, cairo_antialias_t antialias)
{
    AdgFontStylePrivate *data = font_style->data;

    if (antialias == data->antialias)
        return FALSE;

    data->antialias = antialias;
    unset_font(font_style);

    return TRUE;
}

static gboolean
set_subpixel_order(AdgFontStyle *font_style,
                   cairo_subpixel_order_t subpixel_order)
{
    AdgFontStylePrivate *data = font_style->data;

    if (subpixel_order == data->subpixel_order)
        return FALSE;

    data->subpixel_order = subpixel_order;
    unset_font(font_style);

    return TRUE;
}

static gboolean
set_hint_style(AdgFontStyle *font_style, cairo_hint_style_t hint_style)
{
    AdgFontStylePrivate *data = font_style->data;

    if (hint_style == data->hint_style)
        return FALSE;

    data->hint_style = hint_style;
    unset_font(font_style);

    return TRUE;
}

static gboolean
set_hint_metrics(AdgFontStyle *font_style, cairo_hint_metrics_t hint_metrics)
{
    AdgFontStylePrivate *data = font_style->data;

    if (hint_metrics == data->hint_metrics)
        return FALSE;

    data->hint_metrics = hint_metrics;
    unset_font(font_style);

    return TRUE;
}

static void
unset_face(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data = font_style->data;

    if (data->face == NULL)
        return;

    /* Changing the face invalidates the scaled font too */
    unset_font(font_style);

    cairo_font_face_destroy(data->face);
    data->face = NULL;
}

static void
unset_font(AdgFontStyle *font_style)
{
    AdgFontStylePrivate *data = font_style->data;

    if (data->font == NULL)
        return;

    cairo_scaled_font_destroy(data->font);
    data->font = NULL;
}
