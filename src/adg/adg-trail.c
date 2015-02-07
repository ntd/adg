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
 * SECTION:adg-trail
 * @short_description: A bare model built around cairo path
 *
 * The #AdgTrail model is a really basic model built around the #cairo_path_t
 * struct: for a full fledged path model consider using #AdgPath.
 *
 * A trail is a path model that demands all the implementation details to
 * the caller: this requires a deep knowledge of the ADG details but
 * provides a great customization level. It should be used when an
 * #AdgPath is not enough, such as when a model is subject to change
 * dynamically and the global and local maps do not suffice to express
 * this alteration. A typical example is the path used to draw extension
 * lines and base line of #AdgLDim: every point is subject to different
 * constrains not expressible with a single affine transformation.
 *
 * Since: 1.0
 **/

/**
 * AdgTrail:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/

/**
 * AdgTrailClass:
 * @get_cairo_path: virtual method to get the #cairo_path_t bound to the trail.
 *
 * The default @get_cairo_path calls the #AdgTrailCallback callback passed
 * to adg_trail_new() during construction. No caching is performed in
 * between.
 *
 * Since: 1.0
 **/

/**
 * AdgTrailCallback:
 * @trail: an #AdgTrail
 * @user_data: the general purpose pointer set by adg_trail_new()
 *
 * This is the callback used to generate the #cairo_path_t and it is
 * called directly by adg_trail_cairo_path(). The caller owns
 * the returned path, that is the finalization of the returned
 * #cairo_path_t should be made by the caller when appropriate.
 *
 * Returns: the #cairo_path_t of this trail model
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <math.h>

#include "adg-model.h"

#include "adg-trail.h"
#include "adg-trail-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_trail_parent_class)
#define _ADG_OLD_MODEL_CLASS   ((AdgModelClass *) adg_trail_parent_class)

#define EMPTY_PATH(p)          ((p) == NULL || (p)->data == NULL || (p)->num_data <= 0)

G_DEFINE_TYPE(AdgTrail, adg_trail, ADG_TYPE_MODEL)

enum {
    PROP_0,
    PROP_MAX_ANGLE
};


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
static GArray *         _adg_arc_to_curves      (GArray         *array,
                                                 const cairo_path_data_t *src,
                                                 gdouble         max_angle);


static void
adg_trail_class_init(AdgTrailClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTrailPrivate));

    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    model_class->clear = _adg_clear;

    klass->get_cairo_path = _adg_get_cairo_path;

    param = g_param_spec_double("max-angle",
                                P_("Max Angle"),
                                P_("Max arc angle to approximate with a single Bezier curve: check adg_trail_set_max_angle() for details"),
                                0, G_PI, G_PI_2,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MAX_ANGLE, param);
}

static void
adg_trail_init(AdgTrail *trail)
{
    AdgTrailPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(trail, ADG_TYPE_TRAIL,
                                                        AdgTrailPrivate);

    data->cairo_path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo_path.data = NULL;
    data->cairo_path.num_data = 0;
    data->callback = NULL;
    data->user_data = NULL;
    data->max_angle = G_PI_2;
    data->in_construction = FALSE;
    data->extents.is_defined = FALSE;

    trail->data = data;
}

static void
_adg_finalize(GObject *object)
{
    _adg_clear((AdgModel *) object);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgTrail *trail;
    AdgTrailPrivate *data;

    trail = (AdgTrail *) object;
    data = trail->data;

    switch (prop_id) {
    case PROP_MAX_ANGLE:
        g_value_set_double(value, data->max_angle);
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
    AdgTrail *trail;
    AdgTrailPrivate *data;

    trail = (AdgTrail *) object;
    data = trail->data;

    switch (prop_id) {
    case PROP_MAX_ANGLE:
        data->max_angle = g_value_get_double(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_trail_new:
 * @callback: (scope notified): the #cairo_path_t constructor function
 * @user_data: generic pointer to pass to the callback
 *
 * Creates a new trail model. The #cairo_path_t must be constructed by
 * the @callback function: #AdgTrail will not cache anything, so you
 * should implement any caching mechanism in the callback, if needed.
 *
 * Returns: (transfer full): a new trail model.
 *
 * Since: 1.0
 **/
AdgTrail *
adg_trail_new(AdgTrailCallback callback, gpointer user_data)
{
    AdgTrail *trail;
    AdgTrailPrivate *data;

    trail = g_object_new(ADG_TYPE_TRAIL, NULL);
    data = trail->data;

    data->callback = callback;
    data->user_data = user_data;

    return trail;
}

