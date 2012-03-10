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
 * SECTION:adg-dim
 * @short_description: Root abstract class for all dimension entities
 *
 * The #AdgDim class is the base stub of all the dimension entities.
 *
 * Since: 1.0
 **/

/**
 * AdgDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-text-internal.h"

#include "adg-container.h"
#include "adg-alignment.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-point.h"
#include "adg-marker.h"
#include "adg-dim-style.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"

#include "adg-dim.h"
#include "adg-dim-private.h"



#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_dim_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_dim_parent_class)


G_DEFINE_ABSTRACT_TYPE(AdgDim, adg_dim, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_DIM_DRESS,
    PROP_REF1,
    PROP_REF2,
    PROP_POS,
    PROP_LEVEL,
    PROP_OUTSIDE,
    PROP_DETACHED,
    PROP_VALUE,
    PROP_MIN,
    PROP_MAX
};


static void     _adg_dispose            (GObject        *object);
static void     _adg_finalize           (GObject        *object);
static void     _adg_get_property       (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     _adg_set_property       (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     _adg_global_changed     (AdgEntity      *entity);
static void     _adg_local_changed      (AdgEntity      *entity);
static void     _adg_invalidate         (AdgEntity      *entity);
static void     _adg_arrange            (AdgEntity      *entity);
static gchar *  _adg_default_value      (AdgDim         *dim);
static gdouble  _adg_quote_angle        (gdouble         angle);
static gboolean _adg_set_outside        (AdgDim         *dim,
                                         AdgThreeState   outside);
static gboolean _adg_set_detached       (AdgDim         *dim,
                                         AdgThreeState   detached);
static gboolean _adg_set_value          (AdgDim         *dim,
                                         const gchar    *value);
static gboolean _adg_set_min            (AdgDim         *dim,
                                         const gchar    *min);
static gboolean _adg_set_max            (AdgDim         *dim,
                                         const gchar    *max);


static void
adg_dim_class_init(AdgDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgDimPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;

    klass->quote_angle = _adg_quote_angle;
    klass->default_value = _adg_default_value;

    param = adg_param_spec_dress("dim-dress",
                                 P_("Dimension Dress"),
                                 P_("The dress to use for rendering this dimension"),
                                 ADG_DRESS_DIMENSION,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DIM_DRESS, param);

    param = g_param_spec_boxed("ref1",
                               P_("First Reference"),
                               P_("First reference point of the dimension"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_REF1, param);

    param = g_param_spec_boxed("ref2",
                               P_("Second Reference"),
                               P_("Second reference point of the dimension"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_REF2, param);

    param = g_param_spec_boxed("pos",
                               P_("Position"),
                               P_("The reference position of the quote: it will be combined with \"level\" to get the real quote position"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS, param);

    param = g_param_spec_double("level",
                                P_("Level"),
                                P_("The dimension level, that is the factor to multiply the baseline spacing (defined in the dimension style) to get the offset from pos where the quote should be rendered"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LEVEL, param);

    param = g_param_spec_enum("outside",
                              P_("Outside"),
                              P_("Whether the arrows must be inside the extension lines (ADG_THREE_STATE_OFF), must be extended outside the extension lines (ADG_THREE_STATE_ON) or should be automatically handled depending on the available space"),
                              ADG_TYPE_THREE_STATE, ADG_THREE_STATE_UNKNOWN,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_OUTSIDE, param);

    param = g_param_spec_enum("detached",
                              P_("Detached Quote"),
                              P_("Where the quote must be positioned: in the middle of the base line (ADG_THREE_STATE_OFF), near the pos point (ADG_THREE_STATE_ON) or should be automatically deducted depending on the available space"),
                              ADG_TYPE_THREE_STATE, ADG_THREE_STATE_UNKNOWN,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DETACHED, param);

    param = g_param_spec_string("value",
                                P_("Value Template"),
                                P_("The template string to be used for generating the set value of the quote"),
                                NULL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_VALUE, param);

    param = g_param_spec_string("min",
                                P_("Minimum Value or Low Tolerance"),
                                P_("The minimum value allowed or the lowest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MIN, param);

    param = g_param_spec_string("max",
                                P_("Maximum Value or High Tolerance"),
                                P_("The maximum value allowed or the highest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MAX, param);
}

static void
adg_dim_init(AdgDim *dim)
{
    AdgDimPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(dim, ADG_TYPE_DIM,
                                                      AdgDimPrivate);

    data->dim_dress = ADG_DRESS_DIMENSION;
    data->ref1 = NULL;
    data->ref2 = NULL;
    data->pos = NULL;
    data->level = 1;
    data->outside = ADG_THREE_STATE_UNKNOWN;
    data->detached = ADG_THREE_STATE_UNKNOWN;
    data->min = NULL;
    data->max = NULL;

#if 0
    /* These ones are G_PARAM_CONSTRUCT, so initialized by GObject */
    data->value = NULL
