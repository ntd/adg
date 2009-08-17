/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


/**
 * SECTION:adg-entity
 * @short_description: The base class for renderable objects
 *
 * This abstract class provides a base interface for all renderable objects
 * (all the objects that can be printed or viewed).
 **/

/**
 * AdgEntity:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-entity.h"
#include "adg-entity-private.h"
#include "adg-canvas.h"
#include "adg-context.h"
#include "adg-util.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_PARENT,
    PROP_CONTEXT,
    PROP_GLOBAL_MAP,
    PROP_LOCAL_MAP
};

enum {
    PARENT_SET,
    INVALIDATE,
    RENDER,
    LAST_SIGNAL
};


static void             get_property            (GObject         *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void             set_property            (GObject         *object,
                                                 guint            prop_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);
static void             dispose                 (GObject         *object);
static AdgContainer *   get_parent              (AdgEntity       *entity);
static void             set_parent              (AdgEntity       *entity,
                                                 AdgContainer    *parent);
static void             parent_set              (AdgEntity       *entity,
                                                 AdgContainer    *old_parent);
static AdgContext *     get_context             (AdgEntity       *entity);
static void             set_context             (AdgEntity       *entity,
                                                 AdgContext      *context);
static void             set_global_map          (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static void             set_local_map           (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static void             render                  (AdgEntity       *entity,
                                                 cairo_t         *cr);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_ABSTRACT_TYPE(AdgEntity, adg_entity, G_TYPE_INITIALLY_UNOWNED);


static void
adg_entity_class_init(AdgEntityClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgEntityPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
    gobject_class->dispose = dispose;

    klass->get_parent = get_parent;
    klass->set_parent = set_parent;
    klass->parent_set = parent_set;
    klass->get_context = get_context;
    klass->invalidate = NULL;
    klass->render = render;

    param = g_param_spec_object("parent",
                                P_("Parent Container"),
                                P_("The parent AdgContainer of this entity or NULL if this is a top-level entity"),
                                ADG_TYPE_CONTAINER, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PARENT, param);

    param = g_param_spec_object("context",
                                P_("Context"),
                                P_("The context associated to this entity or NULL to inherit the parent context"),
                                ADG_TYPE_CONTEXT, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CONTEXT, param);

    param = g_param_spec_boxed("global-map",
                               P_("Global Map"),
                               P_("The transformation to be combined with the parent ones to get the global matrix"),
                               ADG_TYPE_MATRIX, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GLOBAL_MAP, param);

    param = g_param_spec_boxed("local-map",
                               P_("Local Map"),
                               P_("The transformation to be combined with the parent ones to get the local matrix"),
                               ADG_TYPE_MATRIX, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOCAL_MAP, param);

    /**
     * AdgEntity::parent-set:
     * @entity: an #AdgEntity
     * @parent: the #AdgContainer parent of @entity
     *
     * Emitted after the parent container has changed.
     **/
    signals[PARENT_SET] =
        g_signal_new("parent-set",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, parent_set),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, ADG_TYPE_CONTAINER);

    /**
     * AdgEntity::invalidate:
     * @entity: an #AdgEntity
     *
     * Clears the cached data of @entity.
     **/
    signals[INVALIDATE] =
        g_signal_new("invalidate",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, invalidate),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOOLEAN,
                     G_TYPE_NONE, 0);

    /**
     * AdgEntity::render:
     * @entity: an #AdgEntity
     * @cr: a #cairo_t drawing context
     *
     * Causes the rendering of @entity on @cr.
     **/
    signals[RENDER] =
        g_signal_new("render",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, render),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__POINTER,
                     G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void
