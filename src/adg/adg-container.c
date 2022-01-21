/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
 * maps: see http://adg.entidi.com/home/details/ for further details.
 *
 * Adding an entity to a container will create a strong reference to the
 * container in the entity and a weak reference to the entity on the
 * container. This way the container will be able to destroy its children
 * when destroyed and it will be able to update its children when an entity
 * is destroyed.
 *
 * Since: 1.0
 **/

/**
 * AdgContainer:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgContainerClass:
 * @children: virtual method that gets the list of entities (that is
 *            #AdgEntity and derived instances) owned by the container.
 * @add:      signal that adds a new entity to the container.
 * @remove:   signal that removes a specific entity from the container.
 *
 * #AdgContainer effectively stores a #GSList of children into its
 * private data and keeps a reference to every child it owns.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-container.h"
#include "adg-container-private.h"


#define _ADG_PARENT_OBJECT_CLASS  ((GObjectClass *) adg_container_parent_class)
#define _ADG_PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_container_parent_class)


G_DEFINE_TYPE_WITH_PRIVATE(AdgContainer, adg_container, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_CHILD
};

enum {
    ADD,
    REMOVE,
    LAST_SIGNAL
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_destroy            (AdgEntity      *entity);
static void             _adg_global_changed     (AdgEntity      *entity);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_invalidate         (AdgEntity      *entity);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_add_extents        (AdgEntity      *entity,
                                                 CpmlExtents    *extents);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static GSList *         _adg_children           (AdgContainer   *container);
static void             _adg_add                (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             _adg_remove             (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             _adg_remove_from_list   (gpointer        container,
                                                 GObject        *entity);

static guint            _adg_signals[LAST_SIGNAL] = { 0 };


static void
adg_container_class_init(AdgContainerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    gobject_class->dispose = _adg_dispose;
    gobject_class->set_property = _adg_set_property;

    entity_class->destroy = _adg_destroy;
    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    klass->children = _adg_children;
    klass->add = _adg_add;
    klass->remove = _adg_remove;

    param = g_param_spec_object("child",
                                P_("Child"),
                                P_("Can be used to add a new child to the container"),
                                ADG_TYPE_ENTITY,
                                G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_CHILD, param);

    /**
     * AdgContainer::add:
     * @container: an #AdgContainer
     * @entity: the #AdgEntity to add
     *
     * Adds @entity to @container. @entity must not be inside another
     * container or the operation will fail.
     *
     * Since: 1.0
     **/
    _adg_signals[ADD] = g_signal_new("add",
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
     *
     * Since: 1.0
     **/
    _adg_signals[REMOVE] = g_signal_new("remove",
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
    AdgContainerPrivate *data = adg_container_get_instance_private(container);
    data->children = NULL;
}

static void
_adg_dispose(GObject *object)
{
    AdgContainer *container = (AdgContainer *) object;
    AdgContainerPrivate *data = adg_container_get_instance_private(container);

    /* Remove all the children from the container: these will emit
     * a "remove" signal for every child and will drop all the
     * references from the children to this container (and, obviously,
     * from the container to the children). */
    while (data->children != NULL)
        adg_container_remove(container, (AdgEntity *) data->children->data);

    if (_ADG_PARENT_OBJECT_CLASS->dispose)
        _ADG_PARENT_OBJECT_CLASS->dispose(object);
}

static void
_adg_set_property(GObject *object,
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
 *
 * Since: 1.0
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
 *
 * Since: 1.0
 **/
void
adg_container_add(AdgContainer *container, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(container, _adg_signals[ADD], 0, entity);
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
 * <informalexample><programlisting language="C">
 * g_object_ref(entity);
 * adg_container_remove(container1, entity);
 * adg_container_add(container2, entity)
 * g_object_unref(entity);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_container_remove(AdgContainer *container, AdgEntity *entity)
{
    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(ADG_IS_ENTITY(entity));

    g_signal_emit(container, _adg_signals[REMOVE], 0, entity);
}

/**
 * adg_container_children:
 * @container: an #AdgContainer
 *
 * Gets the children list of @container. This list must be manually
 * freed with g_slist_free() when no longer user.
 *
 * The returned list is ordered from the most recently added child
 * to the oldest one.
 *
 * Returns: (element-type AdgEntity) (transfer container): a newly allocated #GSList of #AdgEntity or <constant>NULL</constant> on no children or errors
 *
 * Since: 1.0
 **/
GSList *
adg_container_children(AdgContainer *container)
{
    AdgContainerClass *klass;

    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);

    klass = ADG_CONTAINER_GET_CLASS(container);

    if (klass->children == NULL)
        return NULL;

    return klass->children(container);
}

/**
 * adg_container_foreach:
 * @container: an #AdgContainer
 * @callback: (scope call): a callback
 * @user_data: callback user data
 *
 * Invokes @callback on each child of @container.
 * The callback should be declared as:
 *
 * <informalexample><programlisting language="C">
 * void callback(AdgEntity *entity, gpointer user_data);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_container_foreach(AdgContainer *container,
                      GCallback callback, gpointer user_data)
{
    GSList *children;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(callback != NULL);

    children = adg_container_children(container);

    while (children != NULL) {
        if (children->data != NULL)
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
 *
 * Since: 1.0
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
 *
 * Since: 1.0
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
        g_warning(_("%s: signal '%s' is invalid for instance %p"),
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
 *
 * Since: 1.0
 **/
void
adg_container_propagate_valist(AdgContainer *container,
                               guint signal_id, GQuark detail, va_list var_args)
{
    GSList *children;
    va_list var_copy;

    g_return_if_fail(ADG_IS_CONTAINER(container));

    children = adg_container_children(container);

    while (children != NULL) {
        if (children->data != NULL) {
            G_VA_COPY(var_copy, var_args);
            g_signal_emit_valist(children->data, signal_id, detail, var_copy);
        }

        children = g_slist_delete_link(children, children);
    }
}


static void
_adg_destroy(AdgEntity *entity)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "destroy");

    if (_ADG_PARENT_ENTITY_CLASS->destroy)
        _ADG_PARENT_ENTITY_CLASS->destroy(entity);
}

