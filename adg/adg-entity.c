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
 * SECTION:entity
 * @title: AdgEntity
 * @short_description: The base class for renderable objects
 *
 * This abstract class provides a base interface for all renderable objects
 * (all the objects that can be printed or viewed).
 */

#include "adg-entity.h"
#include "adg-entity-private.h"
#include "adg-canvas.h"
#include "adg-context.h"
#include "adg-util.h"
#include "adg-intl.h"

#include <gcontainer/gcontainer.h>

#define PARENT_CLASS ((GInitiallyUnownedClass *) adg_entity_parent_class)


enum
{
  PROP_0,
  PROP_PARENT,
  PROP_CONTEXT
};

enum
{
  MODEL_MATRIX_CHANGED,
  PAPER_MATRIX_CHANGED,
  RENDER,
  LAST_SIGNAL
};


static void	childable_init		(GChildableIface*iface);
static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);

static GContainerable *
		get_parent		(GChildable	*childable);
static void	set_parent		(GChildable	*childable,
					 GContainerable	*parent);
static void	parent_set		(GChildable	*childable,
					 GContainerable	*old_parent);
static void	set_context		(AdgEntity	*entity,
					 AdgContext	*context);
static void	model_matrix_changed	(AdgEntity	*entity,
					 AdgMatrix	*parent_matrix);
static void	paper_matrix_changed	(AdgEntity	*entity,
					 AdgMatrix	*parent_matrix);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static const AdgMatrix *
		get_model_matrix	(AdgEntity	*entity);
static const AdgMatrix *
		get_paper_matrix	(AdgEntity	*entity);

static guint	signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE_EXTENDED (AdgEntity, adg_entity, 
			G_TYPE_INITIALLY_UNOWNED, G_TYPE_FLAG_ABSTRACT,
			G_IMPLEMENT_INTERFACE (G_TYPE_CHILDABLE, childable_init));


