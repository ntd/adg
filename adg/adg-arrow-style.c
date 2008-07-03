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
 * SECTION:adgarrowstyle
 * @title: AdgArrowStyle
 * @short_description: arrow rendering related stuff
 *
 * Contains parameters on how to draw arrows, providing a way to register a
 * custom rendering callback.
 */

#include "adg-arrow-style.h"
#include "adg-arrow-style-private.h"
#include "adg-intl.h"
#include "adg-util.h"

#include <math.h>

#define PARENT_CLASS ((AdgStyleClass *) adg_arrow_style_parent_class)


enum
{
  PROP_0,
  PROP_SIZE,
  PROP_ANGLE,
  PROP_MARGIN,
  PROP_RENDERER
};


static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	arrow_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	triangle_renderer	(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	dot_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	circle_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	block_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	square_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	tick_renderer		(AdgArrowStyle	*arrow_style,
					 cairo_t	*cr,
					 CpmlPath	*segment);
static void	draw_triangle		(cairo_t	*cr,
					 AdgArrowStyle	*arrow_style,
					 CpmlPath	*segment);
static void	draw_circle		(cairo_t	*cr,
					 AdgArrowStyle	*arrow_style,
					 CpmlPath	*segment);


G_DEFINE_TYPE (AdgArrowStyle, adg_arrow_style, ADG_TYPE_STYLE)


static void
adg_arrow_style_class_init (AdgArrowStyleClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgArrowStylePrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  param = g_param_spec_double ("size",
			       P_("Arrow Size"),
			       P_("The size of the arrow, a renderer dependent parameter"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, 14.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_SIZE, param);

  param = g_param_spec_double ("angle",
			       P_("Arrow Angle"),
			       P_("The angle of the arrow, a renderer dependent parameter"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, G_PI / 6.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_ANGLE, param);

  param = g_param_spec_double ("margin",
			       P_("Arrow Margin"),
			       P_("The margin of the arrow, a renderer dependent parameter"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, 14.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_MARGIN, param);

  param = g_param_spec_pointer ("renderer",
				P_("Renderer Callback"),
				P_("The callback to call to renderer this arrow type"),
				G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_RENDERER, param);
}

static void
adg_arrow_style_init (AdgArrowStyle *arrow_style)
{
  AdgArrowStylePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (arrow_style,
							    ADG_TYPE_ARROW_STYLE,
							    AdgArrowStylePrivate);

  priv->size = 14.;
  priv->angle = G_PI / 6.;
  priv->margin = 14.;
  priv->renderer = NULL;

  arrow_style->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgArrowStyle *arrow_style = (AdgArrowStyle *) object;

  switch (prop_id)
    {
    case PROP_SIZE:
      g_value_set_double (value, arrow_style->priv->size);
      break;
    case PROP_ANGLE:
      g_value_set_double (value, arrow_style->priv->angle);
      break;
    case PROP_MARGIN:
      g_value_set_double (value, arrow_style->priv->margin);
      break;
    case PROP_RENDERER:
      g_value_set_pointer (value, arrow_style->priv->renderer);
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
  AdgArrowStyle *arrow_style = (AdgArrowStyle *) object;

  switch (prop_id)
    {
    case PROP_SIZE:
      arrow_style->priv->size = g_value_get_double (value);
      break;
    case PROP_ANGLE:
      arrow_style->priv->angle = g_value_get_double (value);
      break;
    case PROP_MARGIN:
      arrow_style->priv->margin = g_value_get_double (value);
      break;
    case PROP_RENDERER:
      arrow_style->priv->renderer = g_value_get_pointer (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


AdgStyle *
adg_arrow_style_from_id (AdgArrowStyleId id)
{
  static AdgStyle **builtins = NULL;

  if G_UNLIKELY (builtins == NULL)
    {
      builtins = g_new (AdgStyle *, ADG_ARROW_STYLE_LAST);

      builtins[ADG_ARROW_STYLE_ARROW] = g_object_new (ADG_TYPE_ARROW_STYLE,
						      "renderer", arrow_renderer,
						      NULL);
      builtins[ADG_ARROW_STYLE_TRIANGLE] = g_object_new (ADG_TYPE_ARROW_STYLE,
							 "renderer", triangle_renderer,
							 NULL);
      builtins[ADG_ARROW_STYLE_DOT] = g_object_new (ADG_TYPE_ARROW_STYLE,
						    "size", 5.,
						    "angle", 0.,
						    "margin", 2.5,
						    "renderer", dot_renderer,
						    NULL);
      builtins[ADG_ARROW_STYLE_CIRCLE] = g_object_new (ADG_TYPE_ARROW_STYLE,
						       "size", 10.,
						       "angle", 0.,
						       "margin", 5.,
						       "renderer", circle_renderer,
						       NULL);
      builtins[ADG_ARROW_STYLE_BLOCK] = g_object_new (ADG_TYPE_ARROW_STYLE,
						      "size", 10.,
						      "angle", 0.,
						      "margin", 5.,
						      "renderer", block_renderer,
						      NULL);
      builtins[ADG_ARROW_STYLE_SQUARE] = g_object_new (ADG_TYPE_ARROW_STYLE,
						       "size", 10.,
						       "angle", 0.,
						       "margin", -0.1,
						       "renderer", square_renderer,
						       NULL);
      builtins[ADG_ARROW_STYLE_TICK] = g_object_new (ADG_TYPE_ARROW_STYLE,
						     "size", 20.,
						     "angle", G_PI / 3.,
						     "margin", 0.,
						     "renderer", tick_renderer,
						     NULL);
    }

  g_return_val_if_fail (id < ADG_ARROW_STYLE_LAST, NULL);
  return builtins[id];
}

void
adg_arrow_style_render (AdgArrowStyle *arrow_style,
			cairo_t       *cr,
			CpmlPath      *segment)
{
  g_return_if_fail (arrow_style != NULL);
  g_return_if_fail (cr != NULL);
  g_return_if_fail (segment != NULL);

  /* NULL renderer */
  if (arrow_style->priv->renderer == NULL)
    return;

  arrow_style->priv->renderer (arrow_style, cr, segment);
}

static void
arrow_renderer (AdgArrowStyle *arrow_style,
		cairo_t       *cr,
		CpmlPath      *segment)
{
  draw_triangle (cr, arrow_style, segment);
  cairo_fill (cr);
}

static void
triangle_renderer (AdgArrowStyle *arrow_style,
		   cairo_t       *cr,
		   CpmlPath      *segment)
{
  draw_triangle (cr, arrow_style, segment);
  cairo_stroke (cr);
}

static void
dot_renderer (AdgArrowStyle *arrow_style,
	      cairo_t       *cr,
	      CpmlPath      *segment)
{
  draw_circle (cr, arrow_style, segment);
  cairo_fill (cr);
}

static void
circle_renderer (AdgArrowStyle *arrow_style,
		 cairo_t       *cr,
		 CpmlPath      *segment)
{
  draw_circle (cr, arrow_style, segment);
  cairo_stroke (cr);
}

static void
block_renderer (AdgArrowStyle *arrow_style,
		cairo_t       *cr,
		CpmlPath      *segment)
{
  ADG_STUB ();
}

static void
square_renderer (AdgArrowStyle *arrow_style,
		 cairo_t       *cr,
		 CpmlPath      *segment)
{
  ADG_STUB ();
}

static void
tick_renderer (AdgArrowStyle *arrow_style,
	       cairo_t       *cr,
	       CpmlPath      *segment)
{
  ADG_STUB ();
}

static void
draw_triangle (cairo_t       *cr,
	       AdgArrowStyle *arrow_style,
	       CpmlPath      *segment)
{
  double   length, height_2;
  double   tmp;
  CpmlPair tail, tail1, tail2;
  CpmlPair vector;

  length = arrow_style->priv->size;
  height_2 = tan (arrow_style->priv->angle / 2.0) * length;
  cairo_device_to_user_distance (cr, &length, &height_2);

  switch (segment->cairo_path.data[0].header.type)
    {
    case CAIRO_PATH_LINE_TO:
      cpml_primitive_get_pair (segment, &vector, CPML_FIRST);
      vector.x -= segment->org.x;
      vector.y -= segment->org.y;
      cpml_vector_from_pair (&vector, &vector);

      tail.x = vector.x*length + segment->org.x;
      tail.y = vector.y*length + segment->org.y;

      tmp = vector.x;
      vector.x = -vector.y*height_2;
      vector.y = tmp*height_2;

      tail1.x = tail.x+vector.x;
      tail1.y = tail.y+vector.y;

      tail2.x = tail.x-vector.x;
      tail2.y = tail.y-vector.y;

      cairo_move_to (cr, segment->org.x, segment->org.y);
      cairo_line_to (cr, tail1.x, tail1.y);
      cairo_line_to (cr, tail2.x, tail2.y);
      cairo_close_path (cr);
      
      break;
    case CAIRO_PATH_CURVE_TO:
      ADG_STUB ();
      break;
    default:
      g_assert_not_reached ();
    }
}

static void
draw_circle (cairo_t       *cr,
	     AdgArrowStyle *arrow_style,
	     CpmlPath      *segment)
{
  double radius = arrow_style->priv->size / 2.;
  double dummy = 0.;

  cairo_device_to_user_distance (cr, &radius, &dummy);
  cairo_new_path (cr);
  cairo_arc (cr, segment->org.x, segment->org.y, radius, 0., M_PI);
}
