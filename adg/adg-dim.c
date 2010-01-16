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
 * SECTION:adg-dim
 * @short_description: Root abstract class for all dimension entities
 *
 * The #AdgDim class is the base stub of all the dimension entities.
 **/

/**
 * AdgDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-dim.h"
#include "adg-dim-private.h"
#include "adg-dim-style.h"
#include "adg-dress-builtins.h"
#include "adg-toy-text.h"
#include "adg-type-builtins.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_dim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_dim_parent_class)


enum {
    PROP_0,
    PROP_DIM_DRESS,
    PROP_REF1,
    PROP_REF2,
    PROP_POS,
    PROP_LEVEL,
    PROP_OUTSIDE,
    PROP_VALUE,
    PROP_MIN,
    PROP_MAX
};


static void     dispose                 (GObject        *object);
static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     global_changed          (AdgEntity      *entity);
static void     local_changed           (AdgEntity      *entity);
static void     invalidate              (AdgEntity      *entity);
static void     arrange                 (AdgEntity      *entity);
static gchar *  default_value           (AdgDim         *dim);
static gdouble  quote_angle             (gdouble         angle);
static gboolean set_dim_dress           (AdgDim         *dim,
                                         AdgDress        dress);
static gboolean set_value               (AdgDim         *dim,
                                         const gchar    *value);
static gboolean set_min                 (AdgDim         *dim,
                                         const gchar    *min);
static gboolean set_max                 (AdgDim         *dim,
                                         const gchar    *max);


G_DEFINE_ABSTRACT_TYPE(AdgDim, adg_dim, ADG_TYPE_ENTITY);


static void
adg_dim_class_init(AdgDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgDimPrivate));

    gobject_class->dispose = dispose;
    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->global_changed = global_changed;
    entity_class->local_changed = local_changed;
    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;

    klass->quote_angle = quote_angle;
    klass->default_value = default_value;

    param = adg_param_spec_dress("dim-dress",
                                 P_("Dimension Dress"),
                                 P_("The dress to use for rendering this dimension"),
                                 ADG_DRESS_DIMENSION,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DIM_DRESS, param);

    param = g_param_spec_boxed("ref1",
                               P_("Reference 1"),
                               P_("First reference point of the dimension"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_REF1, param);

    param = g_param_spec_boxed("ref2",
                               P_("Reference 2"),
                               P_("Second reference point of the dimension"),
                               ADG_TYPE_POINT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_REF2, param);

    param = g_param_spec_boxed("pos",
                               P_("Position"),
                               P_("The reference position in local space of the quote: it will be combined with \"level\" to get the real quote position"),
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

    param = g_param_spec_string("value",
                                P_("Basic Value"),
                                P_("The theoretically exact value for this quote: set to NULL to automatically get the default value"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE, param);

    param = g_param_spec_string("value-min",
                                P_("Minimum Value or Low Tolerance"),
                                P_("The minimum value allowed or the lowest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MIN, param);

    param = g_param_spec_string("value-max",
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
    data->value = NULL;
    data->min = NULL;
    data->max = NULL;

    dim->data = data;
}

static void
dispose(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    if (data->quote.entity != NULL) {
        g_object_unref(data->quote.entity);
        data->quote.entity = NULL;
    }
    if (data->ref1 != NULL) {
        adg_point_destroy(data->ref1);
        data->ref1 = NULL;
    }
    if (data->ref2 != NULL) {
        adg_point_destroy(data->ref2);
        data->ref2 = NULL;
    }
    if (data->pos != NULL) {
        adg_point_destroy(data->pos);
        data->pos = NULL;
    }

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
finalize(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    g_free(data->value);
    g_free(data->min);
    g_free(data->max);

    if (PARENT_OBJECT_CLASS->finalize)
        PARENT_OBJECT_CLASS->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
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
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgDim *dim;
    AdgDimPrivate *data;

    dim = (AdgDim *) object;
    data = dim->data;

    switch (prop_id) {
    case PROP_DIM_DRESS:
        set_dim_dress(dim, g_value_get_int(value));
        break;
    case PROP_REF1:
        if (data->ref1 != NULL)
            adg_point_destroy(data->ref1);
        data->ref1 = g_value_dup_boxed(value);
        break;
    case PROP_REF2:
        if (data->ref2 != NULL)
            adg_point_destroy(data->ref2);
        data->ref2 = g_value_dup_boxed(value);
        break;
    case PROP_POS:
        if (data->pos != NULL)
            adg_point_destroy(data->pos);
        data->pos = g_value_dup_boxed(value);
        break;
    case PROP_LEVEL:
        data->level = g_value_get_double(value);
        break;
    case PROP_OUTSIDE:
        data->outside = g_value_get_enum(value);
        break;
    case PROP_VALUE:
        set_value(dim, g_value_get_string(value));
        break;
    case PROP_MIN:
        set_min(dim, g_value_get_string(value));
        break;
    case PROP_MAX:
        set_max(dim, g_value_get_string(value));
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
 **/