static void
adg_entity_class_init (AdgEntityClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgEntityPrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;
  gobject_class->dispose = g_childable_dispose;

  klass->model_matrix_changed = model_matrix_changed;
  klass->paper_matrix_changed = NULL;
  klass->render = render;
  klass->get_model_matrix = get_model_matrix;
  klass->get_paper_matrix = get_paper_matrix;

  g_object_class_override_property (gobject_class, PROP_PARENT, "parent");

  param = g_param_spec_object ("context",
			       P_("Context"),
			       P_("The context associated to this entity or NULL to inherit the parent context"),
			       ADG_TYPE_CONTEXT,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_CONTEXT, param);

  /**
   * AdgEntity::uptodate-set:
   * @entity: an #AdgEntity
   * @old_state: the old state of the "uptodate" flag
   *
   * The "uptodate" property value is changed.
   */
  signals[RENDER] =
    g_signal_new ("render",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (AdgEntityClass, render),
		  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
		  G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

  /**
   * AdgEntity::model-matrix-changed:
   * @entity: an #AdgEntity
   * @parent_matrix: the parent model matrix
   *
   * Emitted after the current model matrix has changed.
   */
  signals[MODEL_MATRIX_CHANGED] =
    g_signal_new ("model-matrix-changed",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (AdgEntityClass, model_matrix_changed),
		  NULL, NULL,
                  g_cclosure_marshal_VOID__BOXED,
		  G_TYPE_NONE, 1, ADG_TYPE_MATRIX|G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * AdgEntity::paper-matrix-changed:
   * @entity: an #AdgEntity
   * @parent_matrix: the parent paper matrix
   *
   * Emitted after the current paper matrix has changed.
   */
  signals[PAPER_MATRIX_CHANGED] =
    g_signal_new ("paper-matrix-changed",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (AdgEntityClass, paper_matrix_changed),
		  NULL, NULL,
                  g_cclosure_marshal_VOID__BOXED,
		  G_TYPE_NONE, 1, ADG_TYPE_MATRIX|G_SIGNAL_TYPE_STATIC_SCOPE);
}

static void
childable_init (GChildableIface *iface)
{
  iface->get_parent = get_parent;
  iface->set_parent = set_parent;
  iface->parent_set = parent_set;
}

static void
adg_entity_init (AdgEntity *entity)
{
  AdgEntityPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (entity, ADG_TYPE_ENTITY,
							AdgEntityPrivate);

  priv->parent = NULL;
  priv->flags = 0;
  priv->context = NULL;

  entity->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgEntity *entity = (AdgEntity *) object;

  switch (prop_id)
    {
    case PROP_PARENT:
      g_value_set_object (value, entity->priv->parent);
      break;
    case PROP_CONTEXT:
      g_value_set_object (value, entity->priv->context);
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
    case PROP_PARENT:
      entity->priv->parent = (AdgEntity *) g_value_get_object (value);
      break;
    case PROP_CONTEXT:
      set_context (entity, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


/**
 * adg_entity_get_context:
 * @entity: an #AdgEntity instance
 *
 * Gets the context associated to @entity.
 * If no context was explicitely set, get the parent context.
 *
 * Return value: the requested context or %NULL on errors
 **/
AdgContext *
adg_entity_get_context (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  if (entity->priv->context)
    return entity->priv->context;

  if (entity->priv->parent)
    return adg_entity_get_context (entity->priv->parent);

  return NULL;
}

/**
 * adg_entity_set_context:
 * @entity: an #AdgEntity instance
 * @context: the new context
 *
 * Sets a new context. The old context (if any) will be unreferenced
 * while a new reference will be added to @context.
 **/
void
adg_entity_set_context (AdgEntity  *entity,
			AdgContext *context)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));
  g_return_if_fail (ADG_IS_CONTEXT (context));

  set_context (entity, context);
  g_object_notify ((GObject *) entity, "context");
}

/**
 * adg_entity_get_canvas:
 * @entity: an #AdgEntity
 *
 * Walks on the @entity hierarchy and gets the first parent of @entity that is
 * of #AdgCanvas derived type.
 *
 * Return value: the requested object or %NULL if there is no #AdgCanvas in
 *               the parent hierarchy.
 */
AdgCanvas *
adg_entity_get_canvas (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  while (entity)
    {
      if (ADG_IS_CANVAS (entity))
        return (AdgCanvas *) entity;

      entity = (AdgEntity *) g_childable_get_parent ((GChildable *) entity);
    }

  return NULL;
}

/**
 * adg_entity_get_model_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the model matrix to be used in rendering this @entity.
 *
 * Return value: the requested matrix
 */
const AdgMatrix *
adg_entity_get_model_matrix (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);
  return ADG_ENTITY_GET_CLASS (entity)->get_model_matrix (entity);
}

/**
 * adg_entity_get_paper_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the paper matrix to be used in rendering this @entity.
 *
 * Return value: the requested matrix
 */
const AdgMatrix *
adg_entity_get_paper_matrix (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);
  return ADG_ENTITY_GET_CLASS (entity)->get_paper_matrix (entity);
}

/**
 * adg_entity_model_matrix_changed:
 * @entity: an #AdgEntity
 * @parent_matrix: the parent #AdgMatrix
 *
 * Emits the "model-matrix-changed" signal on @entity.
 *
 * This function is only useful in entity implementations.
 */
void
adg_entity_model_matrix_changed (AdgEntity       *entity,
				 const AdgMatrix *parent_matrix)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  g_signal_emit (entity, signals[MODEL_MATRIX_CHANGED], 0, parent_matrix);
}

/**
 * adg_entity_paper_matrix_changed:
 * @entity: an #AdgEntity
 * @parent_matrix: the parent #AdgMatrix
 *
 * Emits the "paper-matrix-changed" signal on @entity.
 *
 * This function is only useful in entity implementations.
 */
void
adg_entity_paper_matrix_changed (AdgEntity       *entity,
				 const AdgMatrix *parent_matrix)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  g_signal_emit (entity, signals[PAPER_MATRIX_CHANGED], 0, parent_matrix);
}

/**
 * adg_entity_get_style:
 * @entity: an #AdgEntity
 * @style_slot: the slot of the style to get
 *
 * Gets a style from this entity. If the entity has no context associated
 * or the style in undefined within this context, gets the style from its
 * parent container.
 *
 * Return value: the requested style
 **/
