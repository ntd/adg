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
 * implement its arrange() and render() virtual methods. Also, if
 * you are using some sort of caching, ensure to clear it in the
 * invalidate() method.
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
 * @invalidate: invalidating callback, used to clear the cache
 * @arrange: prepare the layout and fill the extents struct
 * @render: rendering callback, it must be implemented
 *
 * Any entity (if not abstract) must implement at least the @render method.
 **/


#include "adg-entity.h"
#include "adg-entity-private.h"
#include "adg-canvas.h"
#include "adg-font-style.h"
#include "adg-dim-style.h"
#include "adg-marshal.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_entity_parent_class)


enum {
    PROP_0,
    PROP_PARENT,
    PROP_GLOBAL_MAP,
    PROP_LOCAL_MAP,
    PROP_LOCAL_MODE
};

enum {
    PARENT_SET,
    GLOBAL_CHANGED,
    LOCAL_CHANGED,
    INVALIDATE,
    ARRANGE,
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
static void             global_changed          (AdgEntity       *entity);
static void             local_changed           (AdgEntity       *entity);
static gboolean         set_global_map          (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static gboolean         set_local_map           (AdgEntity       *entity,
                                                 const AdgMatrix *map);
static gboolean         set_local_mode          (AdgEntity       *entity,
                                                 AdgTransformationMode mode);
static void             real_invalidate         (AdgEntity       *entity);
static void             real_arrange            (AdgEntity       *entity);
static void             real_render             (AdgEntity       *entity,
                                                 cairo_t         *cr);

static guint    signals[LAST_SIGNAL] = { 0 };


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

    gobject_class->dispose = dispose;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    klass->parent_set = NULL;
    klass->global_changed = global_changed;
    klass->local_changed = local_changed;
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
                               ADG_TYPE_MATRIX,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GLOBAL_MAP, param);

    param = g_param_spec_boxed("local-map",
                               P_("Local Map"),
                               P_("The transformation to be combined with the parent ones to get the local matrix"),
                               ADG_TYPE_MATRIX,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOCAL_MAP, param);

    param = g_param_spec_enum("local-mode",
                              P_("Local Mode"),
                              P_("How the local matrix should be combined with the global matrix to get the current transformation matrix (ctm)"),
                              ADG_TYPE_TRANSFORMATION_MODE,
                              ADG_TRANSFORM_NONE,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOCAL_MODE, param);

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
                                       adg_marshal_VOID__OBJECT,
                                       G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgEntity::global-changed
     * @entity: an #AdgEntity
     *
     * Emitted when the global map of @entity or any of its parent
     * has changed. The default handler will compute the new global
     * matrix, updating the internal cache.
     **/
    signals[GLOBAL_CHANGED] = g_signal_new("global-changed",
                                           G_OBJECT_CLASS_TYPE(gobject_class),
                                           G_SIGNAL_RUN_FIRST,
                                           G_STRUCT_OFFSET(AdgEntityClass, global_changed),
                                           NULL, NULL,
                                           adg_marshal_VOID__VOID,
                                           G_TYPE_NONE, 0);

    /**
     * AdgEntity::local-changed
     * @entity: an #AdgEntity
     *
     * Emitted when the local map of @entity or any of its parent
     * has changed. The default handler will compute the new local
     * matrix, updating the internal cache.
     **/
    signals[LOCAL_CHANGED] = g_signal_new("local-changed",
                                          G_OBJECT_CLASS_TYPE(gobject_class),
                                          G_SIGNAL_RUN_FIRST,
                                          G_STRUCT_OFFSET(AdgEntityClass, local_changed),
                                          NULL, NULL,
                                          adg_marshal_VOID__VOID,
                                          G_TYPE_NONE, 0);

