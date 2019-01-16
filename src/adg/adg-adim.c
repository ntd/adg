/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-adim
 * @short_description: Angular dimensions
 *
 * The #AdgADim entity defines an angular dimension.
 *
 * Since: 1.0
 */

/**
 * AdgADim:
 *
 * All fields are privates and should not be used directly.
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
#include "adg-dress.h"
#include "adg-textual.h"
#include "adg-toy-text.h"
#include "adg-dim.h"
#include "adg-dim-private.h"

#include "adg-adim.h"
#include "adg-adim-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_adim_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_adim_parent_class)


G_DEFINE_TYPE(AdgADim, adg_adim, ADG_TYPE_DIM)

enum {
    PROP_0,
    PROP_ORG1,
    PROP_ORG2,
    PROP_HAS_EXTENSION1,
    PROP_HAS_EXTENSION2
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
static gboolean         _adg_compute_geometry   (AdgDim         *dim);
static void             _adg_update_entities    (AdgADim        *adim);
static void             _adg_unset_trail        (AdgADim        *adim);
static void             _adg_dispose_trail      (AdgADim        *adim);
static void             _adg_dispose_markers    (AdgADim        *adim);
static gboolean         _adg_get_info           (AdgADim        *adim,
                                                 CpmlVector      vector[],
                                                 CpmlPair       *center,
                                                 gdouble        *distance);
static cairo_path_t *   _adg_trail_callback     (AdgTrail       *trail,
                                                 gpointer        user_data);


static void
adg_adim_class_init(AdgADimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    AdgDimClass *dim_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;
    dim_class = (AdgDimClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgADimPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    dim_class->default_value = _adg_default_value;
    dim_class->compute_geometry = _adg_compute_geometry;

    param = g_param_spec_boxed("org1",
                               P_("First Origin"),
                               P_("Where the first line comes from: this point is used toghether with \"ref1\" to align the first extension line"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ORG1, param);

    param = g_param_spec_boxed("org2",
                               P_("Second Origin"),
                               P_("Where the second line comes from: this point is used toghether with \"ref2\" to align the second extension line"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ORG2, param);

    param = g_param_spec_boolean("has-extension1",
                                 P_("Has First Extension Line flag"),
                                 P_("Show (TRUE) or hide (FALSE) the first extension line"),
                                 TRUE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HAS_EXTENSION1, param);

    param = g_param_spec_boolean("has-extension2",
                                 P_("Has Second Extension Line flag"),
                                 P_("Show (TRUE) or hide (FALSE) the second extension line"),
                                 TRUE, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HAS_EXTENSION2, param);
}

static void
adg_adim_init(AdgADim *adim)
{
    AdgADimPrivate *data;
    AdgStyle *style;
    AdgDimStyle *dim_style;
    cairo_path_data_t move_to, line_to, arc_to;

    data = G_TYPE_INSTANCE_GET_PRIVATE(adim, ADG_TYPE_ADIM, AdgADimPrivate);
    move_to.header.type = CPML_MOVE;
    move_to.header.length = 2;
    line_to.header.type = CPML_LINE;
    line_to.header.length = 2;
    arc_to.header.type = CPML_ARC;
    arc_to.header.length = 3;

    data->org1 = NULL;
    data->org2 = NULL;
    data->has_extension1 = TRUE;
    data->has_extension2 = TRUE;

    data->cairo.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo.path.data = data->cairo.data;
    data->cairo.path.num_data = G_N_ELEMENTS(data->cairo.data);
    data->cairo.path.data[0] = move_to;
    data->cairo.path.data[2] = arc_to;
    data->cairo.path.data[5] = move_to;
    data->cairo.path.data[7] = line_to;
    data->cairo.path.data[9] = move_to;
    data->cairo.path.data[11] = line_to;

    data->trail = NULL;
    data->marker1 = NULL;
    data->marker2 = NULL;

    adim->data = data;

    /* Override the default style of the dimension dress to express angles in
     * sexagesimal units */
    style = adg_dress_get_fallback(ADG_DRESS_DIMENSION);
    dim_style = (AdgDimStyle *) adg_style_clone(style);

    adg_dim_style_set_decimals(dim_style, 0);
    adg_dim_style_set_rounding(dim_style, 3);
    adg_dim_style_set_number_arguments(dim_style, "Dm");
    adg_dim_style_set_number_format(dim_style, "%g°(%g')");

    adg_entity_set_style((AdgEntity *) adim,
                         ADG_DRESS_DIMENSION,
                         (AdgStyle *) dim_style);
}

