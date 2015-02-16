/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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
 * To create a drawing you usually create entities by calling their
 * specific constructors and add them to a single #AdgCanvas
 * instance. When cleaning up, you therefore need to destroy only
 * that canvas instance with adg_entity_destroy(): this in turn
 * will destroy every contained entity. This is pretty similar to
 * how the GTK+ world works: you add #GtkWidget instances to a
 * single #GtkWindow and destroy only that window when finished.
 *
 * To provide a proper #AdgEntity derived type, you must at least
 * implement its arrange() and render() virtual methods. Also, if
 * you are using some sort of caching, ensure to clear it in the
 * invalidate() method.
 *
 * Since: 1.0
 **/

/**
 * AdgEntity:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgEntityClass:
 * @destroy:        when a destroy request has been explicitely requested
 * @parent_set:     called whenever the parent of an entity has changed
 * @global_changed: the global matrix has been invalidated
 * @local_changed:  the local matrix has been invalidated
 * @invalidate:     invalidating callback, used to clear the internal cache
 * @arrange:        prepare the layout and fill the extents struct
 * @render:         rendering callback, it must be implemented by every entity
 *
 * Any entity (if not abstract) must implement at least the @render method.
 * The other signal handlers can be overriden to provide custom behaviors
 * and usually must chain up the original handler.
 *
 * Since: 1.0
 **/