void
adg_dim_set_dim_dress(AdgDim *dim, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_dim_dress(dim, dress))
        g_object_notify((GObject *) dim, "dim-dress");
}

/**
 * adg_dim_get_dim_dress:
 * @dim: an #AdgDim
 *
 * Gets the dimension dress to be used in rendering @dim.
 *
 * Returns: the current dimension dress
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
 * adg_dim_set_ref:
 * @dim: an #AdgDim
 * @ref1: the ref1 coordinates
 * @ref2: the ref2 coordinates
 *
 * Sets the #AdgDim:ref1 and #AdgDim:ref2 reference points
 * using @ref1 and @ref2 pairs.  @ref1 or @ref2 could be
 * %NULL (but not both), in which case only the non-null
 * reference point is changed.
 **/
void
adg_dim_set_ref(AdgDim *dim, const AdgPair *ref1, const AdgPair *ref2)
{
    GObject *object;
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));
    g_return_if_fail(ref1 != NULL || ref2 != NULL);

    data = dim->data;
    object = (GObject *) dim;

    g_object_freeze_notify(object);

    if (ref1 != NULL) {
        if (data->ref1 == NULL)
            data->ref1 = adg_point_new();

        adg_point_set(data->ref1, ref1);

        g_object_notify(object, "ref1");
    }

    if (ref2 != NULL) {
        if (data->ref2 == NULL)
            data->ref2 = adg_point_new();

        adg_point_set(data->ref2, ref2);

        g_object_notify(object, "ref2");
    }

    g_object_thaw_notify(object);
}

/**
 * adg_dim_set_ref_explicit:
 * @dim: an #AdgDim
 * @ref1_x: x coordinate of ref1
 * @ref1_y: y coordinate of ref1
 * @ref2_x: x coordinate of ref2
 * @ref2_y: y coordinate of ref2
 *
 * Works in the same way as adg_dim_set_ref() but using
 * explicit coordinates instead of #AdgPair args. The
 * notable difference is that, by using gdouble values,
 * you can't set only a single reference point.
 **/
void
adg_dim_set_ref_explicit(AdgDim *dim,
                         gdouble ref1_x, gdouble ref1_y,
                         gdouble ref2_x, gdouble ref2_y)
{
    AdgPair ref1, ref2;

    ref1.x = ref1_x;
    ref1.y = ref1_y;
    ref2.x = ref2_x;
    ref2.y = ref2_y;

    adg_dim_set_ref(dim, &ref1, &ref2);
}

/**
 * adg_dim_set_ref_from_model:
 * @dim: an #AdgDim
 * @model: the source #AdgModel
 * @ref1: name of the pair in @model to use as ref1
 * @ref2: name of the pair in @model to use as ref2
 *
 * Sets #AdgDim:ref1 and #AdgDim:ref2 properties by linking
 * them to the @ref1 and @ref2 named pairs in @model. @ref1
 * or @ref2 could be %NULL (but not both), in which case
 * only the non-null reference point is changed.
 *
 * Using this function twice you can also link the reference
 * points to named pairs taken from different models:
 *
 * |[
 * adg_dim_set_ref_from_model(dim, model1, ref1, NULL);
 * adg_dim_set_ref_from_model(dim, model2, NULL, ref2);
 * ]|
 **/