static void
_adg_dispose(GObject *object)
{
    AdgEntity *entity;
    AdgADim *adim;
    AdgADimPrivate *data;

    entity = (AdgEntity *) object;
    adim = (AdgADim *) entity;
    data = adim->data;

    _adg_dispose_trail(adim);
    _adg_dispose_markers(adim);

    if (data->org1 != NULL)
        data->org1 = adg_entity_point(entity, data->org1, NULL);

    if (data->org2 != NULL)
        data->org2 = adg_entity_point(entity, data->org2, NULL);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgADimPrivate *data = ((AdgADim *) object)->data;

    switch (prop_id) {
    case PROP_ORG1:
        g_value_set_boxed(value, data->org1);
        break;
    case PROP_ORG2:
        g_value_set_boxed(value, data->org2);
        break;
    case PROP_HAS_EXTENSION1:
        g_value_set_boolean(value, data->has_extension1);
        break;
    case PROP_HAS_EXTENSION2:
        g_value_set_boolean(value, data->has_extension2);
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
    AdgEntity *entity;
    AdgADimPrivate *data;

    entity = (AdgEntity *) object;
    data = ((AdgADim *) object)->data;

    switch (prop_id) {
    case PROP_ORG1:
        data->org1 = adg_entity_point(entity, data->org1,
                                      g_value_get_boxed(value));
        break;
    case PROP_ORG2:
        data->org2 = adg_entity_point(entity, data->org2,
                                      g_value_get_boxed(value));
        break;
    case PROP_HAS_EXTENSION1:
        data->has_extension1 = g_value_get_boolean(value);
        break;
    case PROP_HAS_EXTENSION2:
        data->has_extension2 = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_adim_new:
 *
 * Creates a new - undefined - angular dimension. You must, at least,
 * define the first line by setting #AdgADim:org1 (start point) and
 * #AdgDim:ref1 (end point), the second line by setting #AdgADim:org2
 * (start point) and #AdgDim:ref2 (end point) and the position of
 * the quote in #AdgDim:pos.
 *
 * Returns: the newly created angular dimension entity
 *
 * Since: 1.0
 **/
AdgADim *
adg_adim_new(void)
{
    return g_object_new(ADG_TYPE_ADIM, NULL);
}

/**
 * adg_adim_new_full:
 * @ref1: (allow-none): first reference point
 * @ref2: (allow-none): second reference point
 * @org1: (allow-none): first origin point
 * @org2: (allow-none): second origin point
 * @pos:  (allow-none): the position point
 *
 * Creates a new angular dimension, specifing all the needed
 * properties in one shot using #CpmlPair.
 *
 * Returns: the newly created angular dimension entity
 *
 * Since: 1.0
 **/
AdgADim *
adg_adim_new_full(const CpmlPair *ref1, const CpmlPair *ref2,
                  const CpmlPair *org1, const CpmlPair *org2,
                  const CpmlPair *pos)
{
    AdgADim *adim;
    AdgDim *dim;

    adim = adg_adim_new();
    dim = (AdgDim *) adim;

    if (ref1 != NULL)
        adg_dim_set_ref1_from_pair(dim, ref1);

    if (ref2 != NULL)
        adg_dim_set_ref2_from_pair(dim, ref2);

    if (pos != NULL)
        adg_dim_set_pos_from_pair(dim, pos);

    if (org1 != NULL)
        adg_adim_set_org1_from_pair(adim, org1);

    if (org2 != NULL)
        adg_adim_set_org2_from_pair(adim, org2);

    return adim;
}

/**
 * adg_adim_new_full_explicit:
 * @ref1_x: the x coordinate of end point of the first line
 * @ref1_y: the y coordinate of end point of the first line
 * @ref2_x: the x coordinate of end point of the second line
 * @ref2_y: the y coordinate of end point of the second line
 * @org1_x: the x coordinate of start point of the first line
 * @org1_y: the y coordinate of start point of the first line
 * @org2_x: the x coordinate of start point of the second line
 * @org2_y: the y coordinate of start point of the second line
 * @pos_x:  the x coordinate of the position reference
 * @pos_y:  the y coordinate of the position reference
 *
 * Wrappes adg_adim_new_full() with explicit values.
 *
 * Returns: the newly created linear dimension entity
 *
 * Since: 1.0
 **/
AdgADim *
adg_adim_new_full_explicit(gdouble ref1_x, gdouble ref1_y,
                           gdouble ref2_x, gdouble ref2_y,
                           gdouble org1_x, gdouble org1_y,
                           gdouble org2_x, gdouble org2_y,
                           gdouble pos_x,  gdouble pos_y)
{
    CpmlPair ref1, ref2, org1, org2, pos;

    ref1.x = ref1_x;
    ref1.y = ref1_y;
    ref2.x = ref2_x;
    ref2.y = ref2_y;
    org1.x = org1_x;
    org1.y = org1_y;
    org2.x = org2_x;
    org2.y = org2_y;
    pos.x = pos_x;
    pos.y = pos_y;

    return adg_adim_new_full(&ref1, &ref2, &org1, &org2, &pos);
}

/**
 * adg_adim_new_full_from_model:
 * @model: (transfer none): the model from which the named pairs are taken
 * @ref1:  (allow-none):    the end point of the first line
 * @ref2:  (allow-none):    the end point of the second line
 * @org1:  (allow-none):    the origin of the first line
 * @org2:  (allow-none):    the origin of the second line
 * @pos:   (allow-none):    the position reference
 *
 * Creates a new angular dimension, specifing all the needed properties
 * in one shot and using named pairs from @model.
 *
 * Returns: the newly created angular dimension entity
 *
 * Since: 1.0
 **/
AdgADim *
adg_adim_new_full_from_model(AdgModel *model,
                             const gchar *ref1, const gchar *ref2,
                             const gchar *org1, const gchar *org2,
                             const gchar *pos)
{
    AdgADim *adim;
    AdgDim *dim;

    adim = adg_adim_new();
    dim = (AdgDim *) adim;

    if (ref1 != NULL)
        adg_dim_set_ref1_from_model(dim, model, ref1);

    if (ref2 != NULL)
        adg_dim_set_ref2_from_model(dim, model, ref2);

    if (pos != NULL)
        adg_dim_set_pos_from_model(dim, model, pos);

    if (org1 != NULL)
        adg_adim_set_org1_from_model(adim, model, org1);

    if (org2 != NULL)
        adg_adim_set_org2_from_model(adim, model, org2);

    return adim;
}

/**
 * adg_adim_set_org1:
 * @adim: an #AdgADim
 * @org1: the new point to use as first reference
 *
 * Sets the #AdgADim:org1 property to @org1. The old point
 * is silently discarded, unreferencing its model if that
 * point was bound to a named pair (hence, possibly destroying
 * the model if this was the last reference).
 *
 * @org1 can be <constant>NULL</constant>, in which case
 * the point is destroyed.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org1(AdgADim *adim, const AdgPoint *org1)
{
    g_return_if_fail(ADG_IS_ADIM(adim));
    g_object_set(adim, "org1", org1, NULL);
}

/**
 * adg_adim_set_org1_explicit:
 * @adim: an #AdgADim
 * @x: x coordinate of the first reference point
 * @y: y coordinate of the first reference point
 *
 * Sets the #AdgADim:org1 property to the (@x, @y) explicit
 * coordinates. The old point is silently discarded,
 * unreferencing its model if that point was bound to a named
 * pair (hence, possibly destroying the model if this was the
 * last reference).
 *
 * Since: 1.0
 **/
void
adg_adim_set_org1_explicit(AdgADim *adim, gdouble x, gdouble y)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_explicit(point, x, y);
    adg_adim_set_org1(adim, point);

    adg_point_destroy(point);
}

/**
 * adg_adim_set_org1_from_pair:
 * @adim: an #AdgADim
 * @org1: the coordinates pair of the first reference point
 *
 * Convenient function to set the #AdgADim:org1 property using a
 * pair instead of explicit coordinates.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org1_from_pair(AdgADim *adim, const CpmlPair *org1)
{
    g_return_if_fail(org1 != NULL);

    adg_adim_set_org1_explicit(adim, org1->x, org1->y);
}

/**
 * adg_adim_set_org1_from_model:
 * @adim: an #AdgADim
 * @model: the source #AdgModel
 * @org1: a named pair in @model
 *
 * Binds #AdgADim:org1 to the @org1 named pair of @model. If @model
 * is <constant>NULL</constant>, the point will be unset. In any
 * case, the old point is silently discarded, unreferencing its
 * model if that point was bound to a named pair (hence, possibly
 * destroying the model if this was the last reference).
 *
 * The assignment is lazy so @org1 could be not be present in @model.
 * Anyway, at the first access to this point an error will be raised
 * if the named pair is still missing.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org1_from_model(AdgADim *adim, AdgModel *model, const gchar *org1)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_from_model(point, model, org1);
    adg_adim_set_org1(adim, point);

    adg_point_destroy(point);
}

/**
 * adg_adim_get_org1:
 * @adim: an #AdgADim
 *
 * Gets the #AdgADim:org1 point. The returned point is internally owned
 * and must not be freed or modified.
 *
 * Returns: (transfer none): the first reference point
 *
 * Since: 1.0
 **/
AdgPoint *
adg_adim_get_org1(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), NULL);

    data = adim->data;

    return data->org1;
}

