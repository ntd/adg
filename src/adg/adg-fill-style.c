/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-fill-style
 * @short_description: Generic pattern fill
 *
 * A style defining a generic fill based on cairo_pattern_t.
 *
 * Since: 1.0
 **/

/**
 * AdgFillStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgFillStyleClass:
 * @set_extents: virtual method that specifies where a specific fill style
 *               must be applied. It is called by #AdgHatch in the rendering
 *               phase passing with its boundary box as argument.
 *
 * The default <function>set_extents</function> implementation simply sets
 * the extents owned by the fill style instance to the one provided, so the
 * last call has precedence. Any derived class can override it to customize
 * this behavior, for example to keep the greatest boundary box instead of
 * the last one.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-style.h"
#include "adg-cairo-fallback.h"

#include "adg-fill-style.h"
#include "adg-fill-style-private.h"


G_DEFINE_ABSTRACT_TYPE(AdgFillStyle, adg_fill_style, ADG_TYPE_STYLE)

enum {
    PROP_0,
    PROP_PATTERN
};


static void             _adg_finalize           (GObject        *object);
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


static void
adg_fill_style_class_init(AdgFillStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgFillStylePrivate));

    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    style_class->apply = _adg_apply;

    klass->set_extents = _adg_set_extents;

    param = g_param_spec_boxed("pattern",
                               P_("Pattern"),
                               P_("The cairo pattern set for this entity"),
                               CAIRO_GOBJECT_TYPE_PATTERN,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PATTERN, param);
}

static void
adg_fill_style_init(AdgFillStyle *fill_style)
{
    AdgFillStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(fill_style,
                                                            ADG_TYPE_FILL_STYLE,
                                                            AdgFillStylePrivate);

    data->pattern = NULL;

    fill_style->data = data;
}

static void
_adg_finalize(GObject *object)
{
    AdgFillStylePrivate *data = ((AdgFillStyle *) object)->data;

    if (data->pattern != NULL) {
        cairo_pattern_destroy(data->pattern);
        data->pattern = NULL;
    }
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgFillStylePrivate *data = ((AdgFillStyle *) object)->data;

    switch (prop_id) {
    case PROP_PATTERN:
        g_value_set_boxed(value, data->pattern);
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
    AdgFillStylePrivate *data = ((AdgFillStyle *) object)->data;
    cairo_pattern_t *old_pattern;

    switch (prop_id) {
    case PROP_PATTERN:
        old_pattern = data->pattern;
        data->pattern = g_value_get_boxed(value);

        if (data->pattern)
            cairo_pattern_reference(data->pattern);
        if (old_pattern)
            cairo_pattern_destroy(old_pattern);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_fill_style_set_pattern:
 * @fill_style: an #AdgFillStyle
 * @pattern: the new pattern
 *
 * <note><para>
 * This function is only useful in new fill implementations.
 * </para></note>
 *
 * Sets a new pattern on @fill_style. A new reference is added to
 * @pattern with cairo_pattern_reference() and the old pattern
 * (if any) is unreferenced with cairo_pattern_destroy().
 *
 * Since: 1.0
 **/
void
adg_fill_style_set_pattern(AdgFillStyle *fill_style,
                           cairo_pattern_t *pattern)
{
    g_return_if_fail(ADG_IS_FILL_STYLE(fill_style));
    g_object_set(fill_style, "pattern", pattern, NULL);
}

/**
 * adg_fill_style_get_pattern:
 * @fill_style: an #AdgFillStyle
 *
 * Gets the current pattern binded to @fill_style.
 *
 * Returns: (transfer none): the current pattern
 *
 * Since: 1.0
 **/
cairo_pattern_t *
adg_fill_style_get_pattern(AdgFillStyle *fill_style)
{
    AdgFillStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FILL_STYLE(fill_style), NULL);

    data = fill_style->data;

    return data->pattern;
}

/**
 * adg_fill_style_set_extents:
 * @fill_style: an #AdgFillStyle
 * @extents: the new extents
 *
 * <note><para>
 * This function is only useful in new fill style implementations.
 * </para></note>
 *
 * Forcibly sets new extents on @fill_style. Any fill style class
 * that want to make some kind of customization can override the
 * <function>set_extents</function> virtual method to intercept
 * any extents change.
 *
 * Sets new extents on @fill_style. These extents are usually set
 * by the arrange() method of the entity using this filling style.
 *
 * Since: 1.0
 **/
void
adg_fill_style_set_extents(AdgFillStyle *fill_style,
                           const CpmlExtents *extents)
{
    AdgFillStyleClass *klass;

    g_return_if_fail(ADG_IS_FILL_STYLE(fill_style));
    g_return_if_fail(extents != NULL);

    klass = ADG_FILL_STYLE_GET_CLASS(fill_style);

    if (klass->set_extents)
        klass->set_extents(fill_style, extents);
}

/**
 * adg_fill_style_get_extents:
 * @fill_style: an #AdgFillStyle
 *
 * Stores a copy of the extents of @fill_style in @extents.
 * This struct specifies the maximum portion (in global space)
 * this fill style should be applied: it will clamped by the
 * entities as needed.
 *
 * Returns: (transfer none): the extents of @fill_style or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_fill_style_get_extents(AdgFillStyle *fill_style)
{
    AdgFillStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FILL_STYLE(fill_style), NULL);

    data = fill_style->data;

    return &data->extents;
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgFillStylePrivate *data = ((AdgFillStyle *) style)->data;

    if (data->pattern == NULL)
        g_warning(_("%s: pattern undefined for type '%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(style)));
    else
        cairo_set_source(cr, data->pattern);
}

static void
_adg_set_extents(AdgFillStyle *fill_style, const CpmlExtents *extents)
{
    AdgFillStylePrivate *data = fill_style->data;

    cpml_extents_copy(&data->extents, extents);
}
