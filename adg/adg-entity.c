/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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

#define PARENT_CLASS ((GInitiallyUnownedClass *) adg_entity_parent_class)


enum {
    PROP_0,
    PROP_PARENT,
    PROP_CONTEXT
};

enum {
    PARENT_SET,
    MODEL_MATRIX_CHANGED,
    PAPER_MATRIX_CHANGED,
    INVALIDATE,
    RENDER,
    LAST_SIGNAL
};


static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             dispose                 (GObject        *object);
static AdgContainer *   get_parent              (AdgEntity      *entity);
static void             set_parent              (AdgEntity      *entity,
                                                 AdgContainer   *parent);
static void             parent_set              (AdgEntity      *entity,
                                                 AdgContainer   *old_parent);
static AdgContext *     get_context             (AdgEntity      *entity);
static void             set_context             (AdgEntity      *entity,
                                                 AdgContext     *context);
static void             model_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static void             paper_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static const AdgMatrix *get_model_matrix        (AdgEntity      *entity);
static const AdgMatrix *get_paper_matrix        (AdgEntity      *entity);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_ABSTRACT_TYPE(AdgEntity, adg_entity, G_TYPE_INITIALLY_UNOWNED)


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
    klass->model_matrix_changed = model_matrix_changed;
    klass->paper_matrix_changed = NULL;
    klass->invalidate = NULL;
    klass->render = render;
    klass->get_model_matrix = get_model_matrix;
    klass->get_paper_matrix = get_paper_matrix;

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

  /**
   * AdgEntity::parent-set:
   * @entity: an #AdgEntity
   * @parent: the #AdgContainer parent of @entity
   *
   * Emitted after the parent container has changed.
   */
    signals[PARENT_SET] =
        g_signal_new("parent-set",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, parent_set),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, ADG_TYPE_CONTAINER);

  /**
   * AdgEntity::model-matrix-changed:
   * @entity: an #AdgEntity
   * @parent_matrix: the parent model matrix
   *
   * Emitted after the current model matrix has changed.
   */
    signals[MODEL_MATRIX_CHANGED] =
        g_signal_new("model-matrix-changed",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, model_matrix_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOXED,
                     G_TYPE_NONE, 1,
                     ADG_TYPE_MATRIX | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * AdgEntity::paper-matrix-changed:
   * @entity: an #AdgEntity
   * @parent_matrix: the parent paper matrix
   *
   * Emitted after the current paper matrix has changed.
   */
    signals[PAPER_MATRIX_CHANGED] =
        g_signal_new("paper-matrix-changed",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, paper_matrix_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOXED,
                     G_TYPE_NONE, 1,
                     ADG_TYPE_MATRIX | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * AdgEntity::invalidate:
   * @entity: an #AdgEntity
   *
   * Clears the cached data of @entity.
   */
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
   */
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
    AdgEntityPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(entity,
                                                         ADG_TYPE_ENTITY,
                                                         AdgEntityPrivate);
    priv->parent = NULL;
    priv->flags = 0;
    priv->context = NULL;

    entity->priv = priv;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgEntity *entity = (AdgEntity *) object;

    switch (prop_id) {
    case PROP_PARENT:
        g_value_set_object(value, get_parent(entity));
        break;
    case PROP_CONTEXT:
        g_value_set_object(value, get_context(entity));
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
        set_parent(entity, (AdgContainer *) g_value_get_object(value));
        break;
    case PROP_CONTEXT:
        set_context(entity, g_value_get_object(value));
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

    entity = (AdgEntity *) object;

    if (entity->priv->parent)
        adg_container_remove(entity->priv->parent, entity);

    PARENT_CLASS->dispose(object);
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
 */
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
 */
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
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    if (entity->priv->context)
        return entity->priv->context;

    if (entity->priv->parent)
        return adg_entity_get_context((AdgEntity *) entity->priv->parent);

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
 */
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
 * adg_entity_get_model_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the model matrix to be used in rendering this @entity.
 *
 * Return value: the requested matrix
 */
const AdgMatrix *
adg_entity_get_model_matrix(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);
    return ADG_ENTITY_GET_CLASS(entity)->get_model_matrix(entity);
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
adg_entity_get_paper_matrix(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);
    return ADG_ENTITY_GET_CLASS(entity)->get_paper_matrix(entity);
}

/**
 * adg_entity_scale_to_model:
 * @entity: an #AdgEntity object
 * @cr: a #cairo_t drawing context
 *
 * Sets the model matrix as current matrix on @cr. The translation
 * and rotation component of the previous matrix are kept: only the
 * scale is changed.
 */
void
adg_entity_scale_to_model(AdgEntity *entity, cairo_t *cr)
{
    const AdgMatrix *model_matrix;
    cairo_matrix_t matrix;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    model_matrix = ADG_ENTITY_GET_CLASS(entity)->get_model_matrix(entity);
    cairo_get_matrix(cr, &matrix);

    matrix.xx = model_matrix->xx;
    matrix.yy = model_matrix->yy;
    cairo_set_matrix(cr, &matrix);
}

/**
 * adg_entity_scale_to_paper:
 * @entity: an #AdgEntity object
 * @cr: a #cairo_t drawing context
 *
 * Sets the paper matrix as current matrix on @cr. The translation
 * and rotation component of the previous matrix are kept: only the
 * scale is changed.
 */
void
adg_entity_scale_to_paper(AdgEntity *entity, cairo_t *cr)
{
    const AdgMatrix *paper_matrix;
    cairo_matrix_t matrix;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    paper_matrix = ADG_ENTITY_GET_CLASS(entity)->get_paper_matrix(entity);
    cairo_get_matrix(cr, &matrix);

    matrix.xx = paper_matrix->xx;
    matrix.yy = paper_matrix->yy;
    cairo_set_matrix(cr, &matrix);
}

/**
 * adg_entity_build_paper2model:
 * @entity: an #AdgEntity
 * @matrix: the destination matrix
 *
 * Builds a matrix to translate from paper to model space and
 * put the result in @matrix.
 *
 * Return value: %TRUE on success, %FALSE on errors
 **/
gboolean
adg_entity_build_paper2model(AdgEntity *entity, AdgMatrix *matrix)
{
    cairo_status_t status;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);
    g_return_val_if_fail(matrix != NULL, FALSE);

    adg_matrix_copy(matrix, adg_entity_get_model_matrix(entity));
    status = cairo_matrix_invert(matrix);
    if (status != CAIRO_STATUS_SUCCESS) {
        g_error("Unable to invert model matrix (cairo message: %s)",
                cairo_status_to_string(status));
        return FALSE;
    }

    cairo_matrix_multiply(matrix, matrix, adg_entity_get_paper_matrix(entity));
    return TRUE;
}

/**
 * adg_entity_build_model2paper:
 * @entity: an #AdgEntity
 * @matrix: the destination matrix
 *
 * Builds a matrix to translate from model to paper space and
 * put the result in @matrix.
 *
 * Return value: %TRUE on success, %FALSE on errors
 **/
gboolean
adg_entity_build_model2paper(AdgEntity *entity, AdgMatrix *matrix)
{
    cairo_status_t status;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);
    g_return_val_if_fail(matrix != NULL, FALSE);

    adg_matrix_copy(matrix, adg_entity_get_paper_matrix(entity));
    status = cairo_matrix_invert(matrix);
    if (status != CAIRO_STATUS_SUCCESS) {
        g_error("Unable to invert paper matrix (cairo message: %s)",
                cairo_status_to_string(status));
        return FALSE;
    }

    cairo_matrix_multiply(matrix, matrix, adg_entity_get_model_matrix(entity));
    return TRUE;
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
adg_entity_model_matrix_changed(AdgEntity *entity,
                                const AdgMatrix *parent_matrix)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[MODEL_MATRIX_CHANGED], 0, parent_matrix);
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
adg_entity_paper_matrix_changed(AdgEntity *entity,
                                const AdgMatrix *parent_matrix)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[PAPER_MATRIX_CHANGED], 0, parent_matrix);
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
adg_entity_get_style(AdgEntity *entity, AdgStyleSlot style_slot)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    if (entity->priv->context) {
        AdgStyle *style = adg_context_get_style(entity->priv->context,
                                                style_slot);
        if (style)
            return style;
    }

    if (entity->priv->parent)
        return adg_entity_get_style((AdgEntity *) entity->priv->parent,
                                    style_slot);

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
 * adg_entity_point_to_pair:
 * @entity: an #AdgEntity
 * @point: the source #AdgPoint
 * @pair: the destination #AdgPair
 * @cr: a #cairo_t drawing context
 *
 * Converts @point to @pair using the model and paper matrix of @entity,
 * as if the current matrix is an identity matrix.
 **/