void
adg_dim_set_ref_from_model(AdgDim *dim, AdgModel *model,
                           const gchar *ref1, const gchar *ref2)
{
    GObject *object;
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(ref1 != NULL || ref2 != NULL);

    object = (GObject *) dim;
    data = dim->data;

    g_object_freeze_notify(object);

    if (ref1 != NULL) {
        if (data->ref1 == NULL)
            data->ref1 = adg_point_new();

        adg_point_set_from_model(data->ref1, model, ref1);

        g_object_notify(object, "ref1");
    }

    if (ref2 != NULL) {
        if (data->ref2 == NULL)
            data->ref2 = adg_point_new();

        adg_point_set_from_model(data->ref2, model, ref2);

        g_object_notify(object, "ref2");
    }

    g_object_thaw_notify(object);
}

/**
 * adg_dim_get_ref1:
 * @dim: an #AdgDim
 *
 * Gets the ref1 coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Returns: the ref1 coordinates
 **/
const AdgPair *
adg_dim_get_ref1(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return adg_point_pair(data->ref1);
}

/**
 * adg_dim_get_ref2:
 * @dim: an #AdgDim
 *
 * Gets the ref2 coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Returns: the ref2 coordinates
 **/
const AdgPair *
adg_dim_get_ref2(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return adg_point_pair(data->ref2);
}

/**
 * adg_dim_set_pos:
 * @dim: an #AdgDim
 * @pos: the pos coordinates
 *
 * Sets a new #AdgDim:pos position.
 **/
void
adg_dim_set_pos(AdgDim *dim, const AdgPair *pos)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));
    g_return_if_fail(pos != NULL);

    data = dim->data;

    if (data->pos == NULL)
        data->pos = adg_point_new();

    adg_point_set(data->pos, pos);

    g_object_notify((GObject *) dim, "pos");
}

/**
 * adg_dim_set_pos_explicit:
 * @dim: an #AdgDim
 * @pos_x: x coordinate of pos
 * @pos_y: y coordinate of pos
 *
 * Shortcut to set #AdgDim:pos using explicit coordinates.
 **/
void
adg_dim_set_pos_explicit(AdgDim *dim, gdouble x, gdouble y)
{
    AdgPair pos;

    pos.x = x;
    pos.y = y;

    adg_dim_set_pos(dim, &pos);
}

/**
 * adg_dim_set_pos_from_model:
 * @dim: an #AdgDim
 * @model: the source #AdgModel
 * @ref1: name of the pair in @model to use as pos
 *
 * Sets #AdgDim:pos by linking it to the @pos named pair
 * in @model.
 **/
void
adg_dim_set_pos_from_model(AdgDim *dim, AdgModel *model, const gchar *pos)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(pos != NULL);

    data = dim->data;

    if (data->pos == NULL)
        data->pos = adg_point_new();

    adg_point_set_from_model(data->pos, model, pos);

    g_object_notify((GObject *) dim, "pos");
}

/**
 * adg_dim_get_pos:
 * @dim: an #AdgDim
 *
 * Gets the position coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Returns: the pos coordinates
 **/
const AdgPair *
adg_dim_get_pos(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return adg_point_pair(data->pos);
}

/**
 * adg_dim_set_level:
 * @dim: an #AdgDim
 * @level: the new level
 *
 * Sets a new level for this dimension. The level is used to
 * stack the quotes using a spacing value from dim_style
 * (specified in global space).
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
 * Returns: the level value
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
 **/
void
adg_dim_set_outside(AdgDim *dim, AdgThreeState outside)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    data->outside = outside;

    g_object_notify((GObject *) dim, "outside");
}

/**
 * adg_dim_get_outside:
 * @dim: an #AdgDim
 *
 * Gets the state of the #AdgDim:outside property: check the property
 * documentation for further details.
 *
 * Returns: the current flag state
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
 * adg_dim_set_value:
 * @dim: an #AdgDim
 * @value: the value text
 *
 * Explicitely sets the text to use as value. If @value is %NULL or
 * was never set, an automatic text is calculated using the format
 * specified in the current #AdgDimStyle and getting its value by
 * calling the default_value() virtual method.
 **/
void
adg_dim_set_value(AdgDim *dim, const gchar *value)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_value(dim, value))
        g_object_notify((GObject *) dim, "value");
}

