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
 * SECTION:adg-container
 * @short_description: Base class for entity that can contain other entities
 *
 * The #AdgContainer is an entity that can contains more sub-entities.
 * Moreover, it can apply a common transformation to local and/or global
 * maps: see http://adg.entidi.com/tutorial/view/3 for further details.
 **/

/**
 * AdgContainer:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

#include "adg-container.h"
#include "adg-container-private.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_CHILD
};

enum {
    ADD,
    REMOVE,
    LAST_SIGNAL
};


static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             dispose                 (GObject        *object);
static GSList *         get_children            (AdgContainer   *container);
static void             add                     (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             remove                  (AdgContainer   *container,
                                                 AdgEntity      *entity);
static gboolean         invalidate              (AdgEntity      *entity);
static gboolean         render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE(AdgContainer, adg_container, ADG_TYPE_ENTITY)


static void
adg_container_class_init(AdgContainerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgContainerPrivate));

    gobject_class->set_property = set_property;
    gobject_class->dispose = dispose;

    entity_class->invalidate = invalidate;
    entity_class->render = render;

    klass->get_children = get_children;
    klass->add = add;
    klass->remove = remove;

    param = g_param_spec_object("child",
                                P_("Child"),
                                P_("Can be used to add a new child to the container"),
                                ADG_TYPE_ENTITY, G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_CHILD, param);

    /**
     * AdgContainer::add:
     * @container: an #AdgContainer
     * @entity: the #AdgEntity to add
     *
     * Adds @entity to @container. @entity must not be inside another
     * container or the operation will fail.
     **/
    signals[ADD] = g_signal_new("add",
                                G_OBJECT_CLASS_TYPE(gobject_class),
                                G_SIGNAL_RUN_FIRST,
                                G_STRUCT_OFFSET(AdgContainerClass, add),
                                NULL, NULL,
                                g_cclosure_marshal_VOID__OBJECT,
                                G_TYPE_NONE, 1, ADG_TYPE_ENTITY);

    /**
     * AdgContainer::remove:
     * @container: an #AdgContainer
     * @entity: the #AdgEntity to remove
     *
     * Removes @entity from @container.
     **/
    signals[REMOVE] = g_signal_new("remove",
                                   G_OBJECT_CLASS_TYPE(gobject_class),
                                   G_SIGNAL_RUN_FIRST,
                                   G_STRUCT_OFFSET(AdgContainerClass, remove),
                                   NULL, NULL,
                                   g_cclosure_marshal_VOID__OBJECT,
                                   G_TYPE_NONE, 1, ADG_TYPE_ENTITY);
}