/**
 * adg_adim_set_org2:
 * @adim: an #AdgADim
 * @org2: the new point to use as first reference
 *
 * Sets the #AdgADim:org2 property to @org2. The old point
 * is silently discarded, unreferencing its model if that
 * point was bound to a named pair (hence, possibly destroying
 * the model if this was the last reference).
 *
 * @org2 can be <constant>NULL</constant>, in which case
 * the point is destroyed.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org2(AdgADim *adim, const AdgPoint *org2)
{
    g_return_if_fail(ADG_IS_ADIM(adim));
    g_object_set(adim, "org2", org2, NULL);
}

/**
 * adg_adim_set_org2_explicit:
 * @adim: an #AdgADim
 * @x: x coordinate of the first reference point
 * @y: y coordinate of the first reference point
 *
 * Sets the #AdgADim:org2 property to the (@x, @y) explicit
 * coordinates. The old point is silently discarded,
 * unreferencing its model if that point was bound to a named
 * pair (hence, possibly destroying the model if this was the
 * last reference).
 *
 * Since: 1.0
 **/
void
adg_adim_set_org2_explicit(AdgADim *adim, gdouble x, gdouble y)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_explicit(point, x, y);
    adg_adim_set_org2(adim, point);

    adg_point_destroy(point);
}