/**
 * adg_dim_get_value:
 * @dim: an #AdgDim
 *
 * Gets the value text. The string is internally owned and
 * must not be freed or modified.
 *
 * Returns: the value text
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
 **/
void
adg_dim_set_min(AdgDim *dim, const gchar *min)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_min(dim, min))
        g_object_notify((GObject *) dim, "value-min");
}

/**
 * adg_dim_get_min:
 * @dim: an #AdgDim
 *
 * Gets the minimum value text or %NULL on minimum value disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: the mimimum value text
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
 **/
void
adg_dim_set_max(AdgDim *dim, const gchar *max)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_max(dim, max))
        g_object_notify((GObject *) dim, "value-max");
}

/**
 * adg_dim_get_max:
 * @dim: an #AdgDim
 *
 * Gets the maximum value text or %NULL on maximum value disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: the maximum value text
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
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Gets the quote entity, if any. This function is valid only after
 * the #AdgDim implementation of the arrange() virtual method has
 * been called.
 *
 * Returns: the quote entity
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
 * Returns: the angle to use (always in radians)
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
arrange(AdgEntity *entity)
{
    AdgDim *dim;
    AdgDimPrivate *data;
    AdgEntity *quote_entity;
    AdgContainer *quote_container;
    AdgEntity *value_entity;
    AdgEntity *min_entity;
    AdgEntity *max_entity;
    gdouble width;
    const CpmlExtents *extents;
    AdgMatrix map;
    const AdgPair *shift;

    dim = (AdgDim *) entity;
    data = dim->data;

    /* Resolve the dim style */
    if (data->dim_style == NULL)
        data->dim_style = (AdgDimStyle *)
            adg_entity_style(entity, data->dim_dress);

    if (data->quote.entity == NULL) {
        AdgPair factor = { 0.5, 0 };
        data->quote.entity = g_object_new(ADG_TYPE_ALIGNMENT,
                                          "local-method", ADG_MIX_NONE,
                                          "factor", &factor,
                                          "parent", dim, NULL);
    }

    quote_entity = (AdgEntity *) data->quote.entity;
    quote_container = (AdgContainer *) data->quote.entity;

    if (data->quote.value == NULL) {
        AdgDress dress = adg_dim_style_get_value_dress(data->dim_style);

        data->quote.value = g_object_new(ADG_TYPE_TOY_TEXT,
                                         "local-method", ADG_MIX_PARENT,
                                         "font-dress", dress, NULL);

        adg_container_add(quote_container, (AdgEntity *) data->quote.value);

        if (data->value == NULL) {
            AdgDimClass *klass = ADG_DIM_GET_CLASS(dim);

            if (klass->default_value) {
                /* Automatically generate the value text */
                gchar *text = klass->default_value(dim);
                adg_toy_text_set_label(data->quote.value, text);
                g_free(text);
            }
        } else {
            adg_toy_text_set_label(data->quote.value, data->value);
        }
    }

    if (data->quote.min == NULL && data->min != NULL) {
        AdgDress dress = adg_dim_style_get_min_dress(data->dim_style);

        data->quote.min = g_object_new(ADG_TYPE_TOY_TEXT,
                                       "local-method", ADG_MIX_PARENT,
                                       "font-dress", dress, NULL);

        adg_container_add(quote_container, (AdgEntity *) data->quote.min);
        adg_toy_text_set_label(data->quote.min, data->min);
    }

    if (data->quote.max == NULL && data->max != NULL) {
        AdgDress dress = adg_dim_style_get_max_dress(data->dim_style);

        data->quote.max = g_object_new(ADG_TYPE_TOY_TEXT,
                                       "local-method", ADG_MIX_PARENT,
                                       "font-dress", dress, NULL);

        adg_container_add(quote_container, (AdgEntity *) data->quote.max);
        adg_toy_text_set_label(data->quote.max, data->max);
    }

    value_entity = (AdgEntity *) data->quote.value;
    min_entity = (AdgEntity *) data->quote.min;
    max_entity = (AdgEntity *) data->quote.max;

    /* Propagate the arrange signal to the quote */
    adg_entity_arrange(quote_entity);

    /* Basic value */
    extents = adg_entity_get_extents(value_entity);
    width = extents->size.x;

    /* Limit values (min and max) */
    if (min_entity != NULL || max_entity != NULL) {
        CpmlExtents min_extents = { 0 };
        CpmlExtents max_extents = { 0 };
        gdouble spacing = 0;

        /* Minimum limit */
        if (min_entity != NULL)
            cpml_extents_copy(&min_extents, adg_entity_get_extents(min_entity));

        /* Maximum limit */
        if (max_entity != NULL)
            cpml_extents_copy(&max_extents, adg_entity_get_extents(max_entity));

        shift = adg_dim_style_get_limits_shift(data->dim_style);
        if (min_entity != NULL && max_entity != NULL)
            spacing = adg_dim_style_get_limits_spacing(data->dim_style);

        cairo_matrix_init_translate(&map, width + shift->x,
                                    (spacing + min_extents.size.y +
                                     max_extents.size.y) / 2 +
                                    shift->y - extents->size.y / 2);

        if (min_entity != NULL)
            adg_entity_set_local_map(min_entity, &map);

        if (max_entity != NULL) {
            cairo_matrix_translate(&map, 0, -min_extents.size.y - spacing);
            adg_entity_set_local_map(max_entity, &map);
        }

        width += shift->x + MAX(min_extents.size.x, max_extents.size.x);
    }

    /* Apply the style displacements */
    shift = adg_dim_style_get_quote_shift(data->dim_style);
    cairo_matrix_init_translate(&map, shift->x, shift->y);
    adg_entity_set_local_map(quote_entity, &map);

    adg_entity_arrange(quote_entity);
}

