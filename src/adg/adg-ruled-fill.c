/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-ruled-fill
 * @short_description: A style composed of evenly spaced lines
 *
 * The spacing between the lines could be changed using the
 * adg_ruled_fill_set_spacing() method. The angle of the lines should
 * be changed with adg_ruled_fill_set_angle().
 *
 * Since: 1.0
 **/

/**
 * AdgRuledFill:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-style.h"
#include "adg-fill-style.h"
#include "adg-dress.h"
#include "adg-param-dress.h"

#include "adg-ruled-fill.h"
#include "adg-ruled-fill-private.h"

#include <math.h>


#define _ADG_OLD_STYLE_CLASS      ((AdgStyleClass *) adg_ruled_fill_parent_class)
#define _ADG_OLD_FILL_STYLE_CLASS ((AdgFillStyleClass *) adg_ruled_fill_parent_class)


G_DEFINE_TYPE(AdgRuledFill, adg_ruled_fill, ADG_TYPE_FILL_STYLE)

enum {
    PROP_0,
    PROP_LINE_DRESS,
    PROP_SPACING,
    PROP_ANGLE
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
static void             _adg_set_extents        (AdgFillStyle   *fill_style,
                                                 const CpmlExtents *extents);
static cairo_pattern_t *_adg_create_pattern     (AdgRuledFill   *ruled_fill,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_draw_lines         (const CpmlPair *spacing,
                                                 const CpmlPair *size,
                                                 cairo_t        *cr);


static void
adg_ruled_fill_class_init(AdgRuledFillClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    AdgFillStyleClass *fill_style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;
    fill_style_class = (AdgFillStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgRuledFillPrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->apply = _adg_apply;

    fill_style_class->set_extents = _adg_set_extents;

    param = adg_param_spec_dress("line-dress",
                                 P_("Line Dress"),
                                 P_("Dress to be used for rendering the lines"),
                                 ADG_DRESS_LINE_FILL,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE_DRESS, param);

    param = g_param_spec_double("spacing",
                               P_("Spacing"),
                               P_("The spacing in global spaces between the lines"),
                               0, G_MAXDOUBLE, 16,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SPACING, param);

    param = g_param_spec_double("angle",
                               P_("Angle"),
                               P_("The angle (in radians) of the lines"),
                               0, G_PI, G_PI_4,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE, param);
}

static void
adg_ruled_fill_init(AdgRuledFill *ruled_fill)
{
    AdgRuledFillPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(ruled_fill,
                                                            ADG_TYPE_RULED_FILL,
                                                            AdgRuledFillPrivate);

    data->line_dress = ADG_DRESS_LINE_FILL;
    data->angle = G_PI_4;
    data->spacing = 16;

    ruled_fill->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgRuledFillPrivate *data = ((AdgRuledFill *) object)->data;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        g_value_set_enum(value, data->line_dress);
        break;
    case PROP_SPACING:
        g_value_set_double(value, data->spacing);
        break;
    case PROP_ANGLE:
        g_value_set_double(value, data->angle);
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
    AdgRuledFill *ruled_fill;
    AdgRuledFillPrivate *data;

    ruled_fill = (AdgRuledFill *) object;
    data = ruled_fill->data;

    switch (prop_id) {
    case PROP_LINE_DRESS:
        data->line_dress = g_value_get_enum(value);
        break;
    case PROP_SPACING:
        data->spacing = g_value_get_double(value);
        adg_fill_style_set_pattern((AdgFillStyle *) object, NULL);
        break;
    case PROP_ANGLE:
        data->angle = g_value_get_double(value);
        adg_fill_style_set_pattern((AdgFillStyle *) object, NULL);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_ruled_fill_new:
 *
 * Constructs a new empty ruled fill style initialized with default params.
 *
 * Returns: a newly created ruled fill style
 *
 * Since: 1.0
 **/
AdgRuledFill *
adg_ruled_fill_new(void)
{
    return g_object_new(ADG_TYPE_RULED_FILL, NULL);
}

