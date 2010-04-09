/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-color-style
 * @short_description: RGBA color information
 *
 * A style containing a single color expressed in RGB format.
 * #AdgColorStyle also supports the alpha compositing that should be
 * expressed with a double value between %0 and %1, where %0 is the
 * "completely transparent" value while %1 is "fully opaque".
 **/

/**
 * AdgColorStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-color-style.h"
#include "adg-color-style-private.h"

#define PARENT_STYLE_CLASS  ((AdgStyleClass *) adg_color_style_parent_class)


enum {
    PROP_0,
    PROP_RED,
    PROP_GREEN,
    PROP_BLUE,
    PROP_ALPHA
};


static void             get_property    (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void             set_property    (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void             apply           (AdgStyle       *style,
                                         AdgEntity      *entity,
                                         cairo_t        *cr);
static gboolean         set_channel     (gdouble        *channel,
                                         gdouble         value);


G_DEFINE_TYPE(AdgColorStyle, adg_color_style, ADG_TYPE_STYLE);


static void
adg_color_style_class_init(AdgColorStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgColorStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    param = g_param_spec_double("red",
                                P_("Red Channel"),
                                P_("The red value, where 0 means no red and 1 is full red"),
                                0, 1, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RED, param);

    param = g_param_spec_double("green",
                                P_("Green Channel"),
                                P_("The green value, where 0 means no green and 1 is full green"),
                                0, 1, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GREEN, param);

    param = g_param_spec_double("blue",
                                P_("Blue Channel"),
                                P_("The blue value, where 0 means no blue and 1 is full blue"),
                                0, 1, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BLUE, param);

    param = g_param_spec_double("alpha",
                                P_("Alpha Channel"),
                                P_("The alpha value, where 0 means completely transparent and 1 is fully opaque"),
                                0, 1, 1,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ALPHA, param);
}

static void
adg_color_style_init(AdgColorStyle *color_style)
{
    AdgColorStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(color_style,
                                                            ADG_TYPE_COLOR_STYLE,
                                                            AdgColorStylePrivate);

    data->red = 0;
    data->green = 0;
    data->blue = 0;
    data->alpha = 1;

    color_style->data = data;
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgColorStylePrivate *data = ((AdgColorStyle *) object)->data;

    switch (prop_id) {
    case PROP_RED:
        g_value_set_double(value, data->red);
        break;
    case PROP_GREEN:
        g_value_set_double(value, data->green);
        break;
    case PROP_BLUE:
        g_value_set_double(value, data->blue);
        break;
    case PROP_ALPHA:
        g_value_set_double(value, data->alpha);
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
    AdgColorStylePrivate *data = ((AdgColorStyle *) object)->data;

    switch (prop_id) {
    case PROP_RED:
        set_channel(&data->red, g_value_get_double(value));
        break;
    case PROP_GREEN:
        set_channel(&data->green, g_value_get_double(value));
        break;
    case PROP_BLUE:
        set_channel(&data->blue, g_value_get_double(value));
        break;
    case PROP_ALPHA:
        set_channel(&data->alpha, g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_color_style_new:
 *
 * Constructs a new color initialized to opaque black.
 *
 * Returns: a new color style
 **/
AdgColorStyle *
adg_color_style_new(void)
{
    return g_object_new(ADG_TYPE_COLOR_STYLE, NULL);
}

/**
 * adg_color_style_set_red:
 * @color_style: an #AdgColorStyle
 * @red: the new value
 *
 * Sets a new value for the red channel, where %0 means no red and
 * %1 is full red.
 **/
void
adg_color_style_set_red(AdgColorStyle *color_style, gdouble red)
{
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    data = color_style->data;

    if (set_channel(&data->red, red))
        g_object_notify((GObject *) color_style, "red");
}

/**
 * adg_color_style_get_red:
 * @color_style: an #AdgColorStyle
 *
 * Gets the current value of the red channel, where %0 means no red and
 * %1 is full red.
 *
 * Returns: the requested red value
 **/
gdouble
adg_color_style_get_red(AdgColorStyle *color_style)
{
    AdgColorStylePrivate *data;

    g_return_val_if_fail(ADG_IS_COLOR_STYLE(color_style), 0.);

    data = color_style->data;

    return data->red;
}

/**
 * adg_color_style_set_green:
 * @color_style: an #AdgColorStyle
 * @green: the new value
 *
 * Sets a new value for the green channel, where %0 means no green and
 * %1 is full green.
 **/