static void
adg_container_init(AdgContainer *container)
{
    AdgContainerPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(container,
                                                            ADG_TYPE_CONTAINER,
                                                            AdgContainerPrivate);

    data->children = NULL;

    container->data = data;
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgContainer *container;
    AdgContainerPrivate *data;

    container = (AdgContainer *) object;
    data = container->data;

    switch (prop_id) {
    case PROP_CHILD:
        adg_container_add(container, g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}

static void
dispose(GObject *object)
{
    GObjectClass *object_class = (GObjectClass *) adg_container_parent_class;

    adg_container_foreach((AdgContainer *) object,
                          G_CALLBACK(adg_entity_unparent), NULL);

    if (object_class->dispose != NULL)
        object_class->dispose(object);
}


/**
 * adg_container_new:
 *
 * Creates a new container entity.
 *
 * Return value: the newly created entity
 **/
AdgEntity *
adg_container_new(void)
{
    return (AdgEntity *) g_object_new(ADG_TYPE_CONTAINER, NULL);
}


/**
 * adg_container_add:
 * @container: an #AdgContainer
 * @entity: an #AdgEntity
 *
 * Emits a #AdgContainer::add signal on @container passing
 * @entity as argument.
 *
 * @entity may be added to only one container at a time; you can't
 * place the same entity inside two different containers.
 **/
void
adg_container_add(AdgContainer *container, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(container, signals[ADD], 0, entity);
}

/**
 * adg_container_remove:
 * @container: an #AdgContainer
 * @entity: an #AdgEntity
 *
 * Emits a #AdgContainer::remove signal on @container passing
 * @entity as argument. @entity must be inside @container.
 *
 * Note that @container will own a reference to @entity
 * and that this may be the last reference held; so removing an
 * entity from its container can destroy it.
 *
 * If you want to use @entity again, you need to add a reference
 * to it, using g_object_ref(), before removing it from @container.
 *
 * If you don't want to use @entity again, it's usually more
 * efficient to simply destroy it directly using g_object_unref()
 * since this will remove it from the container.
 **/
void
adg_container_remove(AdgContainer *container, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(container, signals[REMOVE], 0, entity);
}

/**
 * adg_container_get_children:
 * @container: an #AdgContainer
 *
 * Gets the children list of @container.
 * This list must be manually freed when no longer user.
 *
 * Returns: a newly allocated #GSList or %NULL on error
 **/
GSList *
adg_container_get_children(AdgContainer *container)
{
    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);

    return ADG_CONTAINER_GET_CLASS(container)->get_children(container);
}

/**
 * adg_container_foreach:
 * @container: an #AdgContainer
 * @callback: a callback
 * @user_data: callback user data
 * 
 * Invokes @callback on each child of @container.
 * The callback should be declared as:
 *
 * <code>
 * void callback(AdgEntity *entity, gpointer user_data);
 * </code>
 **/
void
adg_container_foreach(AdgContainer *container,
                      GCallback callback, gpointer user_data)
{
    GSList *children;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(callback != NULL);

    children = adg_container_get_children (container);

    while (children) {
        if (children->data)
            ((void (*) (gpointer, gpointer)) callback) (children->data, user_data);

        children = g_slist_delete_link(children, children);
    }
}

/**
 * adg_container_propagate:
 * @container: an #AdgContainer
 * @signal_id: the signal id
 * @detail: the detail
 * @...: parameters to be passed to the signal, followed by a location for
 *       the return value. If the return type of the signal is G_TYPE_NONE,
 *       the return value location can be omitted.
 *
 * Emits the specified signal to all the children of @container
 * using g_signal_emit_valist() calls.
 **/
void
adg_container_propagate(AdgContainer *container,
                        guint signal_id, GQuark detail, ...)
{
    va_list var_args;

    va_start(var_args, detail);
    adg_container_propagate_valist(container, signal_id, detail, var_args);
    va_end(var_args);
}

/**
 * adg_container_propagate_by_name:
 * @container: an #AdgContainer
 * @detailed_signal: a string of the form "signal-name::detail".
 * @...: a list of parameters to be passed to the signal, followed by
 *       a location for the return value. If the return type of the signal
 *       is G_TYPE_NONE, the return value location can be omitted.
 *
 * Emits the specified signal to all the children of @container
 * using g_signal_emit_valist() calls.
 **/
void
adg_container_propagate_by_name(AdgContainer *container,
                                const gchar *detailed_signal, ...)
{
    guint   signal_id;
    GQuark  detail = 0;
    va_list var_args;

    if (!g_signal_parse_name(detailed_signal, G_TYPE_FROM_INSTANCE(container),
                             &signal_id, &detail, FALSE)) {
        g_warning("%s: signal `%s' is invalid for instance `%p'",
                  G_STRLOC, detailed_signal, container);
        return;
    }

    va_start(var_args, detailed_signal);
    adg_container_propagate_valist(container, signal_id, detail, var_args);
    va_end(var_args);
}

/**
 * adg_container_propagate_valist:
 * @container: an #AdgContainer
 * @signal_id: the signal id
 * @detail: the detail
 * @var_args: a list of parameters to be passed to the signal, followed by a
 *            location for the return value. If the return type of the signal
 *            is G_TYPE_NONE, the return value location can be omitted.
 *
 * Emits the specified signal to all the children of @container
 * using g_signal_emit_valist() calls.
 **/
void
adg_container_propagate_valist(AdgContainer *container,
                               guint signal_id, GQuark detail, va_list var_args)
{
    GSList *children;
    va_list var_copy;

    g_return_if_fail(ADG_IS_CONTAINER(container));

    children = adg_container_get_children(container);

    while (children) {
        if (children->data) {
            G_VA_COPY(var_copy, var_args);
            g_signal_emit_valist(children->data, signal_id, detail, var_copy);
        }

        children = g_slist_delete_link(children, children);
    }
}


static GSList *
get_children(AdgContainer *container)
{
    AdgContainerPrivate *data = container->data;

    return g_slist_copy(data->children);
}

static void
add(AdgContainer *container, AdgEntity *entity)
{
    AdgContainer *old_parent;
    AdgContainerPrivate *data;

    old_parent = adg_entity_get_parent(entity);
    if (old_parent != NULL) {
        g_warning("Attempting to add an entity with type %s to a container "
                  "of type %s, but the entity is already inside a container "
                  "of type %s.",
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(container)),
                  g_type_name(G_OBJECT_TYPE(old_parent)));
        return;
    }

    data = container->data;
    data->children = g_slist_append(data->children, entity);
    adg_entity_set_parent(entity, container);
}

static void
remove(AdgContainer *container, AdgEntity *entity)
{
    AdgContainerPrivate *data;
    GSList *node;

    data = container->data;
    node = g_slist_find(data->children, entity);

    if (node == NULL) {
        g_warning("Attempting to remove an entity with type %s from a "
                  "container of type %s, but the entity is not present.",
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(container)));
        return;
    }

    data->children = g_slist_delete_link(data->children, node);
    adg_entity_unparent(entity);
}


static gboolean
invalidate(AdgEntity *entity)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "invalidate");
    return TRUE;
}

static gboolean
render(AdgEntity *entity, cairo_t *cr)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "render", cr);
    return TRUE;
}