#endif

    dim->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgEntity *entity;
    AdgDimPrivate *data;

    entity = (AdgEntity *) object;
    data = ((AdgDim *) object)->data;

    if (data->quote.entity) {
        g_object_unref(data->quote.entity);
        data->quote.entity = NULL;
    }

    if (data->ref1)
        data->ref1 = adg_entity_point(entity, data->ref1, NULL);

    if (data->ref2)
        data->ref2 = adg_entity_point(entity, data->ref2, NULL);

    if (data->pos)
        data->pos = adg_entity_point(entity, data->pos, NULL);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_finalize(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    g_free(data->value);
    g_free(data->min);
    g_free(data->max);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    switch (prop_id) {
    case PROP_DIM_DRESS:
        g_value_set_int(value, data->dim_dress);
        break;
    case PROP_REF1:
        g_value_set_boxed(value, data->ref1);
        break;
    case PROP_REF2:
        g_value_set_boxed(value, data->ref2);
        break;
    case PROP_POS:
        g_value_set_boxed(value, data->pos);
        break;
    case PROP_LEVEL:
        g_value_set_double(value, data->level);
        break;
    case PROP_OUTSIDE:
        g_value_set_enum(value, data->outside);
        break;
    case PROP_DETACHED:
        g_value_set_enum(value, data->detached);
        break;
    case PROP_VALUE:
        g_value_set_string(value, data->value);
        break;
    case PROP_MIN:
        g_value_set_string(value, data->min);
        break;
    case PROP_MAX:
        g_value_set_string(value, data->max);
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
    AdgDim *dim;
    AdgDimPrivate *data;

    entity = (AdgEntity *) object;
    dim = (AdgDim *) object;
    data = dim->data;

    switch (prop_id) {
    case PROP_DIM_DRESS:
        data->dim_dress = g_value_get_int(value);
        break;
    case PROP_REF1:
        data->ref1 = adg_entity_point(entity, data->ref1,
                                      g_value_get_boxed(value));
        break;
    case PROP_REF2:
        data->ref2 = adg_entity_point(entity, data->ref2,
                                      g_value_get_boxed(value));
        break;
    case PROP_POS:
        data->pos = adg_entity_point(entity, data->pos,
                                     g_value_get_boxed(value));
        break;
    case PROP_LEVEL:
        data->level = g_value_get_double(value);
        break;
    case PROP_OUTSIDE:
        _adg_set_outside(dim, g_value_get_enum(value));
        break;
    case PROP_DETACHED:
        _adg_set_detached(dim, g_value_get_enum(value));
        break;
    case PROP_VALUE:
        _adg_set_value(dim, g_value_get_string(value));
        break;
    case PROP_MIN:
        _adg_set_min(dim, g_value_get_string(value));
        break;
    case PROP_MAX:
        _adg_set_max(dim, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_dim_set_dim_dress:
 * @dim: an #AdgDim
 * @dress: the new #AdgDress to use
 *
 * Sets a new dimension dress to @dim. The new dress must be
 * related to the original dress for this property: you cannot
 * set a dress used for line styles to a dress managing fonts.
 *
 * The check is done by calling adg_dress_are_related() with
 * @dress and the previous dress as arguments. Check out its
 * documentation for details on what is a related dress.
 *
 * Since: 1.0
 **/
void
adg_dim_set_dim_dress(AdgDim *dim, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM(dim));
    g_object_set(dim, "dim-dress", dress, NULL);
}

/**
 * adg_dim_get_dim_dress:
 * @dim: an #AdgDim
 *
 * Gets the dimension dress to be used in rendering @dim.
 *
 * Returns: (transfer none): the current dimension dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_dim_get_dim_dress(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), ADG_DRESS_UNDEFINED);

    data = dim->data;

    return data->dim_dress;
}

/**
 * adg_dim_set_ref1:
 * @dim: an #AdgDim
 * @ref1: the new point to use as first reference
 *
 * Sets the #AdgDim:ref1 property to @ref1. The old point
 * is silently discarded, unreferencing its model if that
 * point was bound to a named pair (hence, possibly destroying
 * the model if this was the last reference).
 *
 * @ref1 can be %NULL, in which case the point is destroyed.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref1(AdgDim *dim, const AdgPoint *ref1)
{
    g_return_if_fail(ADG_IS_DIM(dim));
    g_object_set(dim, "ref1", ref1, NULL);
}

/**
 * adg_dim_set_ref1_explicit:
 * @dim: an #AdgDim
 * @x: x coordinate of the first reference point
 * @y: y coordinate of the first reference point
 *
 * Sets the #AdgDim:ref1 property to the (@x, @y) explicit
 * coordinates. The old point is silently discarded,
 * unreferencing its model if that point was bound to a named
 * pair (hence, possibly destroying the model if this was the
 * last reference).
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref1_explicit(AdgDim *dim, gdouble x, gdouble y)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_explicit(point, x, y);
    adg_dim_set_ref1(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_set_ref1_from_pair:
 * @dim: an #AdgDim
 * @ref1: the coordinates pair of the first reference point
 *
 * Convenient function to set the #AdgDim:ref1 property using a
 * pair instead of explicit coordinates.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref1_from_pair(AdgDim *dim, const AdgPair *ref1)
{
    g_return_if_fail(ref1 != NULL);

    adg_dim_set_ref1_explicit(dim, ref1->x, ref1->y);
}

/**
 * adg_dim_set_ref1_from_model:
 * @dim: an #AdgDim
 * @model: the source #AdgModel
 * @ref1: a named pair in @model
 *
 * Binds #AdgDim:ref1 to the @ref1 named pair of @model. If @model
 * is %NULL, the point will be unset. In any case, the old point
 * is silently discarded, unreferencing its model if that point
 * was bound to a named pair (hence, possibly destroying the model
 * if this was the last reference).
 *
 * The assignment is lazy so @ref1 could be not be present in @model.
 * Anyway, at the first access to this point an error will be raised
 * if the named pair is still missing.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref1_from_model(AdgDim *dim, AdgModel *model, const gchar *ref1)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_from_model(point, model, ref1);
    adg_dim_set_ref1(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_get_ref1:
 * @dim: an #AdgDim
 *
 * Gets the #AdgDim:ref1 point of @dim.
 *
 * The returned point is internally owned and must not be freed
 * or modified. Anyway it is not const because a call to
 * adg_point_get_pair() with the returned value must be able to
 * modify the internal cache.
 *
 * Returns: (transfer none): the first reference point.
 *
 * Since: 1.0
 **/
AdgPoint *
adg_dim_get_ref1(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->ref1;
}

/**
 * adg_dim_set_ref2:
 * @dim: an #AdgDim
 * @ref2: the new point to use as second reference
 *
 * Sets the #AdgDim:ref2 property to @ref2. The old point
 * is silently discarded, unreferencing its model if that
 * point was bound to a named pair (hence, possibly destroying
 * the model if it was the last reference).
 *
 * @ref2 can be %NULL, in which case the point is destroyed.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref2(AdgDim *dim, const AdgPoint *ref2)
{
    g_return_if_fail(ADG_IS_DIM(dim));
    g_object_set(dim, "ref2", ref2, NULL);
}

/**
 * adg_dim_set_ref2_explicit:
 * @dim: an #AdgDim
 * @x: x coordinate of the second reference point
 * @y: y coordinate of the second reference point
 *
 * Sets the #AdgDim:ref2 property to the (@x, @y) explicit
 * coordinates. The old point is silently discarded,
 * unreferencing its model if that point was bound to a named
 * pair (hence, possibly destroying the model if this was the
 * last reference).
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref2_explicit(AdgDim *dim, gdouble x, gdouble y)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_explicit(point, x, y);
    adg_dim_set_ref2(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_set_ref2_from_pair:
 * @dim: an #AdgDim
 * @ref2: the coordinates pair of the second reference point
 *
 * Convenient function to set the #AdgDim:ref2 property using a
 * pair instead of explicit coordinates.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref2_from_pair(AdgDim *dim, const AdgPair *ref2)
{
    g_return_if_fail(ref2 != NULL);

    adg_dim_set_ref2_explicit(dim, ref2->x, ref2->y);
}

/**
 * adg_dim_set_ref2_from_model:
 * @dim: an #AdgDim
 * @model: the source #AdgModel
 * @ref2: a named pair in @model
 *
 * Binds #AdgDim:ref2 to the @ref2 named pair of @model. If @model
 * is %NULL, the point will be unset. In any case, the old point
 * is silently discarded, unreferencing its model if that point
 * was bound to a named pair (hence, possibly destroying the model
 * if this was the last reference).
 *
 * The assignment is lazy so @ref2 could be not be present in @model.
 * Anyway, at the first access to this point an error will be raised
 * if the named pair is still missing.
 *
 * Since: 1.0
 **/
void
adg_dim_set_ref2_from_model(AdgDim *dim, AdgModel *model, const gchar *ref2)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_from_model(point, model, ref2);
    adg_dim_set_ref2(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_get_ref2:
 * @dim: an #AdgDim
 *
 * Gets the #AdgDim:ref2 point of @dim.
 *
 * The returned point is internally owned and must not be freed
 * or modified. Anyway it is not const because a call to
 * adg_point_get_pair() with the returned value must be able to
 * modify the internal cache.
 *
 * Returns: (transfer none): the second reference point.
 *
 * Since: 1.0
 **/
AdgPoint *
adg_dim_get_ref2(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->ref2;
}

/**
 * adg_dim_set_pos:
 * @dim: an #AdgDim
 * @pos: the new point to use as position
 *
 * Sets the #AdgDim:pos property of @dim to @pos. The old point
 * is silently discarded, unreferencing its model if that
 * point was bound to a named pair (hence, possibly destroying
 * the model if it was the last reference).
 *
 * @pos can be %NULL, in which case the point is destroyed.
 *
 * Since: 1.0
 **/
void
adg_dim_set_pos(AdgDim *dim, const AdgPoint *pos)
{
    g_return_if_fail(ADG_IS_DIM(dim));
    g_object_set(dim, "pos", pos, NULL);
}

/**
 * adg_dim_set_pos_explicit:
 * @dim: an #AdgDim
 * @x: x coordinate of the position
 * @y: y coordinate of the position
 *
 * Sets the #AdgDim:pos property to the (@x, @y) explicit
 * coordinates. The old point is silently discarded,
 * unreferencing its model if that point was bound to a named
 * pair (hence, possibly destroying the model if this was the
 * last reference).
 *
 * Since: 1.0
 **/
void
adg_dim_set_pos_explicit(AdgDim *dim, gdouble x, gdouble y)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_explicit(point, x, y);
    adg_dim_set_pos(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_set_pos_from_pair:
 * @dim: an #AdgDim
 * @pos: the coordinates pair of the position point
 *
 * Convenient function to set the #AdgDim:pos property using a
 * pair instead of explicit coordinates.
 *
 * Since: 1.0
 **/
void
adg_dim_set_pos_from_pair(AdgDim *dim, const AdgPair *pos)
{
    g_return_if_fail(pos != NULL);

    adg_dim_set_pos_explicit(dim, pos->x, pos->y);
}

/**
 * adg_dim_set_pos_from_model:
 * @dim: an #AdgDim
 * @model: the source #AdgModel
 * @pos: a named pair in @model
 *
 * Binds #AdgDim:pos to the @pos named pair of @model. If @model
 * is %NULL, the point will be unset. In any case, the old point
 * is silently discarded, unreferencing its model if that point
 * was bound to a named pair (hence, possibly destroying the model
 * if this was the last reference).
 *
 * The assignment is lazy so @pos could be not be present in @model.
 * Anyway, at the first access to this point an error will be raised
 * if the named pair is still missing.
 *
 * Since: 1.0
 **/
void
adg_dim_set_pos_from_model(AdgDim *dim, AdgModel *model, const gchar *pos)
{
    AdgPoint *point = adg_point_new();

    adg_point_set_pair_from_model(point, model, pos);
    adg_dim_set_pos(dim, point);

    adg_point_destroy(point);
}

/**
 * adg_dim_get_pos:
 * @dim: an #AdgDim
 *
 * Gets the #AdgDim:pos point of @dim.
 *
 * The returned point is internally owned and must not be freed
 * or modified. Anyway it is not const because a call to
 * adg_point_get_pair() with the returned value must be able to
 * modify the internal cache.
 *
 * Returns: (transfer none): the position point.
 *
 * Since: 1.0
 **/
AdgPoint *
adg_dim_get_pos(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->pos;
}

/**
 * adg_dim_set_level:
 * @dim: an #AdgDim
 * @level: the new level
 *
 * Sets a new level for this dimension. The level is used to
 * stack the quotes using a spacing value from dim_style
 * (specified in global space).
 *
 * Since: 1.0
 **/
void
adg_dim_set_level(AdgDim *dim, gdouble level)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    data->level = level;

    g_object_notify((GObject *) dim, "level");
}

/**
 * adg_dim_get_level:
 * @dim: an #AdgDim
 *
 * Gets the level of this dimension.
 *
 * Returns: the level value.
 *
 * Since: 1.0
 **/
gdouble
adg_dim_get_level(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), 0);

    data = dim->data;

    return data->level;
}

/**
 * adg_dim_set_outside:
 * @dim: an #AdgDim
 * @outside: the new outside state
 *
 * Sets a new state for the #AdgDim:outside flag: check the property
 * documentation for further details.
 *
 * Since: 1.0
 **/
void
adg_dim_set_outside(AdgDim *dim, AdgThreeState outside)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (_adg_set_outside(dim, outside))
        g_object_notify((GObject *) dim, "outside");
}

/**
 * adg_dim_get_outside:
 * @dim: an #AdgDim
 *
 * Gets the state of the #AdgDim:outside property: check the property
 * documentation for further details.
 *
 * Returns: the current flag state.
 *
 * Since: 1.0
 **/
AdgThreeState
adg_dim_get_outside(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), ADG_THREE_STATE_UNKNOWN);

    data = dim->data;

    return data->outside;
}

