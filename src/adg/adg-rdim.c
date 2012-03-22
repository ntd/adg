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
 * SECTION:adg-rdim
 * @short_description: Radial dimensions
 *
 * The #AdgRDim entity represents a radial dimension.
 *
 * Since: 1.0
 **/

/**
 * AdgRDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-container.h"
#include "adg-alignment.h"
#include "adg-model.h"
#include "adg-point.h"
#include "adg-trail.h"
#include "adg-marker.h"
#include "adg-style.h"
#include "adg-dim-style.h"
#include "adg-textual.h"
#include "adg-toy-text.h"
#include "adg-dim.h"
#include "adg-dim-private.h"
#include <math.h>

#include "adg-rdim.h"
#include "adg-rdim-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_rdim_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_rdim_parent_class)


G_DEFINE_TYPE(AdgRDim, adg_rdim, ADG_TYPE_DIM)

enum {
    PROP_0,
    PROP_VALUE
};


static void             _adg_dispose            (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_global_changed     (AdgEntity      *entity);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_invalidate         (AdgEntity      *entity);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static gchar *          _adg_default_value      (AdgDim         *dim);
static gboolean         _adg_update_geometry    (AdgRDim        *rdim);
static void             _adg_update_entities    (AdgRDim        *rdim);
static void             _adg_clear_trail        (AdgRDim        *rdim);
static void             _adg_dispose_trail      (AdgRDim        *rdim);
static void             _adg_dispose_marker     (AdgRDim        *rdim);
static CpmlPath *       _adg_trail_callback     (AdgTrail       *trail,
                                                 gpointer        user_data);


static void
adg_rdim_class_init(AdgRDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    AdgDimClass *dim_class;
    GParamSpec *param, *old_param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;
    dim_class = (AdgDimClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgRDimPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    dim_class->default_value = _adg_default_value;

    /* Override #AdgDim:value to prepend "R "
     * to the default set value template string */
    old_param = g_object_class_find_property(gobject_class, "value");
    param = g_param_spec_string(old_param->name,
                                g_param_spec_get_nick(old_param),
                                g_param_spec_get_blurb(old_param),
                                "R <>",
                                old_param->flags);
    g_object_class_install_property(gobject_class, PROP_VALUE, param);
}