/**
 * adg_ruled_fill_set_line_dress:
 * @ruled_fill: an #AdgRuledFill object
 * @dress: the new line dress
 *
 * Sets a new line dress on @ruled_fill.
 *
 * Since: 1.0
 **/
void
adg_ruled_fill_set_line_dress(AdgRuledFill *ruled_fill, AdgDress dress)
{
    g_return_if_fail(ADG_IS_RULED_FILL(ruled_fill));
    g_object_set(ruled_fill, "line-dress", dress, NULL);
}

/**
 * adg_ruled_fill_get_line_dress:
 * @ruled_fill: an #AdgRuledFill object
 *
 * Gets the @ruled_fill dress to be used for rendering the lines.
 *
 * Returns: (transfer none): the line dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_ruled_fill_get_line_dress(AdgRuledFill *ruled_fill)
{
    AdgRuledFillPrivate *data;

    g_return_val_if_fail(ADG_IS_RULED_FILL(ruled_fill), ADG_DRESS_UNDEFINED);

    data = ruled_fill->data;

    return data->line_dress;
}

/**
 * adg_ruled_fill_set_spacing:
 * @ruled_fill: an #AdgRuledFill
 * @spacing: the new spacing
 *
 * Sets a new spacing on @ruled_fill.
 *
 * Since: 1.0
 **/
void
adg_ruled_fill_set_spacing(AdgRuledFill *ruled_fill, gdouble spacing)
{
    g_return_if_fail(ADG_IS_RULED_FILL(ruled_fill));
    g_object_set(ruled_fill, "spacing", spacing, NULL);
}

/**
 * adg_ruled_fill_get_spacing:
 * @ruled_fill: an #AdgRuledFill
 *
 * Gets the current spacing of @ruled_fill.
 *
 * Returns: the spacing (in global space)
 *
 * Since: 1.0
 **/
gdouble
adg_ruled_fill_get_spacing(AdgRuledFill *ruled_fill)
{
    AdgRuledFillPrivate *data;

    g_return_val_if_fail(ADG_IS_RULED_FILL(ruled_fill), 0);

    data = ruled_fill->data;

    return data->spacing;
}

/**
 * adg_ruled_fill_set_angle:
 * @ruled_fill: an #AdgRuledFill
 * @angle: the new angle
 *
 * Sets a new angle on @ruled_fill.
 *
 * Since: 1.0
 **/
void
adg_ruled_fill_set_angle(AdgRuledFill *ruled_fill, gdouble angle)
{
    g_return_if_fail(ADG_IS_RULED_FILL(ruled_fill));
    g_object_set(ruled_fill, "angle", angle, NULL);
}

/**
 * adg_ruled_fill_get_angle:
 * @ruled_fill: an #AdgRuledFill
 *
 * Gets the current angle of @ruled_fill.
 *
 * Returns: the angle (in radians)
 *
 * Since: 1.0
 **/
gdouble
adg_ruled_fill_get_angle(AdgRuledFill *ruled_fill)
{
    AdgRuledFillPrivate *data;

    g_return_val_if_fail(ADG_IS_RULED_FILL(ruled_fill), 0);

    data = ruled_fill->data;

    return data->angle;
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgFillStyle *fill_style;
    cairo_pattern_t *pattern;
    const CpmlExtents *extents;

    fill_style = (AdgFillStyle *) style;
    pattern = adg_fill_style_get_pattern(fill_style);
    extents = adg_fill_style_get_extents(fill_style);

    if (pattern == NULL) {
        pattern = _adg_create_pattern((AdgRuledFill *) style, entity, cr);
        if (pattern == NULL)
            return;

        adg_fill_style_set_pattern(fill_style, pattern);
        cairo_pattern_destroy(pattern);
    }

    cairo_translate(cr, extents->org.x, extents->org.y);

    if (_ADG_OLD_STYLE_CLASS->apply)
        _ADG_OLD_STYLE_CLASS->apply(style, entity, cr);
}

