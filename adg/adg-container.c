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
containerable_init (GContainerableIface *iface)
{
  iface->get_children = get_children;
  iface->add = add;
  iface->remove = remove;
}

static void
adg_container_class_init (AdgContainerClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

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
adg_container_init (AdgContainer *container)
{
  container->children = NULL;

  cairo_matrix_init_identity (&container->matrix);
  cairo_matrix_init_identity (&container->ctm);
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgContainer *container = ADG_CONTAINER (object);

  switch (prop_id)
    {
    case PROP_MATRIX:
      g_value_set_boxed (value, &container->matrix);
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
  AdgContainer *container = ADG_CONTAINER (object);

  switch (prop_id)
    {
    case PROP_CHILD:
      g_containerable_add ((GContainerable *) container, g_value_get_object (value));
      break;
    case PROP_MATRIX:
      adg_container_set_matrix (container, g_value_get_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static GSList *
get_children (GContainerable *containerable)
{
  AdgContainer *container = ADG_CONTAINER (containerable);

  return g_slist_copy (container->children);
}

static gboolean
add (GContainerable *containerable,
     GChildable     *childable)
{
  AdgContainer *container;

  g_return_val_if_fail (ADG_IS_ENTITY (childable), FALSE);

  container = ADG_CONTAINER (containerable);
  container->children = g_slist_append (container->children, childable);
  return TRUE;
}

static gboolean
remove (GContainerable *containerable,
	GChildable     *childable)
{
  AdgContainer *container;
  GSList       *node;

  g_return_val_if_fail (ADG_IS_ENTITY (childable), FALSE);

  container = ADG_CONTAINER (containerable);
  node = g_slist_find (container->children, childable);

  if (! node)
    return FALSE;

  container->children = g_slist_delete_link (container->children, node);
  return TRUE;
}


static void
ctm_changed (AdgEntity *entity,
	     AdgMatrix *dummy_ctm)
{
  AdgContainer *container;
  AdgContainer *parent;
  AdgMatrix     old_ctm;

  container = ADG_CONTAINER (entity);
  parent = (AdgContainer *) g_childable_get_parent ((GChildable *) entity);
  adg_matrix_set (&old_ctm, &container->ctm);

  /* Refresh the ctm */
  if (ADG_IS_CONTAINER (parent))
    adg_matrix_set (&container->ctm, &parent->ctm);
  else
    cairo_matrix_init_identity (&container->ctm);

  cairo_matrix_multiply (&container->ctm, &container->ctm, &container->matrix);

  /* Check for ctm != old_ctm */
  if (! adg_matrix_equal (&container->ctm, &old_ctm))
    g_containerable_propagate_by_name (G_CONTAINERABLE (container),
                                       "ctm-changed", &old_ctm);
}

const AdgMatrix *
get_ctm (AdgEntity *entity)
{
  return & ADG_CONTAINER (entity)->ctm;
}


static void
update (AdgEntity *entity,
	gboolean   recursive)
{
  if (recursive)
    g_containerable_foreach (G_CONTAINERABLE (entity),
                             G_CALLBACK (adg_entity_update_all), NULL);

  PARENT_CLASS->update (entity, recursive);
}

static void
outdate (AdgEntity *entity,
	 gboolean   recursive)
{
  if (recursive)
    g_containerable_foreach (G_CONTAINERABLE (entity),
                             G_CALLBACK (adg_entity_outdate_all), NULL);

  PARENT_CLASS->outdate (entity, recursive);
}

static void
render (AdgEntity *entity,
	cairo_t   *cr)
{
  AdgContainer *container = ADG_CONTAINER (entity);

  cairo_set_matrix (cr, &container->ctm);
  g_containerable_foreach (G_CONTAINERABLE (entity), G_CALLBACK (adg_entity_render), cr);
}


const AdgMatrix *
adg_container_get_matrix (AdgContainer *container)
{
  g_return_val_if_fail (ADG_IS_CONTAINER (container), NULL);

  return &container->matrix;
}

void
adg_container_set_matrix (AdgContainer *container,
                          AdgMatrix    *matrix)
{
  g_return_if_fail (ADG_IS_CONTAINER (container));
  g_return_if_fail (matrix != NULL);

  adg_matrix_set (&container->matrix, matrix);

  g_object_notify (G_OBJECT (container), "matrix");
  adg_entity_ctm_changed ((AdgEntity *) container);
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
  g_return_if_fail (ADG_IS_CONTAINER (container));

  container->matrix.xx = sx;
  container->matrix.yy = sy;

  g_object_notify (G_OBJECT (container), "matrix");
  adg_entity_ctm_changed ((AdgEntity *) container);
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
  AdgMatrix *matrix;

  g_return_if_fail (ADG_IS_CONTAINER (container));

  matrix = & container->matrix;
  matrix->x0 = dx + ux * matrix->xx;
  matrix->y0 = dy + uy * matrix->yy;

  g_object_notify (G_OBJECT (container), "matrix");
  adg_entity_ctm_changed ((AdgEntity *) container);
}
