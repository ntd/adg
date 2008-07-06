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
 * SECTION:adgfontstyle
 * @title: AdgFontStyle
 * @short_description: font style related stuff
 *
 * Contains parameters on how to draw texts such as font family, slanting,
 * weight, hinting and so on.
 */

#include "adg-font-style.h"
#include "adg-font-style-private.h"
#include "adg-intl.h"
#include "adg-util.h"

#define PARENT_CLASS ((AdgStyleClass *) adg_font_style_parent_class)


enum
{
  PROP_0,
  PROP_FAMILY,
  PROP_SLANT,
  PROP_WEIGHT,
  PROP_SIZE,
  PROP_ANTIALIAS,
  PROP_SUBPIXEL_ORDER,
  PROP_HINT_STYLE,
  PROP_HINT_METRICS
};


static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);


G_DEFINE_TYPE (AdgFontStyle, adg_font_style, ADG_TYPE_STYLE)


static void
adg_font_style_class_init (AdgFontStyleClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgFontStylePrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  param = g_param_spec_string ("family",
			       P_("Font Family"),
			       P_("The font family name, encoded in UTF-8"),
			       NULL,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_FAMILY, param);

  param = g_param_spec_int ("slant",
			    P_("Font Slant"),
			    P_("Variant of a font face based on its slant"),
			    G_MININT, G_MAXINT, CAIRO_FONT_SLANT_NORMAL,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_SLANT, param);

  param = g_param_spec_int ("weight",
			    P_("Font Weight"),
			    P_("Variant of a font face based on its weight"),
			    G_MININT, G_MAXINT, CAIRO_FONT_WEIGHT_NORMAL,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_WEIGHT, param);

  param = g_param_spec_double ("size",
			       P_("Font Size"),
			       P_("Font size in user space units"),
			       0., G_MAXDOUBLE, 10.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_SIZE, param);

  param = g_param_spec_int ("antialias",
			    P_("Font Antialiasing Mode"),
			    P_("Type of antialiasing to do when rendering text"),
			    G_MININT, G_MAXINT, CAIRO_ANTIALIAS_DEFAULT,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_ANTIALIAS, param);

  param = g_param_spec_int ("subpixel-order",
			    P_("Font Subpixel Order"),
			    P_("The order of color elements within each pixel on the display device when rendering with subpixel antialiasing mode"),
			    G_MININT, G_MAXINT, CAIRO_SUBPIXEL_ORDER_DEFAULT,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_SUBPIXEL_ORDER, param);

  param = g_param_spec_int ("hint-style",
			    P_("Type of Hinting"),
			    P_("How outlines must fit to the pixel grid in order to improve the glyph appearance"),
			    G_MININT, G_MAXINT, CAIRO_HINT_STYLE_DEFAULT,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_HINT_STYLE, param);

  param = g_param_spec_int ("hint-metrics",
			    P_("Font Metric Hinting"),
			    P_("Whether to hint font metrics, that is align them to integer values in device space"),
			    G_MININT, G_MAXINT, CAIRO_HINT_METRICS_DEFAULT,
			    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_HINT_METRICS, param);
}

