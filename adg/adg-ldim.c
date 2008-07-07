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
 * SECTION:ldim
 * @title: AdgLDim
 * @short_description: Linear dimensions
 *
 * The #AdgLDim entity represents a linear dimension.
 */

#include "adg-ldim.h"
#include "adg-ldim-private.h"
#include "adg-dim-private.h"
#include "adg-dim-style.h"
#include "adg-dim-style-private.h"
#include "adg-container.h"
#include "adg-arrow-style.h"
#include "adg-util.h"
#include "adg-intl.h"

#include <gcontainer/gcontainer.h>

#define PARENT_CLASS ((AdgDimClass *) adg_ldim_parent_class)


enum
{
  PROP_0,
  PROP_DIRECTION
};


static void	finalize		(GObject	*object);
static void	get_property		(GObject	*object,
					 guint		 param_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 param_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	update			(AdgEntity	*entity);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static gchar *	default_label		(AdgDim		*dim);


G_DEFINE_TYPE (AdgLDim, adg_ldim, ADG_TYPE_DIM);


static void
adg_ldim_class_init (AdgLDimClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  AdgDimClass    *dim_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;
  dim_class = (AdgDimClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgLDimPrivate));

  gobject_class->finalize = finalize;
  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  entity_class->render = render;

  dim_class->default_label = default_label;

  param = g_param_spec_double ("direction",
			       P_("Direction"),
			       P_("The inclination angle of the extension lines"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, CPML_DIR_RIGHT,
			       G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
  g_object_class_install_property (gobject_class, PROP_DIRECTION, param);
}

static void
adg_ldim_init (AdgLDim *ldim)
{
  AdgLDimPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (ldim, ADG_TYPE_LDIM,
						      AdgLDimPrivate);

  priv->direction = CPML_DIR_RIGHT;

  priv->extension1.status = CAIRO_STATUS_SUCCESS;
  priv->extension1.data = NULL;
  priv->extension1.num_data = 4;

  priv->extension2.status = CAIRO_STATUS_SUCCESS;
  priv->extension2.data = NULL;
  priv->extension2.num_data = 4;

  priv->arrow_path.status = CAIRO_STATUS_SUCCESS;
  priv->arrow_path.data = NULL;
  priv->arrow_path.num_data = 4;

  priv->baseline.status = CAIRO_STATUS_SUCCESS;
  priv->baseline.data = NULL;
  priv->baseline.num_data = 4;

  ldim->priv = priv;
}

static void
finalize (GObject *object)
{
  AdgLDimPrivate *priv = ((AdgLDim *) object)->priv;

  g_free (priv->extension1.data);
  g_free (priv->extension2.data);
  g_free (priv->arrow_path.data);
  g_free (priv->baseline.data);
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgLDim *ldim = ADG_LDIM (object);

  switch (prop_id)
    {
    case PROP_DIRECTION:
      g_value_set_double (value, ldim->priv->direction);
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
  AdgLDim *ldim = ADG_LDIM (object);
  
  switch (prop_id)
    {
    case PROP_DIRECTION:
      ldim->priv->direction = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
update (AdgEntity *entity)
{
  AdgDim             *dim;
  AdgLDim            *ldim;
  AdgDimStylePrivate *style_data;
  AdgMatrix           device2user;
  CpmlPair            vector;
  AdgPair             offset;
  AdgPair             from1, to1;
  AdgPair             from2, to2;
  AdgPair             arrow1, arrow2;
  AdgPair             baseline1, baseline2;
  cairo_path_data_t  *path_data;

  g_return_if_fail (((AdgDim *) entity)->priv->dim_style != NULL);

  dim = (AdgDim *) entity;
  ldim = (AdgLDim *) entity;
  style_data = dim->priv->dim_style->priv;

  /* Get the inverted transformation matrix */
  adg_matrix_set (&device2user, adg_entity_get_model_matrix (entity));
  g_return_if_fail (cairo_matrix_invert (&device2user) == CAIRO_STATUS_SUCCESS);

  /* Set vector to the director of the extension lines */
  cpml_vector_from_angle (&vector, ldim->priv->direction);

  /* Calculate from1 and from2 */
  offset.x = vector.x * style_data->from_offset;
  offset.y = vector.y * style_data->from_offset;
  cairo_matrix_transform_distance (&device2user, &offset.x, &offset.y);

  from1.x = dim->priv->ref1.x + offset.x;
  from1.y = dim->priv->ref1.y + offset.y;
  from2.x = dim->priv->ref2.x + offset.x;
  from2.y = dim->priv->ref2.y + offset.y;

  /* Calculate arrow1 and arrow2 */
  offset.x = vector.x * style_data->baseline_spacing * dim->priv->level;
  offset.y = vector.y * style_data->baseline_spacing * dim->priv->level;
  cairo_matrix_transform_distance (&device2user, &offset.x, &offset.y);

  arrow1.x = dim->priv->pos1.x + offset.x;
  arrow1.y = dim->priv->pos1.y + offset.y;
  arrow2.x = dim->priv->pos2.x + offset.x;
  arrow2.y = dim->priv->pos2.y + offset.y;

  /* Calculate to1 and to2 */
  offset.x = vector.x * style_data->to_offset;
  offset.y = vector.y * style_data->to_offset;
  cairo_matrix_transform_distance (&device2user, &offset.x, &offset.y);

  to1.x = arrow1.x + offset.x;
  to1.y = arrow1.y + offset.y;
  to2.x = arrow2.x + offset.x;
  to2.y = arrow2.y + offset.y;

  /* Set vector to the director of the baseline */
  offset.x = arrow2.x - arrow1.x;
  offset.y = arrow2.y - arrow1.y;
  cpml_vector_from_pair (&vector, &offset);

  /* Update the AdgDim cache contents */
  dim->priv->quote_org.x = (arrow1.x + arrow2.x) / 2.;
  dim->priv->quote_org.y = (arrow1.y + arrow2.y) / 2.;
  cpml_pair_angle (NULL, &vector, &dim->priv->quote_angle);

  /* Calculate baseline1 and baseline2 */
  g_object_get (style_data->arrow_style, "margin", &offset.y, NULL);
  offset.x = vector.x * offset.y;
  offset.y *= vector.y;
  cairo_matrix_transform_distance (&device2user, &offset.x, &offset.y);

  baseline1.x = arrow1.x + offset.x;
  baseline1.y = arrow1.y + offset.y;
  baseline2.x = arrow2.x - offset.x;
  baseline2.y = arrow2.y - offset.y;

  /* Set extension1 */
  if (ldim->priv->extension1.data == NULL)
    ldim->priv->extension1.data = g_new (cairo_path_data_t, 4);

  path_data = ldim->priv->extension1.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = from1.x;
  path_data[1].point.y = from1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = to1.x;
  path_data[3].point.y = to1.y;

  /* Set extension2 */
  if (ldim->priv->extension2.data == NULL)
    ldim->priv->extension2.data = g_new (cairo_path_data_t, 4);

  path_data = ldim->priv->extension2.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = from2.x;
  path_data[1].point.y = from2.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = to2.x;
  path_data[3].point.y = to2.y;

  /* Set arrow_path */
  if (ldim->priv->arrow_path.data == NULL)
    ldim->priv->arrow_path.data = g_new (cairo_path_data_t, 4);

  path_data = ldim->priv->arrow_path.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = arrow1.x;
  path_data[1].point.y = arrow1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = arrow2.x;
  path_data[3].point.y = arrow2.y;

  /* Set baseline */
  if (ldim->priv->baseline.data == NULL)
    ldim->priv->baseline.data = g_new (cairo_path_data_t, 4);

  path_data = ldim->priv->baseline.data;
  path_data[0].header.type = CAIRO_PATH_MOVE_TO;
  path_data[0].header.length = 2;
  path_data[1].point.x = baseline1.x;
  path_data[1].point.y = baseline1.y;
  path_data[2].header.type = CAIRO_PATH_LINE_TO;
  path_data[2].header.length = 2;
  path_data[3].point.x = baseline2.x;
  path_data[3].point.y = baseline2.y;
}

static void
render (AdgEntity *entity,
	cairo_t   *cr)
{
  AdgDim             *dim;
  AdgLDim            *ldim;
  AdgDimStylePrivate *style_data;
  AdgArrowStyle      *arrow_style;
  CpmlPath            primitive;

  g_return_if_fail (((AdgDim *) entity)->priv->dim_style != NULL);

  dim = (AdgDim *) entity;
  ldim = (AdgLDim *) entity;
  style_data = dim->priv->dim_style->priv;

  arrow_style = (AdgArrowStyle *) style_data->arrow_style;

  /* TODO: caching
  if (!adg_entity_model_applied (entity)) */
    update (entity);
    
  cairo_save (cr);

  /* Arrows */
  if (cpml_path_from_cairo (&primitive, &ldim->priv->arrow_path, NULL))
    {
      adg_arrow_style_render (arrow_style, cr, &primitive);
      if (cpml_primitive_reverse (&primitive))
	adg_arrow_style_render (arrow_style, cr, &primitive);
    }

  /* Lines */
  adg_line_style_apply (ADG_LINE_STYLE (style_data->line_style), cr);

  cairo_append_path (cr, &ldim->priv->extension1);
  cairo_append_path (cr, &ldim->priv->extension2);
  cairo_append_path (cr, &ldim->priv->baseline);

  cairo_stroke (cr);

  _adg_dim_render_quote (dim, cr);

  cairo_restore (cr);

  ((AdgEntityClass *) PARENT_CLASS)->render (entity, cr);
}

static gchar *
default_label (AdgDim *dim)
{
  double number;

  if (!cpml_pair_distance(&dim->priv->pos2, &dim->priv->pos1, &number))
    return NULL;

  return g_strdup_printf (dim->priv->dim_style->priv->number_format, number);
}


/**
 * adg_ldim_new:
 *
 * Creates a new - unreferenced - linear dimension. You must, at least, define
 * the reference points with adg_dim_set_ref(), the dimension direction with
 * adg_ldim_set_direction() and the position reference using adg_dim_set_pos()
 * or, better, adg_ldim_set_pos().
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new (void)
{
  return (AdgEntity *) g_object_new (ADG_TYPE_LDIM, NULL);
}

/**
 * adg_ldim_new_full:
 * @ref1: the first reference point
 * @ref2: the second reference point
 * @direction: angle where to extend the dimension
 * @pos: the position reference
 *
 * Creates a new linear dimension, specifing all the needed properties in
 * one shot.
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new_full (const AdgPair *ref1,
                   const AdgPair *ref2,
                   double         direction,
                   const AdgPair *pos)
{
  AdgEntity *entity = (AdgEntity *) g_object_new (ADG_TYPE_LDIM,
                                                  "ref1", ref1,
                                                  "ref2", ref2,
                                                  "direction", direction,
                                                  NULL);
  adg_ldim_set_pos ((AdgLDim *) entity, pos);
  return entity;
}

/**
 * adg_ldim_new_full_explicit:
 * @ref1_x: the x coordinate of the first reference point
 * @ref1_y: the y coordinate of the first reference point
 * @ref2_x: the x coordinate of the second reference point
 * @ref2_y: the y coordinate of the second reference point
 * @direction: angle where to extend the dimension
 * @pos_x: the x coordinate of the position reference
 * @pos_y: the y coordinate of the position reference
 *
 * Wrappes adg_ldim_new_full() with explicit quotes.
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new_full_explicit (double ref1_x,
                            double ref1_y,
                            double ref2_x,
                            double ref2_y,
                            double direction,
                            double pos_x,
                            double pos_y)
{
  AdgPair ref1;
  AdgPair ref2;
  AdgPair pos;

  ref1.x = ref1_x;
  ref1.y = ref1_y;
  ref2.x = ref2_x;
  ref2.y = ref2_y;
  pos.x = pos_x;
  pos.y = pos_y;

  return adg_ldim_new_full (&ref1, &ref2, direction, &pos);
}



/**
 * adg_ldim_set_pos:
 * @ldim: an #AdgLDim entity
 * @pos: an #AdgPair structure
 *
 * Sets the position references (pos1 and pos2 properties) of @ldim using a
 * single @pos point. Before this call, @ldim MUST HAVE defined the reference
 * points and the direction. If these conditions are not met, an error message
 * is logged and the position references will not be set.
 */
void
adg_ldim_set_pos (AdgLDim       *ldim,
                  const AdgPair *pos)
{
  AdgDim  *dim;
  GObject *object;
  CpmlPair extension_vector;
  CpmlPair baseline_vector;
  gdouble  d, k;

  g_return_if_fail (ADG_IS_LDIM (ldim));

  dim = (AdgDim *) ldim;
  object = (GObject *) ldim;

  cpml_vector_from_angle (&extension_vector, ldim->priv->direction);

  baseline_vector.x = -extension_vector.y;
  baseline_vector.y = extension_vector.x;

  d = extension_vector.y * baseline_vector.x -
      extension_vector.x * baseline_vector.y;
  g_return_if_fail (d != 0.);

  k = ((pos->y - dim->priv->ref1.y) * baseline_vector.x -
       (pos->x - dim->priv->ref1.x) * baseline_vector.y) / d;
  dim->priv->pos1.x = dim->priv->ref1.x + k*extension_vector.x;
  dim->priv->pos1.y = dim->priv->ref1.y + k*extension_vector.y;

  k = ((pos->y - dim->priv->ref2.y) * baseline_vector.x -
       (pos->x - dim->priv->ref2.x) * baseline_vector.y) / d;
  dim->priv->pos2.x = dim->priv->ref2.x + k*extension_vector.x;
  dim->priv->pos2.y = dim->priv->ref2.y + k*extension_vector.y;

  g_object_freeze_notify (object);
  g_object_notify (object, "pos1");
  g_object_notify (object, "pos2");
  g_object_thaw_notify (object);
}

/**
 * adg_ldim_set_pos_explicit:
 * @ldim: an #AdgLDim entity
 * @pos_x: the new x coordinate position reference
 * @pos_y: the new y coordinate position reference
 *
 * Wrappers adg_ldim_set_pos() with explicit coordinates.
 */
void
adg_ldim_set_pos_explicit (AdgLDim *ldim,
                           double   pos_x,
                           double   pos_y)
{
  AdgPair pos;

  pos.x = pos_x;
  pos.y = pos_y;

  adg_ldim_set_pos (ldim, &pos);
}

/**
 * adg_ldim_get_direction:
 * @ldim: an #AdgLDim entity
 *
 * Gets the direction where @ldim will extend.
 *
 * Return value: the direction angle in radians
 */
double
adg_ldim_get_direction (AdgLDim *ldim)
{
  g_return_val_if_fail (ADG_IS_LDIM (ldim), 0.);

  return ldim->priv->direction;
}

/**
 * adg_ldim_set_direction:
 * @ldim: an #AdgLDim entity
 * @direction: an angle value, in radians
 *
 * Sets the direction angle where to extend @ldim.
 */
void
adg_ldim_set_direction (AdgLDim *ldim,
                        double   direction)
{
  g_return_if_fail (ADG_IS_LDIM (ldim));

  ldim->priv->direction = direction;
  g_object_notify ((GObject *) ldim, "direction");
}