adg_entity_init(AdgEntity *entity)
{
    AdgEntityPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(entity,
                                                         ADG_TYPE_ENTITY,
                                                         AdgEntityPrivate);
    data->parent = NULL;
    data->flags = 0;
    data->context = NULL;
    cairo_matrix_init_identity(&data->local_map);
    cairo_matrix_init_identity(&data->global_map);

    entity->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgEntity *entity;
    AdgEntityPrivate *data;

    entity = (AdgEntity *) object;
    data = entity->data;

    switch (prop_id) {
    case PROP_PARENT:
        g_value_set_object(value, get_parent(entity));
        break;
    case PROP_CONTEXT:
        g_value_set_object(value, get_context(entity));
        break;
    case PROP_GLOBAL_MAP:
        g_value_set_boxed(value, &data->global_map);
        break;
    case PROP_LOCAL_MAP:
        g_value_set_boxed(value, &data->local_map);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgEntity *entity = (AdgEntity *) object;

    switch (prop_id) {
    case PROP_PARENT:
        set_parent(entity, g_value_get_object(value));
        break;
    case PROP_CONTEXT:
        set_context(entity, g_value_get_object(value));
        break;
    case PROP_GLOBAL_MAP:
        set_global_map(entity, g_value_get_boxed(value));
        break;
    case PROP_LOCAL_MAP:
        set_local_map(entity, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
dispose(GObject *object)
{
    AdgEntity *entity;
    AdgEntityPrivate *data;
    GObjectClass *object_class;

    entity = (AdgEntity *) object;
    data = entity->data;
    object_class = (GObjectClass *) adg_entity_parent_class;

    if (data->parent)
        adg_container_remove(data->parent, entity);

    if (object_class->dispose != NULL)
        object_class->dispose(object);
}


/**
 * adg_entity_get_parent:
 * @entity: an #AdgEntity
 *
 * Gets the container parent of @entity.
 *
 * This function is only useful in entity implementations.
 *
 * Return value: the container object or %NULL if @entity is not contained
 **/
AdgContainer *
adg_entity_get_parent(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    return ADG_ENTITY_GET_CLASS(entity)->get_parent(entity);
}

/**
 * adg_entity_set_parent:
 * @entity: an #AdgEntity
 * @parent: an #AdgContainer
 *
 * Sets a new container of @entity.
 *
 * This function is only useful in entity implementations.
 **/
void
adg_entity_set_parent(AdgEntity *entity, AdgContainer *parent)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    ADG_ENTITY_GET_CLASS(entity)->set_parent(entity, parent);
    g_object_notify((GObject *) entity, "parent");
}

/**
 * adg_entity_unparent:
 * @entity: an #AdgEntity
 *
 * Removes the current parent of @entity, properly handling
 * the references between them.
 *
 * If @entity has no parent, this function simply returns.
 **/
void
adg_entity_unparent(AdgEntity *entity)
{
    AdgContainer *old_parent;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    old_parent = ADG_ENTITY_GET_CLASS(entity)->get_parent(entity);

    if (old_parent == NULL)
        return;

    ADG_ENTITY_GET_CLASS(entity)->set_parent(entity, NULL);
    g_signal_emit(entity, signals[PARENT_SET], 0, old_parent);

    g_object_unref(entity);
}

/**
 * adg_entity_reparent:
 * @entity: an #AdgEntity
 * @parent: the new container
 *
 * Moves @entity from the old parent to @parent, handling reference
 * count issues to avoid destroying the object.
 **/
void
adg_entity_reparent(AdgEntity *entity, AdgContainer *parent)
{
    AdgContainer *old_parent;

    g_return_if_fail(ADG_IS_CONTAINER(parent));

    old_parent = adg_entity_get_parent(entity);

    /* Reparenting on the same container: do nothing */
    if (old_parent == parent)
        return;

    g_return_if_fail(ADG_IS_CONTAINER(old_parent));

    g_object_ref(entity);
    adg_container_remove(old_parent, entity);
    adg_container_add(parent, entity);
    g_object_unref(entity);
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
adg_entity_get_context(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    if (data->context)
        return data->context;

    if (data->parent)
        return adg_entity_get_context((AdgEntity *) data->parent);

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
adg_entity_set_context(AdgEntity *entity, AdgContext *context)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(ADG_IS_CONTEXT(context));

    set_context(entity, context);
    g_object_notify((GObject *) entity, "context");
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
adg_entity_get_canvas(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    while (entity) {
        if (ADG_IS_CANVAS(entity))
            return (AdgCanvas *) entity;

        entity = (AdgEntity *) adg_entity_get_parent(entity);
    }

    return NULL;
}

/**
 * adg_entity_get_global_map:
 * @entity: an #AdgEntity object
 * @map: where to store the global map
 *
 * Gets the transformation to be used to compute the global matrix
 * of @entity and store it in @map.
 **/
void
adg_entity_get_global_map(AdgEntity *entity, AdgMatrix *map)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(map != NULL);

    data = entity->data;
    adg_matrix_copy(map, &data->global_map);
}

/**
 * adg_entity_set_global_map:
 * @entity: an #AdgEntity object
 * @map: the new map
 *
 * Sets the new global transformation of @entity to @map:
 * the old map is discarded. If @map is %NULL an identity
 * matrix is implied.
 **/
void
adg_entity_set_global_map(AdgEntity *entity, const AdgMatrix *map)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    set_global_map(entity, map);
    g_object_notify((GObject *) entity, "global-map");
}

/**
 * adg_entity_get_local_map:
 * @entity: an #AdgEntity object
 * @map: where to store the local map
 *
 * Gets the transformation to be used to compute the local matrix
 * of @entity and store it in @map.
 **/
void
adg_entity_get_local_map(AdgEntity *entity, AdgMatrix *map)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(map != NULL);

    data = entity->data;
    adg_matrix_copy(map, &data->local_map);
}

/**
 * adg_entity_set_local_map:
 * @entity: an #AdgEntity object
 * @map: the new map
 *
 * Sets the new global transformation of @entity to @map:
 * the old map is discarded. If @map is %NULL an identity
 * matrix is implied.
 **/
void
adg_entity_set_local_map(AdgEntity *entity, const AdgMatrix *map)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    set_local_map(entity, map);
    g_object_notify((GObject *) entity, "local-map");
}

/**
 * adg_entity_get_global_matrix:
 * @entity: an #AdgEntity object
 * @matrix: where to store the global matrix
 *
 * Computes the global matrix by combining all the global maps of the
 * @entity hierarchy and stores the result in @matrix.
 **/
void
adg_entity_get_global_matrix(AdgEntity *entity, AdgMatrix *matrix)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(matrix != NULL);

    data = entity->data;

    if (data->parent == NULL) {
        adg_matrix_copy(matrix, &data->global_map);
    } else {
        adg_entity_get_global_matrix((AdgEntity *) data->parent, matrix);
        cairo_matrix_multiply(matrix, &data->global_map, matrix);
    }
}

