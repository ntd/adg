/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>Actually the edges of the source trail are always computed
 *           taking the y=0 axis as the origin: anyway, it would be not
 *           too hard to apply an arbitrary transformation to aling the
 *           trail on the y=0 axis, compute the edges as usual and apply
 *           the inverse transformation to the result.</listitem>
 * </itemizedlist>
 * </important>
 **/

/**
 * AdgEdges:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-trail.h"
#include <math.h>

#include "adg-edges.h"
#include "adg-edges-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_edges_parent_class)
#define _ADG_OLD_MODEL_CLASS   ((AdgModelClass *) adg_edges_parent_class)


G_DEFINE_TYPE(AdgEdges, adg_edges, ADG_TYPE_TRAIL)

enum {
    PROP_0,
    PROP_SOURCE,
    PROP_CRITICAL_ANGLE
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_finalize           (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_clear              (AdgModel       *model);
static CpmlPath *       _adg_get_cpml_path      (AdgTrail       *trail);
static void             _adg_unset_source       (AdgEdges       *edges);
static void             _adg_clear_cpml_path    (AdgEdges       *edges);
static GSList *         _adg_get_vertices       (CpmlSegment    *segment,
                                                 gdouble         threshold);
static GSList *         _adg_optimize_vertices  (GSList         *vertices);
static GArray *         _adg_build_array        (const GSList   *vertices);


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

    gobject_class->dispose = _adg_dispose;
    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    model_class->clear = _adg_clear;

    trail_class->get_cpml_path = _adg_get_cpml_path;

    param = g_param_spec_object("source",
                                P_("Source"),
                                P_("The source where the edges should be computed from"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_SOURCE, param);

    param = g_param_spec_double("critical-angle",
                                P_("Critical Angle"),
                                P_("The angle that defines which corner generates an edge (if the corner angle is greater than this critical angle) and which edge is ignored"),
                                0, G_PI, G_PI / 45,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CRITICAL_ANGLE, param);
}

static void
adg_edges_init(AdgEdges *edges)
{
    AdgEdgesPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(edges, ADG_TYPE_EDGES,
                                                        AdgEdgesPrivate);

    data->source = NULL;
    data->threshold = sin(G_PI / 45);
    data->threshold *= data->threshold * 2;

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.array = NULL;

    edges->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgEdges *edges = (AdgEdges *) object;

    adg_edges_set_source(edges, NULL);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_finalize(GObject *object)
{
    _adg_clear_cpml_path((AdgEdges *) object);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgEdges *edges;
    AdgEdgesPrivate *data;

    edges = (AdgEdges *) object;
    data = edges->data;

    switch (prop_id) {
    case PROP_SOURCE:
        g_value_set_object(value, data->source);
        break;
    case PROP_CRITICAL_ANGLE:
        g_value_set_double(value, adg_edges_get_critical_angle(edges));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgEdgesPrivate *data = ((AdgEdges *) object)->data;
    gpointer tmp_pointer;

    switch (prop_id) {
    case PROP_SOURCE:
        tmp_pointer = data->source;
        data->source = g_value_get_object(value);

        if (tmp_pointer != data->source) {
            if (data->source)
                g_object_weak_ref((GObject *) data->source,
                                  (GWeakNotify) _adg_unset_source, object);
            if (tmp_pointer)
                g_object_weak_unref((GObject *) tmp_pointer,
                                    (GWeakNotify) _adg_unset_source, object);
        }

        _adg_clear((AdgModel *) object);
        break;
    case PROP_CRITICAL_ANGLE:
        data->threshold = sin(g_value_get_double(value));
        data->threshold *= data->threshold * 2;
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
    g_object_set(edges, "source", source, NULL);
}

/**
 * adg_edges_get_critical_angle:
 * @edges: an #AdgEdges
 *
 * Gets the current critical angle of @edges. The angle is internally
 * converted to a threshold value, so the returned angle could be not
 * exactly what set throught adg_edges_set_critical_angle().
 *
 * Returns: the value (in radians) of the critical angle
 **/
gdouble
adg_edges_get_critical_angle(AdgEdges *edges)
{
    AdgEdgesPrivate *data;

    g_return_val_if_fail(ADG_IS_EDGES(edges), 0);

    data = edges->data;

    return asin(sqrt(data->threshold / 2));
}

/**
 * adg_edges_set_critical_angle:
 * @edges: an #AdgEdges
 * @angle: the new angle (in radians)
 *
 * Sets a new critical angle on @edges. The critical angle defines
 * what corner should generate an edge and what not. Typical values
 * are close to %0, being %0 the lowest angle where all the corners
 * generate an edge.
 **/