AdgStyle *
adg_entity_get_style (AdgEntity   *entity,
		      AdgStyleSlot style_slot)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  if (entity->priv->context)
    {
      AdgStyle *style = adg_context_get_style (entity->priv->context,
					       style_slot);
      if (style)
	return style;
    }

  if (entity->priv->parent)
    return adg_entity_get_style (entity->priv->parent, style_slot);

  return NULL;
}

/**
 * adg_entity_apply:
 * @entity: an #AdgEntity
 * @style_slot: the slot of the style to apply
 * @cr: a cairo context
 *
 * Applies the specified style to the @cr cairo context.
 **/
void
adg_entity_apply (AdgEntity   *entity,
		  AdgStyleSlot style_slot,
		  cairo_t     *cr)
{
  AdgStyle *style = adg_entity_get_style (entity, style_slot);

  if (style)
    adg_style_apply (style, cr);
}

/**
 * adg_entity_model_matrix_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the model matrix didn't change from the last render
 */
gboolean
adg_entity_model_matrix_applied (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), FALSE);
  return ADG_ISSET (entity->priv->flags, MODEL_MATRIX_APPLIED);
}

/**
 * adg_entity_paper_matrix_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the paper matrix didn't change from the last render
 */
gboolean
adg_entity_paper_matrix_applied (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), FALSE);
  return ADG_ISSET (entity->priv->flags, PAPER_MATRIX_APPLIED);
}

/**
 * adg_entity_model_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the model didn't change from the last render
 */
gboolean
adg_entity_model_applied (AdgEntity *entity)
{
  g_return_val_if_fail (ADG_IS_ENTITY (entity), FALSE);
  return ADG_ISSET (entity->priv->flags, MODEL_APPLIED);
}

/**
 * adg_entity_render:
 * @entity: an #AdgEntity
 * @cr: a #cairo_t drawing context
 *
 * Renders @entity and all its children, if any, on the @cr drawing context.
 */
void
adg_entity_render (AdgEntity *entity,
                   cairo_t   *cr)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  g_signal_emit (entity, signals[RENDER], 0, cr);
}


static GContainerable *
get_parent (GChildable *childable)
{
  return (GContainerable *) ((AdgEntity *) childable)->priv->parent;
}

static void
set_parent (GChildable     *childable,
	    GContainerable *parent)
{
  ((AdgEntity *) childable)->priv->parent = (AdgEntity *) parent;
  g_object_notify ((GObject *) childable, "parent");
}

static void
parent_set (GChildable     *childable,
	    GContainerable *old_parent)
{
  if (ADG_IS_CONTAINER (old_parent))
    {
      AdgEntity       *entity;
      const AdgMatrix *old_model;
      const AdgMatrix *old_paper;

      entity = (AdgEntity *) childable;
      old_model = adg_entity_get_model_matrix ((AdgEntity *) old_parent);
      old_paper = adg_entity_get_paper_matrix ((AdgEntity *) old_parent);

      adg_entity_model_matrix_changed (entity, old_model);
      adg_entity_paper_matrix_changed (entity, old_paper);
    }
}

static void
set_context (AdgEntity  *entity,
	     AdgContext *context)
{
  if (entity->priv->context)
    g_object_unref ((GObject *) entity->priv->context);

  g_object_ref ((GObject *) context);
  entity->priv->context = context;
}

static void
model_matrix_changed (AdgEntity *entity,
		      AdgMatrix *parent_matrix)
{
  ADG_UNSET (entity->priv->flags, MODEL_MATRIX_APPLIED);
}

static void
paper_matrix_changed (AdgEntity *entity,
		      AdgMatrix *parent_matrix)
{
  ADG_UNSET (entity->priv->flags, PAPER_MATRIX_APPLIED);
}

static void
render (AdgEntity *entity,
	cairo_t   *cr)
{
  ADG_SET (entity->priv->flags, MODEL_MATRIX_APPLIED|PAPER_MATRIX_APPLIED|MODEL_APPLIED);
}

static const AdgMatrix *
get_model_matrix (AdgEntity *entity)
{
  return adg_entity_get_model_matrix ((AdgEntity *) entity->priv->parent);
}

static const AdgMatrix *
get_paper_matrix (AdgEntity *entity)
{
  return adg_entity_get_paper_matrix ((AdgEntity *) entity->priv->parent);
}