static void
adg_rdim_init(AdgRDim *rdim)
{
    AdgRDimPrivate *data;
    cairo_path_data_t move_to, line_to;

    data = G_TYPE_INSTANCE_GET_PRIVATE(rdim, ADG_TYPE_RDIM, AdgRDimPrivate);
    move_to.header.type = CPML_MOVE;
    move_to.header.length = 2;
    line_to.header.type = CPML_LINE;
    line_to.header.length = 2;

    data->trail = NULL;
    data->marker = NULL;
    data->geometry_arranged = FALSE;
    data->radius = -1.;
    data->shift.base.x = data->shift.base.y = 0;
    cairo_matrix_init_identity(&data->quote.global_map);

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.path.data = data->cpml.data;
    data->cpml.path.num_data = G_N_ELEMENTS(data->cpml.data);
    data->cpml.path.data[0] = move_to;
    data->cpml.path.data[2] = line_to;
    data->cpml.path.data[4] = move_to;
    data->cpml.path.data[6] = line_to;

    rdim->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgRDim *rdim = (AdgRDim *) object;

    _adg_dispose_trail(rdim);
    _adg_dispose_marker(rdim);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_VALUE:
        g_value_set_string(value, adg_dim_get_value((AdgDim *) object));
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
    switch (prop_id) {
    case PROP_VALUE:
        adg_dim_set_value((AdgDim *) object, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_rdim_new:
 *
 * Creates a new uninitialized radial dimension. To be useful, you
 * need at least define the center of the arc to quote in #AdgDim:ref1,
 * a point on the arc in #AdgDim:ref2 and the position of the quote
 * in #AdgDim:pos using any valid #AdgDim method.
 *
 * Returns: a newly created quote
 *
 * Since: 1.0
 **/
AdgRDim *
adg_rdim_new(void)
{
    return g_object_new(ADG_TYPE_RDIM, NULL);
}

/**
 * adg_rdim_new_full:
 * @center: allow-none: center of the arc to quote
 * @radius: allow-none: where the quote must be applied on the arc
 * @pos: allow-none: position of the quote text
 *
 * Creates a new quote by specifying explicitely all the needed
 * data to get a valid quote.
 *
 * Returns: a newly created quote
 *
 * Since: 1.0
 **/
AdgRDim *
adg_rdim_new_full(const AdgPair *center, const AdgPair *radius, const AdgPair *pos)
{
    AdgRDim *rdim;
    AdgDim *dim;

    rdim = adg_rdim_new();
    dim = (AdgDim *) rdim;

    if (center != NULL)
        adg_dim_set_ref1_from_pair(dim, center);

    if (radius != NULL)
        adg_dim_set_ref2_from_pair(dim, radius);

    if (pos != NULL)
        adg_dim_set_pos_from_pair(dim, pos);

    return rdim;
}

/**
 * adg_rdim_new_full_explicit:
 * @center_x: x coordinate of the center of the arc to quote
 * @center_y: y coordinate of the center of the arc to quote
 * @radius_x: x coordiante where the quote must be applied on the arc
 * @radius_y: y coordiante where the quote must be applied on the arc
 * @pos_x: x coordinate of the quote text
 * @pos_y: y coordinate of the quote text
 *
 * Does the same job of adg_rdim_full() but using specific coordinates
 * instead of AdgPair structures.
 * data to get a valid quote.
 *
 * Returns: a newly created quote
 *
 * Since: 1.0
 **/
AdgRDim *
adg_rdim_new_full_explicit(gdouble center_x, gdouble center_y,
                           gdouble radius_x, gdouble radius_y,
                           gdouble pos_x,    gdouble pos_y)
{
    AdgPair center, radius, pos;

    center.x = center_x;
    center.y = center_y;
    radius.x = radius_x;
    radius.y = radius_y;
    pos.x = pos_x;
    pos.y = pos_y;

    return adg_rdim_new_full(&center, &radius, &pos);
}

/**
 * adg_rdim_new_full_from_model:
 * @model: transfer-none: the model from which the named pairs are taken
 * @center: allow-none: the center point of the arc to quote
 * @radius: allow-none: an arbitrary point on the arc
 * @pos: allow-none: the position reference
 *
 * Creates a new radial dimension, specifing all the needed properties in
 * one shot and using named pairs from @model.
 *
 * Returns: the newly created radial dimension entity
 *
 * Since: 1.0
 **/
AdgRDim *
adg_rdim_new_full_from_model(AdgModel *model, const gchar *center,
                             const gchar *radius, const gchar *pos)
{
    AdgRDim *rdim;
    AdgDim *dim;

    g_return_val_if_fail(model != NULL, NULL);

    rdim = adg_rdim_new();
    dim = (AdgDim *) rdim;

    if (center != NULL)
        adg_dim_set_ref1_from_model(dim, model, center);

    if (radius != NULL)
        adg_dim_set_ref2_from_model(dim, model, radius);

    if (pos != NULL)
        adg_dim_set_pos_from_model(dim, model, pos);

    return rdim;
}


static void
_adg_global_changed(AdgEntity *entity)
{
    AdgRDimPrivate *data = ((AdgRDim *) entity)->data;

    _adg_clear_trail((AdgRDim *) entity);

    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    if (data->marker != NULL)
        adg_entity_global_changed((AdgEntity *) data->marker);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    _adg_clear_trail((AdgRDim *) entity);

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;

    rdim = (AdgRDim *) entity;
    data = rdim->data;

    _adg_dispose_trail(rdim);
    _adg_dispose_marker(rdim);
    data->geometry_arranged = FALSE;
    _adg_clear_trail(rdim);

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgRDim *rdim;
    AdgDim *dim;
    AdgRDimPrivate *data;
    AdgAlignment *quote;
    AdgEntity *quote_entity;
    gboolean outside;
    const AdgMatrix *global, *local;
    AdgPair ref2, base;
    AdgPair pair;
    CpmlExtents extents;

    if (_ADG_OLD_ENTITY_CLASS->arrange != NULL)
        _ADG_OLD_ENTITY_CLASS->arrange(entity);

    rdim = (AdgRDim *) entity;
    dim = (AdgDim *) rdim;
    data = rdim->data;
    quote = adg_dim_get_quote(dim);
    quote_entity = (AdgEntity *) quote;

    if (!_adg_update_geometry(rdim))
        return;

    _adg_update_entities(rdim);

    /* Check for cached result */
    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS) {
        adg_entity_set_global_map(quote_entity, &data->quote.global_map);
        return;
    }

    outside = adg_dim_get_outside(dim);
    if (outside == ADG_THREE_STATE_UNKNOWN)
        outside = ADG_THREE_STATE_OFF;

    global = adg_entity_get_global_matrix(entity);
    local = adg_entity_get_local_matrix(entity);
    extents.is_defined = FALSE;

    cpml_pair_copy(&ref2, adg_point_get_pair(adg_dim_get_ref2(dim)));
    cpml_pair_copy(&base, &data->point.base);

    cpml_pair_transform(&ref2, local);
    cpml_pair_transform(&base, local);
    base.x += data->shift.base.x;
    base.y += data->shift.base.y;

    /* baseline start */
    cpml_pair_to_cairo(&base, &data->cpml.data[1]);

    /* baseline end */
    cpml_pair_to_cairo(&ref2, &data->cpml.data[3]);

    if (outside) {
        AdgDimStyle *dim_style;
        gdouble beyond;
        CpmlVector vector;

        dim_style = _ADG_GET_DIM_STYLE(dim);
        beyond = adg_dim_style_get_beyond(dim_style);
        vector.x = ref2.x - base.x;
        vector.y = ref2.y - base.y;
        cpml_vector_set_length(&vector, beyond);
        pair.x = ref2.x + vector.x;
        pair.y = ref2.y + vector.y;

        data->cpml.data[2].header.length = 2;

        /* Outside segment start */
        cpml_pair_to_cairo(&pair, &data->cpml.data[5]);

        /* Outside segment end */
        cpml_pair_to_cairo(&ref2, &data->cpml.data[7]);
    } else {
        data->cpml.data[2].header.length = 6;
    }

    /* Arrange the quote */
    if (quote != NULL) {
        AdgMatrix map;

        adg_alignment_set_factor_explicit(quote, 1, 0);

        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);
        cairo_matrix_init_translate(&map, pair.x, pair.y);
        cairo_matrix_rotate(&map, data->angle);
        adg_entity_set_global_map(quote_entity, &map);
        adg_entity_arrange(quote_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(quote_entity));

        adg_matrix_copy(&data->quote.global_map, &map);
    }

    data->cpml.path.status = CAIRO_STATUS_SUCCESS;

    /* Arrange the trail */
    if (data->trail != NULL) {
        CpmlExtents trail_extents;
        cpml_extents_copy(&trail_extents, adg_trail_get_extents(data->trail));
        cpml_extents_transform(&trail_extents, global);
        cpml_extents_add(&extents, &trail_extents);
    } else {
        _adg_dispose_marker(rdim);
    }

    /* Arrange the marker */
    if (data->marker != NULL) {
        AdgEntity *marker_entity = (AdgEntity *) data->marker;
        adg_marker_set_segment(data->marker, data->trail, outside ? 2 : 1);
        adg_entity_local_changed(marker_entity);
        adg_entity_arrange(marker_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(marker_entity));
    }

    adg_entity_set_extents(entity, &extents);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgRDim *rdim;
    AdgDim *dim;
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;
    const cairo_path_t *cairo_path;

    rdim = (AdgRDim *) entity;
    data = rdim->data;

    if (!data->geometry_arranged) {
        /* Entity not arranged, probably due to undefined pair found */
        return;
    }

    dim = (AdgDim *) entity;
    dim_style = _ADG_GET_DIM_STYLE(dim);

    adg_style_apply((AdgStyle *) dim_style, entity, cr);
    adg_entity_render((AdgEntity *) adg_dim_get_quote(dim), cr);

    if (data->marker != NULL)
        adg_entity_render((AdgEntity *) data->marker, cr);

    cairo_transform(cr, adg_entity_get_global_matrix(entity));
    dress = adg_dim_style_get_line_dress(dim_style);
    adg_entity_apply_dress(entity, dress, cr);

    cairo_path = adg_trail_get_cairo_path(data->trail);
    cairo_append_path(cr, cairo_path);
    cairo_stroke(cr);
}

static gchar *
_adg_default_value(AdgDim *dim)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;
    const gchar *format;

    rdim = (AdgRDim *) dim;
    data = rdim->data;
    dim_style = _ADG_GET_DIM_STYLE(dim);
    format = adg_dim_style_get_number_format(dim_style);

    if (!_adg_update_geometry(rdim))
        return g_strdup("undef");

    return g_strdup_printf(format, data->radius);
}

