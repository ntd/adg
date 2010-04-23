/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-model
 * @short_description: The base class of the ADG model infrastructure
 *
 * A model is a conceptual representation of something. From an ADG
 * user point of view, it is a collection of data and rules that
 * defines how an object should be represented on a drawing.
 *
 * Because #AdgModel instances are only a conceptual idea, they are
 * not renderable (that is, #AdgModel is not derived from #AdgEntity).
 * Instead, it must be passed as subject to entities such as #AdgStroke
 * or #AdgHatch.
 **/

/**
 * AdgModel:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

/**
 * AdgDependencyFunc:
 * @model: the #AdgModel
 * @entity: the #AdgEntity dependent on @model
 * @user_data: a general purpose pointer
 *
 * Callback used by adg_model_foreach_dependency().
 **/

/**
 * AdgNamedPairFunc:
 * @model: the #AdgModel
 * @name: the name of the named pair
 * @pair: an #AdgPair
 * @user_data: a general purpose pointer
 *
 * Callback used by adg_model_foreach_named_pair().
 **/


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-model-private.h"
#include "adg-entity.h"
#include "adg-marshal.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_model_parent_class)


enum {
    PROP_0,
    PROP_DEPENDENCY
};

enum {
    ADD_DEPENDENCY,
    REMOVE_DEPENDENCY,
    SET_NAMED_PAIR,
    CLEAR,
    CHANGED,
    LAST_SIGNAL
};


static void     dispose                 (GObject        *object);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     add_dependency          (AdgModel       *model,
                                         AdgEntity      *entity);
static void     remove_dependency       (AdgModel       *model,
                                         AdgEntity      *entity);
static const AdgPair *
                named_pair              (AdgModel       *model,
                                         const gchar    *name);
static void     _adg_clear              (AdgModel       *model);
static void     set_named_pair          (AdgModel       *model,
                                         const gchar    *name,
                                         const AdgPair  *pair);
static void     changed                 (AdgModel       *model);
static void     _adg_named_pair_wrapper (gpointer        key,
                                         gpointer        value,
                                         gpointer        user_data);
static void     _adg_invalidate_wrapper (AdgModel       *model,
                                         AdgEntity      *entity,
                                         gpointer        user_data);

static guint    _adg_signals[LAST_SIGNAL] = { 0 };


G_DEFINE_ABSTRACT_TYPE(AdgModel, adg_model, G_TYPE_OBJECT);


static void
adg_model_class_init(AdgModelClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgModelPrivate));

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;

    klass->add_dependency = add_dependency;
    klass->remove_dependency = remove_dependency;
    klass->named_pair = named_pair;
    klass->set_named_pair = set_named_pair;
    klass->clear = NULL;
    klass->changed = changed;

    param = g_param_spec_object("dependency",
                                P_("Dependency"),
                                P_("Can be used to add a new dependency from this model (this entity will be invalidated on model changed)"),
                                ADG_TYPE_ENTITY,
                                G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_DEPENDENCY, param);

    /**
     * AdgModel::add-dependency:
     * @model: an #AdgModel
     * @entity: an #AdgEntity that depends on @model
     *
     * Adds @entity to @model. After that @entity will depend on @model,
     * that is #AdgModel::changed on @model will invalidate @entity.
     **/
    _adg_signals[ADD_DEPENDENCY] = g_signal_new("add-dependency",
                                                G_OBJECT_CLASS_TYPE(gobject_class),
                                                G_SIGNAL_RUN_FIRST,
                                                G_STRUCT_OFFSET(AdgModelClass, add_dependency),
                                                NULL, NULL,
                                                adg_marshal_VOID__OBJECT,
                                                G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgModel::remove-dependency:
     * @model: an #AdgModel
     * @entity: the #AdgEntity that does not depend on @model anymore
     *
     * Removes the @entity from @model, that is @entity will not depend
     * on @model anymore.
     **/
    _adg_signals[REMOVE_DEPENDENCY] = g_signal_new("remove-dependency",
                                                   G_OBJECT_CLASS_TYPE(gobject_class),
                                                   G_SIGNAL_RUN_FIRST,
                                                   G_STRUCT_OFFSET(AdgModelClass, remove_dependency),
                                                   NULL, NULL,
                                                   adg_marshal_VOID__OBJECT,
                                                   G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgModel::set-named-pair:
     * @model: an #AdgModel
     * @name: an arbitrary name
     * @pair: an #AdgPair
     *
     * Adds, updates or deletes a named pair, accordling to the given
     * parameters.
     *
     * If @pair is %NULL, the @name named pair is searched and deleted.
     * If it is not found, a warning is raised.
     *
     * Otherwise, the @name named pair is searched: if it is found,
     * its data are updated with @pair. If it is not found, a new
     * named pair is created using @name and @pair.
     **/
    _adg_signals[SET_NAMED_PAIR] = g_signal_new("set-named-pair",
                                                G_OBJECT_CLASS_TYPE(gobject_class),
                                                G_SIGNAL_RUN_FIRST,
                                                G_STRUCT_OFFSET(AdgModelClass, set_named_pair),
                                                NULL, NULL,
                                                adg_marshal_VOID__STRING_POINTER,
                                                G_TYPE_NONE, 2,
                                                G_TYPE_STRING, G_TYPE_POINTER);

    /**
     * AdgModel::clear:
     * @model: an #AdgModel
     *
     * Removes any cached information from @model.
     **/
    _adg_signals[CLEAR] = g_signal_new("clear", ADG_TYPE_MODEL,
                                       G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                                       G_STRUCT_OFFSET(AdgModelClass, clear),
                                       NULL, NULL,
                                       adg_marshal_VOID__VOID,
                                       G_TYPE_NONE, 0);

    /**
     * AdgModel::changed:
     * @model: an #AdgModel
     *
     * Notificates that the model has changed. By default, all the
     * dependent entities are invalidated.
     **/
    _adg_signals[CHANGED] = g_signal_new("changed", ADG_TYPE_MODEL,
                                         G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                                         G_STRUCT_OFFSET(AdgModelClass, changed),
                                         NULL, NULL,
                                         adg_marshal_VOID__VOID,
                                         G_TYPE_NONE, 0);
}

static void
adg_model_init(AdgModel *model)
{
    AdgModelPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(model, ADG_TYPE_MODEL,
                                                        AdgModelPrivate);

    data->dependencies = NULL;

    model->data = data;
}

static void
dispose(GObject *object)
{
    AdgModel *model;
    AdgModelPrivate *data;
    AdgEntity *entity;

    model = (AdgModel *) object;
    data = model->data;

    /* Remove all the dependencies from the model: this will emit
     * a "remove" signal for every dependency and will drop all the
     * references from those entities to this model */
    while (data->dependencies != NULL) {
        entity = (AdgEntity *) data->dependencies->data;
        adg_model_remove_dependency(model, entity);
    }

    if (data->named_pairs)
        _adg_clear(model);

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgModel *model = (AdgModel *) object;

    switch (prop_id) {
    case PROP_DEPENDENCY:
        adg_model_add_dependency(model, g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}


/**
 * adg_model_add_dependency:
 * @model: an #AdgModel
 * @entity: an #AdgEntity
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Emits a #AdgModel::add-dependency signal on @model passing @entity
 * as argument. This will add a reference to @entity owned by @model.
 **/
void
adg_model_add_dependency(AdgModel *model, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(model, _adg_signals[ADD_DEPENDENCY], 0, entity);
}

/**
 * adg_model_remove_dependency:
 * @model: an #AdgModel
 * @entity: an #AdgEntity
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Emits a #AdgModel::remove-dependency signal on @model passing
 * @entity as argument. @entity must be inside @model.
 *
 * Note that @model will own a reference to @entity and it
 * may be the last reference held: this means removing an entity
 * from the model can destroy it.
 **/
void
adg_model_remove_dependency(AdgModel *model, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(model, _adg_signals[REMOVE_DEPENDENCY], 0, entity);
}

/**
 * adg_model_get_dependencies:
 * @model: an #AdgModel
 *
 * Gets the list of entities dependending on @model. This list
 * is owned by @model and must not be modified or freed.
 *
 * Returns: a #GSList of dependencies or %NULL on error
 **/
const GSList *
adg_model_get_dependencies(AdgModel *model)
{
    AdgModelPrivate *data;

    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);

    data = model->data;

    return data->dependencies;
}

/**
 * adg_model_foreach_dependency:
 * @model: an #AdgModel
 * @callback: the entity callback
 * @user_data: general purpose user data passed "as is" to @callback
 *
 * Invokes @callback on each entity linked to @model.
 **/
void
adg_model_foreach_dependency(AdgModel *model, AdgDependencyFunc callback,
                             gpointer user_data)
{
    AdgModelPrivate *data;
    GSList *dependency;
    AdgEntity *entity;

    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(callback != NULL);

    data = model->data;
    dependency = data->dependencies;

    while (dependency) {
        entity = dependency->data;

        if (entity != NULL && ADG_IS_ENTITY(entity))
            callback(model, entity, user_data);

        dependency = dependency->next;
    }
}

/**
 * adg_model_set_named_pair:
 * @model: an #AdgModel
 * @name: the name to associate to the pair
 * @pair: the #AdgPair
 *
 * <note><para>
 * This function is only useful in model definitions, such as
 * inside an #AdgTrailCallback function or while constructing
 * an #AdgPath instance.
 * </para></note>
 *
 * Emits a #AdgModel::set-named-pair signal on @model passing
 * @name and @pair as arguments.
 **/
void
adg_model_set_named_pair(AdgModel *model, const gchar *name,
                         const AdgPair *pair)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(name != NULL);

    g_signal_emit(model, _adg_signals[SET_NAMED_PAIR], 0, name, pair);
}

/**
 * adg_model_set_named_pair_explicit:
 * @model: an #AdgModel
 * @name: the name to associate to the pair
 * @x: the x coordinate of the point
 * @y: the y coordinate of the point
 *
 * <note><para>
 * This function is only useful in model definitions, such as
 * inside an #AdgTrailCallback function or while constructing
 * an #AdgPath instance.
 * </para></note>
 *
 * Convenient wrapper on adg_model_set_named_pair() that accepts
 * explicit coordinates.
 **/
void
adg_model_set_named_pair_explicit(AdgModel *model, const gchar *name,
                                  gdouble x, gdouble y)
{
    AdgPair pair;

    pair.x = x;
    pair.y = y;

    adg_model_set_named_pair(model, name, &pair);
}

/**
 * adg_model_get_named_pair:
 * @model: an #AdgModel
 * @name: the name of the pair to get
 *
 * Gets the @name named pair associated to @model. The returned
 * pair is owned by @model and must not be modified or freed.
 *
 * Returns: the requested #AdgPair or %NULL if not found
 **/
const AdgPair *
adg_model_get_named_pair(AdgModel *model, const gchar *name)
{
    AdgModelClass *klass;

    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);
    g_return_val_if_fail(name != NULL, NULL);

    klass = ADG_MODEL_GET_CLASS(model);

    if (klass->named_pair == NULL)
        return NULL;

    return klass->named_pair(model, name);
}