void
adg_entity_point_to_pair(AdgEntity *entity, const AdgPoint *point,
                         AdgPair *pair, cairo_t *cr)
{
    AdgMatrix inverted_ctm;
    const AdgMatrix *model_matrix;
    const AdgMatrix *paper_matrix;
    AdgPair model_pair, paper_pair;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(point != NULL);
    g_return_if_fail(pair != NULL);

    cairo_get_matrix(cr, &inverted_ctm);
    cairo_matrix_invert(&inverted_ctm);
    model_matrix = ADG_ENTITY_GET_CLASS(entity)->get_model_matrix(entity);
    paper_matrix = ADG_ENTITY_GET_CLASS(entity)->get_paper_matrix(entity);

    adg_pair_copy(&model_pair, &point->model);
    cpml_pair_transform(&model_pair, model_matrix);
    
    adg_pair_copy(&paper_pair, &point->paper);
    cpml_pair_transform(&paper_pair, paper_matrix);

    pair->x = model_pair.x + paper_pair.x;
    pair->y = model_pair.y + paper_pair.y;

    cpml_pair_transform(pair, &inverted_ctm);
}

/**
 * adg_entity_point_to_model_pair:
 * @entity: an #AdgEntity
 * @point: the source #AdgPoint
 * @pair: the destination #AdgPair
 *
 * Converts @point to @pair in model space.
 **/