/**
 * adg_dim_set_detached:
 * @dim: an #AdgDim
 * @detached: the new detached state
 *
 * Sets a new state for the #AdgDim:detached flag: check the property
 * documentation for further details.
 *
 * This is used only by dimensions where detaching has meaning.
 * In some cases, such as with #AdgRDim dimensions, this property is
 * not used.
 *
 * Since: 1.0
 **/
void
adg_dim_set_detached(AdgDim *dim, AdgThreeState detached)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (_adg_set_detached(dim, detached))
        g_object_notify((GObject *) dim, "detached");
}

/**
 * adg_dim_get_detached:
 * @dim: an #AdgDim
 *
 * Gets the state of the #AdgDim:detached property: check the property
 * documentation for further details.
 *
 * Returns: the current flag state.
 *
 * Since: 1.0
 **/
AdgThreeState
adg_dim_get_detached(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), ADG_THREE_STATE_UNKNOWN);

    data = dim->data;

    return data->detached;
}

/**
 * adg_dim_set_value:
 * @dim: an #AdgDim
 * @value: the value text
 *
 * Explicitely sets the text to use as value. If @value is %NULL or
 * was never set, an automatic text is calculated using the format
 * specified in the current #AdgDimStyle and getting its value by
 * calling the default_value() virtual method.
 *
 * Inside the template string, the "<>" tag (or whatever specified
 * by the #AdgDimStyle:number-tag property) is substituted with the
 * string returned by the default_value() virtual method.
 *
 * Since: 1.0
 **/
