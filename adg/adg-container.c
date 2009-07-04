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
 * SECTION:container
 * @title: AdgContainer
 * @short_description: Base class for entity that can contain other entities
 *
 * The #AdgContainer is an entity that can contains more sub-entities.
 * Each AdgContainer has its paper matrix (#AdgContainer:paper_matrix) to be
 * used on paper-dependent references (such as font and arrow sizes, line
 * thickness etc...) and a model matrix usually applied to the model view. See
 * http://adg.entidi.com/tutorial/view/3 for details on this topic.
 *
 * This means an AdgContainer can be thought as a group of entities with the
 * same geometrical identity (same scale, reference point ecc...).
 */

#include "adg-container.h"
#include "adg-container-private.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_CHILD,
    PROP_MODEL_TRANSFORMATION,
    PROP_PAPER_TRANSFORMATION
};

enum {
    ADD,
    REMOVE,
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
static const AdgMatrix *get_model_matrix        (AdgEntity      *entity);
static const AdgMatrix *get_paper_matrix        (AdgEntity      *entity);
static void             model_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static void             paper_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static GSList *         get_children            (AdgContainer   *container);
static gboolean         add                     (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             real_add                (AdgContainer   *container,
                                                 AdgEntity      *entity,
                                                 gpointer        user_data);
static gboolean         remove                  (AdgContainer   *container,
                                                 AdgEntity      *entity);
static void             real_remove             (AdgContainer   *container,
                                                 AdgEntity      *entity,
                                                 gpointer        user_data);
static void             invalidate              (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE(AdgContainer, adg_container, ADG_TYPE_ENTITY)


static void
adg_container_class_init(AdgContainerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;
    GClosure *closure;
    GType param_types[1];

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgContainerPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
    gobject_class->dispose = dispose;

    entity_class->model_matrix_changed = model_matrix_changed;
    entity_class->paper_matrix_changed = paper_matrix_changed;
    entity_class->get_model_matrix = get_model_matrix;
    entity_class->get_paper_matrix = get_paper_matrix;
    entity_class->invalidate = invalidate;
    entity_class->render = render;

    klass->get_children = get_children;
    klass->add = add;
    klass->remove = remove;

    param = g_param_spec_boxed("child",
                               P_("Child"),
                               P_("Can be used to add a new child to the container"),
                               ADG_TYPE_ENTITY, G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_CHILD, param);

    param = g_param_spec_boxed("model-transformation",
                               P_("The model transformation"),
                               P_("The model transformation to be applied to this container and its children entities"),
                               ADG_TYPE_MATRIX, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class,
                                    PROP_MODEL_TRANSFORMATION, param);

    param = g_param_spec_boxed("paper-transformation",
                               P_("The paper transformation"),
                               P_("The paper transformation to be applied to this container and its children entities"),
                               ADG_TYPE_MATRIX, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class,
                                    PROP_PAPER_TRANSFORMATION, param);

    /**
     * AdgContainer::add:
     * @container: an #AdgContainer
     * @entity: the #AdgEntity to add
     *
     * Adds @entity to @container.
     **/
    closure = g_cclosure_new(G_CALLBACK(real_add), (gpointer)0xdeadbeaf, NULL);
    param_types[0] = G_TYPE_OBJECT;
    signals[ADD] = g_signal_newv("add", ADG_TYPE_CONTAINER,
                                 G_SIGNAL_RUN_FIRST, closure, NULL, NULL,
                                 g_cclosure_marshal_VOID__OBJECT,
                                 G_TYPE_NONE, 1, param_types);

    /**
     * AdgContainer::remove:
     * @container: an #AdgContainer
     * @entity: the #AdgEntity to remove
     *
     * Removes @entity from @container.
     **/
    closure = g_cclosure_new(G_CALLBACK(real_remove), (gpointer)0xdeadbeaf, NULL);
    param_types[0] = G_TYPE_OBJECT;
    signals[REMOVE] = g_signal_newv("remove", ADG_TYPE_CONTAINER,
                                    G_SIGNAL_RUN_FIRST, closure, NULL, NULL,
                                    g_cclosure_marshal_VOID__OBJECT,
                                    G_TYPE_NONE, 1, param_types);
}

static void
adg_container_init(AdgContainer *container)
{
    AdgContainerPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(container,
                                                            ADG_TYPE_CONTAINER,
                                                            AdgContainerPrivate);

    priv->children = NULL;
    cairo_matrix_init_identity(&priv->model_transformation);
    cairo_matrix_init_identity(&priv->paper_transformation);
    cairo_matrix_init_identity(&priv->model_matrix);
    cairo_matrix_init_identity(&priv->paper_matrix);

    container->priv = priv;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgContainer *container = (AdgContainer *) object;

    switch (prop_id) {
    case PROP_MODEL_TRANSFORMATION:
        g_value_set_boxed(value, &container->priv->model_transformation);
        break;
    case PROP_PAPER_TRANSFORMATION:
        g_value_set_boxed(value, &container->priv->paper_transformation);
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
    AdgContainer *container = (AdgContainer *) object;

    switch (prop_id) {
    case PROP_CHILD:
        adg_container_add(container, g_value_get_object(value));
        break;
    case PROP_MODEL_TRANSFORMATION:
        adg_matrix_copy(&container->priv->model_transformation,
                        g_value_get_boxed(value));
        break;
    case PROP_PAPER_TRANSFORMATION:
        adg_matrix_copy(&container->priv->paper_transformation,
                        g_value_get_boxed(value));
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


static GSList *
get_children(AdgContainer *container)
{
    return g_slist_copy(container->priv->children);
}

static gboolean
add(AdgContainer *container, AdgEntity *entity)
{
    container->priv->children = g_slist_append(container->priv->children,
                                               entity);
    return TRUE;
}

static void
real_add(AdgContainer *container, AdgEntity *entity, gpointer user_data)
{
    AdgContainer *old_parent;

    g_assert(user_data == (gpointer) 0xdeadbeaf);

    old_parent = adg_entity_get_parent(entity);

    if (old_parent != NULL) {
        g_warning("Attempting to add an object with type %s to a container "
                  "of type %s, but the object is already inside a container "
                  "of type %s.",
                  g_type_name(G_OBJECT_TYPE(entity)),
                  g_type_name(G_OBJECT_TYPE(container)),
                  g_type_name(G_OBJECT_TYPE(old_parent)));
        return;
    }

    if (ADG_CONTAINER_GET_CLASS(container)->add(container, entity))
        adg_entity_set_parent(entity, container);
    else
        g_signal_stop_emission(container, signals[ADD], 0);
}

static gboolean
remove(AdgContainer *container, AdgEntity *entity)
{
    GSList *node = g_slist_find(container->priv->children, entity);

    if (!node)
        return FALSE;

    container->priv->children = g_slist_delete_link(container->priv->children,
                                                    node);
    return TRUE;
}

static void
real_remove(AdgContainer *container, AdgEntity *entity, gpointer user_data)
{
    g_assert(user_data == (gpointer) 0xdeadbeaf);

    if (ADG_CONTAINER_GET_CLASS(container)->remove(container, entity))
        adg_entity_unparent(entity);
    else
        g_signal_stop_emission(container, signals[REMOVE], 0);
}


static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgContainer *container;
    AdgEntityClass *entity_class;

    container = (AdgContainer *) entity;
    entity_class = (AdgEntityClass *) adg_container_parent_class;

    if (entity_class->model_matrix_changed != NULL)
        entity_class->model_matrix_changed(entity, parent_matrix);

    if (parent_matrix)
        cairo_matrix_multiply(&container->priv->model_matrix,
                              &container->priv->model_transformation,
                              parent_matrix);
    else
        adg_matrix_copy(&container->priv->model_matrix,
                        &container->priv->model_transformation);

    adg_container_propagate_by_name(container, "model-matrix-changed",
                                    &container->priv->model_matrix);
}

static void
paper_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgContainer *container;
    AdgEntityClass *entity_class;

    container = (AdgContainer *) entity;
    entity_class = (AdgEntityClass *) adg_container_parent_class;

    if (entity_class->paper_matrix_changed != NULL)
        entity_class->paper_matrix_changed(entity, parent_matrix);

    if (parent_matrix)
        cairo_matrix_multiply(&container->priv->paper_matrix,
                              &container->priv->paper_transformation,
                              parent_matrix);
    else
        adg_matrix_copy(&container->priv->paper_matrix,
                        &container->priv->paper_transformation);

    adg_container_propagate_by_name(container, "paper-matrix-changed",
                                    &container->priv->paper_matrix);
}

static const AdgMatrix *
get_model_matrix(AdgEntity *entity)
{
    AdgContainer *container = (AdgContainer *) entity;

    return &container->priv->model_matrix;
}

static const AdgMatrix *
get_paper_matrix(AdgEntity *entity)
{
    AdgContainer *container = (AdgContainer *) entity;

    return &container->priv->paper_matrix;
}


static void
invalidate(AdgEntity *entity)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_container_parent_class;

    adg_container_propagate_by_name((AdgContainer *) entity, "invalidate");

    if (entity_class->invalidate)
        entity_class->invalidate(entity);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_container_parent_class;

    cairo_set_matrix(cr, adg_entity_get_model_matrix(entity));
    adg_container_propagate_by_name((AdgContainer *) entity, "render", cr);

    if (entity_class->render)
        entity_class->render(entity, cr);
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

/**
 * adg_container_get_model_transformation:
 * @container: an #AdgContainer
 *
 * Returns the transformation to be combined with the transformations of the
 * parent hierarchy to get the final matrix to be applied in the model space.
 *
 * Return value: the model transformation
 **/
const AdgMatrix *
adg_container_get_model_transformation(AdgContainer *container)
{
    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);
    return &container->priv->model_transformation;
}

/**
 * adg_container_set_model_transformation:
 * @container: an #AdgContainer
 * @transformation: the new model transformation
 *
 * Sets the transformation to be applied in model space.
 **/
void
adg_container_set_model_transformation(AdgContainer *container,
                                       const AdgMatrix *transformation)
{
    AdgEntity *entity;
    AdgEntity *parent;
    const AdgMatrix *parent_matrix;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(transformation != NULL);

    entity = (AdgEntity *) container;
    parent = (AdgEntity *) adg_entity_get_parent(entity);
    parent_matrix = parent ? adg_entity_get_model_matrix(parent) : NULL;

    adg_matrix_copy(&container->priv->model_transformation, transformation);
    adg_entity_model_matrix_changed(entity, parent_matrix);
}

/**
 * adg_container_get_paper_transformation:
 * @container: an #AdgContainer
 *
 * Returns the transformation to be combined with the transformations of the
 * parent hierarchy to get the final matrix to be applied in the paper space.
 *
 * Return value: the paper transformation
 **/
const AdgMatrix *
adg_container_get_paper_transformation(AdgContainer *container)
{
    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);
    return &container->priv->paper_transformation;
}

/**
 * adg_container_set_paper_transformation:
 * @container: an #AdgContainer
 * @transformation: the new paper transformation
 *
 * Sets the transformation to be applied in paper space.
 **/
void
adg_container_set_paper_transformation(AdgContainer *container,
                                       const AdgMatrix *transformation)
{
    AdgEntity *entity;
    AdgEntity *parent;
    const AdgMatrix *parent_matrix;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(transformation != NULL);

    entity = (AdgEntity *) container;
    parent = (AdgEntity *) adg_entity_get_parent(entity);
    parent_matrix = parent ? adg_entity_get_paper_matrix(parent) : NULL;

    adg_matrix_copy(&container->priv->paper_transformation, transformation);
    adg_entity_paper_matrix_changed(entity, parent_matrix);
}