/**
 * AdgEntityCallback:
 * @entity: an #AdgEntity
 * @user_data: a general purpose pointer
 *
 * Callback used when inspecting or browsing entities. For example,
 * it is passed to adg_model_foreach_dependency() to perform an
 * operation on all the entities depending on an #AdgModel.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#if GTK3_ENABLED || GTK2_ENABLED
#include <gtk/gtk.h>
#endif

#include "adg-container.h"
#include "adg-table.h"
#include "adg-title-block.h"
#include <adg-canvas.h>
#include "adg-dress.h"
#include "adg-style.h"
#include "adg-model.h"
#include "adg-point.h"
#include "adg-cairo-fallback.h"

#include "adg-entity-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_entity_parent_class)


G_DEFINE_ABSTRACT_TYPE(AdgEntity, adg_entity, G_TYPE_INITIALLY_UNOWNED)

enum {
    PROP_0,
    PROP_PARENT,
    PROP_GLOBAL_MAP,
    PROP_LOCAL_MAP,
    PROP_LOCAL_MIX
};

enum {
    DESTROY,
    PARENT_SET,
    GLOBAL_CHANGED,
    LOCAL_CHANGED,
    INVALIDATE,
    ARRANGE,
    RENDER,
    LAST_SIGNAL
};


static void             _adg_dispose            (GObject         *object);
static void             _adg_get_property       (GObject         *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void             _adg_set_property       (GObject         *object,
                                                 guint            prop_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);
static void             _adg_destroy            (AdgEntity       *entity);
static void             _adg_set_parent         (AdgEntity       *entity,
                                                 AdgEntity       *parent);
static void             _adg_global_changed     (AdgEntity       *entity);
static void             _adg_local_changed      (AdgEntity       *entity);
static void             _adg_real_invalidate    (AdgEntity       *entity);
static void             _adg_real_arrange       (AdgEntity       *entity);
static void             _adg_real_render        (AdgEntity       *entity,
                                                 cairo_t         *cr);
static guint            _adg_signals[LAST_SIGNAL] = { 0 };
static gboolean         _adg_show_extents = FALSE;


static void
adg_entity_class_init(AdgEntityClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;
    GClosure *closure;
    GType param_types[1];

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgEntityPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    klass->destroy = _adg_destroy;
    klass->parent_set = NULL;
    klass->global_changed = _adg_global_changed;
    klass->local_changed = _adg_local_changed;
    klass->invalidate = NULL;
    klass->arrange= NULL;
    klass->render = NULL;

    param = g_param_spec_object("parent",
                                P_("Parent Entity"),
                                P_("The parent entity of this entity or NULL if this is a top-level entity"),
                                ADG_TYPE_ENTITY,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PARENT, param);

    param = g_param_spec_boxed("global-map",
                               P_("Global Map"),
                               P_("The transformation to be combined with the parent ones to get the global matrix"),
                               CAIRO_GOBJECT_TYPE_MATRIX,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GLOBAL_MAP, param);

    param = g_param_spec_boxed("local-map",
                               P_("Local Map"),
                               P_("The local transformation that could be used to compute the local matrix in the way specified by the #AdgEntity:local-mix property"),
                               CAIRO_GOBJECT_TYPE_MATRIX,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOCAL_MAP, param);

    param = g_param_spec_enum("local-mix",
                              P_("Local Mix Method"),
                              P_("Define how the local maps of the entity and its ancestors should be combined to get the local matrix"),
                              ADG_TYPE_MIX, ADG_MIX_ANCESTORS,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOCAL_MIX, param);

    /**
     * AdgEntity::destroy:
     * @entity: an #AdgEntity
     *
     * Emitted to explicitely destroy @entity. It unreferences
     * @entity so that will be destroyed, unless the caller owns
     * an additional references added with g_object_ref().
     *
     * In the usual case, this is equivalent of calling
     * g_object_unref() on @entity but, for composite entities or
     * containers, the destroy signal is propagated to the children.
     *
     * Since: 1.0
     **/
    _adg_signals[DESTROY] =
        g_signal_new("destroy",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, destroy),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgEntity::parent-set:
     * @entity: an #AdgEntity
     * @old_parent: the old parent
     *
     * Emitted after the parent entity has changed. The new parent
     * can be inspected using adg_entity_get_parent().
     *
     * It is allowed for both old and new parent to
     * be <constant>NULL</constant>.
     *
     * Since: 1.0
     **/
    _adg_signals[PARENT_SET] =
        g_signal_new("parent-set",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, parent_set),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgEntity::global-changed:
     * @entity: an #AdgEntity
     *
     * Emitted when the global map of @entity or any of its parent
     * has changed. The default handler will compute the new global
     * matrix, updating the internal cache.
     *
     * This signal has lazy emission, i.e. it is emitted only when
     * the global matrix is requested, typically in the arrange phase.
     *
     * Since: 1.0
     **/
    _adg_signals[GLOBAL_CHANGED] =
        g_signal_new("global-changed",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, global_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgEntity::local-changed:
     * @entity: an #AdgEntity
     *
     * Emitted when the local map of @entity or any of its parent
     * has changed. The default handler will compute the new local
     * matrix, updating the internal cache.
     *
     * This signal has lazy emission, i.e. it is emitted only when
     * the local matrix is requested, typically in the arrange phase.
     *
     * Since: 1.0
     **/
    _adg_signals[LOCAL_CHANGED] =
        g_signal_new("local-changed",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(AdgEntityClass, local_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgEntity::invalidate:
     * @entity: an #AdgEntity
     *
     * Invalidates the whole @entity, that is resets all the cache
     * (if present) built during the #AdgEntity::arrange signal.
     * The resulting state is a clean entity, similar to what you
     * have just before the first rendering.
     *
     * Since: 1.0
     **/
    closure = g_cclosure_new(G_CALLBACK(_adg_real_invalidate), NULL, NULL);
    _adg_signals[INVALIDATE] =
        g_signal_newv("invalidate", ADG_TYPE_ENTITY,
                      G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0, param_types);

    /**
     * AdgEntity::arrange:
     * @entity: an #AdgEntity
     *
     * Arranges the layout of @entity, updating the cache if necessary,
     * and computes the extents of @entity.
     *
     * Since: 1.0
     **/
    closure = g_cclosure_new(G_CALLBACK(_adg_real_arrange), NULL, NULL);
    _adg_signals[ARRANGE] =
        g_signal_newv("arrange", ADG_TYPE_ENTITY,
                      G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0, param_types);

    /**
     * AdgEntity::render:
     * @entity: an #AdgEntity
     * @cr: a #cairo_t drawing context
     *
     * Causes the rendering of @entity on @cr. A render signal will
     * automatically emit #AdgEntity::arrange just before the real
     * rendering on the cairo context.
     *
     * Since: 1.0
     **/
    closure = g_cclosure_new(G_CALLBACK(_adg_real_render), NULL, NULL);
    param_types[0] = G_TYPE_POINTER;
    _adg_signals[RENDER] =
        g_signal_newv("render", ADG_TYPE_ENTITY,
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
    cairo_matrix_init_identity(&data->global_map);
    cairo_matrix_init_identity(&data->local_map);
    data->local_mix = ADG_MIX_ANCESTORS;
    data->hash_styles = NULL;
    data->global.is_defined = FALSE;
    adg_matrix_copy(&data->global.matrix, adg_matrix_null());
    data->local.is_defined = FALSE;
    adg_matrix_copy(&data->local.matrix, adg_matrix_null());
    data->extents.is_defined = FALSE;

    entity->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgEntity *entity;
    AdgEntityPrivate *data;

    entity = (AdgEntity *) object;
    data = entity->data;

    /* This call will emit a "notify" signal for parent.
     * Consequentially, the references to the old parent is dropped. */
    adg_entity_set_parent(entity, NULL);

    if (data->hash_styles != NULL) {
        g_hash_table_destroy(data->hash_styles);
        data->hash_styles = NULL;
    }

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgEntity *entity;
    AdgEntityPrivate *data;

    entity = (AdgEntity *) object;
    data = entity->data;

    switch (prop_id) {
    case PROP_PARENT:
        g_value_set_object(value, data->parent);
        break;
    case PROP_GLOBAL_MAP:
        g_value_set_boxed(value, &data->global_map);
        break;
    case PROP_LOCAL_MAP:
        g_value_set_boxed(value, &data->local_map);
        break;
    case PROP_LOCAL_MIX:
        g_value_set_enum(value, data->local_mix);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgEntityPrivate *data = ((AdgEntity *) object)->data;

    switch (prop_id) {
    case PROP_PARENT:
        _adg_set_parent((AdgEntity *) object,
                        (AdgEntity *) g_value_get_object(value));
        break;
    case PROP_GLOBAL_MAP:
        adg_matrix_copy(&data->global_map, g_value_get_boxed(value));
        data->global.is_defined = FALSE;
        break;
    case PROP_LOCAL_MAP:
        adg_matrix_copy(&data->local_map, g_value_get_boxed(value));
        data->local.is_defined = FALSE;
        break;
    case PROP_LOCAL_MIX:
        data->local_mix = g_value_get_enum(value);
        data->local.is_defined = FALSE;
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_switch_extents:
 * @state: new extents state
 *
 * Strokes (if @state is <constant>TRUE</constant>) a rectangle
 * around every entity to show their extents. Useful for
 * debugging purposes.
 *
 * Since: 1.0
 **/
void
adg_switch_extents(gboolean state)
{
    _adg_show_extents = state;
}

/**
 * adg_entity_destroy:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::destroy signal on @entity and on all of
 * its children, if any.
 *
 * Since: 1.0
 **/
void
adg_entity_destroy(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[DESTROY], 0);
}

/**
 * adg_entity_get_canvas:
 * @entity: an #AdgEntity
 *
 * Walks on the @entity hierarchy and gets the first parent of @entity,
 * that is the first #AdgCanvas instance. The returned object is
 * owned by @entity and should not be freed or modified.
 *
 * Returns: (transfer none): the requested canvas or <constant>NULL</constant> on errors or if there is no #AdgCanvas in the @entity hierarchy.
 *
 * Since: 1.0
 **/
AdgCanvas *
adg_entity_get_canvas(AdgEntity *entity)
{
    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    while (entity) {
        if (ADG_IS_CANVAS(entity))
            return (AdgCanvas *) entity;

        entity = adg_entity_get_parent(entity);
    }

    return NULL;
}

/**
 * adg_entity_set_parent:
 * @entity: an #AdgEntity
 * @parent: the parent entity
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Sets a new parent on @entity. Changing the @parent of an entity
 * emits the #AdgEntity::parent-set signal on it.
 *
 * There is no reference management at this level: they should be
 * handled at a higher level, e.g. by #AdgContainer.
 *
 * Since: 1.0
 **/
void
adg_entity_set_parent(AdgEntity *entity, AdgEntity *parent)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_object_set(entity, "parent", parent, NULL);
}

/**
 * adg_entity_get_parent:
 * @entity: an #AdgEntity
 *
 * Gets the parent of @entity. The returned object is owned
 * by @entity and should not be freed or modified.
 *
 * Returns: (transfer none): the parent entity or <constant>NULL</constant> on errors or if @entity is a toplevel.
 *
 * Since: 1.0
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
 * adg_entity_set_global_map:
 * @entity: an #AdgEntity object
 * @map: the new map
 *
 * Sets the new global transformation of @entity to @map:
 * the old map is discarded. If @map is <constant>NULL</constant>,
 * the global map is left unchanged.
 *
 * Since: 1.0
 **/
void
adg_entity_set_global_map(AdgEntity *entity, const cairo_matrix_t *map)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_object_set(entity, "global-map", map, NULL);
}

/**
 * adg_entity_transform_global_map:
 * @entity: an #AdgEntity object
 * @transformation: the transformation to apply
 * @mode: how @transformation should be applied
 *
 * Convenient function to change the global map of @entity by
 * applying @tranformation using the @mode operator. This is
 * logically equivalent to the following:
 *
 * <informalexample><programlisting language="C">
 * cairo_matrix_t map;
 * adg_matrix_copy(&map, adg_entity_get_global_map(entity));
 * adg_matrix_transform(&map, transformation, mode);
 * adg_entity_set_global_map(entity, &map);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_entity_transform_global_map(AdgEntity *entity,
                                const cairo_matrix_t *transformation,
                                AdgTransformMode mode)
{
    AdgEntityPrivate *data;
    cairo_matrix_t map;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(transformation != NULL);

    data = entity->data;

    adg_matrix_copy(&map, &data->global_map);
    adg_matrix_transform(&map, transformation, mode);

    g_object_set(entity, "global-map", &map, NULL);
}

/**
 * adg_entity_get_global_map:
 * @entity: an #AdgEntity object
 *
 * Gets the transformation to be used to compute the global matrix
 * of @entity.
 *
 * Returns: the requested map or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const cairo_matrix_t *
adg_entity_get_global_map(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->global_map;
}

/**
 * adg_entity_get_global_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the current global matrix of @entity. The returned value
 * is owned by @entity and should not be changed or freed.
 *
 * The global matrix is computed in the arrange() phase by
 * combining all the global maps of the @entity hierarchy using
 * the %ADG_MIX_ANCESTORS method.
 *
 * Returns: the global matrix or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const cairo_matrix_t *
adg_entity_get_global_matrix(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->global.matrix;
}

/**
 * adg_entity_set_local_map:
 * @entity: an #AdgEntity object
 * @map: the new map
 *
 * Sets the new local transformation of @entity to @map:
 * the old map is discarded. If @map is <constant>NULL</constant>,
 * the local map is left unchanged.
 *
 * Since: 1.0
 **/
void
adg_entity_set_local_map(AdgEntity *entity, const cairo_matrix_t *map)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_object_set(entity, "local-map", map, NULL);
}

/**
 * adg_entity_transform_local_map:
 * @entity: an #AdgEntity object
 * @transformation: the transformation to apply
 * @mode: how @transformation should be applied
 *
 * Convenient function to change the local map of @entity by
 * applying @tranformation using the @mode operator. This is
 * logically equivalent to the following:
 *
 * <informalexample><programlisting language="C">
 * cairo_matrix_t map;
 * adg_matrix_copy(&map, adg_entity_get_local_map(entity));
 * adg_matrix_transform(&map, transformation, mode);
 * adg_entity_set_local_map(entity, &map);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_entity_transform_local_map(AdgEntity *entity,
                               const cairo_matrix_t *transformation,
                               AdgTransformMode mode)
{
    AdgEntityPrivate *data;
    cairo_matrix_t map;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(transformation != NULL);

    data = entity->data;

    adg_matrix_copy(&map, &data->local_map);
    adg_matrix_transform(&map, transformation, mode);
    g_object_set(entity, "local-map", &map, NULL);
}

/**
 * adg_entity_get_local_map:
 * @entity: an #AdgEntity object
 *
 * Gets the transformation to be used to compute the local matrix
 * of @entity and store it in @map.
 *
 * Returns: the requested map or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const cairo_matrix_t *
adg_entity_get_local_map(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->local_map;
}

/**
 * adg_entity_get_local_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the current local matrix of @entity. The returned value
 * is owned by @entity and should not be changed or freed.
 *
 * The local matrix is computed in the arrange() phase by
 * combining all the local maps of the @entity hierarchy using
 * the method specified by the #AdgEntity:local-mix property.
 *
 * Returns: the local matrix or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const cairo_matrix_t *
adg_entity_get_local_matrix(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->local.matrix;
}

/**
 * adg_entity_set_local_mix:
 * @entity: an #AdgEntity object
 * @local_mix: new mix method
 *
 * Sets a new local mix method on @entity. The
 * #AdgEntity:local-mix property defines how the local
 * matrix must be computed: check out the #AdgMix
 * documentation to know what are the availables methods
 * and how they affect the local matrix computation.
 *
 * Setting a different local mix method emits an
 * #AdgEntity::local-changed signal on @entity.
 *
 * Since: 1.0
 **/
void
adg_entity_set_local_mix(AdgEntity *entity, AdgMix local_mix)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_object_set(entity, "local-mix", local_mix, NULL);
}

/**
 * adg_entity_get_local_mix:
 * @entity: an #AdgEntity object
 *
 * Gets the local mix method of @entity. Check out the
 * adg_entity_set_local_mix() documentation to know what the
 * local mix method is used for.
 *
 * Returns: the local mix method of @entity or %ADG_MIX_UNDEFINED on errors
 *
 * Since: 1.0
 **/
AdgMix
adg_entity_get_local_mix(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), ADG_MIX_UNDEFINED);

    data = entity->data;

    return data->local_mix;
}

