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
 * SECTION:adg-rdim
 * @short_description: Linear dimensions
 *
 * The #AdgRDim entity represents a linear dimension.
 **/

/**
 * AdgRDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-rdim.h"
#include "adg-rdim-private.h"
#include "adg-dim-private.h"
#include "adg-dim-style.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_rdim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_rdim_parent_class)


static void             dispose                 (GObject        *object);
static void             local_changed           (AdgEntity      *entity);
static void             invalidate              (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static gchar *          default_value           (AdgDim         *dim);
static void             update_geometry         (AdgRDim        *rdim);
static void             update_entities         (AdgRDim        *rdim);
static void             unset_trail             (AdgRDim        *rdim);
static void             dispose_marker          (AdgRDim        *rdim);
static CpmlPath *       trail_callback          (AdgTrail       *trail,
                                                 gpointer        user_data);


G_DEFINE_TYPE(AdgRDim, adg_rdim, ADG_TYPE_DIM);


static void
adg_rdim_class_init(AdgRDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    AdgDimClass *dim_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;
    dim_class = (AdgDimClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgRDimPrivate));

    gobject_class->dispose = dispose;

    entity_class->local_changed = local_changed;
    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    dim_class->default_value = default_value;
}

static void
adg_rdim_init(AdgRDim *rdim)
{
    AdgRDimPrivate *data;
    cairo_path_data_t move_to, line_to;

    data = G_TYPE_INSTANCE_GET_PRIVATE(rdim, ADG_TYPE_RDIM, AdgRDimPrivate);
    move_to.header.type = CAIRO_PATH_MOVE_TO;
    move_to.header.length = 2;
    line_to.header.type = CAIRO_PATH_LINE_TO;
    line_to.header.length = 2;

    data->trail = NULL;
    data->marker = NULL;
    data->geometry_arranged = FALSE;
    data->radius = -1.;
    data->shift.base.x = data->shift.base.y = 0;
    cairo_matrix_init_identity(&data->quote.local_map);
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
dispose(GObject *object)
{
    dispose_marker((AdgRDim *) object);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}


/**
 * adg_rdim_new:
 *
 * Creates a new uninitialized radial dimension. To be useful, you
 * need at least define the center of the arc to quote (ref1) and
 * a point on the arc (ref2) with adg_dim_set_ref() and the
 * position of the quote with adg_dim_set_pos().
 *
 * Returns: a newly created quote
 **/
AdgRDim *
adg_rdim_new(void)
{
    return g_object_new(ADG_TYPE_RDIM, NULL);
}

/**
 * adg_rdim_new_full:
 * @center: center of the arc to quote
 * @radius: where the quote must be applied on the arc
 * @pos: position of the quote text
 *
 * Creates a new quote by specifying explicitely all the needed
 * data to get a valid quote.
 *
 * Returns: a newly created quote
 **/
AdgRDim *
adg_rdim_new_full(const AdgPair *center, const AdgPair *radius,
                  const AdgPair *pos)
{
    AdgRDim *rdim;
    AdgDim *dim;

    rdim = adg_rdim_new();
    dim = (AdgDim *) rdim;

    adg_dim_set_ref(dim, center, radius);
    adg_dim_set_pos(dim, pos);

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
 * @model: the model from which the named pairs are taken
 * @center: the center point of the arc to quote
 * @radius: an arbitrary point on the arc
 * @pos: the position reference
 *
 * Creates a new radial dimension, specifing all the needed properties in
 * one shot and using named pairs from @model.
 *
 * Returns: the newly created radial dimension entity
 **/
AdgRDim *
adg_rdim_new_full_from_model(AdgModel *model, const gchar *center,
                             const gchar *radius, const gchar *pos)
{
    AdgDim *dim = g_object_new(ADG_TYPE_RDIM, NULL);

    adg_dim_set_ref_from_model(dim, model, center, radius);
    adg_dim_set_pos_from_model(dim, model, pos);

    return (AdgRDim *) dim;
}


static void
local_changed(AdgEntity *entity)
{
    unset_trail((AdgRDim *) entity);

    PARENT_ENTITY_CLASS->local_changed(entity);
}

static void
invalidate(AdgEntity *entity)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;

    rdim = (AdgRDim *) entity;
    data = rdim->data;

    dispose_marker(rdim);
    data->geometry_arranged = FALSE;
    unset_trail(rdim);

    if (PARENT_ENTITY_CLASS->invalidate != NULL)
        PARENT_ENTITY_CLASS->invalidate(entity);
}

