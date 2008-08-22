/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/**
 * SECTION:line-style
 * @title: AdgLineStyle
 * @short_description: Line style related stuff
 *
 * Contains parameters on how to draw lines such as width, cap mode, join mode
 * and dash composition, if used.
 */

#include "adg-line-style.h"
#include "adg-line-style-private.h"
#include "adg-context.h"
#include "adg-intl.h"
#include "adg-util.h"

#define PARENT_CLASS ((AdgStyleClass *) adg_line_style_parent_class)


enum {
    PROP_0,
    PROP_WIDTH,
    PROP_CAP,
    PROP_JOIN,
    PROP_MITER_LIMIT,
    PROP_ANTIALIAS,
    PROP_DASH
};


static void             get_property    (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void             set_property    (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static GPtrArray *      get_pool        (void);
static void             apply           (AdgStyle       *style,
                                         cairo_t        *cr);


G_DEFINE_TYPE(AdgLineStyle, adg_line_style, ADG_TYPE_STYLE)


static void
adg_line_style_class_init(AdgLineStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgLineStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->get_pool = get_pool;
    style_class->apply = apply;

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
                                P_
                                ("Whether the lines should be joined with a bevel instead of a miter"),
                                0., G_MAXDOUBLE, 10., G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MITER_LIMIT,
                                    param);

    param = g_param_spec_int("antialias",
                             P_("Antialiasing Mode"),
                             P_
                             ("Type of antialiasing to do when rendering lines"),
                             G_MININT, G_MAXINT, CAIRO_ANTIALIAS_DEFAULT,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANTIALIAS, param);

    /* TODO: PROP_DASH (PROP_DASHES, PROP_NUM_DASHES, PROP_DASH_OFFSET) */
}

static void
adg_line_style_init(AdgLineStyle *line_style)
{
    AdgLineStylePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(line_style,
                                                            ADG_TYPE_LINE_STYLE,
                                                            AdgLineStylePrivate);

    priv->width = 2.;
    priv->cap = CAIRO_LINE_CAP_ROUND;
    priv->join = CAIRO_LINE_JOIN_MITER;
    priv->miter_limit = 10.;
    priv->antialias = CAIRO_ANTIALIAS_DEFAULT;
    priv->dashes = NULL;
    priv->num_dashes = 0;
    priv->dash_offset = 0.;

    line_style->priv = priv;
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgLineStyle *line_style = (AdgLineStyle *) object;

    switch (prop_id) {
    case PROP_WIDTH:
        g_value_set_double(value, line_style->priv->width);
        break;
    case PROP_CAP:
        g_value_set_int(value, line_style->priv->cap);
        break;
    case PROP_JOIN:
        g_value_set_int(value, line_style->priv->join);
        break;
    case PROP_MITER_LIMIT:
        g_value_set_double(value, line_style->priv->miter_limit);
        break;
    case PROP_ANTIALIAS:
        g_value_set_int(value, line_style->priv->antialias);
        break;
    case PROP_DASH:
        /* TODO */
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
    AdgLineStyle *line_style = (AdgLineStyle *) object;

    switch (prop_id) {
    case PROP_WIDTH:
        line_style->priv->width = g_value_get_double(value);
        break;
    case PROP_CAP:
        line_style->priv->cap = g_value_get_int(value);
        break;
    case PROP_JOIN:
        line_style->priv->join = g_value_get_int(value);
        break;
    case PROP_MITER_LIMIT:
        line_style->priv->miter_limit = g_value_get_double(value);
        break;
    case PROP_ANTIALIAS:
        line_style->priv->antialias = g_value_get_int(value);
        break;
    case PROP_DASH:
        /* TODO */
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_line_style_get_slot:
 *
 * Gets the slot id for this style class.
 *
 * Return value: the slot
 **/
AdgStyleSlot
adg_line_style_get_slot(void)
{
    static AdgStyleSlot slot = -1;

    if (G_UNLIKELY(slot < 0))
        slot = adg_context_get_slot(ADG_TYPE_LINE_STYLE);

    return slot;
}

/**
 * adg_line_style_new:
 *
 * Constructs a new line style initialized with default params.
 *
 * Return value: a new line style
 **/
AdgStyle *
adg_line_style_new(void)
{
    return g_object_new(ADG_TYPE_LINE_STYLE, NULL);
}

/**
 * adg_line_style_get_width:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line thickness value (in paper units).
 *
 * Return value: the requested width
 **/
gdouble
adg_line_style_get_width(AdgLineStyle *line_style)
{
    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style), 0.);

    return line_style->priv->width;
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

    line_style->priv->width = width;
    g_object_notify((GObject *) line_style, "width");
}

/**
 * adg_line_style_get_cap:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line cap mode.
 *
 * Return value: the requested line cap mode
 **/
cairo_line_cap_t
adg_line_style_get_cap(AdgLineStyle *line_style)
{
    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_LINE_CAP_BUTT);

    return line_style->priv->cap;
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
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    line_style->priv->cap = cap;
    g_object_notify((GObject *) line_style, "cap");
}

/**
 * adg_line_style_get_join:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line join mode.
 *
 * Return value: the requested line join mode
 **/
cairo_line_join_t
adg_line_style_get_join(AdgLineStyle *line_style)
{
    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_LINE_JOIN_MITER);

    return line_style->priv->join;
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
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    line_style->priv->join = join;
    g_object_notify((GObject *) line_style, "join");
}