/**
 * adg_entity_set_extents:
 * @entity: an #AdgEntity
 * @extents: the new extents
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Sets a new bounding box for @entity. @extents can
 * be <constant>NULL</constant>, in which case the extents are unset.
 *
 * Since: 1.0
 **/
void
adg_entity_set_extents(AdgEntity *entity, const CpmlExtents *extents)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    data = entity->data;

    if (extents == NULL)
        data->extents.is_defined = FALSE;
    else
        cpml_extents_copy(&data->extents, extents);
}

/**
 * adg_entity_get_extents:
 * @entity: an #AdgEntity
 *
 * Gets the bounding box of @entity. The returned struct is
 * owned by @entity and should not modified or freed.
 *
 * This struct specifies the surface portion (in global space
 * of @entity) occupied by the entity without taking into
 * account rendering properties such as line thickness or caps.
 *
 * The #AdgEntity::arrange signal should be emitted before
 * this call (either explicitely trought adg_entity_arrange()
 * or implicitely with adg_entity_render()) in order to get
 * an up to date boundary box.
 *
 * Returns: the bounding box of @entity or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_entity_get_extents(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->extents;
}

/**
 * adg_entity_set_style:
 * @entity: an #AdgEntity
 * @dress: a dress style
 * @style: the new style to use
 *
 * Overrides the style of @dress for @entity and its children.
 * If @style is <constant>NULL</constant>, any previous
 * override is removed.
 *
 * The new style must still be compatible with @dress: check out
 * the adg_dress_style_is_compatible() documentation to know
 * what a compatible style means.
 *
 * Since: 1.0
 **/
