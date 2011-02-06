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
 * SECTION:adg-style
 * @short_description: The base class of all styling objects
 *
 * This is the fundamental abstract class for styles.
 **/

/**
 * AdgStyle:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"

#include "adg-style.h"


G_DEFINE_ABSTRACT_TYPE(AdgStyle, adg_style, G_TYPE_OBJECT);


static void             _adg_apply      (AdgStyle       *style,
                                         AdgEntity      *entity,
                                         cairo_t        *cr);


static void
adg_style_class_init(AdgStyleClass *klass)
{
    klass->apply = _adg_apply;
}

static void
adg_style_init(AdgStyle *style)
{
}


/**
 * adg_style_apply:
 * @style: an #AdgStyle derived style
 * @entity: the caller #AdgEntity
 * @cr: the subject cairo context
 *
 * Applies @style to @cr so the next rendering operations will be
 * done accordling to this style directives. The @entity parameter
 * is used to resolve the internal dresses of @style, if any.
 **/
void
adg_style_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    AdgStyleClass *klass;

    g_return_if_fail(ADG_IS_STYLE(style));
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(cr != NULL);

    klass = ADG_STYLE_GET_CLASS(style);

    if (klass->apply)
        klass->apply(style, entity, cr);
}


static void
_adg_apply(AdgStyle *style, AdgEntity *entity, cairo_t *cr)
{
    /* The apply method must be defined */
    g_warning(_("%s: `apply' method not implemented for type `%s'"),
              G_STRLOC, g_type_name(G_OBJECT_TYPE(style)));
}
