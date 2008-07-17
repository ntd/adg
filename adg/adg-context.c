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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

/**
 * SECTION:context
 * @title: AdgContext
 * @short_description: Base class for styling the rendering process
 *
 * The context is a collection of styles, one for each #AdgStyle derived
 * class. To achieve this result, there is a common register where all
 * the style derived class are stored and every #AdgContext instance keeps
 * its own register of style instances (one instance per class).
 *
 * Although this could be implemented by accessing the underlying registers
 * using GType id, to be able to access efficiently, that is O(1) magnitude,
 * the concept of slot was introduced.
 *
 * The slot serves the same purpose than a GType, that is identify a type
 * class, but the slots are a strict sequence starting from 0, useful to be
 * used as an array index.
 **/

/**
 * AdgContextFiller:
 * @style_class: the new style class
 * @user_data: the user provided data
 *
 * Callback to be used to get the default instance for
 * newly registered style classes.
 *
 * Return value: the instance to use for the passed in style class
 **/

#include "adg-context.h"
#include "adg-context-private.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_context_parent_class)


static AdgStyle *	context_filler		(AdgStyleClass	*style_class,
						 gpointer	 user_data);
static void		fill_style_slots	(AdgContext	*context,
						 guint		 last_slot);

static GPtrArray *class_slots = NULL;


G_DEFINE_TYPE(AdgContext, adg_context, G_TYPE_OBJECT)


static void
adg_context_class_init(AdgContextClass *klass)
{
    g_type_class_add_private(klass, sizeof(AdgContextPrivate));
}

static void
adg_context_init(AdgContext *context)
{
    AdgContextPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(context,
							  ADG_TYPE_CONTEXT,
							  AdgContextPrivate);

    priv->style_slots = g_ptr_array_sized_new(class_slots ?
					      class_slots->len : 10);
    priv->context_filler = context_filler;
    priv->user_data = NULL;

    context->priv = priv;
}


/**
 * adg_context_get_slot:
 * @type: an #AdgStyle type
 *
 * Gets the slot associated to @type. If not found, registers a new
 * style family by assigning a new slot id to it: the internal register
 * will keep a reference to this style class.
 *
 * Return value: the requested slot id
 **/
AdgStyleSlot
adg_context_get_slot(GType type)
{
    AdgStyleClass *klass = g_type_class_ref(type);

    g_return_val_if_fail(ADG_IS_STYLE_CLASS(klass), -1);

    if (G_UNLIKELY(class_slots == NULL)) {
	class_slots = g_ptr_array_sized_new(10);
    } else {
	guint n;
	for (n = 0; n < class_slots->len; ++n)
	    if (class_slots->pdata[n] == klass)
		return n;
    }

    g_ptr_array_add(class_slots, klass);
    return class_slots->len - 1;
}

/**
 * adg_context_new:
 * @context_filler: callback to use to fill uninitialized styles
 * @user_data: additional pointer to pass to @context_filler
 *
 * Constructs a new empty context. If @context_filler is %NULL, the default
 * filler callback will be used (the new style instances will be %NULL).
 *
 * Return value: a new context
 **/
AdgContext *
adg_context_new(AdgContextFiller context_filler, gpointer user_data)
{
    AdgContext *context = g_object_new(ADG_TYPE_CONTEXT, NULL);

    if (context_filler) {
	context->priv->context_filler = context_filler;
	context->priv->user_data = user_data;
    }

    return context;
}

/**
 * adg_context_get_style:
 * @context: an #AdgContext instance
 * @slot: the style slot where to get the style
 *
 * Gets a style instance from the specified @slot of @context.
 *
 * Return value: the style instance
 **/
AdgStyle *
adg_context_get_style(AdgContext *context, AdgStyleSlot slot)
{
    GPtrArray *style_slots;

    g_return_val_if_fail(ADG_IS_CONTEXT(context), NULL);
    g_return_val_if_fail(slot >= 0 && slot < class_slots->len, NULL);

    style_slots = context->priv->style_slots;

    if (G_UNLIKELY(slot >= style_slots->len))
	fill_style_slots(context, slot);

    return (AdgStyle *) g_ptr_array_index(style_slots, slot);
}

/**
 * adg_context_set_style:
 * @context: an #AdgContext instance
 * @style: the new style to include
 *
 * Sets a new style inside @context. The old style (if any)
 * will be unreferenced while a new reference will be added to @style.
 **/
void
adg_context_set_style(AdgContext *context, AdgStyle *style)
{
    AdgStyleSlot slot;
    GPtrArray *style_slots;

    g_return_if_fail(ADG_IS_CONTEXT(context));
    g_return_if_fail(ADG_IS_STYLE(style));

    slot = adg_context_get_slot(G_TYPE_FROM_INSTANCE(style));
    style_slots = context->priv->style_slots;

    g_object_ref(style);

    if (G_LIKELY(slot < style_slots->len)) {
	g_object_unref(style_slots->pdata[slot]);
	style_slots->pdata[slot] = style;
    } else {
	fill_style_slots(context, slot - 1);
	g_ptr_array_add(style_slots, style);
    }
}


static AdgStyle *
context_filler(AdgStyleClass *style_class, gpointer user_data)
{
    return NULL;
}

static void
fill_style_slots(AdgContext *context, guint last_slot)
{
    GPtrArray *style_slots;
    AdgStyleClass *klass;
    AdgStyle *style;
    guint n;

    style_slots = context->priv->style_slots;

    for (n = style_slots->len; n <= last_slot; ++n) {
	klass = (AdgStyleClass *) g_ptr_array_index(class_slots, n);
	style =
	    context->priv->context_filler(klass, context->priv->user_data);
	g_object_ref((GObject *) style);
	g_ptr_array_add(style_slots, style);
    }
}