void
adg_entity_set_style(AdgEntity *entity, AdgDress dress, AdgStyle *style)
{
    AdgEntityPrivate *data;
    gpointer p_dress;
    AdgStyle *old_style;

    g_return_if_fail(ADG_IS_ENTITY(entity));

    data = entity->data;

    if (data->hash_styles == NULL && style == NULL)
        return;

    if (data->hash_styles == NULL)
        data->hash_styles = g_hash_table_new_full(NULL, NULL,
                                                  NULL, g_object_unref);

    p_dress = GINT_TO_POINTER(dress);
    old_style = g_hash_table_lookup(data->hash_styles, p_dress);

    if (style == old_style)
        return;

    if (style == NULL) {
        g_hash_table_remove(data->hash_styles, p_dress);
        return;
    }

    if (!adg_dress_style_is_compatible(dress, style)) {
        GType ancestor_type = adg_dress_get_ancestor_type(dress);

        g_warning(_("%s: '%s' is not compatible with '%s' for '%s' dress (%d)"),
                  G_STRLOC, g_type_name(G_TYPE_FROM_INSTANCE(style)),
                  g_type_name(ancestor_type), adg_dress_get_name(dress), dress);

        return;
    }

    g_object_ref(style);
    g_hash_table_replace(data->hash_styles, p_dress, style);
}

