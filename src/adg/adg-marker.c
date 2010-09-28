/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-marker
 * @short_description: Base entity for markers
 *
 * A marker is an entity to be applied at the start or end of a segment.
 * Typical examples include arrows, ticks, dots and so on.
 *
 * The #AdgMarker:trail and #AdgMarker:n-segment properties specify the
 * segment where the marker should be applied. Similarly to the
 * #AdgStroke type, if the associated trail is destroyed the above
 * properties are unset.
 *
 * The local map is used internally to align the marker to the trail end,
 * so adg_entity_set_local_map() and friends is reserved. Therefore, if
 * the trail is modified and the marker had no way to know it, you should
 * call adg_entity_local_changed() to update the marker position.
 *
 * Use adg_marker_set_pos() to select the position where the marker
 * should be put: %0 means the start point of the segment while %1
 * means the end point.
 *
 * The #AdgMarker:model property and APIs are intended only for marker
 * implementation purposes.
 **/

/**
 * AdgMarker:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-marker.h"
#include "adg-marker-private.h"
#include <string.h>

#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_marker_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_marker_parent_class)


G_DEFINE_ABSTRACT_TYPE(AdgMarker, adg_marker, ADG_TYPE_ENTITY);

enum {
    PROP_0,
    PROP_TRAIL,
    PROP_N_SEGMENT,
    PROP_POS,
    PROP_SIZE,
    PROP_MODEL
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_invalidate         (AdgEntity      *entity);
static void             _adg_clear_trail        (AdgMarker      *marker);
static gboolean         _adg_set_segment        (AdgMarker      *marker,
                                                 AdgTrail       *trail,
                                                 guint           n_segment);
static AdgModel *       _adg_create_model       (AdgMarker      *marker);


static void
adg_marker_class_init(AdgMarkerClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgMarkerPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->set_property = _adg_set_property;
    gobject_class->get_property = _adg_get_property;

    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;

    klass->create_model = _adg_create_model;

    param = g_param_spec_object("trail",
                                P_("Trail"),
                                P_("The subject AdgTrail for this marker"),
                                ADG_TYPE_TRAIL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_TRAIL, param);

    param = g_param_spec_uint("n-segment",
                              P_("Segment Index"),
                              P_("The segment on trail where this marker should be applied (where 0 means undefined segment, 1 the first segment and so on)"),
                              0, G_MAXUINT, 0,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_N_SEGMENT, param);

    param = g_param_spec_double("pos",
                                P_("Position"),
                                P_("The position ratio inside the segment where to put the marker (0 means the start point while 1 means the end point)"),
                                0, 1, 0,
                                G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_POS, param);

    param = g_param_spec_double("size",
                                P_("Marker Size"),
                                P_("The size (in global space) of the marker"),
                                0, G_MAXDOUBLE, 10,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = g_param_spec_object("model",
                                P_("Model"),
                                P_("A general purpose model usable by the marker implementations"),
                                ADG_TYPE_MODEL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MODEL, param);
}

static void
adg_marker_init(AdgMarker *marker)
{
    AdgMarkerPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(marker,
                                                         ADG_TYPE_MARKER,
                                                         AdgMarkerPrivate);
    data->trail = NULL;
    data->n_segment = 0;
    data->backup_segment = NULL;
    memset(&data->segment, 0, sizeof(data->segment));
    data->pos = 0;
    data->size = 10;
    data->model = NULL;

    marker->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgMarker *marker = (AdgMarker *) object;

    adg_marker_set_model(marker, NULL);
    adg_marker_set_segment(marker, NULL, 0);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}


static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgMarkerPrivate *data = ((AdgMarker *) object)->data;

    switch (prop_id) {
    case PROP_TRAIL:
        g_value_set_object(value, data->trail);
        break;
    case PROP_N_SEGMENT:
        g_value_set_uint(value, data->n_segment);
        break;
    case PROP_POS:
        g_value_set_double(value, data->pos);
        break;
    case PROP_SIZE:
        g_value_set_double(value, data->size);
        break;
    case PROP_MODEL:
        g_value_set_object(value, data->model);
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
    AdgMarker *marker = (AdgMarker *) object;
    AdgMarkerPrivate *data = ((AdgMarker *) object)->data;
    AdgModel *old_model;

    switch (prop_id) {
    case PROP_TRAIL:
        _adg_set_segment(marker, g_value_get_object(value), data->n_segment);
        break;
    case PROP_N_SEGMENT:
        _adg_set_segment(marker, data->trail, g_value_get_uint(value));
        break;
    case PROP_POS:
        data->pos = g_value_get_double(value);
        break;
    case PROP_SIZE:
        data->size = g_value_get_double(value);
        break;
    case PROP_MODEL:
        old_model = data->model;
        data->model = g_value_get_object(value);

        if (data->model) {
            g_object_ref((GObject *) data->model);
            adg_entity_local_changed((AdgEntity *) object);
        }
        if (old_model)
            g_object_unref(old_model);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_marker_set_trail:
 * @marker: an #AdgMarker
 * @trail: the new trail to use
 *
 * Sets the #AdgMarker:trail property to @trail. It is allowed to pass
 * %NULL to clear the current trail.
 *
 * This method could fail unexpectedly if the segment index specified
 * by the #AdgMarker:n-segment property is not present inside the new
 * segment: if you want to set a new segment it is more convenient to
 * change both properties (#AdgMarker:trail and #AdgMarker:n-segment)
 * at once with adg_marker_set_segment().
 **/