void
adg_edges_set_critical_angle(AdgEdges *edges, gdouble angle)
{
    g_return_if_fail(ADG_IS_EDGES(edges));
    g_object_set(edges, "critical-angle", angle, NULL);
}


static void
_adg_clear(AdgModel *model)
{
    _adg_clear_cpml_path((AdgEdges *) model);

    if (_ADG_OLD_MODEL_CLASS->clear)
        _ADG_OLD_MODEL_CLASS->clear(model);
}

static CpmlPath *
_adg_get_cpml_path(AdgTrail *trail)
{
    AdgEdges *edges;
    AdgEdgesPrivate *data;

    edges = (AdgEdges *) trail;
    data = edges->data;

    /* Check for cached path */
    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS)
        return &data->cpml.path;

    _adg_clear_cpml_path((AdgEdges *) trail);

    if (data->source != NULL) {
        CpmlSegment segment;
        GSList *vertices;

        adg_trail_put_segment(data->source, 1, &segment);
        vertices = _adg_get_vertices(&segment, 0.01);
        vertices = _adg_optimize_vertices(vertices);
        data->cpml.array = _adg_build_array(vertices);

        g_slist_foreach(vertices, (GFunc) g_free, NULL);
        g_slist_free(vertices);

        data->cpml.path.status = CAIRO_STATUS_SUCCESS;
        data->cpml.path.data = (cairo_path_data_t *) (data->cpml.array)->data;
        data->cpml.path.num_data = (data->cpml.array)->len;
    }

    return &data->cpml.path;
}

static void
_adg_unset_source(AdgEdges *edges)
{
    g_object_set(edges, "source", NULL, NULL);
}

static void
_adg_clear_cpml_path(AdgEdges *edges)
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
_adg_get_vertices(CpmlSegment *segment, gdouble threshold)
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
            cpml_vector_set_length(&old, 1);
            cpml_primitive_put_vector_at(&primitive, 0, &new);
            cpml_vector_set_length(&new, 1);

            /* Vertical vectors are always added, as they represent
             * a vertical side and could be filleted, thus skipping
             * the edge detection */
            if (new.x == 0 ||
                cpml_pair_squared_distance(&old, &new) > threshold) {
                AdgPair pair;

                cpml_primitive_put_pair_at(&primitive, 0, &pair);
                vertices = g_slist_prepend(vertices, adg_pair_dup(&pair));
            }
        }

        cpml_primitive_put_vector_at(&primitive, 1, &old);
    } while (cpml_primitive_next(&primitive));

    return g_slist_reverse(vertices);
}

/* Removes adjacent vertices lying on the same edge */
static GSList *
_adg_optimize_vertices(GSList *vertices)
{
    GSList *vertex, *old_vertex;
    AdgPair *pair, *old_pair;

    /* Check for empty list */
    if (vertices == NULL)
        return vertices;

    old_vertex = vertices;

    while ((vertex = old_vertex->next) != NULL) {
        pair = vertex->data;
        old_pair = old_vertex->data;

        if (pair->x != old_pair->x) {
            old_vertex = vertex;
            continue;
        }

        if (old_pair->y < pair->y) {
            /* Preserve the old vertex and remove the current one */
            g_free(pair);
            vertices = g_slist_delete_link(vertices, vertex);
        } else {
            /* Preserve the current vertex and remove the old one */
            g_free(old_pair);
            vertices = g_slist_delete_link(vertices, old_vertex);
            old_vertex = vertex;
        }
    }

    return vertices;
}

static GArray *
_adg_build_array(const GSList *vertices)
{
    cairo_path_data_t line[4];
    GArray *array;
    const GSList *vertex, *vertex2;
    const AdgPair *pair, *pair2;

    line[0].header.type = CPML_MOVE;
    line[0].header.length = 2;
    line[2].header.type = CPML_LINE;
    line[2].header.length = 2;

    array = g_array_new(FALSE, FALSE, sizeof(cairo_path_data_t));
    vertex = vertices;

    while (vertex != NULL) {
        pair = vertex->data;
        vertex = vertex->next;
        vertex2 = vertex;

        while (vertex2 != NULL) {
            pair2 = vertex2->data;

            if (pair->x == pair2->x) {
                /* Opposite vertex found: append a line in the path
                 * and quit from this loop */
                cpml_pair_to_cairo(pair, &line[1]);
                cpml_pair_to_cairo(pair2, &line[3]);
                array = g_array_append_vals(array, line, G_N_ELEMENTS(line));
                break;
            }

            vertex2 = vertex2->next;
        }
    }

    return array;
}