/**
 * adg_entity_get_style:
 * @entity: an #AdgEntity
 * @dress: the dress of the style to get
 *
 * Gets the overriden @dress style from @entity. This is a kind
 * of accessor function: for rendering purpose use adg_entity_style()
 * instead. The returned object is owned by @entity and should not be
 * freed or modified.
 *
 * Returns: (transfer none): the requested style or <constant>NULL</constant> if the @dress style is not overriden.
 *
 * Since: 1.0
 **/
AdgStyle *
adg_entity_get_style(AdgEntity *entity, AdgDress dress)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    if (data->hash_styles == NULL)
        return NULL;

    return g_hash_table_lookup(data->hash_styles, GINT_TO_POINTER(dress));
}

/**
 * adg_entity_style:
 * @entity: an #AdgEntity
 * @dress: the dress of the style to get
 *
 * Gets the style to be used for @entity. @dress specifies which
 * "family" of style to get.
 *
 * The following sequence of checks is performed to get the proper
 * style, stopping at the first succesfull result:
 *
 * <orderedlist>
 * <listitem>check if the style is directly overriden by this entity,
 *           as returned by adg_entity_get_style();</listitem>
 * <listitem>check if @entity has a parent, in which case returns the
 *           adg_entity_style() of the parent;</listitem>
 * <listitem>returns the main style with adg_dress_get_fallback().</listitem>
 * </orderedlist>
 *
 * The returned object is owned by @entity and should not be
 * freed or modified.
 *
 * Returns: (transfer none): the requested style or <constant>NULL</constant> for transparent dresses or errors.
 *
 * Since: 1.0
 **/