/**
 * adg_adim_set_org2_from_pair:
 * @adim: an #AdgADim
 * @org2: the coordinates pair of the first reference point
 *
 * Convenient function to set the #AdgADim:org2 property using a
 * pair instead of explicit coordinates.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org2_from_pair(AdgADim *adim, const CpmlPair *org2)
{
    g_return_if_fail(org2 != NULL);

    adg_adim_set_org2_explicit(adim, org2->x, org2->y);
}

/**
 * adg_adim_set_org2_from_model:
 * @adim: an #AdgADim
 * @model: the source #AdgModel
 * @org2: a named pair in @model
 *
 * Binds #AdgADim:org2 to the @org2 named pair of @model. If @model
 * is <constant>NULL</constant>, the point will be unset. In any
 * case, the old point is silently discarded, unreferencing its
 * model if that point was bound to a named pair (hence, possibly
 * destroying the model if this was the last reference).
 *
 * The assignment is lazy so @org2 could be not be present in @model.
 * Anyway, at the first access to this point an error will be raised
 * if the named pair is still missing.
 *
 * Since: 1.0
 **/
void
adg_adim_set_org2_from_model(AdgADim *adim, AdgModel *model, const gchar *org2)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_from_model(point, model, org2);
    adg_adim_set_org2(adim, point);

    adg_point_destroy(point);
}