/**
 * adg_line_style_get_miter_limit:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the line miter limit value. The miter limit is used to determine
 * whether the lines should be joined with a bevel instead of a miter.
 *
 * Return value: the requested miter limit
 **/
gdouble
adg_line_style_get_miter_limit(AdgLineStyle *line_style)
{
    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style), 0.);

    return line_style->priv->miter_limit;
}

/**
 * adg_line_style_set_miter_limit:
 * @line_style: an #AdgLineStyle object
 * @miter_limit: the new miter limit
 *
 * Sets a new miter limit value.
 **/
void
adg_line_style_set_miter_limit(AdgLineStyle *line_style,
                               gdouble miter_limit)
{
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    line_style->priv->miter_limit = miter_limit;
    g_object_notify((GObject *) line_style, "miter-limit");
}

/**
 * adg_line_style_get_antialias:
 * @line_style: an #AdgLineStyle object
 *
 * Gets the antialias mode used.
 *
 * Return value: the requested antialias mode
 **/
cairo_antialias_t
adg_line_style_get_antialias(AdgLineStyle *line_style)
{
    g_return_val_if_fail(ADG_IS_LINE_STYLE(line_style),
                         CAIRO_ANTIALIAS_DEFAULT);

    return line_style->priv->antialias;
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
    g_return_if_fail(ADG_IS_LINE_STYLE(line_style));

    line_style->priv->antialias = antialias;
    g_object_notify((GObject *) line_style, "antialias");
}


static GPtrArray *
get_pool(void)
{
    static GPtrArray *pool = NULL;

    if (G_UNLIKELY(pool == NULL)) {
        cairo_pattern_t *pattern;

        pool = g_ptr_array_sized_new(ADG_LINE_STYLE_LAST);

        pool->pdata[ADG_LINE_STYLE_DRAW] =
            g_object_new(ADG_TYPE_LINE_STYLE, "width", 2., NULL);

        pattern = cairo_pattern_create_rgb(0., 1., 0.);
        pool->pdata[ADG_LINE_STYLE_CENTER] =
            g_object_new(ADG_TYPE_LINE_STYLE, "pattern", pattern, "width",
                         0.75, NULL);
        cairo_pattern_destroy(pattern);

        pattern = cairo_pattern_create_rgba(0., 0., 0., 0.5);
        pool->pdata[ADG_LINE_STYLE_HIDDEN] =
            g_object_new(ADG_TYPE_LINE_STYLE, "pattern", pattern, "width",
                         0.75, NULL);
        cairo_pattern_destroy(pattern);

        pattern = cairo_pattern_create_rgb(0., 0., 1.);
        pool->pdata[ADG_LINE_STYLE_XATCH] =
            g_object_new(ADG_TYPE_LINE_STYLE, "pattern", pattern, "width",
                         1.25, NULL);
        cairo_pattern_destroy(pattern);

        pool->pdata[ADG_LINE_STYLE_DIM] = g_object_new(ADG_TYPE_LINE_STYLE,
                                                       "width", 0.75,
                                                       NULL);

        pool->len = ADG_LINE_STYLE_LAST;
        }

    return pool;
}

static void
apply(AdgStyle *style, cairo_t *cr)
{
    AdgLineStyle *line_style;
    gdouble device_width;
    gdouble dummy = 0.;

    line_style = (AdgLineStyle *) style;

    PARENT_CLASS->apply(style, cr);

    device_width = line_style->priv->width;
    cairo_device_to_user_distance(cr, &device_width, &dummy);
    cairo_set_line_width(cr, device_width);

    cairo_set_line_cap(cr, line_style->priv->cap);
    cairo_set_line_join(cr, line_style->priv->join);
    cairo_set_miter_limit(cr, line_style->priv->miter_limit);
    cairo_set_antialias(cr, line_style->priv->antialias);

    if (line_style->priv->num_dashes > 0) {
        g_return_if_fail(line_style->priv->dashes != NULL);

        cairo_set_dash(cr, line_style->priv->dashes,
                       line_style->priv->num_dashes,
                       line_style->priv->dash_offset);
    }
}