void
adg_marker_set_trail(AdgMarker *marker, AdgTrail *trail)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    g_object_set(marker, "trail", trail, NULL);
}

/**
 * adg_marker_get_trail:
 * @marker: an #AdgMarker
 *
 * Gets the trail where this marker should be applied.
 *
 * Returns: the trail owned by @marker or %NULL on errors
 **/
AdgTrail *
adg_marker_get_trail(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), NULL);

    data = marker->data;

    return data->trail;
}

/**
 * adg_marker_set_n_segment:
 * @marker: an #AdgMarker
 * @n_segment: the new segment index
 *
 * Sets the #AdgMarker:n-segment property to @n_segment. The trail
 * is unchanged. If you want to set both properties at once (as
 * usually requested to refer to a specific segment),
 * adg_marker_set_segment() should be more convenient.
 **/
void
adg_marker_set_n_segment(AdgMarker *marker, guint n_segment)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    g_object_set(marker, "n-segment", n_segment, NULL);
}

/**
 * adg_marker_get_n_segment:
 * @marker: an #AdgMarker
 *
 * Returns the segment of the associated trail where this marker
 * will be applied, where %1 is the first segment.
 *
 * Returns: an index greather than %0 on success or %0 on errors
 **/
guint
adg_marker_get_n_segment(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), 0);

    data = marker->data;

    return data->n_segment;
}

/**
 * adg_marker_set_segment:
 * @marker: an #AdgMarker
 * @trail: the #AdgTrail
 * @n_segment: a segment index
 *
 * Sets a new segment where the marker should be applied at once.
 * A dependency between @trail and @marker is added, so when @trail
 * changes @marker is invalidated.
 *
 * A callback is added to #AdgTrail::remove-dependency so manually
 * removing the dependency (such as when @trail is destroyed) will
 * unlink @marker from it.
 **/
void
adg_marker_set_segment(AdgMarker *marker, AdgTrail *trail, guint n_segment)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    /* To avoid referring to an inexistent trail/n-segment couple, the
     * "n-segment" property is reset before to avoid segment validation */
    g_object_set(marker, "n-segment", 0,
                 "trail", trail, "n-segment", n_segment, NULL);
}

/**
 * adg_marker_get_segment:
 * @marker: an #AdgMarker
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Gets the segment where the marker will be applied. This segment
 * is eventually a modified version of the backup segment, after
 * having applied the marker.
 *
 * Returns: the segment or %NULL on errors
 **/
const AdgSegment *
adg_marker_get_segment(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), NULL);

    data = marker->data;

    return &data->segment;
}