static void
global_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (PARENT_ENTITY_CLASS->global_changed)
        PARENT_ENTITY_CLASS->global_changed(entity);

    if (data->quote.entity != NULL)
        adg_entity_global_changed((AdgEntity *) data->quote.entity);
}

static void
local_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (PARENT_ENTITY_CLASS->local_changed)
        PARENT_ENTITY_CLASS->local_changed(entity);

    if (data->quote.entity != NULL)
        adg_entity_local_changed((AdgEntity *) data->quote.entity);
}

static void
invalidate(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (data->quote.entity != NULL)
        adg_entity_invalidate((AdgEntity *) data->quote.entity);

    adg_point_invalidate(data->ref1);
    adg_point_invalidate(data->ref2);
    adg_point_invalidate(data->pos);
}

static gchar *
default_value(AdgDim *dim)
{
    g_warning("AdgDim::default_value not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(dim)));
    return g_strdup("undef");
}

static gdouble
quote_angle(gdouble angle)
{
    angle = cpml_angle(angle);

    if (angle > G_PI_4 * 4 / 3 || angle <= -G_PI_4 * 3)
        angle = cpml_angle(angle + G_PI);

    return angle;
}

static gboolean
set_dim_dress(AdgDim *dim, AdgDress dress)
{
    AdgDimPrivate *data = dim->data;

    if (adg_dress_set(&data->dim_dress, dress)) {
        data->dim_style = NULL;
        return TRUE;
    }

    return FALSE;
}

static gboolean
set_value(AdgDim *dim, const gchar *value)
{
    AdgDimPrivate *data;

    data = dim->data;

    if (adg_strcmp(value, data->value) == 0)
        return FALSE;

    g_free(data->value);
    data->value = g_strdup(value);

    if (data->quote.value != NULL) {
        g_object_unref(data->quote.value);
        data->quote.value = NULL;
    }

    return TRUE;
}

static gboolean
set_min(AdgDim *dim, const gchar *min)
{
    AdgDimPrivate *data = dim->data;

    if (adg_strcmp(min, data->min) == 0)
        return FALSE;

    g_free(data->min);
    data->min = g_strdup(min);

    if (data->quote.min != NULL) {
        g_object_unref(data->quote.min);
        data->quote.min = NULL;
    }

    return TRUE;
}

static gboolean
set_max(AdgDim *dim, const gchar *max)
{
    AdgDimPrivate *data = dim->data;

    if (adg_strcmp(max, data->max) == 0)
        return FALSE;

    g_free(data->max);
    data->max = g_strdup(max);

    if (data->quote.max != NULL) {
        g_object_unref(data->quote.max);
        data->quote.max = NULL;
    }

    return TRUE;
}
