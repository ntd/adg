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


#include "adg-model.h"
#include "adg-model-private.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_model_parent_class)


enum {
    PROP_0,
    PROP_DEPENDENCY
};

enum {
    ADD_DEPENDENCY,
    REMOVE_DEPENDENCY,
    CHANGED,
    LAST_SIGNAL
};


static void     dispose                 (GObject        *object);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);

static GSList * get_dependencies        (AdgModel       *model);
static void     add_dependency          (AdgModel       *model,
                                         AdgEntity      *entity);
static void     remove_dependency       (AdgModel       *model,
                                         AdgEntity      *entity);
static void     changed                 (AdgModel       *model);

static guint signals[LAST_SIGNAL] = { 0 };


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

    klass->get_dependencies = get_dependencies;
    klass->add_dependency = add_dependency;
    klass->remove_dependency = remove_dependency;
    klass->changed = changed;

    param = g_param_spec_object("entity",
                                P_("Entity"),
                                P_("Can be used to associate a new entity to this model (will be invalidated on model changed)"),
                                ADG_TYPE_ENTITY, G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_DEPENDENCY, param);

    /**
     * AdgModel::add-dependency:
     * @model: an #AdgModel
     * @entity: an #AdgEntity that depends on @model
     *
     * Adds @entity to @model. After that @entity will depend on @model,
     * that is #AdgModel::changed on @model will invalidate @entity.
     **/
    signals[ADD_DEPENDENCY] = g_signal_new("add-dependency",
                                           G_OBJECT_CLASS_TYPE(gobject_class),
                                           G_SIGNAL_RUN_FIRST,
                                           G_STRUCT_OFFSET(AdgModelClass, add_dependency),
                                           NULL, NULL,
                                           g_cclosure_marshal_VOID__OBJECT,
                                           G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgModel::remove-dependency:
     * @model: an #AdgModel
     * @entity: the #AdgEntity that does not depend on @model anymore
     *
     * Removes the @entity from @model, that is @entity will not depend
     * on @model anymore.
     **/
    signals[REMOVE_DEPENDENCY] = g_signal_new("remove-dependency",
                                              G_OBJECT_CLASS_TYPE(gobject_class),
                                              G_SIGNAL_RUN_FIRST,
                                              G_STRUCT_OFFSET(AdgModelClass, remove_dependency),
                                              NULL, NULL,
                                              g_cclosure_marshal_VOID__OBJECT,
                                              G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgModel::changed:
     * @model: an #AdgModel
     *
     * Notificates that the model has changed. By default, the model
     * cache is invalidated.
     **/
    signals[CHANGED] = g_signal_new("changed", ADG_TYPE_MODEL,
                                     G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                                     G_STRUCT_OFFSET(AdgModelClass, changed),
                                     NULL, NULL,
                                     g_cclosure_marshal_VOID__VOID,
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

    model = (AdgModel *) object;
    data = model->data;

    /* Remove all the dependencies from the model: this will emit
     * a "remove" signal for every dependency and will drop all the
     * references from those entities to this model */
    while (data->dependencies != NULL)
        adg_model_remove_dependency(model,
                                    (AdgEntity *) data->dependencies->data);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
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
 * Emits a #AdgModel::add-dependency signal on @model passing @entity
 * as argument. This will add a reference to @entity owned by @model.
 *
 * This function is only useful in entity implementations.
 **/
void
adg_model_add_dependency(AdgModel *model, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(model, signals[ADD_DEPENDENCY], 0, entity);
}

/**
 * adg_model_remove_dependency:
 * @model: an #AdgModel
 * @entity: an #AdgEntity
 *
 * Emits a #AdgModel::remove-dependency signal on @model passing
 * @entity as argument. @entity must be inside @model.
 *
 * Note that @model will own a reference to @entity and it
 * may be the last reference held: this means removing an entity
 * from the model can destroy it.
 *
 * This function is only useful in entity implementations.
 **/
void
adg_model_remove_dependency(AdgModel *model, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(model, signals[REMOVE_DEPENDENCY], 0, entity);
}

/**
 * adg_model_get_dependencies:
 * @model: an #AdgModel
 *
 * Gets a list of entities dependent on @model.
 * This list must be freed with g_slist_free() when no longer user.
 *
 * Returns: a newly allocated #GSList or %NULL on error
 **/
GSList *
adg_model_get_dependencies(AdgModel *model)
{
    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);

    return ADG_MODEL_GET_CLASS(model)->get_dependencies(model);
}

/**
 * adg_model_foreach_dependency:
 * @model: an #AdgModel
 * @callback: a callback
 * @user_data: callback user data
 *
 * Invokes @callback on each entity linked to @model.
 * The callback should be declared as:
 *
 * |[
 * void callback(AdgEntity *entity, gpointer user_data);
 * ]|
 **/
void
adg_model_foreach_dependency(AdgModel *model, GCallback callback,
                             gpointer user_data)
{
    GSList *dependencies;

    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(callback != NULL);

    dependencies = adg_model_get_dependencies(model);

    while (dependencies) {
        if (dependencies->data)
            ((void (*) (gpointer, gpointer)) callback) (dependencies->data,
                                                        user_data);

        dependencies = g_slist_delete_link(dependencies, dependencies);
    }
}

/**
 * adg_model_changed:
 * @model: an #AdgModel
 *
 * Emits the #AdgModel::changed signal on @model.
 *
 * This function is only useful in model implementations.
 */
void
adg_model_changed(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, signals[CHANGED], 0);
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
        g_warning("Attempting to remove an entity with type %s from a "
                  "model of type %s, but the entity is not present.",
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(model)));
        return;
    }

    data->dependencies = g_slist_delete_link(data->dependencies, node);
    g_object_unref(entity);
}

static GSList *
get_dependencies(AdgModel *model)
{
    AdgModelPrivate *data = model->data;

    /* The NULL case is yet managed by GLib */
    return g_slist_copy(data->dependencies);
}

static void
changed(AdgModel *model)
{
    /* Invalidate all the entities dependent on this model */
    adg_model_foreach_dependency(model,
                                 G_CALLBACK(adg_entity_invalidate), NULL);
}
