/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
 * edge lines can be easily computed.
 *
 * The trail can be set by changing the #AdgEdges:source property
 * or the relevant APIs. If the trail changes, a recomputation
 * can be forced by calling the adg_model_clear() method.
 *
 * The angle of the axis is implied to pass through the (0,0) point
 * and has an angle of #AdgEdges:axis-angle radiants. The default
 * is a 0 radiant angle, meaning the y=0 axis is assumed.
 *
 * Since: 1.0
 **/

/**
 * AdgEdges:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
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
    PROP_CRITICAL_ANGLE,
    PROP_AXIS_ANGLE
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
static cairo_path_t *   _adg_get_cairo_path     (AdgTrail       *trail);
static void             _adg_unset_source       (AdgEdges       *edges);
static void             _adg_clear_cairo_path   (AdgEdges       *edges);
static GSList *         _adg_get_vertices       (CpmlSegment    *segment,
                                                 gdouble         threshold);
static GSList *         _adg_optimize_vertices  (GSList         *vertices);
static GArray *         _adg_path_build         (const GSList   *vertices);
static void             _adg_path_transform     (GArray         *path_data,
                                                 const cairo_matrix_t*map);


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

    trail_class->get_cairo_path = _adg_get_cairo_path;

    param = g_param_spec_object("source",
                                P_("Source"),
                                P_("The source from which the edges should be computed from"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_SOURCE, param);

    param = g_param_spec_double("axis-angle",
                                P_("Axis Angle"),
                                P_("The angle of the axis of the source trail: it is implied this axis passes through (0,0)"),
                                -G_PI, G_PI, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_AXIS_ANGLE, param);

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
    data->critical_angle = G_PI / 45;
    data->axis_angle = 0;

    data->cairo.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo.array = NULL;

    edges->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgEdges *edges = (AdgEdges *) object;

    adg_edges_set_source(edges, NULL);

    if (_ADG_OLD_OBJECT_CLASS->dispose != NULL)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_finalize(GObject *object)
{
    _adg_clear_cairo_path((AdgEdges *) object);

    if (_ADG_OLD_OBJECT_CLASS->finalize != NULL)
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
    case PROP_AXIS_ANGLE:
        g_value_set_double(value, data->axis_angle);
        break;
    case PROP_CRITICAL_ANGLE:
        g_value_set_double(value, data->critical_angle);
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
    AdgEdges *edges;
    AdgEdgesPrivate *data;
    gpointer tmp_pointer;
    gdouble tmp_double;

    edges = (AdgEdges *) object;
    data = edges->data;

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
    case PROP_AXIS_ANGLE:
        tmp_double = g_value_get_double(value);
        if (data->axis_angle != tmp_double) {
            data->axis_angle = tmp_double;
            _adg_clear_cairo_path(edges);
        }
        break;
    case PROP_CRITICAL_ANGLE:
        tmp_double = g_value_get_double(value);
        if (data->critical_angle != tmp_double) {
            data->critical_angle = tmp_double;
            _adg_clear_cairo_path(edges);
        }
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
 *
 * Since: 1.0
 **/
AdgEdges *
adg_edges_new(void)
{
    return g_object_new(ADG_TYPE_EDGES, NULL);
}

/**
 * adg_edges_new_with_source:
 * @source: (transfer none): the new source #AdgTrail
 *
 * Creates a new edges model explicitely specifying the source trail.
 * The returned object will own a weak reference on @source.
 *
 * Returns: the newly created edges model
 *
 * Since: 1.0
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
 * The returned object is owned by @edges and should not be
 * freed or modified.
 *
 * Returns: (transfer none): the requested #AdgTrail or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
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
 * @source: (transfer none): the new source #AdgTrail
 *
 * Sets @source as the source trail for @edges.
 * After the call, @edges will own a weak reference on @source.
 *
 * Since: 1.0
 **/
void
adg_edges_set_source(AdgEdges *edges, AdgTrail *source)
{
    g_return_if_fail(ADG_IS_EDGES(edges));
    g_object_set(edges, "source", source, NULL);
}

/**
 * adg_edges_set_axis_angle:
 * @edges: an #AdgEdges
 * @angle: the new angle (in radians)
 *
 * Sets the axis angle of @edges to @angle, basically setting
 * the #AdgEdges:axis-angle property. All the resulting edge
 * lines will be normal to this axis.
 *
 * It is implied the axis will pass through the (0,0) point,
 * so the underlying trail should be constructed accordingly.
 *
 * Since: 1.0
 **/
void
adg_edges_set_axis_angle(AdgEdges *edges, gdouble angle)
{
    g_return_if_fail(ADG_IS_EDGES(edges));
    g_object_set(edges, "axis-angle", angle, NULL);
}

/**
 * adg_edges_get_axis_angle:
 * @edges: an #AdgEdges
 *
 * Gets the angle of the supposed axis of @edges. Refer to
 * adg_edges_set_axis_angle() for details of what this parameter
 * is used for.
 *
 * Returns: the value (in radians) of the axis angle
 *
 * Since: 1.0
 **/
gdouble
adg_edges_get_axis_angle(AdgEdges *edges)
{
    AdgEdgesPrivate *data;

    g_return_val_if_fail(ADG_IS_EDGES(edges), 0);

    data = edges->data;
    return data->axis_angle;
}