    /**
     * AdgEntity::invalidate:
     * @entity: an #AdgEntity
     *
     * Invalidates the whole @entity, that is resets all the cache
     * (if present) built during the #AdgEntity::arrange signal.
     * The resulting state is a clean entity, similar to what you
     * have just before the first rendering.
     **/
    closure = g_cclosure_new(G_CALLBACK(real_invalidate), NULL, NULL);
    signals[INVALIDATE] = g_signal_newv("invalidate", ADG_TYPE_ENTITY,
                                        G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                                        adg_marshal_VOID__VOID,
                                        G_TYPE_NONE, 0, param_types);

    /**
     * AdgEntity::arrange:
     * @entity: an #AdgEntity
     *
     * Arranges the layout of @entity, updating the cache if necessary,
     * and computes the extents of @entity.
     **/
    closure = g_cclosure_new(G_CALLBACK(real_arrange), NULL, NULL);
    signals[ARRANGE] = g_signal_newv("arrange", ADG_TYPE_ENTITY,
                                     G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                                     adg_marshal_VOID__VOID,
                                     G_TYPE_NONE, 0, param_types);
    /**
     * AdgEntity::render:
     * @entity: an #AdgEntity
     * @cr: a #cairo_t drawing context
     *
     * Causes the rendering of @entity on @cr. A render signal will
     * automatically emit #AdgEntity::arrange just before the real
     * rendering on the cairo context.
     **/
    closure = g_cclosure_new(G_CALLBACK(real_render), NULL, NULL);
    param_types[0] = G_TYPE_POINTER;
    signals[RENDER] = g_signal_newv("render", ADG_TYPE_ENTITY,
                                    G_SIGNAL_RUN_LAST, closure, NULL, NULL,
                                    adg_marshal_VOID__POINTER,
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
    data->local_mode = ADG_TRANSFORM_NONE;
    data->hash_styles = NULL;
    cairo_matrix_init_identity(&data->global_matrix);
    cairo_matrix_init_identity(&data->local_matrix);
    data->extents.is_defined = FALSE;

    entity->data = data;
}

static void
dispose(GObject *object)
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
    case PROP_GLOBAL_MAP:
        g_value_set_boxed(value, &data->global_map);
        break;
    case PROP_LOCAL_MAP:
        g_value_set_boxed(value, &data->local_map);
        break;
    case PROP_LOCAL_MODE:
        g_value_set_enum(value, data->local_mode);
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
    case PROP_GLOBAL_MAP:
        set_global_map(entity, g_value_get_boxed(value));
        break;
    case PROP_LOCAL_MAP:
        set_local_map(entity, g_value_get_boxed(value));
        break;
    case PROP_LOCAL_MODE:
        set_local_mode(entity, g_value_get_enum(value));
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
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Sets a new parent on @entity.
 **/
void
adg_entity_set_parent(AdgEntity *entity, AdgEntity *parent)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    if (set_parent(entity, parent))
        g_object_notify((GObject *) entity, "parent");
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

    if (set_global_map(entity, map))
        g_object_notify((GObject *) entity, "global-map");
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
 * |[
 * AdgMatrix tmp_map;
 * adg_entity_get_global_map(entity, &tmp_map);
 * adg_matrix_transform(&tmp_map, transformation, mode);
 * adg_entity_set_global_map(entity, &tmp_map);
 * ]|
 **/
void
adg_entity_transform_global_map(AdgEntity *entity,
                                const AdgMatrix *transformation,
                                AdgTransformationMode mode)
{
    AdgEntityPrivate *data;
    AdgMatrix map;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(transformation != NULL);

    data = entity->data;

    adg_matrix_copy(&map, &data->global_map);
    adg_matrix_transform(&map, transformation, mode);

    if (set_global_map(entity, &map))
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
 * Sets the new local transformation of @entity to @map:
 * the old map is discarded. If @map is %NULL an identity
 * matrix is implied.
 **/
void
adg_entity_set_local_map(AdgEntity *entity, const AdgMatrix *map)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    if (set_local_map(entity, map))
        g_object_notify((GObject *) entity, "local-map");
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
 * |[
 * AdgMatrix tmp_map;
 * adg_entity_get_local_map(entity, &tmp_map);
 * adg_matrix_transform(&tmp_map, transformation, mode);
 * adg_entity_set_local_map(entity, &tmp_map);
 * ]|
 **/
void
adg_entity_transform_local_map(AdgEntity *entity,
                               const AdgMatrix *transformation,
                               AdgTransformationMode mode)
{
    AdgEntityPrivate *data;
    AdgMatrix map;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(transformation != NULL);

    data = entity->data;

    adg_matrix_copy(&map, &data->local_map);
    adg_matrix_transform(&map, transformation, mode);

    if (set_local_map(entity, &map))
        g_object_notify((GObject *) entity, "local-map");
}

/**
 * adg_entity_get_local_mode:
 * @entity: an #AdgEntity object
 *
 * Gets the transformation mode to be used while applying the
 * local matrix over the global matrix. It is internally used
 * by adg_entity_get_ctm().
 *
 * Returns: the current transformation mode
 **/
AdgTransformationMode
adg_entity_get_local_mode(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), ADG_TRANSFORM_NONE);