AdgStyle *
adg_entity_style(AdgEntity *entity, AdgDress dress)
{
    AdgStyle *style;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    style = adg_entity_get_style(entity, dress);

    if (style == NULL) {
        AdgEntityPrivate *data = entity->data;

        if (data->parent != NULL)
            style = adg_entity_style(data->parent, dress);
        else
            style = adg_dress_get_fallback(dress);
    }

    return style;
}

/**
 * adg_entity_apply_dress:
 * @entity: an #AdgEntity
 * @dress: the dress style to apply
 * @cr: a #cairo_t drawing context
 *
 * Convenient function to apply a @dress style (as returned by
 * adg_entity_style()) to the @cr cairo context.
 *
 * Since: 1.0
 **/
void
adg_entity_apply_dress(AdgEntity *entity, AdgDress dress, cairo_t *cr)
{
    AdgStyle *style;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(cr != NULL);

    style = adg_entity_style(entity, dress);

    if (style != NULL)
        adg_style_apply(style, entity, cr);
}

/**
 * adg_entity_global_changed:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::global-changed signal on @entity and on all of
 * its children, if any.
 *
 * Since: 1.0
 **/
void
adg_entity_global_changed(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[GLOBAL_CHANGED], 0);
}

/**
 * adg_entity_local_changed:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::local-changed signal on @entity and on all of
 * its children, if any.
 *
 * Since: 1.0
 **/
void
adg_entity_local_changed(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[LOCAL_CHANGED], 0);
}

/**
 * adg_entity_invalidate:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::invalidate signal on @entity and on all of
 * its children, if any, clearing the eventual cache stored by the
 * #AdgEntity::arrange signal and setting the entity state similary
 * to the just initialized entity.
 *
 * Since: 1.0
 **/