static gboolean
_adg_update_geometry(AdgRDim *rdim)
{
    AdgRDimPrivate *data;
    AdgDim *dim;
    AdgDimStyle *dim_style;
    const AdgPair *ref1, *ref2;
    const AdgPair *pos;
    gdouble spacing, level, pos_distance;
    CpmlVector vector;

    data = rdim->data;

    if (data->geometry_arranged)
        return TRUE;

    dim = (AdgDim *) rdim;
    ref1 = adg_point_get_pair(adg_dim_get_ref1(dim));
    ref2 = adg_point_get_pair(adg_dim_get_ref2(dim));
    pos = adg_point_get_pair(adg_dim_get_pos(dim));

    if (ref1 == NULL || ref2 == NULL || pos == NULL)
        return FALSE;

    dim_style = _ADG_GET_DIM_STYLE(rdim);
    spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level(dim);
    pos_distance = cpml_pair_distance(pos, ref1);
    vector.x = ref2->x - ref1->x;
    vector.y = ref2->y - ref1->y;

    if (cpml_pair_squared_distance(pos, ref1) <
        cpml_pair_squared_distance(pos, ref2)) {
        vector.x = -vector.x;
        vector.y = -vector.y;
    }

    /* radius */
    data->radius = cpml_pair_distance(&vector, NULL);

    /* angle */
    data->angle = adg_dim_quote_angle(dim, cpml_vector_angle(&vector));

    /* point.base */
    cpml_pair_copy(&data->point.base, &vector);
    cpml_vector_set_length(&data->point.base, pos_distance);
    data->point.base.x += ref1->x;
    data->point.base.y += ref1->y;

    /* shift.base */
    cpml_pair_copy(&data->shift.base, &vector);
    cpml_vector_set_length(&data->shift.base, spacing * level);

    data->geometry_arranged = TRUE;

    return TRUE;
}