    data = entity->data;

    return data->local_mode;
}

/**
 * adg_entity_set_local_mode:
 * @entity: an #AdgEntity object
 * @mode: the new mode
 *
 * Sets a new transformation mode to be used while combining
 * global and local matrices.
 * Sets a new transformation mode to be used while applying
 * the local matrix over the global matrix. It is internally
 * used by adg_entity_get_ctm().
 **/
void
adg_entity_set_local_mode(AdgEntity *entity, AdgTransformationMode mode)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    if (set_local_mode(entity, mode))
        g_object_notify((GObject *) entity, "local-mode");
}

/**
 * adg_entity_get_extents:
 * @entity: an #AdgEntity
 * @extents: where to store the extents
 *
 * Stores a copy of the bounding box of @entity in @extents.
 * This struct specifies the surface portion (in global space)
 * occupied by the entity without taking into account line
 * thickness and caps.
 *
 * The #AdgEntity::arrange should be emitted before this call
 * (either explicitely or throught adg_entity_arrange()) in
 * order to get an up to date boundary box.
 **/
void
adg_entity_get_extents(AdgEntity *entity, CpmlExtents *extents)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(extents != NULL);

    data = entity->data;

    cpml_extents_copy(extents, &data->extents);
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
 * Sets a new bounding box for @entity. @extents can be %NULL,
 * in which case the extents are unset.
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
 * adg_entity_style:
 * @entity: an #AdgEntity
 * @dress: the dress of the style to get
 *
 * Gets the style to be used for @entity. @dress specifies the "type"
 * of style to get.
 *
 * This method is supposed to always return a valid style instance,
 * specifically the most appropriate one for this entity. The following
 * sequence of checks is performed to get the proper @dress style,
 * stopping at the first succesfull result:
 *
 * <orderedlist>
 * <listitem>check if the style is defined directly by this entity type,
 *           as returned by adg_entity_get_style();</listitem>
 * <listitem>check if @entity has a parent, in which case returns the
 *           adg_entity_style() of the parent;</listitem>
 * <listitem>returns the main style with adg_dress_get_style().</listitem>
 * </orderedlist>
 *
 * Returns: the requested style or %NULL on errors
 **/
AdgStyle *
adg_entity_style(AdgEntity *entity, AdgDress dress)
{
    AdgStyle *style;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);
    g_return_val_if_fail(dress != ADG_DRESS_UNDEFINED, NULL);

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
 * adg_entity_get_style:
 * @entity: an #AdgEntity
 * @dress: the dress of the style to get
 *
 * Gets the overriden @dress style from @entity. This is a kind
 * of accessor function: to get the style to be used for rendering
 * purpose, consider adg_entity_style().
 *
 * Returns: the requested style or %NULL if the @dress style
 *          is not overriden
 **/
AdgStyle *
adg_entity_get_style(AdgEntity *entity, AdgDress dress)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);
    g_return_val_if_fail(dress != ADG_DRESS_UNDEFINED, NULL);

    data = entity->data;

    if (data->hash_styles == NULL)
        return NULL;

    return g_hash_table_lookup(data->hash_styles, GINT_TO_POINTER(dress));
}