void
adg_entity_invalidate(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[INVALIDATE], 0);
}

/**
 * adg_entity_arrange:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::arrange signal on @entity and all its children,
 * if any. The arrange call is implicitely called by the
 * #AdgEntity::render signal but not by adg_entity_get_extents().
 *
 * Since: 1.0
 **/
void
adg_entity_arrange(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[ARRANGE], 0);
}

/**
 * adg_entity_render:
 * @entity: an #AdgEntity
 * @cr: a #cairo_t drawing context
 *
 * Emits the #AdgEntity::render signal on @entity and on all of its
 * children, if any, causing the rendering to the @cr cairo context.
 *
 * Since: 1.0
 **/
void
adg_entity_render(AdgEntity *entity, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, _adg_signals[RENDER], 0, cr);
}

/**
 * adg_entity_point:
 * @entity: an #AdgEntity
 * @point: the #AdgPoint to define
 * @new_point: the new #AdgPoint value
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * A convenient method to set an #AdgPoint owned by @entity.
 * @old_point is the old value while @new_point is the new value.
 * It can be used for changing a private #AdgPoint struct, such as:
 *
 * <informalexample><programlisting language="C">
 * data->point = adg_entity_point(entity, data->point, new_point);
 * </programlisting></informalexample>
 *
 * This function takes care of the dependencies between @entity and
 * the eventual models bound to the old and new points.
 *
 * @old_point can be <constant>NULL</constant>, in which case a
 * clone of @new_point will be returned. Also @new_point can
 * be <constant>NULL</constant>, in which case @old_point
 * is destroyed and <constant>NULL</constant> will be returned.
 *
 * Returns: (transfer full): the new properly defined point
 *
 * Since: 1.0
 **/
AdgPoint *
adg_entity_point(AdgEntity *entity, AdgPoint *old_point, const AdgPoint *new_point)
{
    AdgPoint *point;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    point = NULL;

    if (! adg_point_equal(old_point, new_point)) {
        AdgModel *old_model, *new_model;

        old_model = old_point != NULL ? adg_point_get_model(old_point) : NULL;
        new_model = new_point != NULL ? adg_point_get_model(new_point) : NULL;

        if (new_model != old_model) {
            /* Handle model-entity dependencies */
            if (new_model != NULL)
                adg_model_add_dependency(new_model, entity);
            if (old_model != NULL)
                adg_model_remove_dependency(old_model, entity);
        }

        if (new_point != NULL)
            point = adg_point_dup(new_point);
        if (old_point != NULL)
            adg_point_destroy(old_point);
    }

    return point;
}


static void
_adg_destroy(AdgEntity *entity)
{
    GObject *object = (GObject *) entity;

    g_object_run_dispose(object);
    g_object_unref(object);
}

static void
_adg_set_parent(AdgEntity *entity, AdgEntity *parent)
{
    AdgEntityPrivate *data;
    AdgEntity *old_parent;

    data = entity->data;
    old_parent = data->parent;

    data->parent = parent;
    data->global.is_defined = FALSE;
    data->local.is_defined = FALSE;

    g_signal_emit(entity, _adg_signals[PARENT_SET], 0, old_parent);
}

static void
_adg_global_changed(AdgEntity *entity)
{
    AdgEntityPrivate *data;
    const cairo_matrix_t *map;
    cairo_matrix_t *matrix;

    data = entity->data;
    map = &data->global_map;
    matrix = &data->global.matrix;

    if (data->parent) {
        adg_matrix_copy(matrix, adg_entity_get_global_matrix(data->parent));
        adg_matrix_transform(matrix, map, ADG_TRANSFORM_BEFORE);
    } else {
        adg_matrix_copy(matrix, map);
    }
}