/**
 * adg_adim_get_org2:
 * @adim: an #AdgADim
 *
 * Gets the #AdgADim:org2 point. The returned point is internally owned
 * and must not be freed or modified.
 *
 * Returns: (transfer none): the second reference point
 *
 * Since: 1.0
 **/
AdgPoint *
adg_adim_get_org2(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), NULL);

    data = adim->data;

    return data->org2;
}

/**
 * adg_adim_switch_extension1:
 * @adim: an #AdgADim entity
 * @new_state: the new state
 *
 * Shows (if @new_state is <constant>TRUE</constant>) or hides
 * (if @new_state is <constant>FALSE</constant>) the first
 * extension line of @adim.
 *
 * Since: 1.0
 **/
void
adg_adim_switch_extension1(AdgADim *adim, gboolean new_state)
{
    g_return_if_fail(ADG_IS_ADIM(adim));
    g_return_if_fail(adg_is_boolean_value(new_state));
    g_object_set(adim, "has-extension1", new_state, NULL);
}

/**
 * adg_adim_has_extension1:
 * @adim: an #AdgADim entity
 *
 * Checks if @adim should render the first extension line.
 *
 * Returns: <constant>TRUE</constant> on first extension line presents, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_adim_has_extension1(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), FALSE);

    data = adim->data;

    return data->has_extension1;
}

/**
 * adg_adim_switch_extension2:
 * @adim: an #AdgADim entity
 * @new_state: the new new_state
 *
 * Shows (if @new_state is <constant>TRUE</constant>) or hides
 * (if @new_state is <constant>FALSE</constant>) the second
 * extension line of @adim.
 *
 * Since: 1.0
 **/
void
adg_adim_switch_extension2(AdgADim *adim, gboolean new_state)
{
    g_return_if_fail(ADG_IS_ADIM(adim));
    g_return_if_fail(adg_is_boolean_value(new_state));
    g_object_set(adim, "has-extension2", new_state, NULL);
}