/**
 * adg_entity_set_style:
 * @entity: an #AdgEntity
 * @dress: a dress style
 * @style: the new style to use
 *
 * Overrides the style of @dress for @entity and its children.
 * If @style is %NULL, any previous override is removed.
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
    } else {
        g_object_ref(style);
        g_hash_table_replace(data->hash_styles, p_dress, style);
    }
}

/**
 * adg_entity_apply_dress:
 * @entity: an #AdgEntity
 * @dress: the dress style to apply
 * @cr: a #cairo_t drawing context
 *
 * Convenient function to apply a @dress style (as returned by
 * adg_entity_style()) to the @cr cairo context.
 **/
void
adg_entity_apply_dress(AdgEntity *entity, AdgDress dress, cairo_t *cr)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(cr != NULL);

    adg_style_apply(adg_entity_style(entity, dress), cr);
}

/**
 * adg_entity_global_changed:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::global-changed signal on @entity and on all of
 * its children, if any.
 **/
void
adg_entity_global_changed(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[GLOBAL_CHANGED], 0);
}

/**
 * adg_entity_local_changed:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::local-changed signal on @entity and on all of
 * its children, if any.
 **/
void
adg_entity_local_changed(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[LOCAL_CHANGED], 0);
}

/**
 * adg_entity_global_matrix:
 * @entity: an #AdgEntity object
 *
 * Gets the global matrix by combining all the global maps of the
 * @entity hierarchy. The returned value is owned by @entity and
 * should not be changed or freed.
 *
 * Returns: the global matrix or %NULL on errors
 **/
const AdgMatrix *
adg_entity_global_matrix(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->global_matrix;
}

/**
 * adg_entity_local_matrix:
 * @entity: an #AdgEntity object
 * @matrix: where to store the local matrix
 *
 * Gets the local matrix by combining all the local maps of the
 * @entity hierarchy. The returned value is owned by @entity and
 * should not be changed or freed.
 *
 * Returns: the local matrix or %NULL on errors
 **/
const AdgMatrix *
adg_entity_local_matrix(AdgEntity *entity)
{
    AdgEntityPrivate *data;

    g_return_val_if_fail(ADG_IS_ENTITY(entity), NULL);

    data = entity->data;

    return &data->local_matrix;
}

/**
 * adg_entity_get_ctm:
 * @entity: an #AdgEntity object
 * @ctm: where to store the current transformation matrix
 *
 * Gets the current transformation matrix (ctm) of @entity
 * by combining global and local matrices in the way specified
 * by the #AdgEntity:local-mode property. The result is stored
 * in @ctm.
 *
 * This method is only useful after the #AdgEntity::arrange default
 * handler has been called, that is at the rendering stage or while
 * arranging the entity after the parent method has been chained up.
 **/
void
adg_entity_get_ctm(AdgEntity *entity, AdgMatrix *ctm)
{
    AdgEntityPrivate *data;

    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_return_if_fail(ctm != NULL);

    data = entity->data;

    adg_matrix_copy(ctm, &data->global_matrix);
    adg_matrix_transform(ctm, &data->local_matrix, data->local_mode);
}

/**
 * adg_entity_invalidate:
 * @entity: an #AdgEntity
 *
 * Emits the #AdgEntity::invalidate signal on @entity and on all of
 * its children, if any, clearing the eventual cache stored by the
 * #AdgEntity::arrange signal and setting the entity state similary
 * to the just initialized entity.
 **/
void
adg_entity_invalidate(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[INVALIDATE], 0);
}

/**
 * adg_entity_arrange:
 * @entity: an #AdgEntity
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Emits the #AdgEntity::arrange signal on @entity and all its children,
 * if any. This function is rarely needed as the arrange call is usually
 * managed by the #AdgEntity::render signal or indirectly by a call to
 * adg_entity_get_extents().
 **/