/**
 * adg_marker_backup_segment:
 * @marker: an #AdgMarker
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Duplicates the current subject segment for backup purpose: this
 * segment can be accessed by adg_marker_get_backup_segment().
 * Obviously, a current segment should exist (either the
 * #AdgMarker:trail and #AdgMarker:n-segment properties must be
 * properly defined) or this method will fail without further
 * processing.
 *
 * When the subject segment is changed (either by changing
 * #AdgMarker:trail or #AdgMarker:n-segment) the original segment
 * is automatically restored.
 **/
void
adg_marker_backup_segment(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_if_fail(ADG_IS_MARKER(marker));

    data = marker->data;

    if (data->n_segment > 0) {
        g_return_if_fail(data->trail != NULL);

        g_free(data->backup_segment);

        /* Backup the segment, if a segment to backup exists */
        if (adg_trail_put_segment(data->trail, data->n_segment,
                                  &data->segment))
            data->backup_segment = adg_segment_deep_dup(&data->segment);
    }
}

/**
 * adg_marker_get_backup_segment:
 * @marker: an #AdgMarker
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Gets the original segment where the marker has been applied.
 * Applying a marker could modify the underlying trail, usually
 * by trimming the original segment of a #AdgMarker:size dependent
 * length from the ends. The marker instance holds a copy of the
 * original segment, generated by adg_marker_backup_segment(),
 * to be used in recomputation, for example when the marker
 * changes its size.
 *
 * When the subject segment is changed (either by changing
 * #AdgMarker:trail or #AdgMarker:n-segment) the original segment
 * is automatically restored.
 *
 * Returns: the original segment or %NULL on errors
 **/
const AdgSegment *
adg_marker_get_backup_segment(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), NULL);

    data = marker->data;

    return data->backup_segment;
}

/**
 * adg_marker_set_pos:
 * @marker: an #AdgMarker
 * @pos: the new pos
 *
 * Sets a new position on @marker. Check out adg_marker_get_pos() for
 * details on what @pos represents.
 **/
void
adg_marker_set_pos(AdgMarker *marker, gdouble pos)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    g_object_set(marker, "pos", pos, NULL);
}

/**
 * adg_marker_get_pos:
 * @marker: an #AdgMarker
 *
 * Gets the current position of @marker. The returned value is a ratio
 * position referred to the segment associated to @marker: %0 means the
 * start point and %1 means the end point of the segment.
 *
 * Returns: the marker position
 **/
gdouble
adg_marker_get_pos(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), 0);

    data = marker->data;

    return data->pos;
}

/**
 * adg_marker_set_size:
 * @marker: an #AdgMarker
 * @size: the new size
 *
 * Sets a new size on @marker. The @size is an implementation-dependent
 * property: it has meaning only when used by an #AdgMarker derived type.
 **/
void
adg_marker_set_size(AdgMarker *marker, gdouble size)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    g_object_set(marker, "size", size, NULL);
}

/**
 * adg_marker_get_size:
 * @marker: an #AdgMarker
 *
 * Gets the current size of @marker.
 *
 * Returns: the marker size, in global space
 **/
gdouble
adg_marker_get_size(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), 0);

    data = marker->data;

    return data->size;
}

/**
 * adg_marker_set_model:
 * @marker: an #AdgMarker
 * @model: a new #AdgModel
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Sets a new model for @marker. The reference to the old model (if an
 * old model was present) is dropped while a new reference is added to
 * @model.
 **/
void
adg_marker_set_model(AdgMarker *marker, AdgModel *model)
{
    g_return_if_fail(ADG_IS_MARKER(marker));
    g_object_set(marker, "model", model, NULL);
}

/**
 * adg_marker_get_model:
 * @marker: an #AdgMarker
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Gets the current model of @marker. This is an accessor method:
 * if you need to get the model for rendering, use adg_marker_model()
 * instead.
 *
 * Returns: the cached model owned by @marker or %NULL on errors
 **/
AdgModel *
adg_marker_get_model(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), NULL);

    data = marker->data;

    return data->model;
}

