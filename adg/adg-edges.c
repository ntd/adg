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
 * SECTION:adg-edges
 * @short_description: A model with the edges of another model
 *
 * The #AdgEdges can be used to render the edges of a yet existing
 * #AdgTrail source. It is useful for any part made by revolution,
 * where the shape is symmetric along a specific axis and thus the
 * corners can be easily computed.
 **/

/**
 * AdgEdges:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-edges.h"
#include "adg-edges-private.h"
#include "adg-pair.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_edges_parent_class)
#define PARENT_MODEL_CLASS   ((AdgModelClass *) adg_edges_parent_class)


enum {
    PROP_0,
    PROP_SOURCE
};


static void             dispose                 (GObject        *object);
static void             finalize                (GObject        *object);
static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             clear                   (AdgModel       *model);
static CpmlPath *       get_cpml_path           (AdgTrail       *trail);
static gboolean         set_source              (AdgEdges       *edges,
                                                 AdgTrail       *source);
static void             unset_source            (AdgEdges       *edges);
static void             clear_cpml_path         (AdgEdges       *edges);
static GSList *         get_vertices            (CpmlSegment    *segment,
                                                 gdouble         threshold);
static GArray *         build_array             (const GSList   *vertices);


G_DEFINE_TYPE(AdgEdges, adg_edges, ADG_TYPE_TRAIL);


static void
adg_edges_class_init(AdgEdgesClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;
    AdgTrailClass *trail_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;
    trail_class = (AdgTrailClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgEdgesPrivate));

    gobject_class->dispose = dispose;
    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    model_class->clear = clear;

    trail_class->get_cpml_path = get_cpml_path;

    param = g_param_spec_object("source",
                                P_("Source"),
                                P_("The source where the edges should be computed from"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_SOURCE, param);
}

static void
adg_edges_init(AdgEdges *edges)
{
    AdgEdgesPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(edges, ADG_TYPE_EDGES,
                                                        AdgEdgesPrivate);

    data->source = NULL;
    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.array = NULL;

    edges->data = data;
}

static void
dispose(GObject *object)
{
    AdgEdges *edges = (AdgEdges *) object;

    adg_edges_set_source(edges, NULL);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
finalize(GObject *object)
{
    clear_cpml_path((AdgEdges *) object);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgEdgesPrivate *data = ((AdgEdges *) object)->data;

    switch (prop_id) {
    case PROP_SOURCE:
        g_value_set_object(value, &data->source);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgEdges *edges = (AdgEdges *) object;

    switch (prop_id) {
    case PROP_SOURCE:
        set_source(edges, g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_edges_new:
 *
 * Creates a new undefined model to keep track of the edges of
 * another model. You should at least set the referred #AdgTrail
 * with adg_edges_set_source().
 *
 * Returns: the newly created edges model
 **/
AdgEdges *
adg_edges_new(void)
{
    return g_object_new(ADG_TYPE_EDGES, NULL);
}

/**
 * adg_edges_new_with_source:
 *
 * Creates a new edges model explicitely specifying the source trail.
 *
 * Returns: the newly created edges model
 **/
AdgEdges *
adg_edges_new_with_source(AdgTrail *source)
{
    return g_object_new(ADG_TYPE_EDGES, "source", source, NULL);
}

/**
 * adg_edges_get_source:
 * @edges: an #AdgEdges
 *
 * Gets the source #AdgTrail of this @edges model.
 *
 * Returns: the requested #AdgTrail or %NULL on errors
 **/
AdgTrail *
adg_edges_get_source(AdgEdges *edges)
{
    AdgEdgesPrivate *data;

    g_return_val_if_fail(ADG_IS_EDGES(edges), NULL);

    data = edges->data;

    return data->source;
}

/**
 * adg_edges_set_source:
 * @edges: an #AdgEdges
 * @source: the new source #AdgTrail
 *
 * Sets @source as the source trail for @edges.
 **/
void
adg_edges_set_source(AdgEdges *edges, AdgTrail *source)
{
    g_return_if_fail(ADG_IS_EDGES(edges));

    if (set_source(edges, source))
        g_object_notify((GObject *) edges, "source");
}

static void
clear(AdgModel *model)
{
    clear_cpml_path((AdgEdges *) model);

    if (PARENT_MODEL_CLASS->clear != NULL)
        PARENT_MODEL_CLASS->clear(model);
}

static CpmlPath *
get_cpml_path(AdgTrail *trail)
{
    AdgEdges *edges;
    AdgEdgesPrivate *data;

    edges = (AdgEdges *) trail;
    data = edges->data;

    /* Check for cached path */
    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS)
        return &data->cpml.path;

    clear_cpml_path((AdgEdges *) trail);

    if (data->source != NULL) {
        CpmlSegment segment;
        GSList *vertices;

        adg_trail_get_segment(data->source, &segment, 1);
        vertices = get_vertices(&segment, 0.01);
        data->cpml.array = build_array(vertices);

        g_slist_foreach(vertices, (GFunc) g_free, NULL);
        g_slist_free(vertices);

        data->cpml.path.status = CAIRO_STATUS_SUCCESS;
        data->cpml.path.data = (cairo_path_data_t *) (data->cpml.array)->data;
        data->cpml.path.num_data = (data->cpml.array)->len;
    }

    return &data->cpml.path;
}

static gboolean
set_source(AdgEdges *edges, AdgTrail *source)
{
    AdgEntity *entity;
    AdgEdgesPrivate *data;

    entity = (AdgEntity *) edges;
    data = edges->data;

    if (source == data->source)
        return FALSE;

    if (data->source != NULL)
        g_object_weak_unref((GObject *) data->source,
                            (GWeakNotify) unset_source, edges);

    data->source = source;
    clear((AdgModel *) edges);

    if (data->source != NULL)
        g_object_weak_ref((GObject *) data->source,
                          (GWeakNotify) unset_source, edges);

    return TRUE;
}

static void
unset_source(AdgEdges *edges)
{
    AdgEdgesPrivate *data = edges->data;

    if (data->source != NULL)
        set_source(edges, NULL);
}

static void
clear_cpml_path(AdgEdges *edges)
{
    AdgEdgesPrivate *data = edges->data;

    if (data->cpml.array != NULL) {
        g_array_free(data->cpml.array, TRUE);
        data->cpml.array = NULL;
    }

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.path.data = NULL;
    data->cpml.path.num_data = 0;
}

static GSList *
get_vertices(CpmlSegment *segment, gdouble threshold)
{
    GSList *vertices;
    CpmlPrimitive primitive;
    CpmlVector old, new;

    vertices = NULL;
    cpml_primitive_from_segment(&primitive, segment);
    old.x = old.y = 0;

    do {
        /* The first vector and the undefined ones
         * must always be skipped */
        if (old.x != 0 || old.y != 0) {
            cpml_primitive_vector_at(&primitive, &new, 0);
            cpml_vector_set_length(&new, 1);

            if (cpml_pair_squared_distance(&old, &new) > threshold) {
                AdgPair pair;

                cpml_primitive_pair_at(&primitive, &pair, 0);
                vertices = g_slist_prepend(vertices, adg_pair_dup(&pair));
            }
        }

        cpml_primitive_vector_at(&primitive, &old, 1);
        cpml_vector_set_length(&old, 1);
    } while (cpml_primitive_next(&primitive));

    return g_slist_reverse(vertices);
}

static GArray *
build_array(const GSList *vertices)
{
    GArray *array;

    array = g_array_new(FALSE, FALSE, sizeof(cairo_path_data_t));

    return array;
}