static void
adg_font_style_init (AdgFontStyle *font_style)
{
  AdgFontStylePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (font_style,
							   ADG_TYPE_FONT_STYLE,
							   AdgFontStylePrivate);

  priv->family = NULL;
  priv->slant = CAIRO_FONT_SLANT_NORMAL;
  priv->weight = CAIRO_FONT_WEIGHT_NORMAL;
  priv->size = 10.;
  priv->antialias = CAIRO_ANTIALIAS_DEFAULT;
  priv->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
  priv->hint_style = CAIRO_HINT_STYLE_DEFAULT;
  priv->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;

  font_style->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgFontStyle *font_style = (AdgFontStyle *) object;

  switch (prop_id)
    {
    case PROP_FAMILY:
      g_value_set_string (value, font_style->priv->family);
      break;
    case PROP_SLANT:
      g_value_set_int (value, font_style->priv->slant);
      break;
    case PROP_WEIGHT:
      g_value_set_int (value, font_style->priv->weight);
      break;
    case PROP_SIZE:
      g_value_set_double (value, font_style->priv->size);
      break;
    case PROP_ANTIALIAS:
      g_value_set_int (value, font_style->priv->antialias);
      break;
    case PROP_SUBPIXEL_ORDER:
      g_value_set_int (value, font_style->priv->subpixel_order);
      break;
    case PROP_HINT_STYLE:
      g_value_set_int (value, font_style->priv->hint_style);
      break;
    case PROP_HINT_METRICS:
      g_value_set_int (value, font_style->priv->hint_metrics);
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
  AdgFontStyle *font_style = (AdgFontStyle *) object;

  switch (prop_id)
    {
    case PROP_FAMILY:
      font_style->priv->family = g_value_dup_string (value);
      break;
    case PROP_SLANT:
      font_style->priv->slant = g_value_get_int (value);
      break;
    case PROP_WEIGHT:
      font_style->priv->weight = g_value_get_int (value);
      break;
    case PROP_SIZE:
      font_style->priv->size = g_value_get_double (value);
      break;
    case PROP_ANTIALIAS:
      font_style->priv->antialias = g_value_get_int (value);
      break;
    case PROP_SUBPIXEL_ORDER:
      font_style->priv->subpixel_order = g_value_get_int (value);
      break;
    case PROP_HINT_STYLE:
      font_style->priv->hint_style = g_value_get_int (value);
      break;
    case PROP_HINT_METRICS:
      font_style->priv->hint_metrics = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


AdgStyle *
adg_font_style_from_id (AdgFontStyleId id)
{
  static AdgStyle **builtins = NULL;

  if G_UNLIKELY (builtins == NULL)
    {
      builtins = g_new (AdgStyle *, ADG_FONT_STYLE_LAST);

      builtins[ADG_FONT_STYLE_TEXT] = g_object_new (ADG_TYPE_FONT_STYLE,
						    "family", "Serif",
						    "size", 14.,
						    NULL);
      builtins[ADG_FONT_STYLE_QUOTE] = g_object_new (ADG_TYPE_FONT_STYLE,
						     "family", "Sans",
						     "size", 12.,
						     "weight", CAIRO_FONT_WEIGHT_BOLD,
						     NULL);
      builtins[ADG_FONT_STYLE_TOLERANCE] = g_object_new (ADG_TYPE_FONT_STYLE,
							 "family", "Sans",
							 "size", 8.,
							 NULL);
      builtins[ADG_FONT_STYLE_NOTE] = g_object_new (ADG_TYPE_FONT_STYLE,
						    "family", "Sans",
						    "size", 12.,
						    NULL);
    }

  g_return_val_if_fail (id < ADG_FONT_STYLE_LAST, NULL);
  return builtins[id];
}

void
adg_font_style_apply (const AdgFontStyle *font_style,
                      cairo_t            *cr)
{
  cairo_font_options_t *options;
  cairo_matrix_t        matrix;
  double                size;
  cairo_matrix_t        font_matrix;

  g_return_if_fail (font_style != NULL);
  g_return_if_fail (cr != NULL);

  cairo_get_matrix (cr, &matrix);
  size = font_style->priv->size;

  if (font_style->priv->family)
    cairo_select_font_face (cr, font_style->priv->family,
			    font_style->priv->slant, font_style->priv->weight);

  cairo_matrix_init_scale (&font_matrix,
			   size / (matrix.xx - matrix.yx),
			   size / (matrix.yy + matrix.xy));
  cairo_set_font_matrix (cr, &font_matrix);

  options = cairo_font_options_create ();

  cairo_font_options_set_antialias (options, font_style->priv->antialias);
  cairo_font_options_set_subpixel_order (options, font_style->priv->subpixel_order);
  cairo_font_options_set_hint_style (options, font_style->priv->hint_style);
  cairo_font_options_set_hint_metrics (options, font_style->priv->hint_metrics);

  cairo_set_font_options (cr, options);
  cairo_font_options_destroy (options);
}
