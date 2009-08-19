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
 * This abstract class provides the base for all renderable objects.
 *
 * To provide a proper #AdgEntity derived type, you must at least
 * implement the render() virtual method. Also, if you are using
 * some sort of caching, you must implement invalidate() to clear
 * this cache.
 **/

/**
 * AdgEntity:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

/**
 * AdgEntityClass:
 * @parent_set: called after the parent has changed
 * @context_set: called after the context has changed
 * @invalidate: invalidating callback, used to clear the cache
 * @render: rendering callback, it must be implemented
 *
 * Any entity (if not abstract) must implement at least the @render method.
 **/


#include "adg-entity.h"
#include "adg-entity-private.h"
#include "adg-canvas.h"
#include "adg-font-style.h"
#include "adg-dim-style.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_entity_parent_class)


enum {
    PROP_0,
    PROP_PARENT,
    PROP_CONTEXT,
    PROP_GLOBAL_MAP,
    PROP_LOCAL_MAP
};

enum {
    PARENT_SET,
    CONTEXT_SET,
    INVALIDATE,
    RENDER,
    LAST_SIGNAL
};


static void             dispose                 (GObject         *object);
static void             get_property            (GObject         *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void             set_property            (GObject         *object,
                                                 guint            prop_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);
static gboolean         set_parent              (AdgEntity       *entity,
                                                 AdgEntity       *parent);
static gboolean         set_context             (AdgEntity       *entity,
                                                 AdgContext      *context);
static void             set_global_map          (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static void             set_local_map           (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static void             real_invalidate         (AdgEntity       *entity,
                                                 gpointer         user_data);
static void             real_render             (AdgEntity       *entity,
                                                 cairo_t         *cr,
                                                 gpointer         user_data);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_ABSTRACT_TYPE(AdgEntity, adg_entity, G_TYPE_INITIALLY_UNOWNED);


static void
adg_entity_class_init(AdgEntityClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;
    GClosure *closure;
    GType param_types[1];

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgEntityPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
    gobject_class->dispose = dispose;

    klass->parent_set = NULL;
    klass->context_set = NULL;
    klass->invalidate = NULL;
    klass->render = NULL;

    param = g_param_spec_object("parent",
                                P_("Parent Entity"),
                                P_("The parent entity of this entity or NULL if this is a top-level entity"),
                                ADG_TYPE_ENTITY, G_PARAM_READWRITE);
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
     * @old_parent: the old parent
     *
     * Emitted after the parent entity has changed. The new parent
     * can be inspected using adg_entity_get_parent().
     *
     * It is allowed for both old and new parent to be %NULL.
     **/
    signals[PARENT_SET] = g_signal_new("parent-set",
                                       G_OBJECT_CLASS_TYPE(gobject_class),
                                       G_SIGNAL_RUN_FIRST,
                                       G_STRUCT_OFFSET(AdgEntityClass, parent_set),
                                       NULL, NULL,
                                       g_cclosure_marshal_VOID__OBJECT,
                                       G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgEntity::context-set:
     * @entity: an #AdgEntity
     * @old_context: the old context
     *
     * Emitted after the context has changed.
     * Emitted after the context has changed. The new context can be
     * inspected using adg_entity_get_context().
     *
     * It is allowed for both old and new context to be %NULL.
     **/
    signals[CONTEXT_SET] = g_signal_new("context-set",
                                        G_OBJECT_CLASS_TYPE(gobject_class),
                                        G_SIGNAL_RUN_FIRST,
                                        G_STRUCT_OFFSET(AdgEntityClass, context_set),
                                        NULL, NULL,
                                        g_cclosure_marshal_VOID__OBJECT,
                                        G_TYPE_NONE, 1, ADG_TYPE_CONTEXT);

    /**
     * AdgEntity::invalidate:
     * @entity: an #AdgEntity
     *
     * The inverse of the rendering. Usually, invalidation causes the
     * cache of @entity to be cleared. After a succesful invalidation
     * the rendered flag is reset: you can access its state using
     * adg_entity_get_rendered().
     **/
    closure = g_cclosure_new(G_CALLBACK(real_invalidate),
                             (gpointer)0xdeadbeaf, NULL);
    signals[INVALIDATE] = g_signal_newv("invalidate", ADG_TYPE_ENTITY,
                                        G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                                        g_cclosure_marshal_VOID__VOID,
                                        G_TYPE_NONE, 0, param_types);

    /**
     * AdgEntity::render:
     * @entity: an #AdgEntity
     * @cr: a #cairo_t drawing context
     *
     * Causes the rendering of @entity on @cr. After a succesful rendering
     * the rendered flag is set: you can access its state using
     * adg_entity_get_rendered().
     **/
    closure = g_cclosure_new(G_CALLBACK(real_render),
                             (gpointer)0xdeadbeaf, NULL);
    param_types[0] = G_TYPE_POINTER;
    signals[RENDER] = g_signal_newv("render", ADG_TYPE_ENTITY,
                                    G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                                    g_cclosure_marshal_VOID__POINTER,
                                    G_TYPE_NONE, 1, param_types);
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
dispose(GObject *object)
{
    AdgEntity *entity;
    AdgEntity *parent;

    entity = (AdgEntity *) object;
    parent = adg_entity_get_parent(entity);

    /* These calls will emit a "notify" signal both for parent and
     * context if they are not NULL. Consequentially, the references
     * to the old parent and context are dropped. */
    adg_entity_set_parent(entity, NULL);
    adg_entity_set_context(entity, NULL);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
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
        g_value_set_object(value, data->parent);
        break;
    case PROP_CONTEXT:
        g_value_set_object(value, adg_entity_get_context(entity));
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


/**
 * adg_entity_get_parent:
 * @entity: an #AdgEntity
 *
 * Gets the parent of @entity.
 *
 * Returns: the parent entity or %NULL on errors or if @entity is a toplevel
 **/
AdgEntity *
adg_entity_get_parent(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return data->parent;
}

/**
 * adg_entity_set_parent:
 * @entity: an #AdgEntity
 * @parent: the parent entity
 *
 * Sets a new parent on @entity.
 *
 * This function is only useful in entity implementations.
 **/
void
adg_entity_set_parent(AdgEntity *entity, AdgEntity *parent)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    if (set_parent(entity, parent))
        g_object_notify((GObject *) entity, "parent");
}

/**
 * adg_entity_context:
 * @entity: an #AdgEntity instance
 *
 * Gets the context to be used for @entity. If no context was
 * explicitely set, it returns the parent context.
 *
 * Returns: the requested context or %NULL on errors or if @entity
 *          does not have any parent with a context defined
 **/
AdgContext *
adg_entity_context(AdgEntity *entity)
{
    AdgContext *context;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    context = adg_entity_get_context(entity);

    if (context == NULL) {
        AdgEntity *parent = adg_entity_get_parent(entity);

        if (parent != NULL)
            context = adg_entity_get_context(parent);
    }

    return context;
}

/**
 * adg_entity_get_context:
 * @entity: an #AdgEntity instance
 *
 * Gets the context associated to @entity. This is an accessor method:
 * if you need to get the context for rendering, use adg_entity_context()
 * instead.
 *
 * Returns: the context binded to @entity
 **/
AdgContext *
adg_entity_get_context(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return data->context;
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

    if (set_context(entity, context))
        g_object_notify((GObject *) entity, "context");
}

/**
 * adg_entity_get_canvas:
 * @entity: an #AdgEntity
 *
 * Walks on the @entity hierarchy and gets the first parent of @entity that is
 * of #AdgCanvas derived type.
 *
 * Returns: the requested canvas or %NULL on errors or if there is
 *          no #AdgCanvas in the @entity hierarchy
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
 * adg_entity_get_rendered:
 * @entity: an #AdgEntity object
 *
 * This function is only useful in entity implementations.
 * Gets the rendered flag of @entity.
 *
 * Returns: the current rendered state
 **/
gboolean
adg_entity_get_rendered(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), FALSE);

    data = entity->data;

    return ADG_ISSET(data->flags, RENDERED);
}

/**
 * adg_entity_set_rendered:
 * @entity: an #AdgEntity object
 * @rendered: new state for the rendered flag
 *
 * This function is only useful in entity implementations.
 * Sets the rendered flag of @entity to @rendered.
 **/
void
adg_entity_set_rendered(AdgEntity *entity, gboolean rendered)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    data = entity->data;

    if (rendered)
        ADG_SET(data->flags, RENDERED);
    else
        ADG_UNSET(data->flags, RENDERED);
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
 * or the style is undefined within this context, gets the style from its
 * parent entity.
 *
 * Returns: the requested style or %NULL on errors or if there is no
 *          context with this style defined in the @entity hierarchy
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
 * adg_entity_apply_font:
 * @entity: an #AdgEntity
 * @font_id: a font id
 * @cr: a #cairo_t drawing context
 *
 * Applies the specified font to the @cr cairo context. It is similar
 * to adg_entity_apply() but instead of looking for a font slot, it
 * searches for a specific font style by inspecting composite styles too.
 *
 * A typical example is when the tolerance font should be applied: this
 * is a font style embedded in the dim style, so it is not enough to
 * use adg_entity_apply().
 **/
void
adg_entity_apply_font(AdgEntity *entity, AdgFontStyleId font_id, cairo_t *cr)
{
    AdgStyle *style;

    switch (font_id) {

    case ADG_FONT_STYLE_TEXT:
        style = adg_entity_get_style(entity, ADG_SLOT_FONT_STYLE);
        break;

    case ADG_FONT_STYLE_VALUE:
        style = adg_entity_get_style(entity, ADG_SLOT_DIM_STYLE);
        style = adg_dim_style_get_value_style((AdgDimStyle *) style);
        break;

    case ADG_FONT_STYLE_TOLERANCE:
        style = adg_entity_get_style(entity, ADG_SLOT_DIM_STYLE);
        style = adg_dim_style_get_tolerance_style((AdgDimStyle *) style);
        break;

    case ADG_FONT_STYLE_NOTE:
        style = adg_entity_get_style(entity, ADG_SLOT_DIM_STYLE);
        style = adg_dim_style_get_note_style((AdgDimStyle *) style);
        break;

    default:
        g_warning ("%s: invalid font id (%d)", G_STRLOC, font_id);
        return;
    }

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


static gboolean
set_parent(AdgEntity *entity, AdgEntity *parent)
{
    AdgEntityPrivate *data;
    AdgEntity *old_parent;

    data = entity->data;
    old_parent = data->parent;

    /* Check if parent has changed */
    if (parent == old_parent)
        return FALSE;

    if (parent != NULL)
        g_object_ref(parent);

    data->parent = parent;
    g_signal_emit(entity, signals[PARENT_SET], 0, old_parent);

    if (old_parent != NULL)
        g_object_unref(old_parent);

    return TRUE;
}

static gboolean
set_context(AdgEntity *entity, AdgContext *context)
{
    AdgEntityPrivate *data;
    AdgContext *old_context;

    data = entity->data;
    old_context = data->context;

    /* Check if context has changed */
    if (context == old_context)
        return FALSE;

    if (context != NULL)
        g_object_ref(context);

    data->context = context;
    g_signal_emit(entity, signals[CONTEXT_SET], 0, old_context);

    if (old_context != NULL)
        g_object_unref(old_context);

    return TRUE;
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
real_invalidate(AdgEntity *entity, gpointer user_data)
{
    AdgEntityClass *entity_class;
    AdgEntityPrivate *data;

    g_assert(user_data == (gpointer) 0xdeadbeaf);

    entity_class = ADG_ENTITY_GET_CLASS(entity);
    if (entity_class->invalidate == NULL) {
        /* Assume the entity does not have cache to be cleared */
    } else if (!entity_class->invalidate(entity)) {
        return;
    }

    data = entity->data;
    ADG_UNSET(data->flags, RENDERED);
}

static void
real_render(AdgEntity *entity, cairo_t *cr, gpointer user_data)
{
    AdgEntityClass *entity_class;
    AdgEntityPrivate *data;

    g_assert(user_data == (gpointer) 0xdeadbeaf);

    entity_class = ADG_ENTITY_GET_CLASS(entity);
    if (entity_class->render == NULL) {
        /* The render method must be defined */
        g_warning("%s: `render' method not implemented for type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(entity)));
        return;
    }

    data = entity->data;

    cairo_save(cr);
    cairo_transform(cr, &data->global_map);

    if (entity_class->render(entity, cr))
        ADG_SET(data->flags, RENDERED);

    cairo_restore(cr);
}