/**
 * adg_model_foreach_named_pair:
 * @model: an #AdgModel
 * @callback: the named pair callback
 * @user_data: general purpose user data passed "as is" to @callback
 *
 * Invokes @callback for each named pair set on @model. This can
 * be used, for example, to retrieve all the named pairs of a @model
 * or to duplicate a transformed version of every named pair.
 **/
void
adg_model_foreach_named_pair(AdgModel *model, AdgNamedPairFunc callback,
                             gpointer user_data)
{
    AdgModelPrivate *data;
    AdgWrapperHelper helper;

    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(callback != NULL);

    data = model->data;

    if (data->named_pairs == NULL)
        return;

    helper.callback = callback;
    helper.model = model;
    helper.user_data = user_data;

    g_hash_table_foreach(data->named_pairs, _adg_named_pair_wrapper, &helper);
}

/**
 * adg_model_clear:
 * @model: an #AdgModel
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Emits the #AdgModel::clear signal on @model.
 **/
void
adg_model_clear(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, _adg_signals[CLEAR], 0);
}

/**
 * adg_model_changed:
 * @model: an #AdgModel
 *
 * <note><para>
 * This function is only useful in entity implementations.
 * </para></note>
 *
 * Emits the #AdgModel::changed signal on @model.
 **/
void
adg_model_changed(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, _adg_signals[CHANGED], 0);
}


