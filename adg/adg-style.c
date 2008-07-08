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
 * SECTION:style
 * @title: AdgStyle
 * @short_description: The base class of all styling objects.
 */

#include "adg-style.h"
#include "adg-style-private.h"
#include "adg-line-style.h"
#include "adg-font-style.h"
#include "adg-arrow-style.h"
#include "adg-enums.h"
#include "adg-util.h"
#include "adg-intl.h"

#include <string.h>

#define PARENT_CLASS ((GObjectClass *) adg_style_parent_class)


enum
{
  PROP_0,
  PROP_R,
  PROP_G,
  PROP_B,
  PROP_A
};


static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);


G_DEFINE_ABSTRACT_TYPE (AdgStyle, adg_style, G_TYPE_OBJECT)


static void
adg_style_class_init (AdgStyleClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgStylePrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  param = g_param_spec_double ("r",
			       P_("Red"),
			       P_("The red component of the color to be used, from 0 to 1"),
			       0., 1., 0.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_R, param);

  param = g_param_spec_double ("g",
			       P_("Green"),
			       P_("The green component of the color to be used, from 0 to 1"),
			       0., 1., 0.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_G, param);

  param = g_param_spec_double ("b",
			       P_("Blue"),
			       P_("The blue component of the color to be used, from 0 to 1"),
			       0., 1., 0.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_B, param);

  param = g_param_spec_double ("a",
			       P_("Alpha Component"),
			       P_("The alpha component (translucency) to be used, from 0 (totally transparent) to 1 (totally opaque)"),
			       0., 1., 1.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_A, param);
}

static void
adg_style_init (AdgStyle *style)
{
  AdgStylePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (style, ADG_TYPE_STYLE,
						       AdgStylePrivate);

  priv->r = 0.;
  priv->g = 0.;
  priv->b = 0.;
  priv->a = 1.;

  style->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgStyle *style = (AdgStyle *) object;

  switch (prop_id)
    {
    case PROP_R:
      g_value_set_double (value, style->priv->r);
      break;
    case PROP_G:
      g_value_set_double (value, style->priv->g);
      break;
    case PROP_B:
      g_value_set_double (value, style->priv->b);
      break;
    case PROP_A:
      g_value_set_double (value, style->priv->a);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
set_property (GObject      *object,
	      guint         prop_id,
	      const GValue *value,
	      GParamSpec   *pspec)
{
  AdgStyle *style = (AdgStyle *) object;

  switch (prop_id)
    {
    case PROP_R:
      style->priv->r = g_value_get_double (value);
      break;
    case PROP_G:
      style->priv->g = g_value_get_double (value);
      break;
    case PROP_B:
      style->priv->b = g_value_get_double (value);
      break;
    case PROP_A:
      style->priv->a = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}