/**
 * adg_trail_get_cairo_path:
 * @trail: an #AdgTrail
 *
 * Gets a pointer to the cairo path of @trail. The returned path is
 * owned by @trail and must be considered read-only.
 *
 * This function gets the #cairo_path_t of @trail by calling
 * adg_trail_cairo_path() and converts its #CPML_ARC primitives,
 * not recognized by cairo, into approximated Bézier curves
 * primitives (#CPML_CURVE). The conversion is cached, so any further
 * request is O(1). This cache is cleared only by the
 * adg_model_clear() method.
 *
 * Returns: (transfer none): a pointer to the internal cairo path or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const cairo_path_t *
adg_trail_get_cairo_path(AdgTrail *trail)
{
    AdgTrailPrivate *data;
    cairo_path_t *cairo_path;
    GArray *dst;
    const cairo_path_data_t *p_src;
    int i;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    data = trail->data;

    /* Check for cached result */
    if (data->cairo_path.data != NULL)
        return &data->cairo_path;

    cairo_path = adg_trail_cairo_path(trail);
    if (EMPTY_PATH(cairo_path))
        return NULL;

    dst = g_array_sized_new(FALSE, FALSE,
                            sizeof(cairo_path_data_t), cairo_path->num_data);

    /* Cycle the cairo_path_t and convert arcs to Bézier curves */
    for (i = 0; i < cairo_path->num_data; i += p_src->header.length) {
        p_src = (const cairo_path_data_t *) cairo_path->data + i;

        if (p_src->header.type == CPML_ARC)
            dst = _adg_arc_to_curves(dst, p_src, data->max_angle);
        else
            dst = g_array_append_vals(dst, p_src, p_src->header.length);
    }

    cairo_path = &data->cairo_path;
    cairo_path->status = CAIRO_STATUS_SUCCESS;
    cairo_path->num_data = dst->len;
    cairo_path->data = (cairo_path_data_t *) g_array_free(dst, FALSE);

    return cairo_path;
}

/**
 * adg_trail_cairo_path:
 * @trail: an #AdgTrail
 *
 * Gets the cairo path structure defined by @trail. The returned
 * value is managed by the #AdgTrail implementation, that is this
 * function directly calls the <function>get_cairo_path</function>
 * virtual method that any trail instance must have.
 *
 * Whenever used internally by the ADG project, the returned path
 * is (by convention) owned by @trail and so it should not be freed.
 * Anyway, callers are allowed to modify it as long as its size is
 * retained and its data contains a valid path: this is needed to
 * let the #AdgMarker infrastructure work properly (the markers
 * should be able to modify the trail where they are applied).
 *
 * Any further call to this method will probably make the pointer
 * previously returned useless because the #cairo_path_t could be
 * relocated and the old #cairo_path_t will likely contain rubbish.
 *
 * Returns: (transfer none): a pointer to the #cairo_path_t or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
cairo_path_t *
adg_trail_cairo_path(AdgTrail *trail)
{
    AdgTrailClass *klass;
    AdgTrailPrivate *data;
    cairo_path_t *cairo_path;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    klass = ADG_TRAIL_GET_CLASS(trail);
    if (klass->get_cairo_path == NULL)
        return NULL;

    data = trail->data;
    if (data->in_construction) {
        g_warning(_("%s: you cannot access the path from the callback you provided to build it"),
                  G_STRLOC);
        return NULL;
    }

    data->in_construction = TRUE;
    cairo_path = klass->get_cairo_path(trail);
    data->in_construction = FALSE;

    return cairo_path;
}

/**
 * adg_trail_put_segment:
 * @trail: an #AdgTrail
 * @n_segment: the segment to retrieve, where 1 is the first segment
 * @segment: the destination #CpmlSegment
 *
 * Convenient function to get a segment from @trail. The segment is
 * got from the cairo path: check out adg_trail_cairo_path() for
 * further information.
 *
 * When the segment is not found, either because @n_segment is out
 * of range or because there is still no path bound to @trail, this
 * function will return <constant>FALSE</constant> leaving @segment
 * untouched. If the segment is found and @segment is
 * not <constant>NULL</constant>, the resulting segment is copied in @segment.
 *
 * Returns: <constant>TRUE</constant> on success or <constant>FALSE</constant> on errors.
 *
 * Since: 1.0
 **/
