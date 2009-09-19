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
#include "adg-marshal.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_container_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_container_parent_class)


enum {
    PROP_0,
    PROP_CHILD
};

enum {
    ADD,
    REMOVE,
    LAST_SIGNAL
};


static void             dispose                 (GObject        *object);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             global_changed          (AdgEntity      *entity);
static void             local_changed           (AdgEntity      *entity);
static void             invalidate              (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static GSList *         get_children            (AdgContainer   *container);
static void             add                     (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             remove                  (AdgContainer   *container,
                                                 AdgEntity      *entity);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE(AdgContainer, adg_container, ADG_TYPE_ENTITY);


static void
adg_container_class_init(AdgContainerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgContainerPrivate));

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;

    entity_class->global_changed = global_changed;
    entity_class->local_changed = local_changed;
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
                                adg_marshal_VOID__OBJECT,
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
                                   adg_marshal_VOID__OBJECT,
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
dispose(GObject *object)
{
    AdgContainer *container;
    AdgContainerPrivate *data;

    container = (AdgContainer *) object;
    data = container->data;

    /* Remove all the children from the container: these will emit
     * a "remove" signal for every child and will drop all the
     * references from the children to this container (and, obviously,
     * from the container to the children). */
    while (data->children != NULL)
        adg_container_remove(container, (AdgEntity *) data->children->data);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgContainer *container = (AdgContainer *) object;

    switch (prop_id) {
    case PROP_CHILD:
        adg_container_add(container, g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}


/**
 * adg_container_new:
 *
 * Creates a new container entity.
 *
 * Returns: the newly created container entity
 **/
AdgContainer *
adg_container_new(void)
{
    return g_object_new(ADG_TYPE_CONTAINER, NULL);
}


/**
 * adg_container_add:
 * @container: an #AdgContainer
 * @entity: an #AdgEntity
 *
 * Emits a #AdgContainer::add signal on @container passing @entity
 * as argument. @entity must be added to only one container at a time,
 * you can't place the same entity inside two different containers.
 *
 * Once @entity has been added, the floating reference will be removed
 * and @container will own a reference to @entity. This means the only
 * proper way to destroy @entity is to call adg_container_remove().
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
 * Note that @container will own a reference to @entity and it
 * may be the last reference held: this means removing an entity
 * from its container can destroy it.
 *
 * If you want to use @entity again, you need to add a reference
 * to it, using g_object_ref(), before removing it from @container.
 * The following typical example shows you how to properly move
 * <varname>entity</varname> from <varname>container1</varname>
 * to <varname>container2</varname>:
 *
 * |[
 * g_object_ref(entity);
 * adg_container_remove(container1, entity);
 * adg_container_add(container2, entity)
 * g_object_unref(entity);
 * ]|
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
 * Gets the children list of @container. This list must be manually
 * freed with g_slist_free() when no longer user.
 *
 * The returned list is ordered from the most recently added child
 * to the oldest one.
 *
 * Returns: a newly allocated #GSList or %NULL empty list or on errors
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
 * |[
 * void callback(AdgEntity *entity, gpointer user_data);
 * ]|
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
 * @...: parameters to be passed to the signal, followed by a pointer
 *       to the allocated memory where to store the return type: if
 *       the signal is %G_TYPE_NONE (void return type), this trailing
 *       pointer should be omitted
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
 * @...: parameters to be passed to the signal, followed by a pointer
 *       to the allocated memory where to store the return type: if
 *       the signal is %G_TYPE_NONE (void return type), this trailing
 *       pointer should be omitted
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
 * @var_args: parameters to be passed to the signal, followed by a
 *            pointer to the allocated memory where to store the
 *            return type: if the signal is %G_TYPE_NONE (void return
 *            type), this trailing pointer should be omitted
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


static void
global_changed(AdgEntity *entity)
{
    PARENT_ENTITY_CLASS->global_changed(entity);
    adg_container_propagate_by_name((AdgContainer *) entity, "global-changed");
}

static void
local_changed(AdgEntity *entity)
{
    PARENT_ENTITY_CLASS->local_changed(entity);
    adg_container_propagate_by_name((AdgContainer *) entity, "local-changed");
}

static void
invalidate(AdgEntity *entity)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "invalidate");
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "render", cr);
}


static GSList *
get_children(AdgContainer *container)
{
    AdgContainerPrivate *data = container->data;

    /* The NULL case is yet managed by GLib */
    return g_slist_copy(data->children);
}

static void
add(AdgContainer *container, AdgEntity *entity)
{
    AdgEntity *old_parent;
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
    data->children = g_slist_prepend(data->children, entity);

    g_object_ref_sink(entity);
    adg_entity_set_parent(entity, (AdgEntity *) container);
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
    adg_entity_set_parent(entity, NULL);
    g_object_unref(entity);
}
