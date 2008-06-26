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
 * SECTION:adgentity
 * @title: AdgEntity
 * @short_description: The base class for renderable objects
 *
 * This abstract class provides a base interface for all renderable objects
 * (all the objects that can be printed or viewed).
 */

#include "adg-entity.h"
#include "adg-entity-private.h"
#include "adg-canvas.h"
#include "adg-util.h"
#include "adg-intl.h"

#include <gcontainer/gcontainer.h>

#define PARENT_CLASS ((GInitiallyUnownedClass *) adg_entity_parent_class)


enum
{
  PROP_0,
  PROP_PARENT
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

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgEntityPrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;
  gobject_class->dispose = g_childable_dispose;

  g_object_class_override_property (gobject_class, PROP_PARENT, "parent");

  klass->model_matrix_changed = model_matrix_changed;
  klass->paper_matrix_changed = NULL;
  klass->render = render;
  klass->get_line_style = NULL;
  klass->set_line_style = NULL;
  klass->get_font_style = NULL;
  klass->set_font_style = NULL;
  klass->get_arrow_style = NULL;
  klass->set_arrow_style = NULL;
  klass->get_dim_style = NULL;
  klass->set_dim_style = NULL;
  klass->get_model_matrix = get_model_matrix;
  klass->get_paper_matrix = get_paper_matrix;

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
      entity->priv->parent = (AdgContainer *) g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
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
  ((AdgEntity *) childable)->priv->parent = (AdgContainer *) parent;
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
      if (ADG_IS_ENTITY (entity))
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
 * adg_entity_get_line_style:
 * @entity: an #AdgEntity object
 *
 * Gets the line style for @entity. If @entity does not implement
 * get_line_style, %NULL is returned and an error is logged.
 * If get_line_style is implemented but the line style is not set,
 * a fallback line style is searched in the hierarchy of @entity.
 *
 * Return value: the requested line style
 */
const AdgLineStyle *
adg_entity_get_line_style (AdgEntity *entity)
{
  AdgEntityClass     *entity_class;
  const AdgLineStyle *line_style;
  gboolean            is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->priv->parent)
    {
      entity_class = ADG_ENTITY_GET_CLASS (entity);

      if (entity_class->get_line_style != NULL)
        {
          line_style = entity_class->get_line_style (entity);

          if (line_style != NULL)
            break;
        }
      else
        {
          /* Abort if the first entity does not implement get_line_style */
          g_return_val_if_fail (is_parent, NULL);
        }

      is_parent = TRUE;
    }

  return line_style;
}

/**
 * adg_entity_set_line_style:
 * @entity: an #AdgEntity object
 * @line_style: an #AdgLineStyle style
 *
 * Sets the line style for @entity to @line_style.
 * If @entity does not implement set_line_style, an error is logged.
 */
void
adg_entity_set_line_style (AdgEntity    *entity,
                           AdgLineStyle *line_style)
{
  AdgEntityClass *entity_class;

  g_return_if_fail (ADG_IS_ENTITY (entity));

  entity_class = ADG_ENTITY_GET_CLASS (entity);

  g_return_if_fail (entity_class->set_line_style != NULL);

  entity_class->set_line_style (entity, line_style);
}

/**
 * adg_entity_get_font_style:
 * @entity: an #AdgEntity object
 *
 * Gets the font style for @entity. If @entity does not implement
 * get_font_style, %NULL is returned and an error is logged.
 * If get_font_style is implemented but the font style is not set,
 * a fallback font style is searched in the hierarchy of @entity.
 *
 * Return value: the requested font style
 */
const AdgFontStyle *
adg_entity_get_font_style (AdgEntity *entity)
{
  AdgEntityClass     *entity_class;
  const AdgFontStyle *font_style;
  gboolean            is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->priv->parent)
    {
      entity_class = ADG_ENTITY_GET_CLASS (entity);

      if (entity_class->get_font_style != NULL)
        {
          font_style = entity_class->get_font_style (entity);

          if (font_style != NULL)
            break;
        }
      else
        {
          /* Abort if the first entity does not implement get_font_style */
          g_return_val_if_fail (is_parent, NULL);
        }

      is_parent = TRUE;
    }

  return font_style;
}

