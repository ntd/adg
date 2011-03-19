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
 * SECTION:adg-line-style
 * @short_description: Line style related stuff
 *
 * Contains parameters on how to draw lines such as width, cap mode, join mode
 * and dash composition, if used.
 **/

/**
 * AdgLineStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"
#include "adg-style.h"

#include "adg-line-style.h"
#include "adg-line-style-private.h"


G_DEFINE_TYPE(AdgLineStyle, adg_line_style, ADG_TYPE_STYLE)

/* XXX: http://dev.entidi.com/p/adg/issues/51/ */
enum {
    PROP_0,
    PROP_COLOR_DRESS,
    PROP_WIDTH,
    PROP_CAP,
    PROP_JOIN,
    PROP_MITER_LIMIT,
    PROP_ANTIALIAS
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_apply              (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);


static void
adg_line_style_class_init(AdgLineStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgLineStylePrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->apply = _adg_apply;

    param = adg_param_spec_dress("color-dress",
                                 P_("Color Dress"),
                                 P_("The color dress to bind to this line style"),
                                 ADG_DRESS_COLOR,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COLOR_DRESS, param);

    param = g_param_spec_double("width",
                                P_("Line Width"),
                                P_("The line thickness in device unit"),
                                0., G_MAXDOUBLE, 2., G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_WIDTH, param);

    param = g_param_spec_int("cap",
                             P_("Line Cap"),
                             P_("The line cap mode"),
                             G_MININT, G_MAXINT, CAIRO_LINE_CAP_ROUND,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CAP, param);

    param = g_param_spec_int("join",
                             P_("Line Join"),
                             P_("The line join mode"),
                             G_MININT, G_MAXINT, CAIRO_LINE_JOIN_MITER,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_JOIN, param);

    param = g_param_spec_double("miter-limit",
                                P_("Miter Limit"),
                                P_("Whether the lines should be joined with a bevel instead of a miter"),
                                0., G_MAXDOUBLE, 10., G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MITER_LIMIT,
                                    param);

    param = g_param_spec_int("antialias",
                             P_("Antialiasing Mode"),
                             P_("Type of antialiasing to do when rendering lines"),
                             G_MININT, G_MAXINT, CAIRO_ANTIALIAS_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANTIALIAS, param);
}

static void
adg_line_style_init(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(line_style,
                                                            ADG_TYPE_LINE_STYLE,
                                                            AdgLineStylePrivate);

    data->color_dress = ADG_DRESS_COLOR;
    data->width = 2.;
    data->cap = CAIRO_LINE_CAP_ROUND;
    data->join = CAIRO_LINE_JOIN_MITER;
    data->miter_limit = 10.;
    data->antialias = CAIRO_ANTIALIAS_DEFAULT;
    data->dashes = NULL;
    data->num_dashes = 0;
    data->dash_offset = 0.;

    line_style->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgLineStylePrivate *data = ((AdgLineStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        g_value_set_int(value, data->color_dress);
        break;
    case PROP_WIDTH:
        g_value_set_double(value, data->width);
        break;
    case PROP_CAP:
        g_value_set_int(value, data->cap);
        break;
    case PROP_JOIN:
        g_value_set_int(value, data->join);
        break;
    case PROP_MITER_LIMIT:
        g_value_set_double(value, data->miter_limit);
        break;
    case PROP_ANTIALIAS:
        g_value_set_int(value, data->antialias);
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
    AdgLineStylePrivate *data = ((AdgLineStyle *) object)->data;

    switch (prop_id) {
    case PROP_COLOR_DRESS:
        data->color_dress = g_value_get_int(value);
        break;
    case PROP_WIDTH:
        data->width = g_value_get_double(value);
        break;
    case PROP_CAP:
        data->cap = g_value_get_int(value);
        break;
    case PROP_JOIN:
        data->join = g_value_get_int(value);
        break;
    case PROP_MITER_LIMIT:
        data->miter_limit = g_value_get_double(value);
        break;
    case PROP_ANTIALIAS:
        data->antialias = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_line_style_new:
 *
 * Constructs a new line style initialized with default params.
 *
 * Returns: a new line style
 **/
AdgLineStyle *
adg_line_style_new(void)
{
    return g_object_new(ADG_TYPE_LINE_STYLE, NULL);
}

/**
 * adg_line_style_set_color_dress:
 * @line_style: an #AdgLineStyle
 * @dress: the new color dress to use
 *
 * Sets a new color dress on @line_style. The new dress
 * should be related to the original dress: you cannot
 * set a dress used for line styles to a dress managing
 * fonts.
 *
 * The validation of the new dress is done by calling
 * adg_dress_are_related() with @dress and the previous
 * dress as arguments: check out its documentation for
 * details on what is a related dress.
 **/
void
adg_line_style_set_color_dress(AdgLineStyle *line_style, AdgDress dress)
{
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));
    g_object_set(line_style, "color-dress", dress, NULL);
}

/**
 * adg_line_style_get_color_dress:
 * @line_style: an #AdgLineStyle
 *
 * Gets the color dress used by @line_style.
 *
 * Returns: the current color dress
 **/
AdgDress
adg_line_style_get_color_dress(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style), ADG_DRESS_UNDEFINED);

    data = line_style->data;

    return data->color_dress;
}

/**
 * adg_line_style_set_width:
 * @line_style: an #AdgLineStyle object
 * @width: the new width
 *
 * Sets a new line thickness value.
 **/
void
adg_line_style_set_width(AdgLineStyle *line_style, gdouble width)
{
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));
    g_object_set(line_style, "width", width, NULL);
}