/**
 * adg_adim_has_extension2:
 * @adim: an #AdgADim entity
 *
 * Checks if @adim should render the second extension line.
 *
 * Returns: <constant>TRUE</constant> on first extension line presents, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_adim_has_extension2(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), FALSE);

    data = adim->data;

    return data->has_extension2;
}


static void
_adg_global_changed(AdgEntity *entity)
{
    AdgADimPrivate *data = ((AdgADim *) entity)->data;

    _adg_unset_trail((AdgADim *) entity);

    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    if (data->marker1 != NULL)
        adg_entity_global_changed((AdgEntity *) data->marker1);

    if (data->marker2 != NULL)
        adg_entity_global_changed((AdgEntity *) data->marker2);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    _adg_unset_trail((AdgADim *) entity);

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) entity;
    data = adim->data;

    _adg_dispose_trail(adim);
    _adg_dispose_markers(adim);
    _adg_unset_trail(adim);

    if (data->org1)
        adg_point_invalidate(data->org1);
    if (data->org2)
        adg_point_invalidate(data->org2);

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgDim *dim;
    AdgADim *adim;
    AdgADimPrivate *data;
    AdgAlignment *quote;
    const cairo_matrix_t *global, *local;
    CpmlPair ref1, ref2, base1, base12, base2;
    CpmlPair pair;
    CpmlExtents extents;
    AdgEntity *marker_entity;

    if (_ADG_OLD_ENTITY_CLASS->arrange)
        _ADG_OLD_ENTITY_CLASS->arrange(entity);

    dim = (AdgDim *) entity;
    if (! adg_dim_compute_geometry(dim))
        return;

    adim = (AdgADim *) entity;
    data = adim->data;
    quote = adg_dim_get_quote(dim);

    _adg_update_entities(adim);

    if (data->cairo.path.status == CAIRO_STATUS_SUCCESS) {
        AdgEntity *quote_entity = (AdgEntity *) quote;
        adg_entity_set_global_map(quote_entity, &data->quote.global_map);
        return;
    }

    global = adg_entity_get_global_matrix(entity);
    local = adg_entity_get_local_matrix(entity);
    extents.is_defined = FALSE;

    cpml_pair_copy(&ref1, (CpmlPair *) adg_dim_get_ref1(dim));
    cpml_pair_copy(&ref2, (CpmlPair *) adg_dim_get_ref2(dim));
    cpml_pair_copy(&base1, &data->point.base1);
    cpml_pair_copy(&base12, &data->point.base12);
    cpml_pair_copy(&base2, &data->point.base2);

    /* Apply the local matrix to the relevant points */
    cpml_pair_transform(&ref1, local);
    cpml_pair_transform(&ref2, local);
    cpml_pair_transform(&base1, local);
    cpml_pair_transform(&base12, local);
    cpml_pair_transform(&base2, local);

    /* Combine points and global shifts to build the path */
    pair.x = ref1.x + data->shift.from1.x;
    pair.y = ref1.y + data->shift.from1.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[6]);

    pair.x = base1.x + data->shift.base1.x;
    pair.y = base1.y + data->shift.base1.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[1]);

    pair.x += data->shift.to1.x;
    pair.y += data->shift.to1.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[8]);

    pair.x = base12.x + data->shift.base12.x;
    pair.y = base12.y + data->shift.base12.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[3]);

    pair.x = ref2.x + data->shift.from2.x;
    pair.y = ref2.y + data->shift.from2.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[10]);

    pair.x = base2.x + data->shift.base2.x;
    pair.y = base2.y + data->shift.base2.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[4]);

    pair.x += data->shift.to2.x;
    pair.y += data->shift.to2.y;
    cpml_pair_to_cairo(&pair, &data->cairo.data[12]);

    /* Play with header lengths to show or hide the extension lines */
    if (data->has_extension1) {
        data->cairo.data[7].header.length = data->has_extension2 ? 2 : 6;
    } else {
        data->cairo.data[2].header.length = data->has_extension2 ? 7 : 11;
    }

    data->cairo.path.status = CAIRO_STATUS_SUCCESS;

    /* Arrange the quote */
    if (quote != NULL) {
        AdgEntity *quote_entity;
        gdouble angle;
        cairo_matrix_t map;

        quote_entity = (AdgEntity *) quote;
        angle = adg_dim_quote_angle(dim, (data->angle1 + data->angle2) / 2 + G_PI_2);
        cpml_pair_from_cairo(&pair, &data->cairo.data[3]);

        adg_alignment_set_factor_explicit(quote, 0.5, 0);

        cairo_matrix_init_translate(&map, pair.x, pair.y);
        cairo_matrix_rotate(&map, angle);
        adg_entity_set_global_map(quote_entity, &map);
        adg_entity_arrange(quote_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(quote_entity));

        adg_matrix_copy(&data->quote.global_map, &map);
    }

    /* Arrange the trail */
    if (data->trail != NULL) {
        CpmlExtents trail_extents;
        cpml_extents_copy(&trail_extents, adg_trail_get_extents(data->trail));
        cpml_extents_transform(&trail_extents, global);
        cpml_extents_add(&extents, &trail_extents);
    } else {
        _adg_dispose_markers(adim);
    }

    /* Arrange the markers */
    if (data->marker1 != NULL) {
        marker_entity = (AdgEntity *) data->marker1;
        adg_marker_set_segment(data->marker1, data->trail, 1);
        adg_entity_local_changed(marker_entity);
        adg_entity_arrange(marker_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(marker_entity));
    }
    if (data->marker2 != NULL) {
        marker_entity = (AdgEntity *) data->marker2;
        adg_marker_set_segment(data->marker2, data->trail, 1);
        adg_entity_local_changed(marker_entity);
        adg_entity_arrange(marker_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(marker_entity));
    }

    adg_entity_set_extents(entity, &extents);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgDim *dim;
    AdgADim *adim;
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;
    const cairo_path_t *cairo_path;

    dim = (AdgDim *) entity;
    if (! adg_dim_compute_geometry(dim)) {
        /* Entity not arranged, probably due to undefined pair found */
        return;
    }

    adim = (AdgADim *) entity;
    data = adim->data;
    dim_style = _ADG_GET_DIM_STYLE(dim);

    adg_style_apply((AdgStyle *) dim_style, entity, cr);
    adg_entity_render((AdgEntity *) adg_dim_get_quote(dim), cr);

    if (data->marker1 != NULL)
        adg_entity_render((AdgEntity *) data->marker1, cr);
    if (data->marker2 != NULL)
        adg_entity_render((AdgEntity *) data->marker2, cr);

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
    AdgADim *adim;
    AdgADimPrivate *data;
    gdouble angle;

    if (! adg_dim_compute_geometry(dim)) {
        return g_strdup("undef");
    }

    adim = (AdgADim *) dim;
    data = adim->data;
    angle = (data->angle2 - data->angle1) * 180 / G_PI;

    return adg_dim_get_text(dim, angle);
}

