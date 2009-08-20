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
 * SECTION:adg-style
 * @short_description: The base class of all styling objects
 *
 * This is the fundamental abstract class from which all customization object
 * must be derived.
 */

/**
 * AdgStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-style.h"
#include "adg-style-private.h"
#include "adg-line-style.h"
#include "adg-font-style.h"
#include "adg-arrow-style.h"
#include "adg-enums.h"
#include "adg-util.h"
#include "adg-intl.h"

#include <string.h>


enum {
    PROP_0,
    PROP_PATTERN
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
static void             set_pattern     (AdgStyle       *style,
                                         AdgPattern     *pattern);


G_DEFINE_ABSTRACT_TYPE(AdgStyle, adg_style, G_TYPE_OBJECT)


static void
adg_style_class_init(AdgStyleClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgStylePrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    klass->get_pool = get_pool;
    klass->apply = apply;

    param = g_param_spec_boxed("pattern",
                               P_("Pattern"),
                               P_("The pattern associated to this style"),
                               ADG_TYPE_PATTERN, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PATTERN, param);
}

static void
adg_style_init(AdgStyle *style)
{
    AdgStylePrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(style,
                                                        ADG_TYPE_STYLE,
                                                        AdgStylePrivate);

    data->pattern = NULL;

    style->data = data;
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgStylePrivate *data = ((AdgStyle *) object)->data;

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
    AdgStyle *style = (AdgStyle *) object;

    switch (prop_id) {
    case PROP_PATTERN:
        set_pattern(style, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_style_register_id:
 * @style: an #AdgStyle derived instance
 *
 * Registers a new style in the internal register.
 *
 * Returns: the new id associated to this style or %0 on errors
 **/
AdgStyleId
adg_style_register_id(AdgStyle *style)
{
    GPtrArray *pool;

    g_return_val_if_fail(ADG_IS_STYLE(style), 0);

    pool = ADG_STYLE_GET_CLASS(style)->get_pool();
    g_return_val_if_fail(pool != NULL, 0);

    g_ptr_array_add(pool, style);

    return pool->len - 1;
}

/**
 * adg_style_from_id:
 * @type: the style type id
 * @id: the id to get
 *
 * Gets the preregistered style identified by @id of @style family.
 *
 * Returns: the requested style or %NULL on errors
 **/
AdgStyle *
adg_style_from_id(GType type, AdgStyleId id)
{
    AdgStyleClass *klass;
    GPtrArray *pool;

    klass = g_type_class_ref(type);
    g_return_val_if_fail(ADG_IS_STYLE_CLASS(klass), NULL);
    pool = klass->get_pool();

    /* If @type is valid, @klass will be referenced by the pool items */
    g_type_class_unref(klass);

    if (id > pool->len)
        return NULL;

    return (AdgStyle *) g_ptr_array_index(pool, id);
}

AdgStyle *
adg_style_get_default(AdgStyleClass *klass)
{
    GPtrArray *pool;

    g_return_val_if_fail(ADG_IS_STYLE_CLASS(klass), NULL);

    pool = klass->get_pool();
    return (AdgStyle *) g_ptr_array_index(pool, 0);
}

/**
 * adg_style_apply:
 * @style: an #AdgStyle derived object
 * @cr: the cairo context
 *
 * Applies @style to @cr so the next rendering will be done accordling to
 * this style directives.
 **/
void
adg_style_apply(AdgStyle *style, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_STYLE(style));
    g_return_if_fail(cr != NULL);

    return ADG_STYLE_GET_CLASS(style)->apply(style, cr);
}

/**
 * adg_style_get_pattern:
 * @style: an #AdgStyle object
 *
 * Gets the pattern binded to this style. The returned pattern refers to
 * an internally managed struct that must not be modified or freed.
 *
 * Returns: the requested pattern or %NULL on no pattern or errors
 **/
const AdgPattern *
adg_style_get_pattern(AdgStyle *style)
{
    AdgStylePrivate *data;

    g_return_val_if_fail(ADG_IS_STYLE(style), NULL);

    data = style->data;

    return data->pattern;
}

/**
 * adg_style_set_pattern:
 * @style: an #AdgStyle object
 * @pattern: the new pattern
 *
 * Sets a new pattern for this style. This operation will release one
 * reference on the old pattern (if any) and will add a new reference
 * to @pattern.
 *
 * A %NULL pattern is allowed: it means the previous pattern is kept
 * during the rendering process.
 **/
void
adg_style_set_pattern(AdgStyle *style, AdgPattern *pattern)
{
    g_return_if_fail(ADG_IS_STYLE(style));
    g_return_if_fail(pattern != NULL);

    set_pattern(style, pattern);
    g_object_notify((GObject *) style, "pattern");
}


static GPtrArray *
get_pool(void)
{
    g_warning("Uninmplemented get_pool()");
    return NULL;
}

static void
apply(AdgStyle *style, cairo_t *cr)
{
    AdgStylePrivate *data = style->data;

    if (data->pattern != NULL)
        cairo_set_source(cr, data->pattern);
}

static void
set_pattern(AdgStyle *style, AdgPattern *pattern)
{
    AdgStylePrivate *data = style->data;

    if (data->pattern != NULL)
        cairo_pattern_destroy(data->pattern);

    if (pattern != NULL)
        cairo_pattern_reference(pattern);

    data->pattern = pattern;
}
