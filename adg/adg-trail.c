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
 * SECTION:adg-trail
 * @short_description: A bare model built around #CpmlPath
 *
 * The #AdgTrail model is a really basic model built around the #CpmlPath
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
 **/

/**
 * AdgTrail:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

/**
 * AdgTrailCallback:
 * @trail: an #AdgTrail
 * @user_data: the general purpose pointer set by adg_trail_new()
 *
 * This is the callback used to generate the #CpmlPath and it is
 * called directly by adg_trail_get_cpml_path(). The caller owns
 * the returned path, that is the finalization of the returned
 * #CpmlPath should be made by the caller when appropriate.
 *
 * Returns: the #CpmlPath of this trail model
 **/


#include "adg-trail.h"
#include "adg-trail-private.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_trail_parent_class)


static void             finalize                (GObject        *object);
static CpmlPath *       get_cpml_path           (AdgTrail       *trail);
static GArray *         arc_to_curves           (GArray         *array,
                                                 const cairo_path_data_t
                                                                *src);


G_DEFINE_TYPE(AdgTrail, adg_trail, ADG_TYPE_MODEL);


static void
adg_trail_class_init(AdgTrailClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTrailPrivate));

    gobject_class->finalize = finalize;

    klass->get_cpml_path = get_cpml_path;
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

    trail->data = data;
}

static void
finalize(GObject *object)
{
    AdgTrail *trail;
    AdgTrailPrivate *data;

    trail = (AdgTrail *) object;
    data = trail->data;

    adg_trail_invalidate(trail);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
}


/**
 * adg_trail_new:
 * @callback: the #CpmlPath constructor function
 * @user_data: generic pointer to pass to the callback
 *
 * Creates a new trail model. The #CpmlPath must be constructed by the
 * @callback function: #AdgTrail will not cache anything, so you should
 * implement any caching mechanism in the callback, if needed.
 *
 * Returns: a new trail model
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
 * Gets a pointer to the cairo path of @trail. The return path is
 * owned by @trail and must be considered read-only.
 *
 * This function gets the #CpmlPath of @trail by calling
 * adg_trail_get_cpml_path() and converts its %CAIRO_PATH_ARC_TO
 * primitives, not recognized by cairo, into approximated Bézier
 * curves primitives (%CAIRO_PATH_CURVE_TO). The conversion is
 * cached, so any furter request is O(1). This cache is cleared
 * only by the adg_trail_invalidate() method.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>Actually, the arcs are approximated to Bézier using the
 *           hardcoded max angle of PI/2. This should be customizable
 *           by adding, for instance, a property to the #AdgTrail class
 *           with a default value of PI/2.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: a pointer to the internal cairo path or %NULL on errors
 **/
const cairo_path_t *
adg_trail_get_cairo_path(AdgTrail *trail)
{
    AdgTrailPrivate *data;
    cairo_path_t *cairo_path;
    CpmlPath *src;
    GArray *dst;
    const cairo_path_data_t *p_src;
    int i;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    data = trail->data;
    cairo_path = &data->cairo_path;

    /* Check for cached result */
    if (cairo_path->data != NULL)
        return cairo_path;

    src = adg_trail_get_cpml_path(trail);
    dst = g_array_sized_new(FALSE, FALSE,
                            sizeof(cairo_path_data_t), src->num_data);

    /* Cycle the CpmlPath and convert arcs to Bézier curves */
    for (i = 0; i < src->num_data; i += p_src->header.length) {
        p_src = (const cairo_path_data_t *) src->data + i;

        if (p_src->header.type == CAIRO_PATH_ARC_TO)
            dst = arc_to_curves(dst, p_src);
        else
            dst = g_array_append_vals(dst, p_src, p_src->header.length);
    }

    cairo_path->status = CAIRO_STATUS_SUCCESS;
    cairo_path->num_data = dst->len;
    cairo_path->data = (cairo_path_data_t *) g_array_free(dst, FALSE);

    return cairo_path;
}