void
adg_dim_set_value(AdgDim *dim, const gchar *value)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (_adg_set_value(dim, value))
        g_object_notify((GObject *) dim, "value");
}

/**
 * adg_dim_get_value:
 * @dim: an #AdgDim
 *
 * Gets the value text. The string is internally owned and
 * must not be freed or modified.
 *
 * Returns: (transfer none): the value text.
 *
 * Since: 1.0
 **/
const gchar *
adg_dim_get_value(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->value;
}

/**
 * adg_dim_set_limits:
 * @dim: an #AdgDim
 * @min: the new minumum value
 * @max: the new maximum value
 *
 * Shortcut to set both the limits at once.
 *
 * Since: 1.0
 **/
void
adg_dim_set_limits(AdgDim *dim, const gchar *min, const gchar *max)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_object_freeze_notify((GObject *) dim);
    adg_dim_set_min(dim, min);
    adg_dim_set_max(dim, max);
    g_object_thaw_notify((GObject *) dim);
}

/**
 * adg_dim_set_min:
 * @dim: an #AdgDim
 * @min: the new minimum limit
 *
 * Sets the minimum value. Use %NULL as @min to disable it.
 *
 * Since: 1.0
 **/
void
adg_dim_set_min(AdgDim *dim, const gchar *min)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (_adg_set_min(dim, min))
        g_object_notify((GObject *) dim, "min");
}

