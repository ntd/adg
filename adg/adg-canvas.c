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
 * SECTION:adgcanvas
 * @title: AdgCanvas
 * @short_description: The drawing container
 *
 * This container represents the object where the rendering process draws. All
 * the drawing must have a canvas, and only one, as master parent (as all the
 * #GtkWidget must have a #GtkWindow).
 *
 * Internally, the target is mantained as a #cairo_t context pointer.
 */

#include "adg-canvas.h"
#include "adg-canvas-private.h"
#include "adg-line-style.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgContainerClass *) adg_canvas_parent_class)


enum
{
  PROP_0,
  PROP_LINE_STYLE,
  PROP_FONT_STYLE,
  PROP_ARROW_STYLE,
  PROP_DIM_STYLE
};


static void	get_property	(GObject	*object,
				 guint		 prop_id,
				 GValue		*value,
				 GParamSpec	*pspec);
static void	set_property	(GObject	*object,
				 guint		 prop_id,
				 const GValue	*value,
				 GParamSpec	*pspec);
static const AdgLineStyle *
		get_line_style	(AdgEntity	*entity);
static void	set_line_style	(AdgEntity	*entity,
				 AdgLineStyle	*line_style);
static const AdgFontStyle *
		get_font_style	(AdgEntity	*entity);
static void	set_font_style	(AdgEntity	*entity,
				 AdgFontStyle	*font_style);
static const AdgArrowStyle *
		get_arrow_style	(AdgEntity	*entity);
static void	set_arrow_style	(AdgEntity	*entity,
				 AdgArrowStyle	*arrow_style);
static const AdgDimStyle *
		get_dim_style	(AdgEntity	*entity);
static void	set_dim_style	(AdgEntity	*entity,
				 AdgDimStyle	*dim_style);


G_DEFINE_TYPE (AdgCanvas, adg_canvas, ADG_TYPE_CONTAINER);


static void
adg_canvas_class_init (AdgCanvasClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgCanvasPrivate));

  gobject_class->set_property = set_property;
  gobject_class->get_property = get_property;

  entity_class->get_line_style = get_line_style;
  entity_class->set_line_style = set_line_style;
  entity_class->get_font_style = get_font_style;
  entity_class->set_font_style = set_font_style;
  entity_class->get_arrow_style = get_arrow_style;
  entity_class->set_arrow_style = set_arrow_style;
  entity_class->get_dim_style = get_dim_style;
  entity_class->set_dim_style = set_dim_style;

  param = g_param_spec_boxed ("line-style",
                              P_("Line Style"),
                              P_("Fallback line style of the canvas"),
                              ADG_TYPE_LINE_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_LINE_STYLE, param);

  param = g_param_spec_boxed ("font-style",
                              P_("Font Style"),
                              P_("Fallback font style of the canvas"),
                              ADG_TYPE_FONT_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_FONT_STYLE, param);

  param = g_param_spec_boxed ("arrow-style",
                              P_("Arrow Style"),
                              P_("Fallback arrow style of the canvas"),
                              ADG_TYPE_ARROW_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_ARROW_STYLE, param);

  param = g_param_spec_boxed ("dim-style",
                              P_("Dimension Style"),
                              P_("Fallback dimension style of the canvas"),
                              ADG_TYPE_DIM_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_DIM_STYLE, param);
}

static void
adg_canvas_init (AdgCanvas *canvas)
{
  AdgCanvasPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (canvas,
							ADG_TYPE_CANVAS,
							AdgCanvasPrivate);

  priv->line_style = (AdgLineStyle *) adg_line_style_from_id (ADG_LINE_STYLE_DRAW);
  priv->font_style = adg_font_style_from_id (ADG_FONT_STYLE_TEXT);
  priv->arrow_style = adg_arrow_style_from_id (ADG_ARROW_STYLE_ARROW);
  priv->dim_style = adg_dim_style_from_id (ADG_DIM_STYLE_ISO);

  canvas->priv = priv;
}


static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgCanvasPrivate *priv = ((AdgCanvas *) object)->priv;

  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      g_value_set_boxed (value, priv->line_style);
      break;
    case PROP_FONT_STYLE:
      g_value_set_boxed (value, priv->font_style);
      break;
    case PROP_ARROW_STYLE:
      g_value_set_boxed (value, priv->arrow_style);
      break;
    case PROP_DIM_STYLE:
      g_value_set_boxed (value, priv->dim_style);
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
  AdgEntity *entity = (AdgEntity *) object;

  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      set_line_style (entity, g_value_get_boxed (value));
      break;
    case PROP_FONT_STYLE:
      set_font_style (entity, g_value_get_boxed (value));
      break;
    case PROP_ARROW_STYLE:
      set_arrow_style (entity, g_value_get_boxed (value));
      break;
    case PROP_DIM_STYLE:
      set_dim_style (entity, g_value_get_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


AdgCanvas *
adg_canvas_new (void)
{
  return g_object_new (ADG_TYPE_CANVAS, NULL);
}


static const AdgLineStyle *
get_line_style (AdgEntity *entity)
{
  return ((AdgCanvas *) entity)->priv->line_style;
}

static void
set_line_style (AdgEntity    *entity,
		AdgLineStyle *line_style)
{
  ((AdgCanvas *) entity)->priv->line_style = line_style;
  g_object_notify ((GObject *) entity, "line-style");
}

static const AdgFontStyle *
get_font_style (AdgEntity *entity)
{
  return ((AdgCanvas *) entity)->priv->font_style;
}

static void
set_font_style (AdgEntity    *entity,
		AdgFontStyle *font_style)
{
  ((AdgCanvas *) entity)->priv->font_style = font_style;
  g_object_notify ((GObject *) entity, "font-style");
}

static const AdgArrowStyle *
get_arrow_style (AdgEntity *entity)
{
  return ((AdgCanvas *) entity)->priv->arrow_style;
}

static void
set_arrow_style (AdgEntity     *entity,
		 AdgArrowStyle *arrow_style)
{
  ((AdgCanvas *) entity)->priv->arrow_style = arrow_style;
  g_object_notify ((GObject *) entity, "arrow-style");
}

static const AdgDimStyle *
get_dim_style (AdgEntity *entity)
{
  return ((AdgCanvas *) entity)->priv->dim_style;
}

static void
set_dim_style (AdgEntity   *entity,
	       AdgDimStyle *dim_style)
{
  ((AdgCanvas *) entity)->priv->dim_style = dim_style;
  g_object_notify ((GObject *) entity, "dim-style");
}
