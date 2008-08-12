/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


/**
 * SECTION:container
 * @title: AdgContainer
 * @short_description: Base class for entity that can contain other entities
 *
 * The #AdgContainer is an entity that implements the #GContainerable interface.
 * Each AdgContainer has its paper matrix (#AdgContainer:paper_matrix) to be
 * used on paper-dependent references (such as font and arrow sizes, line
 * thickness etc...) and a model matrix usually applied to the model view. See
 * http://www.entidi.it/adg/tutorial/view/3 for details on this topic.
 *
 * This means an AdgContainer can be thought as a group of entities with the
 * same geometrical identity (same scale, reference point ecc...).
 */

#include "adg-container.h"
#include "adg-container-private.h"
#include "adg-intl.h"

#include <gcontainer/gcontainer.h>

#define PARENT_CLASS ((AdgEntityClass *) adg_container_parent_class)


enum {
    PROP_0,
    PROP_CHILD,
    PROP_MODEL_TRANSFORMATION,
    PROP_PAPER_TRANSFORMATION
};


static void             containerable_init      (GContainerableIface *iface);
static void             get_property            (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static const AdgMatrix *get_model_matrix        (AdgEntity      *entity);
static const AdgMatrix *get_paper_matrix        (AdgEntity      *entity);
static void             model_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static void             paper_matrix_changed    (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static GSList *         get_children            (GContainerable *containerable);
static gboolean         add                     (GContainerable *containerable,
                                                 GChildable     *childable);
static gboolean         remove                  (GContainerable *containerable,
                                                 GChildable     *childable);
static void             invalidate              (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);


G_DEFINE_TYPE_EXTENDED(AdgContainer, adg_container,
                       ADG_TYPE_ENTITY, 0,
                       G_IMPLEMENT_INTERFACE(G_TYPE_CONTAINERABLE,
                                             containerable_init))


static void
adg_container_class_init(AdgContainerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgContainerPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
    gobject_class->dispose = g_containerable_dispose;

    entity_class->model_matrix_changed = model_matrix_changed;
    entity_class->paper_matrix_changed = paper_matrix_changed;
    entity_class->get_model_matrix = get_model_matrix;
    entity_class->get_paper_matrix = get_paper_matrix;
    entity_class->invalidate = invalidate;
    entity_class->render = render;

    g_object_class_override_property(gobject_class, PROP_CHILD, "child");

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
}

static void
containerable_init(GContainerableIface *iface)
{
    iface->get_children = get_children;
    iface->add = add;
    iface->remove = remove;
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
        g_containerable_add((GContainerable *) container,
                            g_value_get_object(value));
        break;
    case PROP_MODEL_TRANSFORMATION:
        adg_matrix_set(&container->priv->model_transformation,
                       g_value_get_boxed(value));
        break;
    case PROP_PAPER_TRANSFORMATION:
        adg_matrix_set(&container->priv->paper_transformation,
                       g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}


static GSList *
get_children(GContainerable *containerable)
{
    AdgContainer *container = (AdgContainer *) containerable;

    return g_slist_copy(container->priv->children);
}

static gboolean
add(GContainerable *containerable, GChildable *childable)
{
    AdgContainer *container = (AdgContainer *) containerable;

    container->priv->children =
        g_slist_append(container->priv->children, childable);
    return TRUE;
}

static gboolean
remove(GContainerable *containerable, GChildable *childable)
{
    AdgContainer *container;
    GSList *node;

    container = (AdgContainer *) containerable;
    node = g_slist_find(container->priv->children, childable);

    if (!node)
        return FALSE;

    container->priv->children =
        g_slist_delete_link(container->priv->children, node);
    return TRUE;
}


static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgContainer *container = (AdgContainer *) entity;

    PARENT_CLASS->model_matrix_changed(entity, parent_matrix);

    if (parent_matrix)
        cairo_matrix_multiply(&container->priv->model_matrix,
                              parent_matrix,
                              &container->priv->model_transformation);
    else
        adg_matrix_set(&container->priv->model_matrix,
                       &container->priv->model_transformation);

    g_containerable_propagate_by_name((GContainerable *) entity,
                                      "model-matrix-changed",
                                      &container->priv->model_matrix);
}

static void
paper_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgContainer *container = (AdgContainer *) entity;

    PARENT_CLASS->paper_matrix_changed(entity, parent_matrix);

    if (parent_matrix)
        cairo_matrix_multiply(&container->priv->paper_matrix,
                              parent_matrix,
                              &container->priv->paper_transformation);
    else
        adg_matrix_set(&container->priv->paper_matrix,
                       &container->priv->paper_transformation);

    g_containerable_propagate_by_name((GContainerable *) entity,
                                      "paper-matrix-changed",
                                      &container->priv->paper_matrix);
}

static const AdgMatrix *
get_model_matrix(AdgEntity *entity)
{
    return &((AdgContainer *) entity)->priv->model_matrix;
}

static const AdgMatrix *
get_paper_matrix(AdgEntity *entity)
{
    return &((AdgContainer *) entity)->priv->paper_matrix;
}


static void
invalidate(AdgEntity *entity)
{
    g_containerable_propagate_by_name((GContainerable *) entity,
                                      "invalidate", NULL);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    cairo_set_matrix(cr, adg_entity_get_model_matrix(entity));
    g_containerable_propagate_by_name((GContainerable *) entity,
                                      "render", cr);
    PARENT_CLASS->render(entity, cr);
}


const AdgMatrix *
adg_container_get_model_transformation(AdgContainer *container)
{
    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);
    return &container->priv->model_transformation;
}

void
adg_container_set_model_transformation(AdgContainer *container,
                                       AdgMatrix *transformation)
{
    AdgEntity *entity;
    AdgEntity *parent;
    const AdgMatrix *parent_matrix;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(transformation != NULL);

    entity = (AdgEntity *) container;
    parent = (AdgEntity *) g_childable_get_parent((GChildable *) entity);
    parent_matrix = parent ? adg_entity_get_model_matrix(parent) : NULL;

    adg_matrix_set(&container->priv->model_transformation, transformation);
    adg_entity_model_matrix_changed(entity, parent_matrix);
}

const AdgMatrix *
adg_container_get_paper_transformation(AdgContainer *container)
{
    g_return_val_if_fail(ADG_IS_CONTAINER(container), NULL);
    return &container->priv->paper_transformation;
}

void
adg_container_set_paper_transformation(AdgContainer *container,
                                       AdgMatrix *transformation)
{
    AdgEntity *entity;
    AdgEntity *parent;
    const AdgMatrix *parent_matrix;

    g_return_if_fail(ADG_IS_CONTAINER(container));
    g_return_if_fail(transformation != NULL);

    entity = (AdgEntity *) container;
    parent = (AdgEntity *) g_childable_get_parent((GChildable *) entity);
    parent_matrix = parent ? adg_entity_get_paper_matrix(parent) : NULL;

    adg_matrix_set(&container->priv->paper_transformation, transformation);
    adg_entity_paper_matrix_changed(entity, parent_matrix);
}