/**
 * adg_line_style_get_width:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line thickness value (in global space).
 *
 * Returns: the requested width
 **/
gdouble
adg_line_style_get_width(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style), 0.);

    data = line_style->data;

    return data->width;
}

/**
 * adg_line_style_set_cap:
 * @line_style: an #AdgLineStyle object
 * @cap: the new cap mode
 *
 * Sets a new line cap mode.
 **/
void
adg_line_style_set_cap(AdgLineStyle *line_style, cairo_line_cap_t cap)
{
    AdgLineStylePrivate *data;

    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    data = line_style->data;
    data->cap = cap;

    g_object_notify((GObject *) line_style, "cap");
}

/**
 * adg_line_style_get_cap:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line cap mode.
 *
 * Returns: the requested line cap mode
 **/
cairo_line_cap_t
adg_line_style_get_cap(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_LINE_CAP_BUTT);

    data = line_style->data;

    return data->cap;
}

/**
 * adg_line_style_set_join:
 * @line_style: an #AdgLineStyle object
 * @join: the new join mode
 *
 * Sets a new line join mode.
 **/
void
adg_line_style_set_join(AdgLineStyle *line_style, cairo_line_join_t join)
{
    AdgLineStylePrivate *data;

    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    data = line_style->data;
    data->join = join;

    g_object_notify((GObject *) line_style, "join");
}

/**
 * adg_line_style_get_join:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line join mode.
 *
 * Returns: the requested line join mode
 **/
cairo_line_join_t
adg_line_style_get_join(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_LINE_JOIN_MITER);

    data = line_style->data;

    return data->join;
}

/**
 * adg_line_style_set_miter_limit:
 * @line_style: an #AdgLineStyle object
 * @miter_limit: the new miter limit
 *
 * Sets a new miter limit value.
 **/
void
adg_line_style_set_miter_limit(AdgLineStyle *line_style, gdouble miter_limit)
{
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));
    g_object_set(line_style, "miter-limit", miter_limit, NULL);
}

/**
 * adg_line_style_get_miter_limit:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line miter limit value. The miter limit is used to determine
 * whether the lines should be joined with a bevel instead of a miter.
 *
 * Returns: the requested miter limit
 **/
gdouble
adg_line_style_get_miter_limit(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style), 0.);

    data = line_style->data;

    return data->miter_limit;
}

/**
 * adg_line_style_set_antialias:
 * @line_style: an #AdgLineStyle object
 * @antialias: the new antialias mode
 *
 * Sets a new antialias mode.
 **/
void
adg_line_style_set_antialias(AdgLineStyle *line_style,
                             cairo_antialias_t antialias)
{
    AdgLineStylePrivate *data;

    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    data = line_style->data;
    data->antialias = antialias;

    g_object_notify((GObject *) line_style, "antialias");
}

/**
 * adg_line_style_get_antialias:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the antialias mode used.
 *
 * Returns: the requested antialias mode
 **/
cairo_antialias_t
adg_line_style_get_antialias(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *data;

    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_ANTIALIAS_DEFAULT);

    data = line_style->data;

    return data->antialias;
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgLineStylePrivate *data = ((AdgLineStyle *) style)->data;

    adg_entity_apply_dress(entity, data->color_dress, cr);
    cairo_set_line_width(cr, data->width);
    cairo_set_line_cap(cr, data->cap);
    cairo_set_line_join(cr, data->join);
    cairo_set_miter_limit(cr, data->miter_limit);
    cairo_set_antialias(cr, data->antialias);

    if (data->num_dashes > 0) {
        g_return_if_fail(data->dashes != NULL);
        cairo_set_dash(cr, data->dashes, data->num_dashes, data->dash_offset);
    }
}