gboolean
adg_trail_put_segment(AdgTrail *trail, guint n_segment, CpmlSegment *segment)
{
    cairo_path_t *cairo_path;
    gboolean found;
    CpmlSegment iterator;
    guint cnt;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), FALSE);

    if (n_segment == 0) {
        g_warning(_("%s: requested undefined segment for type '%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(trail)));
        return FALSE;
    }

    cairo_path = adg_trail_cairo_path(trail);
    found = ! EMPTY_PATH(cairo_path) &&
        cpml_segment_from_cairo(&iterator, cairo_path);

    for (cnt = 1; found && cnt < n_segment; ++cnt)
        found = cpml_segment_next(&iterator);

    if (found && segment)
        cpml_segment_copy(segment, &iterator);

    if (!found)
        g_warning(_("%s: segment %u is out of range for type '%s'"),
                  G_STRLOC, n_segment, g_type_name(G_OBJECT_TYPE(trail)));

    return found;
}

/**
 * adg_trail_get_extents:
 * @trail: an #AdgTrail
 *
 * Gets the extents of @trail. The returned pointer is owned by
 * @trail and should not be freed nor modified.
 *
 * Returns: the requested extents or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const CpmlExtents *
adg_trail_get_extents(AdgTrail *trail)
{
    AdgTrailPrivate *data;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    data = trail->data;

    if (!data->extents.is_defined) {
        cairo_path_t *cairo_path;
        CpmlSegment segment;
        CpmlExtents extents;

        cairo_path = adg_trail_cairo_path(trail);
        if (! EMPTY_PATH(cairo_path) &&
            cpml_segment_from_cairo(&segment, cairo_path)) {
            do {
                cpml_segment_put_extents(&segment, &extents);
                cpml_extents_add(&data->extents, &extents);
            } while (cpml_segment_next(&segment));
        }
    }

    return &data->extents;
}

/**
 * adg_trail_dump:
 * @trail: an #AdgTrail
 *
 * Dumps the data content of @trail to stdout in a human readable format.
 *
 * Since: 1.0
 **/
void
adg_trail_dump(AdgTrail *trail)
{
    CpmlSegment segment;
    cairo_path_t *cairo_path;

    g_return_if_fail(ADG_IS_TRAIL(trail));

    cairo_path = (cairo_path_t *) adg_trail_get_cairo_path(trail);

    g_return_if_fail(cairo_path != NULL);

    if (!cpml_segment_from_cairo(&segment, cairo_path)) {
        g_warning(_("%s: invalid path data"), G_STRLOC);
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}

/**
 * adg_trail_set_max_angle:
 * @trail: an #AdgTrail
 * @angle: the new angle (in radians)
 *
 * Sets the max angle of @trail to @angle, basically setting
 * the #AdgTrail:max-angle property.
 *
 * This property is used to specify the maximum ciruclar arc
 * that will be approximated by a single Bézier curve in the
 * adg_trail_get_cairo_path() method. Basically this can be
 * used to fine tune the fitting algorithm: lower values mean
 * an arc will be approximated with more curves, lowering the
 * error but incrementing time and memory needed. The default
 * value of %G_PI_2 is usually good in most cases.
 *
 * Check the cairo-arc.c source file (part of the cairo project)
 * for mathematical details. A copy can probably be consulted
 * online at the cairo repository on freedesktop. Here is a link
 * to the 1.10.2 version:
 *
 * http://cgit.freedesktop.org/cairo/tree/src/cairo-arc.c?id=1.10.2
 *
 * Since: 1.0
 **/
void
adg_trail_set_max_angle(AdgTrail *trail, gdouble angle)
{
    g_return_if_fail(ADG_IS_TRAIL(trail));
    g_object_set(trail, "max-angle", angle, NULL);
}

/**
 * adg_trail_get_max_angle:
 * @trail: an #AdgTrail
 *
 * Gets the #AdgTrail:max-angle property value of @trail.
 * Refer to adg_trail_set_max_angle() for details of what
 * this parameter is used for.
 *
 * Returns: the value (in radians) of the max angle
 *
 * Since: 1.0
 **/
gdouble
adg_trail_get_max_angle(AdgTrail *trail)
{
    AdgTrailPrivate *data;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), 0);

    data = trail->data;
    return data->max_angle;
}


static void
_adg_clear(AdgModel *model)
{
    AdgTrailPrivate *data = ((AdgTrail *) model)->data;

    g_free(data->cairo_path.data);

    data->cairo_path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo_path.data = NULL;
    data->cairo_path.num_data = 0;
    data->extents.is_defined = FALSE;

    if (_ADG_OLD_MODEL_CLASS->clear)
        _ADG_OLD_MODEL_CLASS->clear(model);
}

static cairo_path_t *
_adg_get_cairo_path(AdgTrail *trail)
{
    AdgTrailPrivate *data = trail->data;

    if (data->callback == NULL) {
        g_warning(_("%s: callback not defined for instance of type '%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(trail)));
        return NULL;
    }

    return data->callback(trail, data->user_data);
}

static GArray *
_adg_arc_to_curves(GArray *array, const cairo_path_data_t *src,
                   gdouble max_angle)
{
    CpmlPrimitive arc;
    double start, end;

    /* Build the arc primitive: the arc origin is supposed to be the previous
     * point (src-1): this means a primitive must exist before the arc */
    arc.segment = NULL;
    arc.org = (cairo_path_data_t *) (src-1);
    arc.data = (cairo_path_data_t *) src;

    if (cpml_arc_info(&arc, NULL, NULL, &start, &end)) {
        CpmlSegment segment;
        int n_curves;
        cairo_path_data_t *curves;

        n_curves = ceil(fabs(end-start) / max_angle);
        curves = g_new(cairo_path_data_t, n_curves * 4);
        segment.data = curves;
        cpml_arc_to_curves(&arc, &segment, n_curves);

        array = g_array_append_vals(array, curves, n_curves * 4);

        g_free(curves);
    }

    return array;
}