/**
 * adg_trail_get_cpml_path:
 * @trail: an #AdgTrail
 *
 * Gets the CPML path structure defined by @trail. The returned
 * value is managed by the #AdgTrail implementation, that is this
 * method simply calls the #AdgTrailClass::get_cpml_path() virtual
 * function that any trail instance must implement.
 *
 * Whenever used internally by the ADG project, the returned path
 * is (by convention) owned by @trail and so it should not be freed.
 * Anyway, it is allowed to modify it as long as its size is
 * retained and its data contains a valid path: this is needed to
 * let the #AdgMarker infrastructure work properly (the markers
 * should be able to modify the trail where they are applied).
 *
 * Any further call to this method will probably make the pointer
 * previously returned useless because the #CpmlPath could be
 * relocated and the old #CpmlPath will likely contain rubbish.
 *
 * Returns: a pointer to the #CpmlPath or %NULL on errors
 **/
CpmlPath *
adg_trail_get_cpml_path(AdgTrail *trail)
{
    g_return_val_if_fail(ADG_IS_TRAIL(trail), NULL);

    return ADG_TRAIL_GET_CLASS(trail)->get_cpml_path(trail);
}

/**
 * adg_trail_get_segment:
 * @trail: an #AdgTrail
 * @segment: the destination #AdgSegment
 * @n: the segment number to retrieve, where %1 is the first segment
 *
 * Convenient function to get a segment from @trail. The segment is
 * got from the CPML path: check out adg_trail_get_cpml_path() for
 * further information.
 *
 * Returns: %TRUE on success or %FALSE on errors
 **/
gboolean
adg_trail_get_segment(AdgTrail *trail, AdgSegment *segment, guint n)
{
    CpmlPath *cpml_path;
    guint cnt;

    g_return_val_if_fail(ADG_IS_TRAIL(trail), FALSE);
    g_return_val_if_fail(segment != NULL, FALSE);

    if (n == 0) {
        g_warning("%s: requested undefined segment for type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(trail)));
        return FALSE;
    }

    cpml_path = adg_trail_get_cpml_path(trail);

    cpml_segment_from_cairo(segment, cpml_path);
    for (cnt = 1; cnt < n; ++cnt)
        if (!cpml_segment_next(segment)) {
            g_warning("%s: segment `%u' out of range for type `%s'",
                      G_STRLOC, n, g_type_name(G_OBJECT_TYPE(trail)));
            return FALSE;
        }

    return TRUE;
}

/**
 * adg_trail_invalidate:
 * @trail: an #AdgTrail
 *
 * Clears the internal cairo path of @trail so the next call to
 * adg_trail_get_cairo_path() will recompute cached #cairo_path_t.
 * Check its documentation for details.
 *
 * This must be called by the #AdgTrail implementation whenever
 * the original #CpmlPath changes.
 **/
void
adg_trail_invalidate(AdgTrail *trail)
{
    AdgTrailPrivate *data;
    cairo_path_t *cairo_path;

    g_return_if_fail(ADG_IS_TRAIL(trail));

    data = trail->data;
    cairo_path = &data->cairo_path;

    if (cairo_path->data == NULL)
        return;

    g_free(cairo_path->data);

    cairo_path->status = CAIRO_STATUS_INVALID_PATH_DATA;
    cairo_path->data = NULL;
    cairo_path->num_data = 0;
}

/**
 * adg_trail_dump:
 * @trail: an #AdgTrail
 *
 * Dumps the data content of @trail to stdout in a human readable format.
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
        g_warning("Invalid path data to dump!\n");
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}


static CpmlPath *
get_cpml_path(AdgTrail *trail)
{
    AdgTrailPrivate *data = trail->data;

    if (data->callback == NULL) {
        g_warning("%s: callback not defined for instance of type `%s'",
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(trail)));
        return NULL;
    }

    return data->callback(trail, data->user_data);
}

static GArray *
arc_to_curves(GArray *array, const cairo_path_data_t *src)
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

        n_curves = ceil(fabs(end-start) / M_PI_2);
        curves = g_new(cairo_path_data_t, n_curves * 4);
        segment.data = curves;
        cpml_arc_to_curves(&arc, &segment, n_curves);

        array = g_array_append_vals(array, curves, n_curves * 4);

        g_free(curves);
    }

    return array;
}
