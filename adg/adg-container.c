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
 * SECTION:adgcontainer
 * @title: AdgContainer
 * @short_description: base class for entity that can contain
 *                     other entities
 *
 * The #AdgContainer is an entity that implements the #GContainerable interface.
 * Each AdgContainer has its current transformation matrix (#AdgContainer:matrix)
 * to be applied to all its children.
 * This means an AdgContainer can be thought as a group of entities with the
 * same geometrical identity (same scale, reference point ecc...).
 */

#include "adg-container.h"
#include "adg-container-private.h"
#include "adg-intl.h"

#include <gcontainer/gcontainer.h>

#define PARENT_CLASS ((AdgEntityClass *) adg_container_parent_class)


enum
{
  PROP_0,
  PROP_CHILD,
  PROP_MATRIX
};


static void	containerable_init
				(GContainerableIface *iface);
static void	get_property	(GObject	*object,
				 guint		 prop_id,
				 GValue		*value,
				 GParamSpec	*pspec);
static void	set_property	(GObject	*object,
				 guint		 prop_id,
				 const GValue	*value,
				 GParamSpec	*pspec);
static gboolean	set_matrix	(AdgContainer	*container,
				 AdgMatrix	*matrix);
static GSList *	get_children	(GContainerable	*containerable);
static gboolean	add		(GContainerable	*containerable,
				 GChildable	*childable);
static gboolean	remove		(GContainerable	*containerable,
				 GChildable	*childable);
static void	ctm_changed	(AdgEntity	*entity,
				 AdgMatrix	*dummy_ctm);
static const AdgMatrix *
		get_ctm		(AdgEntity	*entity);
static void	update		(AdgEntity	*entity,
				 gboolean	 recursive);
static void	outdate		(AdgEntity	*entity,
				 gboolean	 recursive);
static void	render		(AdgEntity	*entity,
				 cairo_t	*cr);


G_DEFINE_TYPE_EXTENDED (AdgContainer, adg_container,
                        ADG_TYPE_ENTITY, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_CONTAINERABLE,
                                               containerable_init));


static void
adg_container_class_init (AdgContainerClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgContainerPrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;
  gobject_class->dispose = g_containerable_dispose;

  entity_class->ctm_changed = ctm_changed;
  entity_class->get_ctm = get_ctm;
  entity_class->update = update;
  entity_class->outdate = outdate;
  entity_class->render = render;

  g_object_class_override_property (gobject_class, PROP_CHILD, "child");

  param = g_param_spec_boxed ("matrix",
                              P_("Matrix"),
                              P_("The transformation matrix apported by this container"),
                              ADG_TYPE_MATRIX,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_MATRIX, param);
}

static void
containerable_init (GContainerableIface *iface)
{
  iface->get_children = get_children;
  iface->add = add;
  iface->remove = remove;
}