/**
 * adg_dim_get_min:
 * @dim: an #AdgDim
 *
 * Gets the minimum value text or %NULL on minimum value disabled.
 *
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: (transfer none): the mimimum value text.
 *
 * Since: 1.0
 **/
const gchar *
adg_dim_get_min(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->min;
}

/**
 * adg_dim_set_max:
 * @dim: an #AdgDim
 * @max: the new maximum value
 *
 * Sets the maximum value. Use %NULL as @max to disable it.
 *
 * Since: 1.0
 **/
void
adg_dim_set_max(AdgDim *dim, const gchar *max)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (_adg_set_max(dim, max))
        g_object_notify((GObject *) dim, "max");
}

/**
 * adg_dim_get_max:
 * @dim: an #AdgDim
 *
 * Gets the maximum value text or %NULL on maximum value disabled.
 *
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: (transfer none): the maximum value text.
 *
 * Since: 1.0
 **/
const gchar *
adg_dim_get_max(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->max;
}

/**
 * adg_dim_get_quote:
 * @dim: an #AdgDim
 *
 * Gets the quote entity, if any. This function is valid only after
 * the #AdgDim implementation of the arrange() virtual method has
 * been called.
 *
 * The returned entity is owned by @dim and should not be
 * modified or freed.
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Returns: (transfer none): the quote entity.
 *
 * Since: 1.0
 **/