/**
 * adg_entity_get_local_matrix:
 * @entity: an #AdgEntity object
 * @matrix: where to store the local matrix
 *
 * Computes the local matrix by combining all the local maps of the
 * @entity hierarchy and stores the result in @matrix.
 **/
void
adg_entity_get_local_matrix(AdgEntity *entity, AdgMatrix *matrix)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(matrix != NULL);

    data = entity->data;

    if (data->parent == NULL) {
        adg_matrix_copy(matrix, &data->local_map);
    } else {
        adg_entity_get_local_matrix((AdgEntity *) data->parent, matrix);
        cairo_matrix_multiply(matrix, &data->local_map, matrix);
    }
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
 * Return value: the requested style or %NULL on errors
 **/
AdgStyle *
adg_entity_get_style(AdgEntity *entity, AdgStyleSlot style_slot)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    if (data->context) {
        AdgStyle *style = adg_context_get_style(data->context, style_slot);
        if (style)
            return style;
    }

    if (data->parent)
        return adg_entity_get_style((AdgEntity *) data->parent, style_slot);

    return NULL;
}

/**
 * adg_entity_apply:
 * @entity: an #AdgEntity
 * @style_slot: the slot of the style to apply
 * @cr: a #cairo_t drawing context
 *
 * Applies the specified style to the @cr cairo context.
 **/
void
adg_entity_apply(AdgEntity *entity, AdgStyleSlot style_slot, cairo_t *cr)
{
    AdgStyle *style = adg_entity_get_style(entity, style_slot);

    if (style)
        adg_style_apply(style, cr);
}

/**
 * adg_entity_invalidate:
 * @entity: an #AdgEntity
 *
 * Emits the "invalidate" signal on @entity and all its children, if any,
 * so subsequent rendering will need a global recomputation.
 **/
void
adg_entity_invalidate(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[INVALIDATE], 0, NULL);
}

/**
 * adg_entity_render:
 * @entity: an #AdgEntity
 * @cr: a #cairo_t drawing context
 *
 * Emits the "render" signal on @entity and all its children, if any,
 * causing the rendering operation the @cr cairo context.
 **/
void
adg_entity_render(AdgEntity *entity, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[RENDER], 0, cr);
}


static AdgContainer *
get_parent(AdgEntity *entity)
{
    AdgEntityPrivate *data = entity->data;

    return data->parent;
}

static void
set_parent(AdgEntity *entity, AdgContainer *parent)
{
    AdgEntityPrivate *data = entity->data;

    data->parent = parent;
}

static void
parent_set(AdgEntity *entity, AdgContainer *old_parent)
{
}

static AdgContext *
get_context(AdgEntity *entity)
{
    AdgEntityPrivate *data = entity->data;
    AdgEntity *parent;

    if (data->context)
        return data->context;

    parent = (AdgEntity *) data->parent;

    return parent ? ADG_ENTITY_GET_CLASS(parent)->get_context(parent) : NULL;
}

static void
set_context(AdgEntity *entity, AdgContext *context)
{
    AdgEntityPrivate *data = entity->data;

    if (data->context)
        g_object_unref((GObject *) data->context);

    g_object_ref((GObject *) context);
    data->context = context;
}

static void
set_global_map(AdgEntity *entity, const AdgMatrix *map)
{
    AdgEntityPrivate *data = entity->data;

    if (map == NULL)
        cairo_matrix_init_identity(&data->global_map);
    else
        adg_matrix_copy(&data->global_map, map);
}

static void
set_local_map(AdgEntity *entity, const AdgMatrix *map)
{
    AdgEntityPrivate *data = entity->data;

    if (map == NULL)
        cairo_matrix_init_identity(&data->local_map);
    else
        adg_matrix_copy(&data->local_map, map);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgEntityPrivate *data = entity->data;

    ADG_SET(data->flags, RENDERED);
}
