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
 *
 * The relationships between model and view are handled by dependencies:
 * whenever an #AdgModel changes (that is the #AdgModel::changed signal is
 * emitted), every dependency of the model (#AdgEntity instances) is
 * invalidated with adg_entity_invalidate().
 *
 * To help the interaction between model and view another concept is
 * introduced: named pairs. This provides a way to abstract real values (the
 * coordinates stored in #CpmlPair) by accessing them using a string. To easily
 * the access of named pairs from the view, use #AdgPoint instead of #CpmlPair.
 *
 * Since: 1.0
 **/

/**
 * AdgModel:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgModelClass:
 * @named_pair:        virtual method that returns the #CpmlPair bound to a
 *                     given name.
 * @set_named_pair:    signal for defining or undefining a new named pair.
 * @clear:             signal for removing the internal cache data, if any.
 * @reset:             signal used to redefine a model from scratch.
 * @add_dependency:    signal for adding a new dependency.
 * @remove_dependency: signal used to remove an old dependency.
 * @changed:           signal for emitting an #AdgModel::changed signal.
 *
 *
 * The default @named_pair implementation looks up the #CpmlPair in an internal
 * #GHashTable that uses the pair name as key and the #CpmlPair struct as value.
 *
 * The default @set_named_pair implementation can be used for either adding
 * (if the #CpmlPair is not <constant>NULL</constant>) or removing (if #CpmlPair
 * is <constant>NULL</constant>) an item from the named pairs hash table.
 *
 * The default handler for @clear signals does not do anything.
 *
 * The default @reset involves the clearing of the internal cache data
 * (done by emitting the #AdgModel::clear signal) and the destruction of the
 * internal named pair hash table.
 *
 * The default @add_dependency and @remove_dependency implementations add and
 * remove items from an internal #GSList of #AdgEntity.
 *
 * The default handler of the @changed signal calls adg_entity_invalidate()
 * on every dependency by using adg_model_foreach_dependency().
 *
 * Since: 1.0
 **/

/**
 * AdgDependencyFunc:
 * @model: the #AdgModel
 * @entity: the #AdgEntity dependent on @model
 * @user_data: a general purpose pointer
 *
 * Callback used by adg_model_foreach_dependency().
 *
 * Since: 1.0
 **/

/**
 * AdgNamedPairFunc:
 * @model: the #AdgModel
 * @name: the name of the named pair
 * @pair: an #CpmlPair
 * @user_data: a general purpose pointer
 *
 * Callback used by adg_model_foreach_named_pair().
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-model.h"
#include "adg-model-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_model_parent_class)


G_DEFINE_ABSTRACT_TYPE(AdgModel, adg_model, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_DEPENDENCY
};

enum {
    ADD_DEPENDENCY,
    REMOVE_DEPENDENCY,
    SET_NAMED_PAIR,
    CLEAR,
    RESET,
    CHANGED,
    LAST_SIGNAL
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_add_dependency     (AdgModel       *model,
                                                 AdgEntity      *entity);
static void             _adg_remove_dependency  (AdgModel       *model,
                                                 AdgEntity      *entity);
static const CpmlPair * _adg_named_pair         (AdgModel       *model,
                                                 const gchar    *name);
static void             _adg_reset              (AdgModel       *model);
static void             _adg_set_named_pair     (AdgModel       *model,
                                                 const gchar    *name,
                                                 const CpmlPair *pair);
static void             _adg_changed            (AdgModel       *model);
static void             _adg_named_pair_wrapper (gpointer        key,
                                                 gpointer        value,
                                                 gpointer        user_data);
static void             _adg_invalidate_wrapper (AdgModel       *model,
                                                 AdgEntity      *entity,
                                                 gpointer        user_data);
static guint            _adg_signals[LAST_SIGNAL] = { 0 };


static void
adg_model_class_init(AdgModelClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgModelPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->set_property = _adg_set_property;

    klass->add_dependency = _adg_add_dependency;
    klass->remove_dependency = _adg_remove_dependency;
    klass->named_pair = _adg_named_pair;
    klass->set_named_pair = _adg_set_named_pair;
    klass->clear = NULL;
    klass->reset = _adg_reset;
    klass->changed = _adg_changed;

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
     *
     * Since: 1.0
     **/
    _adg_signals[ADD_DEPENDENCY] =
        g_signal_new("add-dependency",
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
     *
     * Since: 1.0
     **/
    _adg_signals[REMOVE_DEPENDENCY] =
        g_signal_new("remove-dependency",
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
     * @pair: an #CpmlPair
     *
     * Adds, updates or deletes a named pair, accordling to the given
     * parameters.
     *
     * If @pair is <constant>NULL</constant>, the @name named pair is
     * searched and deleted. If it is not found, a warning is raised.
     *
     * Otherwise, the @name named pair is searched: if it is found,
     * its data are updated with @pair. If it is not found, a new
     * named pair is created using @name and @pair.
     *
     * Since: 1.0
     **/
    _adg_signals[SET_NAMED_PAIR] =
        g_signal_new("set-named-pair",
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
     * <note><para>
     * This signal is only useful in model implementations.
     * </para></note>
     *
     * Removes any information from @model cached by the implementation
     * code. Useful to force a recomputation of the cache when something
     * in the model has changed.
     *
     * Since: 1.0
     **/
    _adg_signals[CLEAR] =
        g_signal_new("clear", ADG_TYPE_MODEL,
                     G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                     G_STRUCT_OFFSET(AdgModelClass, clear),
                     NULL, NULL,
                     adg_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgModel::reset:
     * @model: an #AdgModel
     *
     * Resets the state of @model by destroying any named pair
     * associated to it. This step also involves the emission of the
     * #AdgModel::clear signal.
     *
     * This signal is intended to be used while redefining the model.
     * A typical usage would be in these terms:
     *
     * <informalexample><programlisting language="C">
     * adg_model_reset(model);
     * // Definition of model. This also requires the redefinition of
     * // the named pairs because the old ones have been destroyed.
     * ...
     * adg_model_changed(model);
     * </programlisting></informalexample>
     *
     * Since: 1.0
     **/
    _adg_signals[RESET] =
        g_signal_new("reset", ADG_TYPE_MODEL,
                     G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                     G_STRUCT_OFFSET(AdgModelClass, reset),
                     NULL, NULL,
                     adg_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    /**
     * AdgModel::changed:
     * @model: an #AdgModel
     *
     * Notificates that the model has changed. By default, all the
     * dependent entities are invalidated.
     *
     * Since: 1.0
     **/
    _adg_signals[CHANGED] =
        g_signal_new("changed", ADG_TYPE_MODEL,
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
_adg_dispose(GObject *object)
{
    static gboolean is_disposed = FALSE;

    if (G_UNLIKELY(!is_disposed)) {
        AdgModel *model;
        AdgModelPrivate *data;
        AdgEntity *entity;

        model = (AdgModel *) object;
        data = model->data;

        /* Remove all the dependencies: this will emit a
         * "remove-dependency" signal for every dependency, dropping
         * all references from entities to this model */
        while (data->dependencies != NULL) {
            entity = (AdgEntity *) data->dependencies->data;
            adg_model_remove_dependency(model, entity);
        }

        g_signal_emit(model, _adg_signals[RESET], 0);
    }

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_DEPENDENCY:
        g_signal_emit(object, _adg_signals[ADD_DEPENDENCY], 0,
                      g_value_get_object(value));
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
 *
 * Since: 1.0
 **/
void
adg_model_add_dependency(AdgModel *model, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ADG_IS_ENTITY(entity));
    g_object_set(model, "dependency", entity, NULL);
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
 *
 * Since: 1.0
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
 *.
 * Gets the list of entities dependending on @model. This list
 * is owned by @model and must not be modified or freed.
 *
 * Returns: (transfer none) (element-type Adg.Entity): a #GSList of dependencies or <constant>NULL</constant> on error.
 *
 * Since: 1.0
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
 * @callback: (scope call): the entity callback
 * @user_data: general purpose user data passed "as is" to @callback
 *
 * Invokes @callback on each entity linked to @model.
 *
 * Since: 1.0
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
 * @pair: the #CpmlPair
 *
 * <note><para>
 * This function is only useful in model definitions, such as
 * inside an #AdgTrailCallback function or while constructing
 * an #AdgPath instance.
 * </para></note>
 *
 * Emits a #AdgModel::set-named-pair signal on @model passing
 * @name and @pair as arguments.
 *
 * Since: 1.0
 **/
void
adg_model_set_named_pair(AdgModel *model, const gchar *name,
                         const CpmlPair *pair)
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
 *
 * Since: 1.0
 **/
void
adg_model_set_named_pair_explicit(AdgModel *model, const gchar *name,
                                  gdouble x, gdouble y)
{
    CpmlPair pair;

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
 * Returns: the requested #CpmlPair or <constant>NULL</constant> if not found.
 *
 * Since: 1.0
 **/
const CpmlPair *
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
 * @callback: (scope call): the named pair callback
 * @user_data: general purpose user data passed "as is" to @callback
 *
 * Invokes @callback for each named pair set on @model. This can
 * be used, for example, to retrieve all the named pairs of a @model
 * or to duplicate a transformed version of every named pair.
 *
 * Since: 1.0
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
 * This function is only useful new model implementations.
 * </para></note>
 *
 * Emits the #AdgModel::clear signal on @model.
 *
 * Since: 1.0
 **/
void
adg_model_clear(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, _adg_signals[CLEAR], 0);
}

/**
 * adg_model_reset:
 * @model: an #AdgModel
 *
 * Emits the #AdgModel::reset signal on @model.
 *
 * Since: 1.0
 **/
void
adg_model_reset(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, _adg_signals[RESET], 0);
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
 *
 * Since: 1.0
 **/
void
adg_model_changed(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, _adg_signals[CHANGED], 0);
}


static void
_adg_add_dependency(AdgModel *model, AdgEntity *entity)
{
    AdgModelPrivate *data;

    /* Do not add NULL values */
    if (entity == NULL)
        return;

    data = model->data;

    /* The prepend operation is more efficient */
    data->dependencies = g_slist_prepend(data->dependencies, entity);

    g_object_ref(entity);
}

static void
_adg_remove_dependency(AdgModel *model, AdgEntity *entity)
{
    AdgModelPrivate *data;
    GSList *node;

    data = model->data;
    node = g_slist_find(data->dependencies, entity);

    if (node == NULL) {
        g_warning(_("%s: attempting to remove the nonexistent dependency "
                    "on the entity with type %s from a model of type %s"),
                  G_STRLOC,
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(model)));
        return;
    }

    data->dependencies = g_slist_delete_link(data->dependencies, node);
    g_object_unref(entity);
}

static void
_adg_reset(AdgModel *model)
{
    AdgModelPrivate *data = model->data;

    adg_model_clear(model);

    if (data->named_pairs) {
        g_hash_table_destroy(data->named_pairs);
        data->named_pairs = NULL;
    }
}

static void
_adg_set_named_pair(AdgModel *model, const gchar *name, const CpmlPair *pair)
{
    AdgModelPrivate *data;
    GHashTable **hash;
    gchar *key;
    CpmlPair *value;

    data = model->data;
    hash = &data->named_pairs;

    if (pair == NULL) {
        /* Delete mode: raise a warning if @name is not found */
        if (*hash == NULL || !g_hash_table_remove(*hash, name))
            g_warning(_("%s: attempting to remove nonexistent '%s' named pair"),
                      G_STRLOC, name);

        return;
    }

    /* Insert or update mode */
    key = g_strdup(name);
    value = cpml_pair_dup(pair);

    if (*hash == NULL)
        *hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    g_hash_table_insert(*hash, key, value);
}

static const CpmlPair *
_adg_named_pair(AdgModel *model, const gchar *name)
{
    AdgModelPrivate *data = model->data;

    if (data->named_pairs == NULL)
        return NULL;

    return g_hash_table_lookup(data->named_pairs, name);
}

static void
_adg_changed(AdgModel *model)
{
    /* Invalidate all the entities dependent on this model */
    adg_model_foreach_dependency(model, _adg_invalidate_wrapper, NULL);
}

static void
_adg_named_pair_wrapper(gpointer key, gpointer value, gpointer user_data)
{
    const gchar *name;
    CpmlPair *pair;
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
