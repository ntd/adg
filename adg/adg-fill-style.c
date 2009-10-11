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
 * SECTION:adg-fill-style
 * @short_description: Generic pattern fill
 *
 * A style defining a generic fill based on cairo_pattern_t.
 **/

/**
 * AdgFillStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-fill-style.h"
#include "adg-fill-style-private.h"
#include "adg-intl.h"

#define PARENT_STYLE_CLASS  ((AdgStyleClass *) adg_fill_style_parent_class)


enum {
    PROP_0,
    PROP_PATTERN
};


static void             finalize        (GObject        *object);
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
static gboolean         set_pattern     (AdgFillStyle   *fill_style,
                                         AdgPattern     *pattern);
static void             set_extents     (AdgFillStyle   *fill_style,
                                         const CpmlExtents *extents);


G_DEFINE_ABSTRACT_TYPE(AdgFillStyle, adg_fill_style, ADG_TYPE_STYLE);


static void
adg_fill_style_class_init(AdgFillStyleClass *klass)
{
    GObjectClass *gobject_class;
    AdgStyleClass *style_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    style_class = (AdgStyleClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgFillStylePrivate));

    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    style_class->apply = apply;

    klass->set_extents = set_extents;

    param = g_param_spec_boxed("pattern",
                               P_("Pattern"),
                               P_("The cairo pattern set for this entity"),
                               ADG_TYPE_PATTERN,
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
finalize(GObject *object)
{
    AdgFillStylePrivate *data = ((AdgFillStyle *) object)->data;

    if (data->pattern != NULL) {
        cairo_pattern_destroy(data->pattern);
        data->pattern = NULL;
    }
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
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
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgFillStyle *fill_style = (AdgFillStyle *) object;

    switch (prop_id) {
    case PROP_PATTERN:
        set_pattern(fill_style, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_fill_style_get_pattern:
 * @fill_style: an #AdgFillStyle
 *
 * Gets the current pattern binded to @fill_style.
 *
 * Returns: the current pattern
 **/
AdgPattern *
adg_fill_style_get_pattern(AdgFillStyle *fill_style)
{
    AdgFillStylePrivate *data;

    g_return_val_if_fail(ADG_IS_FILL_STYLE(fill_style), NULL);

    data = fill_style->data;

    return data->pattern;
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
 **/
void
adg_fill_style_set_pattern(AdgFillStyle *fill_style, AdgPattern *pattern)
{
    g_return_if_fail(ADG_IS_FILL_STYLE(fill_style));

    if (set_pattern(fill_style, pattern))
        g_object_notify((GObject *) fill_style, "pattern");
}

/**
 * adg_fill_style_get_extents:
 * @fill_style: an #AdgFillStyle
 * @extents: where to store the extents
 *
 * Stores a copy of the extents of @fill_style in @extents.
 * This struct specifies the maximum portion (in global space)
 * this fill style should be applied: it will clamped by the
 * entities as needed.
 **/
void
adg_fill_style_get_extents(AdgFillStyle *fill_style, CpmlExtents *extents)
{
    AdgFillStylePrivate *data;

    g_return_if_fail(ADG_IS_FILL_STYLE(fill_style));
    g_return_if_fail(extents != NULL);

    data = fill_style->data;

    cpml_extents_copy(extents, &data->extents);
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
 * set_extents() virtual method to intercept any extents change.
 *
 * Sets new extents on @fill_style. These extents are usually set
 * by the arrange() method of the entity using this filling style.
 * The default implementation simply sets the extents, so the
 * last one has precedence. Any fill style implementation can
 * override the set_extents() implementation to customize this
 * behavior, for example to keep the greatest boundary box instead
 * of the last one.
 **/
void
adg_fill_style_set_extents(AdgFillStyle *fill_style,
                           const CpmlExtents *extents)
{
    g_return_if_fail(ADG_IS_FILL_STYLE(fill_style));
    g_return_if_fail(extents != NULL);

    ADG_FILL_STYLE_GET_CLASS(fill_style)->set_extents(fill_style, extents);
}


static void
apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgFillStylePrivate *data = ((AdgFillStyle *) style)->data;

    if (data->pattern == NULL)
        g_warning("%s: pattern undefined for type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(style)));
    else
        cairo_set_source(cr, data->pattern);
}

static gboolean
set_pattern(AdgFillStyle *fill_style, AdgPattern *pattern)
{
    AdgFillStylePrivate *data = fill_style->data;

    if (pattern == data->pattern)
        return FALSE;

    if (data->pattern != NULL)
        cairo_pattern_destroy(data->pattern);

    data->pattern = pattern;

    if (data->pattern != NULL)
        cairo_pattern_reference(data->pattern);

    return TRUE;
}

static void
set_extents(AdgFillStyle *fill_style, const CpmlExtents *extents)
{
    AdgFillStylePrivate *data = fill_style->data;

    cpml_extents_copy(&data->extents, extents);
}