static void
adg_container_init (AdgContainer *container)
{
  AdgContainerPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (container,
							   ADG_TYPE_CONTAINER,
							   AdgContainerPrivate);

  priv->children = NULL;
  cairo_matrix_init_identity (&priv->matrix);
  cairo_matrix_init_identity (&priv->ctm);

  container->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgContainer *container = (AdgContainer *) object;

  switch (prop_id)
    {
    case PROP_MATRIX:
      g_value_set_boxed (value, &container->priv->matrix);
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
  AdgContainer *container = (AdgContainer *) object;

  switch (prop_id)
    {
    case PROP_CHILD:
      g_containerable_add ((GContainerable *) container, g_value_get_object (value));
      break;
    case PROP_MATRIX:
      set_matrix (container, g_value_get_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static GSList *
get_children (GContainerable *containerable)
{
  AdgContainer *container = (AdgContainer *) containerable;

  return g_slist_copy (container->priv->children);
}

static gboolean
add (GContainerable *containerable,
     GChildable     *childable)
{
  AdgContainer *container = (AdgContainer *) containerable;

  container->priv->children = g_slist_append (container->priv->children, childable);
  return TRUE;
}

static gboolean
remove (GContainerable *containerable,
	GChildable     *childable)
{
  AdgContainer *container;
  GSList       *node;

  container = (AdgContainer *) containerable;
  node = g_slist_find (container->priv->children, childable);

  if (!node)
    return FALSE;

  container->priv->children = g_slist_delete_link (container->priv->children, node);
  return TRUE;
}


static void
ctm_changed (AdgEntity *entity,
	     AdgMatrix *dummy_ctm)
{
  AdgContainer *container;
  AdgContainer *parent;
  AdgMatrix     old_ctm;

  container = (AdgContainer *) entity;
  parent = (AdgContainer *) g_childable_get_parent ((GChildable *) entity);
  adg_matrix_set (&old_ctm, &container->priv->ctm);

  /* Refresh the ctm */
  if (ADG_IS_CONTAINER (parent))
    adg_matrix_set (&container->priv->ctm, &parent->priv->ctm);
  else
    cairo_matrix_init_identity (&container->priv->ctm);

  cairo_matrix_multiply (&container->priv->ctm, &container->priv->ctm,
			 &container->priv->matrix);

  /* Check for ctm != old_ctm */
  if (!adg_matrix_equal (&container->priv->ctm, &old_ctm))
    g_containerable_propagate_by_name ((GContainerable *) container,
                                       "ctm-changed", &old_ctm);
}

const AdgMatrix *
get_ctm (AdgEntity *entity)
{
  return &((AdgContainer *) entity)->priv->ctm;
}


static void
update (AdgEntity *entity,
	gboolean   recursive)
{
  if (recursive)
    g_containerable_foreach ((GContainerable *) entity,
                             G_CALLBACK (adg_entity_update_all), NULL);

  PARENT_CLASS->update (entity, recursive);
}

static void
outdate (AdgEntity *entity,
	 gboolean   recursive)
{
  if (recursive)
    g_containerable_foreach ((GContainerable *) entity,
                             G_CALLBACK (adg_entity_outdate_all), NULL);

  PARENT_CLASS->outdate (entity, recursive);
}

static void
render (AdgEntity *entity,
	cairo_t   *cr)
{
  cairo_set_matrix (cr, &((AdgContainer *) entity)->priv->ctm);
  g_containerable_foreach ((GContainerable *) entity,
			   G_CALLBACK (adg_entity_render), cr);
}


AdgMatrix *
adg_container_get_matrix (AdgContainer *container)
{
  g_return_val_if_fail (ADG_IS_CONTAINER (container), NULL);

  return &container->priv->matrix;
}

void
adg_container_set_matrix (AdgContainer *container,
                          AdgMatrix    *matrix)
{
  g_return_if_fail (ADG_IS_CONTAINER (container));

  if (set_matrix (container, matrix))
    g_object_notify ((GObject *) container, "matrix");
}


void
adg_container_scale (AdgContainer *container,
                     AdgPair      *factor)
{
  g_return_if_fail (factor != NULL);

  adg_container_scale_explicit (container, factor->x, factor->y);
}

void
adg_container_scale_explicit (AdgContainer *container,
                              double        sx,
                              double        sy)
{
  AdgMatrix matrix;

  g_return_if_fail (ADG_IS_CONTAINER (container));

  adg_matrix_set (&matrix, &container->priv->matrix);
  matrix.xx = sx;
  matrix.yy = sy;

  if (set_matrix (container, &matrix))
    g_object_notify ((GObject *) container, "matrix");
}

void
adg_container_translate (AdgContainer *container,
                         AdgPair      *device_offset,
                         AdgPair      *user_offset)
{
  double dx;
  double dy;
  double ux;
  double uy;

  g_return_if_fail (device_offset != NULL || user_offset != NULL);

  if (device_offset != NULL)
    {
      dx = device_offset->x;
      dy = device_offset->y;
    }
  else
    {
      dx = 0.0;
      dy = 0.0;
    }

  if (user_offset != NULL)
    {
      ux = user_offset->x;
      uy = user_offset->y;
    }
  else
    {
      ux = 0.0;
      uy = 0.0;
    }

  adg_container_translate_explicit (container, dx, dy, ux, uy);
}

void
adg_container_translate_explicit (AdgContainer *container,
                                  double        dx,
                                  double        dy,
                                  double        ux,
                                  double        uy)
{
  AdgMatrix matrix;

  g_return_if_fail (ADG_IS_CONTAINER (container));

  adg_matrix_set (&matrix, &container->priv->matrix);
  matrix.x0 = dx + ux * matrix.xx;
  matrix.y0 = dy + uy * matrix.yy;

  if (set_matrix (container, &matrix))
    g_object_notify ((GObject *) container, "matrix");
}


static gboolean
set_matrix (AdgContainer *container,
	    AdgMatrix    *matrix)
{
  AdgMatrix old_matrix;

  g_return_val_if_fail (matrix != NULL, FALSE);

  adg_matrix_set (&old_matrix, &container->priv->matrix);
  adg_matrix_set (&container->priv->matrix, matrix);

  adg_entity_ctm_changed ((AdgEntity *) container, &old_matrix);
  return TRUE;
}