/* With "geometry" is considered any data (point, vector or angle)
 * that can be cached: this is strictly related on how the arrange()
 * method works */
static gboolean
_adg_compute_geometry(AdgDim *dim)
{
    AdgADim *adim;
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;
    gdouble from_offset, to_offset;
    gdouble spacing, level;
    CpmlVector vector[3];
    CpmlPair center;
    gdouble distance;

    adim = (AdgADim *) dim;
    if (! _adg_get_info(adim, vector, &center, &distance))
        return FALSE;

    data = adim->data;
    dim_style = _ADG_GET_DIM_STYLE(adim);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level((AdgDim *) adim);

    /* shift.from1 */
    cpml_vector_set_length(&vector[0], from_offset);
    cpml_pair_copy(&data->shift.from1, &vector[0]);

    /* shift.base1 */
    cpml_vector_set_length(&vector[0], level * spacing);
    cpml_pair_copy(&data->shift.base1, &vector[0]);

    /* shift.to1 */
    cpml_vector_set_length(&vector[0], to_offset);
    cpml_pair_copy(&data->shift.to1, &vector[0]);

    /* shift.from2 */
    cpml_vector_set_length(&vector[2], from_offset);
    cpml_pair_copy(&data->shift.from2, &vector[2]);

    /* shift.base2 */
    cpml_vector_set_length(&vector[2], level * spacing);
    cpml_pair_copy(&data->shift.base2, &vector[2]);

    /* shift.to2 */
    cpml_vector_set_length(&vector[2], to_offset);
    cpml_pair_copy(&data->shift.to2, &vector[2]);

    /* shift.base12 */
    cpml_vector_set_length(&vector[1], level * spacing);
    cpml_pair_copy(&data->shift.base12, &vector[1]);

    /* Distance can be 0, so the following will leave the
     * vector array in undefined state */

    /* point.base1 */
    cpml_vector_set_length(&vector[0], distance);
    data->point.base1.x = vector[0].x + center.x;
    data->point.base1.y = vector[0].y + center.y;

    /* point.base2 */
    cpml_vector_set_length(&vector[2], distance);
    data->point.base2.x = vector[2].x + center.x;
    data->point.base2.y = vector[2].y + center.y;

    /* point.base12 */
    cpml_vector_set_length(&vector[1], distance);
    data->point.base12.x = vector[1].x + center.x;
    data->point.base12.y = vector[1].y + center.y;

    return TRUE;
}