AdgAlignment *
adg_dim_get_quote(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->quote.entity;
}

/**
 * adg_dim_quote_angle:
 * @dim: an #AdgDim
 * @angle: an angle (in radians)
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Converts @angle accordling to the style of @dim. Any quote angle
 * should be validated by this method because every dimensioning
 * style has its own convention regardling the text rotation.
 *
 * Returns: the angle to use (always in radians).
 *
 * Since: 1.0
 **/
gdouble
adg_dim_quote_angle(AdgDim *dim, gdouble angle)
{
    AdgDimClass *klass;

    g_return_val_if_fail(ADG_IS_DIM(dim), angle);

    klass = ADG_DIM_GET_CLASS(dim);

    if (klass->quote_angle == NULL)
        return angle;

    return klass->quote_angle(angle);
}


static void
_adg_global_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    if (data->quote.entity)
        adg_entity_global_changed((AdgEntity *) data->quote.entity);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    if (data->quote.entity)
        adg_entity_local_changed((AdgEntity *) data->quote.entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (data->quote.value) {
        g_object_unref(data->quote.value);
        data->quote.value = NULL;
    }
    if (data->quote.entity)
        adg_entity_invalidate((AdgEntity *) data->quote.entity);
    if (data->ref1)
        adg_point_invalidate(data->ref1);
    if (data->ref2)
        adg_point_invalidate(data->ref2);
    if (data->pos)
        adg_point_invalidate(data->pos);

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgDim *dim;
    AdgDimPrivate *data;
    AdgEntity *quote_entity;
    AdgContainer *quote_container;
    AdgEntity *value_entity;
    AdgEntity *min_entity;
    AdgEntity *max_entity;
    const AdgPair *shift;
    AdgMatrix map;

    dim = (AdgDim *) entity;
    data = dim->data;

    /* Resolve the dim style */
    if (data->dim_style == NULL)
        data->dim_style = (AdgDimStyle *)
            adg_entity_style(entity, data->dim_dress);

    if (data->quote.entity == NULL)
        data->quote.entity = g_object_new(ADG_TYPE_ALIGNMENT,
                                          "local-method", ADG_MIX_NONE,
                                          "parent", dim, NULL);

    quote_entity = (AdgEntity *) data->quote.entity;
    quote_container = (AdgContainer *) data->quote.entity;

    if (data->quote.value == NULL) {
        AdgDimClass *klass;
        AdgDress dress;
        const gchar *tag;
        gchar *value;
        gchar *text;

        klass = ADG_DIM_GET_CLASS(dim);
        dress = adg_dim_style_get_value_dress(data->dim_style);
        tag = adg_dim_style_get_number_tag(data->dim_style);
        value = klass->default_value ? klass->default_value(dim) : NULL;

        data->quote.value = g_object_new(ADG_TYPE_BEST_TEXT,
                                         "local-method", ADG_MIX_PARENT,
                                         "font-dress", dress, NULL);
        adg_container_add(quote_container, (AdgEntity *) data->quote.value);

        if (data->value)
            text = adg_string_replace(data->value, tag, value);
        else
            text = g_strdup(value);

        g_free(value);

        adg_textual_set_text(data->quote.value, text);
        g_free(text);
    }

    if (data->quote.min == NULL && data->min != NULL) {
        AdgDress dress = adg_dim_style_get_min_dress(data->dim_style);

        data->quote.min = g_object_new(ADG_TYPE_BEST_TEXT,
                                       "local-method", ADG_MIX_PARENT,
                                       "font-dress", dress, NULL);

        adg_container_add(quote_container, (AdgEntity *) data->quote.min);
        adg_textual_set_text(data->quote.min, data->min);
    }

    if (data->quote.max == NULL && data->max != NULL) {
        AdgDress dress = adg_dim_style_get_max_dress(data->dim_style);

        data->quote.max = g_object_new(ADG_TYPE_BEST_TEXT,
                                       "local-method", ADG_MIX_PARENT,
                                       "font-dress", dress, NULL);

        adg_container_add(quote_container, (AdgEntity *) data->quote.max);
        adg_textual_set_text(data->quote.max, data->max);
    }

    value_entity = (AdgEntity *) data->quote.value;
    min_entity = (AdgEntity *) data->quote.min;
    max_entity = (AdgEntity *) data->quote.max;
    shift = adg_dim_style_get_quote_shift(data->dim_style);

    adg_entity_set_global_map(quote_entity, adg_matrix_identity());
    adg_entity_global_changed(quote_entity);

    cairo_matrix_init_translate(&map, 0, shift->y);
    adg_entity_set_global_map(value_entity, &map);
    adg_entity_arrange(value_entity);

    /* Limit values (min and max) */
    if (min_entity != NULL || max_entity != NULL) {
        const AdgPair *limits_shift;
        gdouble spacing;
        AdgPair size;
        AdgPair org_min, org_max;

        limits_shift = adg_dim_style_get_limits_shift(data->dim_style);
        spacing = adg_dim_style_get_limits_spacing(data->dim_style);
        size = adg_entity_get_extents(value_entity)->size;
        org_min.x = size.x + limits_shift->x;
        org_min.y = -size.y / 2 + limits_shift->y;
        org_max = org_min;

        if (min_entity && max_entity) {
            /* Prearrange the min entity to get its extents */
            adg_entity_arrange(min_entity);
            size = adg_entity_get_extents(min_entity)->size;

            org_min.y += spacing / 2;
            org_max.y = org_min.y - size.y - spacing / 2;
        }

        if (min_entity != NULL) {
            cairo_matrix_init_translate(&map, org_min.x, org_min.y);
            adg_entity_set_global_map(min_entity, &map);
            adg_entity_arrange(min_entity);
        }

        if (max_entity != NULL) {
            cairo_matrix_init_translate(&map, org_max.x, org_max.y);
            adg_entity_set_global_map(max_entity, &map);
            adg_entity_arrange(max_entity);
        }
    }
}

