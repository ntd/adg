/* ADG - Automatic Drawing Generation.
 * Copyright (C) 2007 - Fontana Nicola <ntd@users.sourceforge.net>
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
 **/

#include "adgentity.h"
#include "adgcanvas.h"
#include "adgintl.h"


enum
{
  PROP_0,
  PROP_PARENT
};

enum
{
  UPTODATE_SET,
  CTM_CHANGED,
  LAST_SIGNAL
};


static void     adg_entity_init	        	(AdgEntity	*entity);
static void     adg_entity_childable_init	(GChildableIface*iface);
static void     adg_entity_get_property         (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void     adg_entity_set_property         (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static GContainerable *
                adg_entity_get_parent           (GChildable     *childable);
static void     adg_entity_set_parent           (GChildable     *childable,
                                                 GContainerable *new_parent);
static void     adg_entity_parent_set           (GChildable     *childable,
                                                 GContainerable *old_parent);

static void     adg_entity_real_update          (AdgEntity      *entity,
                                                 gboolean        recursive);
static void     adg_entity_real_outdate         (AdgEntity      *entity,
                                                 gboolean        recursive);
static void     adg_entity_render_unimplemented (AdgEntity      *entity,
                                                 cairo_t        *cr);


static guint    signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE_EXTENDED (AdgEntity, adg_entity, 
                        G_TYPE_INITIALLY_UNOWNED, G_TYPE_FLAG_ABSTRACT,
                        G_IMPLEMENT_INTERFACE (G_TYPE_CHILDABLE, adg_entity_childable_init));

#define PARENT_CLASS ((AdgEntityClass *) adg_container_parent_class)


static void
adg_entity_class_init (AdgEntityClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = adg_entity_get_property;
  gobject_class->set_property = adg_entity_set_property;
  gobject_class->dispose = g_childable_dispose;

  g_object_class_override_property (gobject_class, PROP_PARENT, "parent");

  klass->uptodate_set = NULL;
  klass->ctm_changed = NULL;
  klass->get_line_style = NULL;
  klass->set_line_style = NULL;
  klass->get_font_style = NULL;
  klass->set_font_style = NULL;
  klass->get_arrow_style = NULL;
  klass->set_arrow_style = NULL;
  klass->get_dim_style = NULL;
  klass->set_dim_style = NULL;
  klass->get_ctm = NULL;
  klass->update = adg_entity_real_update;
  klass->outdate = adg_entity_real_outdate;
  klass->render = adg_entity_render_unimplemented;

  /**
   * AdgEntity::uptodate-set:
   * @entity: an #AdgEntity
   * @old_state: the old state of the "uptodate" flag
   *
   * The "uptodate" property value is changed.
   **/
  signals[UPTODATE_SET] =
    g_signal_new ("uptodate-set",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (AdgEntityClass, uptodate_set),
		  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
		  G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

  /**
   * AdgEntity::ctm-changed:
   * @entity: an #AdgEntity
   * @old_ctm: the old current transformation matrix
   *
   * Emitted when the current trasformation matrix has changed.
   **/
  signals[CTM_CHANGED] =
    g_signal_new ("ctm-changed",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (AdgEntityClass, ctm_changed),
		  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1, ADG_TYPE_MATRIX);
}

static void
adg_entity_childable_init (GChildableIface *iface)
{
  iface->get_parent = adg_entity_get_parent;
  iface->set_parent = adg_entity_set_parent;
  iface->parent_set = adg_entity_parent_set;
}

static void
adg_entity_init (AdgEntity *entity)
{
  entity->parent = NULL;
  entity->flags = 0;
}

static void
adg_entity_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GChildable *childable = G_CHILDABLE (object);

  switch (prop_id)
    {
    case PROP_PARENT:
      g_value_set_object (value, g_childable_get_parent (childable));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
adg_entity_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GChildable *childable = G_CHILDABLE (object);

  switch (prop_id)
    {
    case PROP_PARENT:
      g_childable_set_parent (childable, (GContainerable *) g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static GContainerable *
adg_entity_get_parent (GChildable *childable)
{
  return ((AdgEntity *) childable)->parent;
}

static void
adg_entity_set_parent (GChildable     *childable,
                       GContainerable *new_parent)
{
  ((AdgEntity *) childable)->parent = new_parent;
}

static void
adg_entity_parent_set (GChildable     *childable,
                       GContainerable *old_parent)
{
  AdgMatrix *old_ctm;

  if (ADG_IS_CONTAINER (old_parent))
    old_ctm = & ((AdgContainer *) old_parent)->ctm;
  else
    old_ctm = NULL;

  g_signal_emit (childable, signals[CTM_CHANGED], 0, old_ctm);
}


static void
adg_entity_real_update (AdgEntity *entity,
                        gboolean   recursive)
{
  ADG_ENTITY_SET_FLAGS (entity, ADG_UPTODATE);
  g_signal_emit (entity, signals[UPTODATE_SET], 0, FALSE);
}

static void
adg_entity_real_outdate (AdgEntity *entity,
                         gboolean   recursive)
{
  ADG_ENTITY_UNSET_FLAGS (entity, ADG_UPTODATE);
  g_signal_emit (entity, signals[UPTODATE_SET], 0, TRUE);
}

static void
adg_entity_render_unimplemented (AdgEntity *entity,
                                 cairo_t   *cr)
{
  g_warning ("AdgEntity::render not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (entity)));
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
 **/
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
 * adg_entity_ctm_changed:
 * @entity: an #AdgEntity
 *
 * Emits the "ctm-changed" signal on @entity.
 *
 * This function is only useful in entity implementations.
 **/
void
adg_entity_ctm_changed  (AdgEntity *entity)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  g_signal_emit (entity, signals[CTM_CHANGED], 0, NULL);
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
 **/
const AdgLineStyle *
adg_entity_get_line_style (AdgEntity *entity)
{
  AdgEntityClass     *entity_class;
  const AdgLineStyle *line_style;
  gboolean            is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->parent)
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
 **/
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
 **/
const AdgFontStyle *
adg_entity_get_font_style (AdgEntity *entity)
{
  AdgEntityClass     *entity_class;
  const AdgFontStyle *font_style;
  gboolean            is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->parent)
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
 **/
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
 **/
const AdgArrowStyle *
adg_entity_get_arrow_style (AdgEntity *entity)
{
  AdgEntityClass      *entity_class;
  const AdgArrowStyle *arrow_style;
  gboolean             is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->parent)
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
 **/
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
 **/
const AdgDimStyle *
adg_entity_get_dim_style (AdgEntity *entity)
{
  AdgEntityClass    *entity_class;
  const AdgDimStyle *dim_style;
  gboolean           is_parent;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  for (is_parent = FALSE; entity != NULL; entity = (AdgEntity *) entity->parent)
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
 **/
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
 * adg_entity_get_ctm:
 * @entity: an #AdgEntity object
 *
 * Gets the current transformation matrix to be applied to @entity.
 * If "get_ctm" is not implemented by @entity itsself, the hierarchy of @entity
 * is scanned to find the first parent which implements "get_ctm" and which
 * returns a valid matrix. If not found, the function will return the fallback
 * matrix returned by adg_matrix_get_fallback().
 *
 * Apart specifing an invalid entity, this function will ever return
 * a valid matrix.
 *
 * Return value: the current transformation matrix of @entity
 **/
const AdgMatrix *
adg_entity_get_ctm (AdgEntity *entity)
{
  AdgEntityClass  *entity_class;
  const AdgMatrix *matrix;

  g_return_val_if_fail (ADG_IS_ENTITY (entity), NULL);

  while (entity != NULL)
    {
      entity_class = ADG_ENTITY_GET_CLASS (entity);

      if (entity_class->get_ctm != NULL)
        {
          matrix = entity_class->get_ctm (entity);

          if (matrix != NULL)
            return matrix;
        }

      entity = (AdgEntity *) entity->parent;
    }

  /* No valid matrix found in entity hierarchy */
  g_return_val_if_reached (adg_matrix_get_fallback ());
}


/**
 * adg_entity_update:
 * @entity: an #AdgEntity
 *
 * Updates @entity, that is regenerates the calculations needed by the
 * rendering process.
 **/
void
adg_entity_update (AdgEntity *entity)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  if (ADG_ENTITY_UPTODATE (entity))
    return;

  ADG_ENTITY_GET_CLASS (entity)->update (entity, FALSE);
}

/**
 * adg_entity_update_all:
 * @entity: an #AdgEntity
 *
 * Same as adg_entity_update(), but also applied to all its children.
 **/
void
adg_entity_update_all (AdgEntity *entity)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  ADG_ENTITY_GET_CLASS (entity)->update (entity, TRUE);
}

/**
 * adg_entity_outdate:
 * @entity: an #AdgEntity
 *
 * Outdates @entity, so its internal calculations will be regenerated before
 * the rendering process.
 **/
void
adg_entity_outdate (AdgEntity *entity)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  if (! ADG_ENTITY_UPTODATE (entity))
    return;

  ADG_ENTITY_GET_CLASS (entity)->outdate (entity, FALSE);
}

/**
 * adg_entity_outdate_all:
 * @entity: an #AdgEntity
 *
 * Same as adg_entity_outdate(), but also applied to all its children.
 **/
void
adg_entity_outdate_all (AdgEntity *entity)
{
  g_return_if_fail (ADG_IS_ENTITY (entity));

  ADG_ENTITY_GET_CLASS (entity)->outdate (entity, TRUE);
}


/**
 * adg_entity_render:
 * @entity: an #AdgEntity
 * @cr: a #cairo_t drawing context
 *
 * Renders @entity and all its children, if any, on specified drawing context.
 *
 * If @entity is not up to date, an update is performed before the rendering
 * process.
 **/
void
adg_entity_render (AdgEntity *entity,
                   cairo_t   *cr)
{
  AdgEntityClass *entity_class;

  g_return_if_fail (ADG_IS_ENTITY (entity));

  entity_class = ADG_ENTITY_GET_CLASS (entity);

  if (! ADG_ENTITY_UPTODATE (entity))
      entity_class->update (entity, FALSE);

  entity_class->render (entity, cr);
}