/**
 * adg_entity_set_font_style:
 * @entity: an #AdgEntity object
 * @font_style: an #AdgFontStyle style
 *
 * Sets the font style for @entity to @font_style.
 * If @entity does not implement set_font_style, an error is logged.
 */
void
adg_entity_set_font_style (AdgEntity    *entity,
                           AdgFontStyle *font_style)
{
  AdgEntityClass *entity_class;

  g_return_if_fail (ADG_IS_ENTITY (entity));

  entity_class = ADG_ENTITY_GET_CLASS (entity);

  g_return_if_fail (entity_class->set_font_style != NULL);

  entity_class->set_font_style (entity, font_style);
}

/**
 * adg_entity_get_arrow_style:
 * @entity: an #AdgEntity object
 *
 * Gets the arrow style for @entity. If @entity does not implement
 * get_arrow_style, %NULL is returned and an error is logged.
 * If get_arrow_style is implemented but the arrow style is not set,
 * a fallback arrow style is searched in the hierarchy of @entity.
 *
 * Return value: the requested arrow style
 */
const AdgArrowStyle *
adg_entity_get_arrow_style (AdgEntity *entity)
{
  AdgEntityClass      *entity_class;
  const AdgArrowStyle *arrow_style;
  gboolean             is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->priv->parent)
    {
      entity_class = ADG_ENTITY_GET_CLASS (entity);

      if (entity_class->get_arrow_style != NULL)
        {
          arrow_style = entity_class->get_arrow_style (entity);

          if (arrow_style != NULL)
            break;
        }
      else
        {
          /* Abort if the first entity does not implement get_arrow_style */
          g_return_val_if_fail (is_parent, NULL);
        }

      is_parent = TRUE;
    }

  return arrow_style;
}

/**
 * adg_entity_set_arrow_style:
 * @entity: an #AdgEntity object
 * @arrow_style: an #AdgArrowStyle style
 *
 * Sets the arrow style for @entity to @arrow_style.
 * If @entity does not implement set_arrow_style, an error is logged.
 */
void
adg_entity_set_arrow_style (AdgEntity     *entity,
                            AdgArrowStyle *arrow_style)
{
  AdgEntityClass *entity_class;

  g_return_if_fail (ADG_IS_ENTITY (entity));

  entity_class = ADG_ENTITY_GET_CLASS (entity);

  g_return_if_fail (entity_class->set_arrow_style != NULL);

  entity_class->set_arrow_style (entity, arrow_style);
}

/**
 * adg_entity_get_dim_style:
 * @entity: an #AdgEntity object
 *
 * Gets the dimension style for @entity. If @entity does not implement
 * get_dim_style, %NULL is returned and an error is logged.
 * If get_dim_style is implemented but the dimension style is not set,
 * a fallback dimension style is searched in the hierarchy of @entity.
 *
 * Return value: the requested dimension style
 */
const AdgDimStyle *
adg_entity_get_dim_style (AdgEntity *entity)
{
  AdgEntityClass    *entity_class;
  const AdgDimStyle *dim_style;
  gboolean           is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->priv->parent)
    {
      entity_class = ADG_ENTITY_GET_CLASS (entity);

      if (entity_class->get_dim_style != NULL)
        {
          dim_style = entity_class->get_dim_style (entity);

          if (dim_style != NULL)
            break;
        }
      else
        {
          /* Abort if the first entity does not implement get_dim_style */
          g_return_val_if_fail (is_parent, NULL);
        }

      is_parent = TRUE;
    }

  return dim_style;
}

/**
 * adg_entity_set_dim_style:
 * @entity: an #AdgEntity object
 * @dim_style: an #AdgDimStyle style
 *
 * Sets the dimension style for @entity to @dim_style.
 * If @entity does not implement set_dim_style, an error is logged.
 */
void
adg_entity_set_dim_style (AdgEntity   *entity,
                          AdgDimStyle *dim_style)
{
  AdgEntityClass *entity_class;

  g_return_if_fail (ADG_IS_ENTITY (entity));

  entity_class = ADG_ENTITY_GET_CLASS (entity);

  g_return_if_fail (entity_class->set_dim_style != NULL);

  entity_class->set_dim_style (entity, dim_style);
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