void
adg_color_style_set_green(AdgColorStyle *color_style, gdouble green)
{
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    data = color_style->data;

    if (set_channel(&data->green, green))
        g_object_notify((GObject *) color_style, "green");
}

/**
 * adg_color_style_get_green:
 * @color_style: an #AdgColorStyle
 *
 * Gets the current value of the green channel, where %0 means no green and
 * %1 is full green.
 *
 * Returns: the requested green value
 **/
gdouble
adg_color_style_get_green(AdgColorStyle *color_style)
{
    AdgColorStylePrivate *data;

    g_return_val_if_fail(ADG_IS_COLOR_STYLE(color_style), 0.);

    data = color_style->data;

    return data->green;
}

/**
 * adg_color_style_set_blue:
 * @color_style: an #AdgColorStyle
 * @blue: the new value
 *
 * Sets a new value for the blue channel, where %0 means no blue and
 * %1 is full blue.
 **/
void
adg_color_style_set_blue(AdgColorStyle *color_style, gdouble blue)
{
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    data = color_style->data;

    if (set_channel(&data->blue, blue))
        g_object_notify((GObject *) color_style, "blue");
}

/**
 * adg_color_style_get_blue:
 * @color_style: an #AdgColorStyle
 *
 * Gets the current value of the blue channel, where %0 means no blue and
 * %1 is full blue.
 *
 * Returns: the requested blue value
 **/
gdouble
adg_color_style_get_blue(AdgColorStyle *color_style)
{
    AdgColorStylePrivate *data;

    g_return_val_if_fail(ADG_IS_COLOR_STYLE(color_style), 0.);

    data = color_style->data;

    return data->blue;
}

/**
 * adg_color_style_set_rgb:
 * @color_style: an #AdgColorStyle
 * @r: the red channel value
 * @g: the green channel value
 * @b: the blue channel value
 *
 * Sets the RGB channels at once.
 **/
void
adg_color_style_set_rgb(AdgColorStyle *color_style,
                        gdouble red, gdouble green, gdouble blue)
{
    GObject *object;
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    object = (GObject *) color_style;
    data = color_style->data;

    data->red = red;
    data->green = green;
    data->blue = blue;
}

/**
 * adg_color_style_put_rgb:
 * @color_style: an #AdgColorStyle
 * @r: where to store the red channel value
 * @g: where to store the green channel value
 * @b: where to store the blue channel value
 *
 * Gets the values of the red, green and blue channels of @color_style.
 * Any of the pointer can be %NULL, in which case the value is not returned.
 **/
void
adg_color_style_put_rgb(AdgColorStyle *color_style,
                        gdouble *r, gdouble *g, gdouble *b)
{
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    data = color_style->data;

    if (r != NULL)
        *r = data->red;

    if (g != NULL)
        *g = data->green;

    if (b != NULL)
        *b = data->blue;
}

/**
 * adg_color_style_set_alpha:
 * @color_style: an #AdgColorStyle
 * @alpha: the new alpha
 *
 * Sets a new color alpha value, where %0 means completely transparent
 * and %1 is fully opaque.
 **/
void
adg_color_style_set_alpha(AdgColorStyle *color_style, gdouble alpha)
{
    AdgColorStylePrivate *data;

    g_return_if_fail(ADG_IS_COLOR_STYLE(color_style));

    data = color_style->data;

    if (set_channel(&data->alpha, alpha))
        g_object_notify((GObject *) color_style, "alpha");
}

/**
 * adg_color_style_get_alpha:
 * @color_style: an #AdgColorStyle
 *
 * Gets the alpha channel value, where %0 means completely transparent
 * and %1 is fully opaque.
 *
 * Returns: the requested alpha value
 **/
gdouble
adg_color_style_get_alpha(AdgColorStyle *color_style)
{
    AdgColorStylePrivate *data;

    g_return_val_if_fail(ADG_IS_COLOR_STYLE(color_style), 0.);

    data = color_style->data;

    return data->alpha;
}


static void
apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgColorStylePrivate *data = ((AdgColorStyle *) style)->data;

    if (data->alpha == 1.)
        cairo_set_source_rgb(cr, data->red, data->green, data->blue);
    else
        cairo_set_source_rgba(cr, data->red, data->green, data->blue,
                              data->alpha);
}

static gboolean
set_channel(gdouble *channel, gdouble value)
{
    g_return_val_if_fail(value >= 0 && value <=1, FALSE);

    if (*channel == value)
        return FALSE;

    *channel = value;
    return TRUE;
}