/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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
 * This is the fundamental abstract class for styles.
 *
 * Since: 1.0
 **/

/**
 * AdgStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgStyleClass:
 * @invalidate: virtual method to reset the style.
 * @apply:      abstract virtual to apply a style to a cairo context.
 *
 * The default @invalidate handler does not do anything.
 *
 * The virtual method @apply *must* be implemented by any derived class.
 * The default implementation will trigger an error if called.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-style.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_style_parent_class)


G_DEFINE_ABSTRACT_TYPE(AdgStyle, adg_style, G_TYPE_OBJECT)

enum {
    INVALIDATE,
    APPLY,
    LAST_SIGNAL
};

static void             _adg_dispose            (GObject        *object);
static void             _adg_apply              (AdgStyle       *style,
                                                 AdgEntity      *entity,
                                                 cairo_t        *cr);
static guint            _adg_signals[LAST_SIGNAL] = { 0 };


static void
adg_style_class_init(AdgStyleClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = (GObjectClass *) klass;

    gobject_class->dispose = _adg_dispose;

    klass->clone = (AdgStyle *(*)(AdgStyle *)) adg_object_clone;
    klass->invalidate = NULL;
    klass->apply = _adg_apply;

    /**
     * AdgStyle::invalidate:
     * @style: an #AdgStyle
     *
     * Invalidates the @style, that is resets all the cache, if any,
     * retained by the internal implementation.
     *
     * This signal is emitted while disposing @style, so be sure it
     * can be called more than once without harms. Furthermore it
     * will be emitted from property setter code of new implementations
     * to force the recomputation of the cache.
     *
     * Since: 1.0
     **/
    _adg_signals[INVALIDATE] =
        g_signal_new("invalidate",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(AdgStyleClass, invalidate),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgStyle::apply:
     * @style: an #AdgStyle
     * @entity: the caller #AdgEntity
     * @cr: the #cairo_t context
     *
     * Applies @style to @cr so the next rendering operations will be
     * done accordling to this style directives. The @entity parameter
     * is used to resolve the internal dresses of @style, if any.
     *
     * Since: 1.0
     **/
    _adg_signals[APPLY] =
        g_signal_new("apply",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(AdgStyleClass, apply),
                     NULL, NULL,
                     adg_marshal_VOID__OBJECT_POINTER,
                     G_TYPE_NONE, 2, ADG_TYPE_ENTITY, G_TYPE_POINTER);
}

static void
adg_style_init(AdgStyle *style)
{
}

static void
_adg_dispose(GObject *object)
{
    g_signal_emit(object, _adg_signals[INVALIDATE], 0);

    if (_ADG_OLD_OBJECT_CLASS->dispose != NULL)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}


/**
 * adg_style_invalidate:
 * @style: an #AdgStyle derived style
 *
 * Emits the #AdgStyle::invalidate signal on @style. This signal
 * is always emitted while disposing @style, so be sure it
 * can be called more than once without harms.
 *
 * <note><para>
 * This function is only useful in new style implementations.
 * </para></note>
 *
 * Since: 1.0
 **/
void
adg_style_invalidate(AdgStyle *style)
{
    g_return_if_fail(ADG_IS_STYLE(style));

    g_signal_emit(style, _adg_signals[INVALIDATE], 0);
}

/**
 * adg_style_clone:
 * @style: (transfer none): an #AdgStyle derived style
 *
 * Clones @style. Useful for customizing styles.
 *
 * Returns: (transfer full): a newly created style.
 *
 * Since: 1.0
 **/
AdgStyle *
adg_style_clone(AdgStyle *style)
{
    AdgStyleClass *klass;

    g_return_val_if_fail(ADG_IS_STYLE(style), NULL);

    klass = ADG_STYLE_GET_CLASS(style);

    if (klass->clone == NULL)
        return NULL;

    return klass->clone(style);
}

/**
 * adg_style_apply:
 * @style: an #AdgStyle derived style
 * @entity: the caller #AdgEntity
 * @cr: the subject cairo context
 *
 * Emits the #AdgStyle::apply signal on @style, passing @entity and
 * @cr as parameters to the signal.
 *
 * Since: 1.0
 **/
void
adg_style_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_STYLE(style));
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(cr != NULL);

    g_signal_emit(style, _adg_signals[APPLY], 0, entity, cr);
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    /* The apply method must be defined */
    g_warning(_("%s: 'apply' method not implemented for type '%s'"),
              G_STRLOC, g_type_name(G_OBJECT_TYPE(style)));
}
