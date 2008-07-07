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
 */

#include "adg-context.h"
#include "adg-context-private.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_context_parent_class)


G_DEFINE_TYPE (AdgContext, adg_context, G_TYPE_OBJECT)


static void
adg_context_class_init (AdgContextClass *klass)
{
  g_type_class_add_private (klass, sizeof (AdgContextPrivate));
}

static void
adg_context_init (AdgContext *context)
{
  AdgContextPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (context,
							 ADG_TYPE_CONTEXT,
							 AdgContextPrivate);
  context->priv = priv;
}