static gchar *
_adg_default_value(AdgDim *dim)
{
    g_warning(_("AdgDim::default_value not implemented for `%s'"),
              g_type_name(G_TYPE_FROM_INSTANCE(dim)));
    return g_strdup("undef");
}

static gdouble
_adg_quote_angle(gdouble angle)
{
    angle = cpml_angle(angle);

    if (angle > G_PI_4 * 4 / 3 || angle <= -G_PI_4 * 3)
        angle = cpml_angle(angle + G_PI);

    return angle;
}

static gboolean
_adg_set_outside(AdgDim *dim, AdgThreeState outside)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(adg_is_enum_value(outside, ADG_TYPE_THREE_STATE),
                         FALSE);

    data = dim->data;

    if (data->outside == outside)
        return FALSE;

    data->outside = outside;

    return TRUE;
}

static gboolean
_adg_set_detached(AdgDim *dim, AdgThreeState detached)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(adg_is_enum_value(detached, ADG_TYPE_THREE_STATE),
                         FALSE);

    data = dim->data;

    if (data->detached == detached)
        return FALSE;

    data->detached = detached;

    return TRUE;
}

static gboolean
_adg_set_value(AdgDim *dim, const gchar *value)
{
    AdgDimPrivate *data;

    data = dim->data;

    if (g_strcmp0(value, data->value) == 0)
        return FALSE;

    g_free(data->value);
    data->value = g_strdup(value);

    if (data->quote.value) {
        g_object_unref(data->quote.value);
        data->quote.value = NULL;
    }

    return TRUE;
}

static gboolean
_adg_set_min(AdgDim *dim, const gchar *min)
{
    AdgDimPrivate *data = dim->data;

    if (g_strcmp0(min, data->min) == 0)
        return FALSE;

    g_free(data->min);
    data->min = g_strdup(min);

    if (data->quote.min) {
        g_object_unref(data->quote.min);
        data->quote.min = NULL;
    }

    return TRUE;
}

static gboolean
_adg_set_max(AdgDim *dim, const gchar *max)
{
    AdgDimPrivate *data = dim->data;

    if (g_strcmp0(max, data->max) == 0)
        return FALSE;

    g_free(data->max);
    data->max = g_strdup(max);

    if (data->quote.max) {
        g_object_unref(data->quote.max);
        data->quote.max = NULL;
    }

    return TRUE;
}