/**
 * adg_edges_set_critical_angle:
 * @edges: an #AdgEdges
 * @angle: the new angle (in radians)
 *
 * Sets the critical angle of @edges to @angle, basically setting
 * the #AdgEdges:critical-angle property.
 *
 * The critical angle defines what corner should generate an edge and
 * what not. Typical values are close to 0, being 0 the lowest angle
 * where every corner generates an edge.
 *
 * Since: 1.0
 **/
void
adg_edges_set_critical_angle(AdgEdges *edges, gdouble angle)
{
    g_return_if_fail(ADG_IS_EDGES(edges));
    g_object_set(edges, "critical-angle", angle, NULL);
}

/**
 * adg_edges_get_critical_angle:
 * @edges: an #AdgEdges
 *
 * Gets the current critical angle of @edges. Refer to
 * adg_edges_set_critical_angle() for details of what this parameter
 * is used for.
 *
 * Returns: the value (in radians) of the critical angle
 *
 * Since: 1.0
 **/
gdouble
adg_edges_get_critical_angle(AdgEdges *edges)
{
    AdgEdgesPrivate *data;

    g_return_val_if_fail(ADG_IS_EDGES(edges), 0);

    data = edges->data;
    return data->critical_angle;
}


static void
_adg_clear(AdgModel *model)
{
    _adg_clear_cairo_path((AdgEdges *) model);

    if (_ADG_OLD_MODEL_CLASS->clear != NULL)
        _ADG_OLD_MODEL_CLASS->clear(model);
}

static cairo_path_t *
_adg_get_cairo_path(AdgTrail *trail)
{
    AdgEdges *edges;
    AdgEdgesPrivate *data;
    gdouble threshold;
    CpmlSegment segment;
    GSList *vertices;
    cairo_matrix_t map;

    edges = (AdgEdges *) trail;
    data = edges->data;

    /* Check for cached path */
    if (data->cairo.path.status == CAIRO_STATUS_SUCCESS)
        return &data->cairo.path;

    _adg_clear_cairo_path((AdgEdges *) trail);

    if (data->source != NULL) {
        adg_trail_put_segment(data->source, 1, &segment);

        /* The threshold is squared because the _adg_get_vertices()
         * function uses cpml_pair_squared_distance() against the
         * two vectors of every corner to avoid sqrt()ing everything */
        threshold = sin(data->critical_angle);
        threshold *= threshold * 2;

        vertices = _adg_get_vertices(&segment, threshold);

        /* Rotate all the vertices so the axis will always be on y=0:
         * this is mainly needed to not complicate the _adg_path_build()
         * code which assumes the y=0 axis is in effect */
        cairo_matrix_init_rotate(&map, -data->axis_angle);
        g_slist_foreach(vertices, (GFunc) cpml_pair_transform, &map);

        vertices = _adg_optimize_vertices(vertices);
        data->cairo.array = _adg_path_build(vertices);

        g_slist_foreach(vertices, (GFunc) g_free, NULL);
        g_slist_free(vertices);

        /* Reapply the inverse of the previous transformation to
         * move the vertices to their original positions */
        cairo_matrix_invert(&map);
        _adg_path_transform(data->cairo.array, &map);

        data->cairo.path.status = CAIRO_STATUS_SUCCESS;
        data->cairo.path.data = (cairo_path_data_t *) (data->cairo.array)->data;
        data->cairo.path.num_data = (data->cairo.array)->len;
    }

    return &data->cairo.path;
}

static void
_adg_unset_source(AdgEdges *edges)
{
    g_object_set(edges, "source", NULL, NULL);
}

static void
_adg_clear_cairo_path(AdgEdges *edges)
{
    AdgEdgesPrivate *data = edges->data;

    if (data->cairo.array != NULL) {
        g_array_free(data->cairo.array, TRUE);
        data->cairo.array = NULL;
    }

    data->cairo.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo.path.data = NULL;
    data->cairo.path.num_data = 0;
}

/**
 * _adg_get_vertices:
 * @segment: a #CpmlSegment
 * @threshold: a theshold value
 *
 * Collects a list of #CpmlPair corners where the angle has a minimum
 * threshold incidence of @threshold. The threshold is considered as
 * the squared distance between the two unit vectors, the one before
 * and the one after every corner.
 *
 * Returns: a #GSList of #CpmlPair: the list should be freed with
 *          g_slist_free() and all the pairs freed with g_free()
 *          when no longer needed
 *
 * Since: 1.0
 **/
static GSList *
_adg_get_vertices(CpmlSegment *segment, gdouble threshold)
{
    GSList *vertices;
    CpmlPrimitive primitive;
    CpmlVector old, new;
    CpmlPair pair;

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
                cpml_primitive_put_pair_at(&primitive, 0, &pair);
                vertices = g_slist_prepend(vertices, cpml_pair_dup(&pair));
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
    CpmlPair *pair, *old_pair;

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
_adg_path_build(const GSList *vertices)
{
    cairo_path_data_t line[4];
    GArray *array;
    const GSList *vertex, *vertex2;
    const CpmlPair *pair, *pair2;

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

static void
_adg_path_transform(GArray *path_data, const cairo_matrix_t *map)
{
    guint n;
    cairo_path_data_t *data;

    /* Only the odd items are transformed: the even ones are either
     * header items, CPML_MOVE and CPML_LINE alternatively */
    for (n = 1; n < path_data->len; n += 2) {
        data = &g_array_index(path_data, cairo_path_data_t, n);
        cairo_matrix_transform_point(map, &data->point.x, &data->point.y);
    }
}