static void
arrange(AdgEntity *entity)
{
    AdgRDim *rdim;
    AdgDim *dim;
    AdgRDimPrivate *data;
    AdgContainer *quote;
    AdgDimStyle *dim_style;
    gboolean outside;
    const AdgMatrix *local;
    AdgPair ref1, ref2, base;
    AdgPair pair;

    PARENT_ENTITY_CLASS->arrange(entity);

    rdim = (AdgRDim *) entity;
    dim = (AdgDim *) rdim;
    data = rdim->data;
    quote = adg_dim_get_quote(dim);

    update_geometry(rdim);
    update_entities(rdim);

    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS) {
        AdgEntity *quote_entity = (AdgEntity *) quote;
        adg_entity_set_global_map(quote_entity, &data->quote.global_map);
        adg_entity_set_local_map(quote_entity, &data->quote.local_map);
        return;
    }

    dim_style = GET_DIM_STYLE(dim);
    outside = adg_dim_get_outside(dim);
    if (outside == ADG_THREE_STATE_UNKNOWN)
        outside = ADG_THREE_STATE_OFF;

    local = adg_entity_local_matrix(entity);
    cpml_pair_copy(&ref1, adg_dim_get_ref1(dim));
    cpml_pair_copy(&ref2, adg_dim_get_ref2(dim));
    cpml_pair_copy(&base, &data->point.base);

    cpml_pair_transform(&ref1, local);
    cpml_pair_transform(&ref2, local);
    cpml_pair_transform(&base, local);
    cpml_pair_add(&base, &data->shift.base);

    /* baseline start */
    cpml_pair_to_cairo(&base, &data->cpml.data[1]);

    /* baseline end */
    cpml_pair_to_cairo(&ref2, &data->cpml.data[3]);

    if (outside) {
        data->cpml.data[2].header.length = 2;
        /* TODO: outside radius dimensions */
    } else {
        data->cpml.data[2].header.length = 6;
    }

    data->cpml.path.status = CAIRO_STATUS_SUCCESS;

    if (quote != NULL) {
        AdgEntity *quote_entity;
        AdgMatrix map;

        quote_entity = (AdgEntity *) quote;
        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);

        cairo_matrix_init_translate(&map, pair.x, pair.y);
        cairo_matrix_rotate(&map, data->angle);
        adg_entity_set_global_map(quote_entity, &map);

        adg_entity_get_global_map(quote_entity, &data->quote.global_map);
        adg_entity_get_local_map(quote_entity, &data->quote.local_map);
    }

    if (data->marker != NULL) {
        adg_marker_set_segment(data->marker, data->trail, outside ? 2 : 1);
        adg_entity_local_changed((AdgEntity *) data->marker);
    }

    /* TODO: compute the extents */
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgRDim *rdim;
    AdgDim *dim;
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;
    const cairo_path_t *cairo_path;

    rdim = (AdgRDim *) entity;
    dim = (AdgDim *) entity;
    data = rdim->data;
    dim_style = GET_DIM_STYLE(dim);

    adg_style_apply((AdgStyle *) dim_style, entity, cr);

    if (data->marker != NULL)
        adg_entity_render((AdgEntity *) data->marker, cr);

    adg_entity_render((AdgEntity *) adg_dim_get_quote(dim), cr);

    dress = adg_dim_style_get_line_dress(dim_style);
    adg_entity_apply_dress(entity, dress, cr);

    cairo_path = adg_trail_get_cairo_path(data->trail);
    cairo_append_path(cr, cairo_path);
    cairo_stroke(cr);
}

static gchar *
default_value(AdgDim *dim)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;
    const gchar *format;

    rdim = (AdgRDim *) dim;
    data = rdim->data;
    dim_style = GET_DIM_STYLE(dim);
    format = adg_dim_style_get_number_format(dim_style);

    update_geometry(rdim);

    return g_strdup_printf(format, data->radius);
}

static void
update_geometry(AdgRDim *rdim)
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
        return;

    dim = (AdgDim *) rdim;
    dim_style = GET_DIM_STYLE(rdim);
    ref1 = adg_dim_get_ref1(dim);
    ref2 = adg_dim_get_ref2(dim);
    pos = adg_dim_get_pos(dim);
    spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level(dim);
    pos_distance = cpml_pair_distance(pos, ref1);
    cpml_pair_sub(cpml_pair_copy(&vector, ref2), ref1);
    if (cpml_pair_squared_distance(pos, ref1) <
        cpml_pair_squared_distance(pos, ref2))
        cpml_pair_negate(&vector);

    /* radius */
    data->radius = cpml_pair_distance(&vector, NULL);

    /* angle */
    data->angle = adg_dim_quote_angle(dim, cpml_vector_angle(&vector));

    /* point.base */
    cpml_pair_copy(&data->point.base, &vector);
    cpml_vector_set_length(&data->point.base, pos_distance);
    cpml_pair_add(&data->point.base, ref1);

    /* shift.base */
    cpml_pair_copy(&data->shift.base, &vector);
    cpml_vector_set_length(&data->shift.base, spacing * level);

    data->geometry_arranged = TRUE;
}

static void
update_entities(AdgRDim *rdim)
{
    AdgRDimPrivate *data;
    AdgDimStyle *dim_style;

    data = rdim->data;
    dim_style = GET_DIM_STYLE(rdim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(trail_callback, rdim);

    if (data->marker == NULL)
        data->marker = adg_dim_style_marker2_new(dim_style);
}

static void
unset_trail(AdgRDim *rdim)
{
    AdgRDimPrivate *data = rdim->data;

    if (data->trail != NULL)
        adg_model_clear((AdgModel *) data->trail);

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}

static void
dispose_marker(AdgRDim *rdim)
{
    AdgRDimPrivate *data = rdim->data;

    if (data->trail != NULL) {
        g_object_unref(data->trail);
        data->trail = NULL;
    }

    if (data->marker != NULL) {
        g_object_unref(data->marker);
        data->marker = NULL;
    }
}

static CpmlPath *
trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgRDim *rdim;
    AdgRDimPrivate *data;

    rdim = (AdgRDim *) user_data;
    data = rdim->data;

    return &data->cpml.path;
}