void
adg_entity_arrange(AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(entity, signals[ARRANGE], 0);
}

/**
 * adg_entity_render:
 * @entity: an #AdgEntity
 * @cr: a #cairo_t drawing context
 *
 * Emits the #AdgEntity::render signal on @entity and on all of its
 * children, if any, causing the rendering to the @cr cairo context.
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

static void
global_changed(AdgEntity *entity)
{
    AdgEntityPrivate *data;
    AdgMatrix *matrix;

    data = entity->data;
    matrix = &data->global_matrix;

    if (data->parent == NULL) {
        adg_matrix_copy(matrix, &data->global_map);
    } else {
        adg_matrix_copy(matrix, adg_entity_global_matrix(data->parent));
        cairo_matrix_multiply(matrix, &data->global_map, matrix);
    }
}

static void
local_changed(AdgEntity *entity)
{
    AdgEntityPrivate *data;
    AdgMatrix *matrix;

    data = entity->data;
    matrix = &data->local_matrix;

    if (data->parent == NULL) {
        adg_matrix_copy(matrix, &data->local_map);
    } else {
        adg_matrix_copy(matrix, adg_entity_local_matrix(data->parent));
        cairo_matrix_multiply(matrix, &data->local_map, matrix);
    }
}

static gboolean
set_global_map(AdgEntity *entity, const AdgMatrix *map)
{
    AdgEntityPrivate *data = entity->data;

    if (map != NULL && adg_matrix_equal(&data->global_map, map))
        return FALSE;

    if (map == NULL)
        cairo_matrix_init_identity(&data->global_map);
    else
        adg_matrix_copy(&data->global_map, map);

    g_signal_emit(entity, signals[GLOBAL_CHANGED], 0, &data->global_matrix);
    return TRUE;
}

static gboolean
set_local_map(AdgEntity *entity, const AdgMatrix *map)
{
    AdgEntityPrivate *data = entity->data;

    if (map != NULL && adg_matrix_equal(&data->local_map, map))
        return FALSE;

    if (map == NULL)
        cairo_matrix_init_identity(&data->local_map);
    else
        adg_matrix_copy(&data->local_map, map);

    g_signal_emit(entity, signals[LOCAL_CHANGED], 0, &data->local_matrix);
    return TRUE;
}

static gboolean
set_local_mode(AdgEntity *entity, AdgTransformationMode mode)
{
    AdgEntityPrivate *data = entity->data;

    if (mode == data->local_mode)
        return FALSE;

    data->local_mode = mode;

    return TRUE;
}

static void
real_invalidate(AdgEntity *entity)
{
    AdgEntityPrivate *data = entity->data;
    AdgEntityClass *klass = ADG_ENTITY_GET_CLASS(entity);

    if (klass->invalidate == NULL) {
        /* Do not raise any warning if invalidate() is not defined,
         * assuming entity does not have cache to be cleared */
    } else {
        klass->invalidate(entity);
    }

    data->extents.is_defined = FALSE;
}

static void
real_arrange(AdgEntity *entity)
{
    AdgEntityClass *klass = ADG_ENTITY_GET_CLASS(entity);

    if (klass->arrange == NULL) {
        /* The arrange() method must be defined */
        g_warning("%s: `arrange' method not implemented for type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(entity)));
    } else {
        klass->arrange(entity);
    }
}

static void
real_render(AdgEntity *entity, cairo_t *cr)
{
    AdgEntityPrivate *data = entity->data;
    AdgEntityClass *klass = ADG_ENTITY_GET_CLASS(entity);

    if (klass->render == NULL) {
        /* The render method must be defined */
        g_warning("%s: `render' method not implemented for type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(entity)));
    } else {
        /* Before the rendering, the entity should be arranged */
        g_signal_emit(entity, signals[ARRANGE], 0);

        cairo_save(cr);
        cairo_set_matrix(cr, &data->global_matrix);
        klass->render(entity, cr);
        cairo_restore(cr);
    }
}