void
adg_entity_point_to_model_pair(AdgEntity *entity,
                               const AdgPoint *point, AdgPair *pair)
{
    const AdgMatrix *paper_matrix;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(point != NULL);
    g_return_if_fail(pair != NULL);

    paper_matrix = ADG_ENTITY_GET_CLASS(entity)->get_paper_matrix(entity);
    adg_pair_copy(pair, &point->paper);
    cpml_pair_transform(pair, paper_matrix);

    pair->x += point->model.x;
    pair->y += point->model.y;
}

/**
 * adg_entity_point_to_paper_pair:
 * @entity: an #AdgEntity
 * @point: the source #AdgPoint
 * @pair: the destination #AdgPair
 *
 * Converts @point to @pair in paper space.
 **/
void
adg_entity_point_to_paper_pair(AdgEntity *entity,
                               const AdgPoint *point, AdgPair *pair)
{
    const AdgMatrix *model_matrix;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(point != NULL);
    g_return_if_fail(pair != NULL);

    model_matrix = ADG_ENTITY_GET_CLASS(entity)->get_model_matrix(entity);
    adg_pair_copy(pair, &point->model);
    cpml_pair_transform(pair, model_matrix);

    pair->x += point->paper.x;
    pair->y += point->paper.y;
    g_print("Pair (%lf, %lf)\n", pair->x, pair->y);
}

/**
 * adg_entity_model_matrix_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the model matrix didn't change from the last render
 */
gboolean
adg_entity_model_matrix_applied(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);
    return ADG_ISSET(entity->priv->flags, MODEL_MATRIX_APPLIED);
}

/**
 * adg_entity_paper_matrix_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the paper matrix didn't change from the last render
 */
gboolean
adg_entity_paper_matrix_applied(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);
    return ADG_ISSET(entity->priv->flags, PAPER_MATRIX_APPLIED);
}

/**
 * adg_entity_model_applied:
 * @entity: an #AdgEntity
 *
 * Return value: %TRUE if the model didn't change from the last render
 */
gboolean
adg_entity_model_applied(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);
    return ADG_ISSET(entity->priv->flags, MODEL_APPLIED);
}

/**
 * adg_entity_invalidate:
 * @entity: an #AdgEntity
 *
 * Emits the "invalidate" signal on @entity and all its children, if any,
 * so subsequent rendering will need a global recomputation.
 */
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
 */
void
adg_entity_render(AdgEntity *entity, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[RENDER], 0, cr);
}


static AdgContainer *
get_parent(AdgEntity *entity)
{
    return entity->priv->parent;
}

static void
set_parent(AdgEntity *entity, AdgContainer *parent)
{
    entity->priv->parent = parent;
}

static void
parent_set(AdgEntity *entity, AdgContainer *old_parent)
{
    if (ADG_IS_CONTAINER(old_parent)) {
        const AdgMatrix *old_model;
        const AdgMatrix *old_paper;

        old_model = adg_entity_get_model_matrix((AdgEntity *) old_parent);
        old_paper = adg_entity_get_paper_matrix((AdgEntity *) old_parent);

        adg_entity_model_matrix_changed(entity, old_model);
        adg_entity_paper_matrix_changed(entity, old_paper);
    }
}

static AdgContext *
get_context(AdgEntity *entity)
{
    AdgEntity *parent;

    if (entity->priv->context)
        return entity->priv->context;

    parent = (AdgEntity *) entity->priv->parent;

    return parent ? ADG_ENTITY_GET_CLASS(parent)->get_context(parent) : NULL;
}

static void
set_context(AdgEntity *entity, AdgContext *context)
{
    if (entity->priv->context)
        g_object_unref((GObject *) entity->priv->context);

    g_object_ref((GObject *) context);
    entity->priv->context = context;
}

static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    ADG_UNSET(entity->priv->flags, MODEL_MATRIX_APPLIED);
}

static void
paper_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    ADG_UNSET(entity->priv->flags, PAPER_MATRIX_APPLIED);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    ADG_SET(entity->priv->flags,
            MODEL_MATRIX_APPLIED | PAPER_MATRIX_APPLIED | MODEL_APPLIED);
}

static const AdgMatrix *
get_model_matrix(AdgEntity *entity)
{
    return adg_entity_get_model_matrix((AdgEntity *) entity->priv->parent);
}

static const AdgMatrix *
get_paper_matrix(AdgEntity *entity)
{
    return adg_entity_get_paper_matrix((AdgEntity *) entity->priv->parent);
}