static void
_adg_update_entities(AdgADim *adim)
{
    AdgEntity *entity;
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;

    entity = (AdgEntity *) adim;
    data = adim->data;
    dim_style = _ADG_GET_DIM_STYLE(adim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(_adg_trail_callback, adim);

    if (data->marker1 == NULL) {
        data->marker1 = adg_dim_style_marker1_new(dim_style);
        adg_entity_set_parent((AdgEntity *) data->marker1, entity);
    }

    if (data->marker2 == NULL) {
        data->marker2 = adg_dim_style_marker2_new(dim_style);
        adg_entity_set_parent((AdgEntity *) data->marker2, entity);
    }
}

static void
_adg_unset_trail(AdgADim *adim)
{
    AdgADimPrivate *data = adim->data;

    if (data->trail != NULL)
        adg_model_clear((AdgModel *) data->trail);

    data->cairo.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}

static void
_adg_dispose_trail(AdgADim *adim)
{
    AdgADimPrivate *data = adim->data;

    if (data->trail != NULL) {
        g_object_unref(data->trail);
        data->trail = NULL;
    }
}

static void
_adg_dispose_markers(AdgADim *adim)
{
    AdgADimPrivate *data = adim->data;

    if (data->marker1 != NULL) {
        g_object_unref(data->marker1);
        data->marker1 = NULL;
    }

    if (data->marker2 != NULL) {
        g_object_unref(data->marker2);
        data->marker2 = NULL;
    }
}

static gboolean
_adg_get_info(AdgADim *adim, CpmlVector vector[],
              CpmlPair *center, gdouble *distance)
{
    AdgDim *dim;
    AdgADimPrivate *data;
    AdgPoint *ref1_point, *ref2_point, *pos_point;
    const CpmlPair *ref1, *ref2, *pos;
    const CpmlPair *org1, *org2;
    gdouble factor;

    dim = (AdgDim *) adim;
    data = adim->data;
    ref1_point = adg_dim_get_ref1(dim);
    ref2_point = adg_dim_get_ref2(dim);
    pos_point  = adg_dim_get_pos(dim);

    if (! adg_point_update(ref1_point)) {
        adg_dim_geometry_missing(dim, "ref1");
        return FALSE;
    }

    if (! adg_point_update(ref2_point)) {
        adg_dim_geometry_missing(dim, "ref2");
        return FALSE;
    }

    if (! adg_point_update(pos_point)) {
        adg_dim_geometry_missing(dim, "pos");
        return FALSE;
    }

    if (! adg_point_update(data->org1)) {
        adg_dim_geometry_missing(dim, "org1");
        return FALSE;
    }

    if (! adg_point_update(data->org2)) {
        adg_dim_geometry_missing(dim, "org2");
        return FALSE;
    }

    ref1 = (CpmlPair *) ref1_point;
    ref2 = (CpmlPair *) ref2_point;
    pos  = (CpmlPair *) pos_point;
    org1 = (CpmlPair *) data->org1;
    org2 = (CpmlPair *) data->org2;

    /* Check if the given points have valid coordinates */
    if (cpml_pair_equal(ref1, org1)) {
        adg_dim_geometry_coincident(dim, "ref1", "org1", ref1);
        return FALSE;
    }

    if (cpml_pair_equal(ref2, org2)) {
        adg_dim_geometry_coincident(dim, "ref2", "org2", ref2);
        return FALSE;
    }

    vector[0].x = ref1->x - org1->x;
    vector[0].y = ref1->y - org1->y;
    vector[2].x = ref2->x - org2->x;
    vector[2].y = ref2->y - org2->y;

    factor = vector[0].x * vector[2].y - vector[0].y * vector[2].x;
    if (factor == 0) {
        adg_dim_set_geometry_notice(dim, _("Trying to set an angular dimension on parallel lines"));
        return FALSE;
    }

    factor = ((ref1->y - ref2->y) * vector[2].x -
              (ref1->x - ref2->x) * vector[2].y) / factor;

    center->x = ref1->x + vector[0].x * factor;
    center->y = ref1->y + vector[0].y * factor;
    *distance = cpml_pair_distance(center, pos);
    data->angle1 = cpml_vector_angle(&vector[0]);
    data->angle2 = cpml_vector_angle(&vector[2]);
    while (data->angle2 < data->angle1)
        data->angle2 += G_PI * 2;

    cpml_vector_from_angle(&vector[1], (data->angle1 + data->angle2) / 2);

    return TRUE;
}

static cairo_path_t *
_adg_trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) user_data;
    data = adim->data;

    return &data->cairo.path;
}