static void
_adg_set_extents(AdgFillStyle *fill_style, const CpmlExtents *extents)
{
    CpmlExtents old, new;

    cpml_extents_copy(&old, adg_fill_style_get_extents(fill_style));

    /* The pattern is invalidated (and thus regenerated) only
     * when the new extents are wider than the old ones */
    if (old.size.x >= extents->size.x && old.size.y >= extents->size.y) {
        new.org = extents->org;
        new.size = old.size;
    } else {
        cpml_extents_copy(&new, extents);
        adg_fill_style_set_pattern(fill_style, NULL);
    }

    if (_ADG_OLD_FILL_STYLE_CLASS->set_extents)
        _ADG_OLD_FILL_STYLE_CLASS->set_extents(fill_style, &new);
}

static cairo_pattern_t *
_adg_create_pattern(AdgRuledFill *ruled_fill, AdgEntity *entity, cairo_t *cr)
{
    AdgFillStyle *fill_style;
    const CpmlExtents *extents;
    AdgRuledFillPrivate *data;
    AdgStyle *line_style;
    cairo_pattern_t *pattern;
    cairo_surface_t *surface;
    CpmlPair spacing;
    cairo_t *context;

    fill_style = (AdgFillStyle *) ruled_fill;
    extents = adg_fill_style_get_extents(fill_style);

    /* Check for valid extents */
    if (!extents->is_defined)
        return NULL;

    data = ruled_fill->data;
    line_style = adg_entity_style(entity, data->line_dress);
    surface = cairo_surface_create_similar(cairo_get_target(cr),
                                           CAIRO_CONTENT_COLOR_ALPHA,
                                           extents->size.x, extents->size.y);
    pattern = cairo_pattern_create_for_surface(surface);

    /* The pattern holds a reference to the surface, so
     * there is no need to hold another reference here */
    cairo_surface_destroy(surface);

    spacing.x = cos(data->angle) * data->spacing;
    spacing.y = sin(data->angle) * data->spacing;

    context = cairo_create(surface);
    adg_style_apply(line_style, entity, context);
    _adg_draw_lines(&spacing, &extents->size, context);
    cairo_destroy(context);

    return pattern;
}

static void
_adg_draw_lines(const CpmlPair *spacing, const CpmlPair *size, cairo_t *cr)
{
    CpmlPair step, step1, step2;
    CpmlPair p1, p2;

    /* There should be some sort of spacing and a destination area */
    if ((spacing->x == 0 && spacing->y == 0) ||
        (size->x <= 0 && size->y <= 0))
        return;

    /* Revert spacings if needed to inspect only the x >= 0 cases */
    cpml_pair_copy(&step, spacing);
    if (spacing->x < 0 || (spacing->x == 0 && spacing->y < 0)) {
        step.x = -step.x;
        step.y = -step.y;
    }

    p1.x = step.x / 2;
    p2.y = step.y / 2;
    p1.y = step.y == 0 ? p2.y : 0;
    p2.x = step.x == 0 ? p1.x : 0;
    if (step.y < 0) {
        p1.y += size->y;
        p2.y += size->y;
    }

    step2.x = 0;
    step2.y = step.y;

    if (step.x != 0) {
        step1.x = step.x;
        step1.y = 0;

        while (p1.x < size->x) {
            if (p2.y <= 0 || p2.y >= size->y) {
                step2.x = step.x;
                step2.y = 0;
            }
            cairo_move_to(cr, p1.x, p1.y);
            cairo_line_to(cr, p2.x, p2.y);
            p1.x += step1.x;
            p1.y += step1.y;
            p2.x += step2.x;
            p2.y += step2.y;
        }
    }

    if (step.y != 0) {
        step1.x = 0;
        step1.y = step.y;

        while (p1.y >= 0 && p1.y <= size->y) {
            if (p2.y <= 0 || p2.y >= size->y) {
                step2.x = step.x;
                step2.y = 0;
            }
            cairo_move_to(cr, p1.x, p1.y);
            cairo_line_to(cr, p2.x, p2.y);
            p1.x += step1.x;
            p1.y += step1.y;
            p2.x += step2.x;
            p2.y += step2.y;
        }
    }

    cairo_stroke(cr);
}