static void
_adg_global_changed(AdgEntity *entity)
{
    if (_ADG_PARENT_ENTITY_CLASS->global_changed)
        _ADG_PARENT_ENTITY_CLASS->global_changed(entity);

    adg_container_propagate_by_name((AdgContainer *) entity, "global-changed");
}

static void
_adg_local_changed(AdgEntity *entity)
{
    if (_ADG_PARENT_ENTITY_CLASS->local_changed)
        _ADG_PARENT_ENTITY_CLASS->local_changed(entity);

    adg_container_propagate_by_name((AdgContainer *) entity, "local-changed");
}

static void
_adg_invalidate(AdgEntity *entity)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "invalidate");
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgContainer *container = (AdgContainer *) entity;
    CpmlExtents extents = { 0 };

    adg_container_propagate_by_name(container, "arrange", NULL);
    adg_container_foreach(container, G_CALLBACK(_adg_add_extents), &extents);
    adg_entity_set_extents(entity, &extents);
}

static void
_adg_add_extents(AdgEntity *entity, CpmlExtents *extents)
{
    if (! adg_entity_has_floating(entity)) {
        cpml_extents_add(extents, adg_entity_get_extents(entity));
    }
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    adg_container_propagate_by_name((AdgContainer *) entity, "render", cr);
}


static GSList *
_adg_children(AdgContainer *container)
{
    AdgContainerPrivate *data = adg_container_get_instance_private(container);

    /* The NULL case is already managed by GLib */
    return g_slist_copy(data->children);
}

static void
_adg_add(AdgContainer *container, AdgEntity *entity)
{
    const AdgEntity *old_parent;
    AdgContainerPrivate *data;

    old_parent = adg_entity_get_parent(entity);
    if (old_parent != NULL) {
        g_warning(_("Attempting to add an entity with type %s to a container "
                    "of type %s, but the entity is already inside a container "
                    "of type %s"),
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(container)),
                  g_type_name(G_OBJECT_TYPE(old_parent)));
        return;
    }

    data = adg_container_get_instance_private(container);
    data->children = g_slist_prepend(data->children, entity);

    g_object_ref_sink(entity);
    adg_entity_set_parent(entity, (AdgEntity *) container);
    g_object_weak_ref((GObject *) entity, _adg_remove_from_list, container);
}

static void
_adg_remove_from_list(gpointer container, GObject *entity)
{
    AdgContainerPrivate *data = adg_container_get_instance_private((AdgContainer *) container);
    data->children = g_slist_remove(data->children, entity);
}

static void
_adg_remove(AdgContainer *container, AdgEntity *entity)
{
    AdgContainerPrivate *data = adg_container_get_instance_private(container);
    GSList *node = g_slist_find(data->children, entity);

    if (node == NULL) {
        g_warning(_("Attempting to remove an entity with type %s from a "
                    "container of type %s, but the entity is not present"),
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(container)));
        return;
    }

    g_object_weak_unref((GObject *) entity, _adg_remove_from_list, container);
    data->children = g_slist_delete_link(data->children, node);
    adg_entity_set_parent(entity, NULL);
    g_object_unref(entity);
}