static void
_adg_update_entities(AdgRDim *rdim)
{
    AdgEntity *entity;
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;

    entity = (AdgEntity *) rdim;
    data = rdim->data;
    dim_style = _ADG_GET_DIM_STYLE(rdim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(_adg_trail_callback, rdim);

    if (data->marker == NULL) {
        data->marker = adg_dim_style_marker2_new(dim_style);
        adg_entity_set_parent((AdgEntity *) data->marker, entity);
    }
}

static void
_adg_clear_trail(AdgRDim *rdim)
{
    AdgRDimPrivate *data = rdim->data;

    if (data->trail != NULL)
        adg_model_clear((AdgModel *) data->trail);

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}

static void
_adg_dispose_trail(AdgRDim *rdim)
{
    AdgRDimPrivate *data = rdim->data;

    if (data->trail != NULL) {
        g_object_unref(data->trail);
        data->trail = NULL;
    }
}

static void
_adg_dispose_marker(AdgRDim *rdim)
{
    AdgRDimPrivate *data = rdim->data;

    if (data->marker != NULL) {
        g_object_unref(data->marker);
        data->marker = NULL;
    }
}

static CpmlPath *
_adg_trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;

    rdim = (AdgRDim *) user_data;
    data = rdim->data;

    return &data->cpml.path;
}