/**
 * adg_marker_model:
 * @marker: an #AdgMarker
 *
 * <note><para>
 * This function is only useful in marker implementations.
 * </para></note>
 *
 * Gets the model of @marker. If the model is not found, it is
 * automatically created by calling the create_model() virtual method.
 *
 * Returns: the current model owned by @marker or %NULL on errors
 **/
AdgModel *
adg_marker_model(AdgMarker *marker)
{
    AdgMarkerPrivate *data;

    g_return_val_if_fail(ADG_IS_MARKER(marker), NULL);

    data = marker->data;

    if (data->model == NULL) {
        /* Model not found: regenerate it */
        AdgMarkerClass *marker_class = ADG_MARKER_GET_CLASS(marker);

        if (marker_class->create_model)
            adg_marker_set_model(marker, marker_class->create_model(marker));
    }

    return data->model;
}


static void
_adg_local_changed(AdgEntity *entity)
{
    AdgMarkerPrivate *data;
    CpmlPair pair;
    CpmlVector vector;
    AdgMatrix map;

    data = ((AdgMarker *) entity)->data;
    if (data->trail == NULL)
        return;

    cpml_segment_put_pair_at(&data->segment, data->pos, &pair);
    cpml_segment_put_vector_at(&data->segment, data->pos, &vector);
    cpml_vector_set_length(&vector, data->size);

    if (data->pos > 0.5) {
        vector.x = -vector.x;
        vector.y = -vector.y;
    }

    cairo_matrix_init(&map, vector.x, vector.y,
                      -vector.y, vector.x, pair.x, pair.y);

    adg_entity_set_local_map(entity, &map);

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    adg_marker_set_model((AdgMarker *) entity, NULL);
}


static void
_adg_clear_trail(AdgMarker *marker)
{
    AdgMarkerPrivate *data = marker->data;

    if (data->trail && data->backup_segment) {
        /* Restore the original segment in the old trail */
        adg_segment_deep_copy(&data->segment, data->backup_segment);
        g_free(data->backup_segment);
        data->backup_segment = NULL;
    }

    data->trail = NULL;
    data->n_segment = 0;
}

static gboolean
_adg_set_segment(AdgMarker *marker, AdgTrail *trail, guint n_segment)
{
    AdgMarkerPrivate *data;
    AdgSegment segment = { 0 };

    g_return_val_if_fail(trail == NULL || ADG_IS_TRAIL(trail), FALSE);

    /* Segment validation, but only if n_segment is specified */
    if (trail && n_segment > 0 && !adg_trail_put_segment(trail, n_segment, &segment))
        return FALSE;

    data = marker->data;

    /* Do not try to cache results! Although @trail and @n_segment
     * could be the same, the internal CpmlSegment could change.
     * This is the case when AdgLDim arranges the layout and changes
     * the path data (to force outside arrows for example) reusing
     * the same CpmlPath. In other words, do not do this:
     *
     * if (trail == data->trail && n_segment == data->n_segment)
     *     return FALSE;
     *
     * Incidentally, on a 64 bit platform this issue has been never
     * exposed. Avoiding the cache will solve the issue 33:
     *
     * http://dev.entidi.com/p/adg/issues/33/
     */

    if (trail != data->trail) {
        AdgEntity *entity = (AdgEntity *) marker;

        if (data->trail)
            adg_model_remove_dependency((AdgModel *) data->trail, entity);

        data->trail = trail;

        if (trail) {
            adg_model_add_dependency((AdgModel *) trail, entity);
            g_signal_connect_swapped(trail, "remove-dependency",
                                     G_CALLBACK(_adg_clear_trail), marker);
        }
    }

    cpml_segment_copy(&data->segment, &segment);
    data->n_segment = n_segment;

    return TRUE;
}

static AdgModel *
_adg_create_model(AdgMarker *marker)
{
    g_warning(_("%s: `create_model' method not implemented for type `%s'"),
              G_STRLOC, g_type_name(G_OBJECT_TYPE(marker)));
    return NULL;
}