static void
_adg_local_changed(AdgEntity *entity)
{
    AdgEntityPrivate *data;
    const cairo_matrix_t *map;
    cairo_matrix_t *matrix;

    data = entity->data;
    map = &data->local_map;
    matrix = &data->local.matrix;

    switch (data->local_mix) {
    case ADG_MIX_DISABLED:
        adg_matrix_copy(matrix, adg_matrix_identity());
        break;
    case ADG_MIX_NONE:
        adg_matrix_copy(matrix, map);
        break;
    case ADG_MIX_ANCESTORS:
        if (data->parent) {
            adg_matrix_copy(matrix, adg_entity_get_local_matrix(data->parent));
            adg_matrix_transform(matrix, map, ADG_TRANSFORM_BEFORE);
        } else {
            adg_matrix_copy(matrix, map);
        }
        break;
    case ADG_MIX_ANCESTORS_NORMALIZED:
        if (data->parent) {
            adg_matrix_copy(matrix, adg_entity_get_local_matrix(data->parent));
            adg_matrix_transform(matrix, map, ADG_TRANSFORM_BEFORE);
        } else {
            adg_matrix_copy(matrix, map);
        }
        adg_matrix_normalize(matrix);
        break;
    case ADG_MIX_PARENT:
        if (data->parent) {
            adg_matrix_copy(matrix, adg_entity_get_local_map(data->parent));
            adg_matrix_transform(matrix, map, ADG_TRANSFORM_BEFORE);
        } else {
            adg_matrix_copy(matrix, map);
        }
        break;
    case ADG_MIX_PARENT_NORMALIZED:
        if (data->parent) {
            adg_matrix_copy(matrix, adg_entity_get_local_map(data->parent));
            adg_matrix_transform(matrix, map, ADG_TRANSFORM_BEFORE);
        } else {
            adg_matrix_copy(matrix, map);
        }
        adg_matrix_normalize(matrix);
        break;
    case ADG_MIX_UNDEFINED:
        g_warning(_("%s: requested to mix the maps using an undefined mix method"),
                  G_STRLOC);
        break;
    default:
        g_return_if_reached();
        break;
    }
}

static void
_adg_real_invalidate(AdgEntity *entity)
{
    AdgEntityClass *klass = ADG_ENTITY_GET_CLASS(entity);
    AdgEntityPrivate *data = entity->data;

    /* Do not raise any warning if invalidate() is not defined,
     * assuming entity does not have additional cache to be cleared */
    if (klass->invalidate)
        klass->invalidate(entity);

    data->extents.is_defined = FALSE;
}

static void
_adg_real_arrange(AdgEntity *entity)
{
    AdgEntityClass *klass;
    AdgEntityPrivate *data;

    klass = ADG_ENTITY_GET_CLASS(entity);
    data = entity->data;

    /* Update the global matrix, if required */
    if (!data->global.is_defined) {
        data->global.is_defined = TRUE;
        g_signal_emit(entity, _adg_signals[GLOBAL_CHANGED], 0);
    }

    /* Update the local matrix, if required */
    if (!data->local.is_defined) {
        data->local.is_defined = TRUE;
        g_signal_emit(entity, _adg_signals[LOCAL_CHANGED], 0);
    }

    /* The arrange() method must be defined */
    if (klass->arrange == NULL) {
        g_warning(_("%s: 'arrange' method not implemented for type '%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(entity)));
        data->extents.is_defined = FALSE;
        return;
    }

    klass->arrange(entity);
}

static void
_adg_real_render(AdgEntity *entity, cairo_t *cr)
{
    AdgEntityClass *klass = ADG_ENTITY_GET_CLASS(entity);

    /* The render method must be defined */
    if (klass->render == NULL) {
        g_warning(_("%s: 'render' method not implemented for type '%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(entity)));
        return;
    }

    /* Before the rendering, the entity should be arranged */
    g_signal_emit(entity, _adg_signals[ARRANGE], 0);

    cairo_save(cr);
    klass->render(entity, cr);
    cairo_restore(cr);

    if (_adg_show_extents) {
        AdgEntityPrivate *data = entity->data;
        CpmlExtents *extents = &data->extents;

        if (extents->is_defined) {
            cairo_save(cr);
            cairo_set_source_rgba(cr, 0.15, 0.15, 0.15, 0.15);
            cairo_rectangle(cr, extents->org.x, extents->org.y,
                            extents->size.x, extents->size.y);
            cairo_fill(cr);
            cairo_restore(cr);
        }
    }
}