static void
add_dependency(AdgModel *model, AdgEntity *entity)
{
    AdgModelPrivate *data = model->data;

    /* The prepend operation is more efficient */
    data->dependencies = g_slist_prepend(data->dependencies, entity);

    g_object_ref(entity);
}

static void
remove_dependency(AdgModel *model, AdgEntity *entity)
{
    AdgModelPrivate *data;
    GSList *node;

    data = model->data;
    node = g_slist_find(data->dependencies, entity);

    if (node == NULL) {
        g_warning("%s: attempting to remove an entity with type %s "
                  "from a model of type %s, but the entity is not present.",
                  G_STRLOC,
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(model)));
        return;
    }

    data->dependencies = g_slist_delete_link(data->dependencies, node);
    g_object_unref(entity);
}

static void
_adg_clear(AdgModel *model)
{
    AdgModelPrivate *data = model->data;

    g_hash_table_destroy(data->named_pairs);
    data->named_pairs = NULL;
}

static void
set_named_pair(AdgModel *model, const gchar *name, const AdgPair *pair)
{
    AdgModelPrivate *data;
    GHashTable **hash;
    gchar *key;
    AdgPair *value;

    data = model->data;
    hash = &data->named_pairs;

    if (pair == NULL) {
        /* Delete mode: raise a warning if @name is not found */
        if (*hash == NULL || !g_hash_table_remove(*hash, name))
            g_warning("%s: attempting to remove inexistent `%s' named pair",
                      G_STRLOC, name);

        return;
    }

    /* Insert/update mode */
    key = g_strdup(name);
    value = adg_pair_dup(pair);

    if (*hash == NULL)
        *hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    g_hash_table_insert(*hash, key, value);
}

static const AdgPair *
named_pair(AdgModel *model, const gchar *name)
{
    AdgModelPrivate *data = model->data;

    if (data->named_pairs == NULL)
        return NULL;

    return g_hash_table_lookup(data->named_pairs, name);
}

static void
changed(AdgModel *model)
{
    /* Invalidate all the entities dependent on this model */
    adg_model_foreach_dependency(model, _adg_invalidate_wrapper, NULL);
}

static void
_adg_named_pair_wrapper(gpointer key, gpointer value, gpointer user_data)
{
    const gchar *name;
    AdgPair *pair;
    AdgWrapperHelper *helper;

    name = key;
    pair = value;
    helper = user_data;

    helper->callback(helper->model, name, pair, helper->user_data);
}

static void
_adg_invalidate_wrapper(AdgModel *model, AdgEntity *entity, gpointer user_data)
{
    adg_entity_invalidate(entity);
}
