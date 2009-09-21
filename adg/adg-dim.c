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
#include "adg-toy-text.h"
#include "adg-type-builtins.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_dim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_dim_parent_class)


enum {
    PROP_0,
    PROP_DRESS,
    PROP_REF1,
    PROP_REF2,
    PROP_POS1,
    PROP_POS2,
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
static gboolean set_dress               (AdgDim         *dim,
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

    param = adg_param_spec_dress("dress",
                                 P_("Dress Style"),
                                 P_("The dress to use for rendering this dimension"),
                                 ADG_DRESS_DIMENSION_REGULAR,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DRESS, param);

    param = g_param_spec_boxed("ref1",
                               P_("Reference 1"),
                               P_("First reference point of the dimension"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_REF1, param);

    param = g_param_spec_boxed("ref2",
                               P_("Reference 2"),
                               P_("Second reference point of the dimension"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_REF2, param);

    param = g_param_spec_boxed("pos1",
                               P_("Position 1"),
                               P_("First position point: it will be computed with the level property to get the real dimension position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS1, param);

    param = g_param_spec_boxed("pos2",
                               P_("Position 2"),
                               P_("Second position point: it will be computed with the level property to get the real dimension position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS2, param);

    param = g_param_spec_double("level",
                                P_("Level"),
                                P_("The dimension level, that is the factor to multiply dim_style->baseline_spacing to get the offset (in device units) from pos1..pos2 where render the dimension baseline"),
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
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE, param);

    param = g_param_spec_string("value-min",
                                P_("Minimum Value or Low Tolerance"),
                                P_("The minimum value allowed or the lowest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MIN, param);

    param = g_param_spec_string("value-max",
                                P_("Maximum Value or High Tolerance"),
                                P_("The maximum value allowed or the highest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MAX, param);
}

static void
adg_dim_init(AdgDim *dim)
{
    AdgDimPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(dim, ADG_TYPE_DIM,
                                                      AdgDimPrivate);

    data->dress = ADG_DRESS_DIMENSION_REGULAR;
    data->ref1.x = data->ref1.y = 0;
    data->ref2.x = data->ref2.y = 0;
    data->pos1.x = data->pos1.y = 0;
    data->pos2.x = data->pos2.y = 0;
    data->level = 1;
    data->value = NULL;
    data->min = NULL;
    data->max = NULL;

    dim->data = data;
}

static void
dispose(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    if (data->quote.container != NULL) {
        g_object_unref(data->quote.container);
        data->quote.container = NULL;
    }

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
finalize(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    g_free(data->value);
    g_free(data->min);
    g_free(data->max);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    switch (prop_id) {
    case PROP_DRESS:
        g_value_set_int(value, data->dress);
        break;
    case PROP_REF1:
        g_value_set_boxed(value, &data->ref1);
        break;
    case PROP_REF2:
        g_value_set_boxed(value, &data->ref2);
        break;
    case PROP_POS1:
        g_value_set_boxed(value, &data->pos1);
        break;
    case PROP_POS2:
        g_value_set_boxed(value, &data->pos1);
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
    case PROP_DRESS:
        set_dress(dim, g_value_get_int(value));
        break;
    case PROP_REF1:
        cpml_pair_copy(&data->ref1, (AdgPair *) g_value_get_boxed(value));
        break;
    case PROP_REF2:
        cpml_pair_copy(&data->ref2, (AdgPair *) g_value_get_boxed(value));
        break;
    case PROP_POS1:
        cpml_pair_copy(&data->pos1, (AdgPair *) g_value_get_boxed(value));
        break;
    case PROP_POS2:
        cpml_pair_copy(&data->pos2, (AdgPair *) g_value_get_boxed(value));
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
 * adg_dim_get_dress:
 * @dim: an #AdgDim
 *
 * Gets the dimension dress to be used in rendering @dim.
 *
 * Returns: the current dimension dress
 **/
AdgDress
adg_dim_get_dress(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), ADG_DRESS_UNDEFINED);

    data = dim->data;

    return data->dress;
}

/**
 * adg_dim_set_dress:
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
adg_dim_set_dress(AdgDim *dim, AdgDress dress)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_dress(dim, dress))
        g_object_notify((GObject *) dim, "dress");
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

    return &data->ref1;
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

    return &data->ref2;
}

/**
 * adg_dim_set_ref:
 * @dim: an #AdgDim
 * @ref1: the ref1 coordinates
 * @ref2: the ref2 coordinates
 *
 * Shortcut to set ref1 and ref2 points at once.
 **/
void
adg_dim_set_ref(AdgDim *dim, const AdgPair *ref1, const AdgPair *ref2)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (ref1 != NULL || ref2 != NULL) {
        GObject *object;
        AdgDimPrivate *data;

        data = dim->data;
        object = (GObject *) dim;

        g_object_freeze_notify(object);

        if (ref1 != NULL) {
            data->ref1 = *ref1;
            g_object_notify(object, "ref1");
        }

        if (ref2 != NULL) {
            data->ref2 = *ref2;
            g_object_notify(object, "ref2");
        }

        g_object_thaw_notify(object);
    }
}

/**
 * adg_dim_set_ref_explicit:
 * @dim: an #AdgDim
 * @ref1_x: x component of pos1
 * @ref1_y: y component of pos1
 * @ref2_x: x component of pos2
 * @ref2_y: y component of pos2
 *
 * Shortcut to set ref1 and ref2 points at once,
 * using explicit coordinates.
 **/
void
adg_dim_set_ref_explicit(AdgDim *dim, gdouble ref1_x, gdouble ref1_y,
                         gdouble ref2_x, gdouble ref2_y)
{
    AdgPair ref1;
    AdgPair ref2;

    ref1.x = ref1_x;
    ref1.y = ref1_y;
    ref2.x = ref2_x;
    ref2.y = ref2_y;

    adg_dim_set_ref(dim, &ref1, &ref2);
}

/**
 * adg_dim_get_pos1:
 * @dim: an #AdgDim
 *
 * Gets the pos1 coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Returns: the pos1 coordinates
 **/
const AdgPair *
adg_dim_get_pos1(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return &data->pos1;
}

/**
 * adg_dim_get_pos2:
 * @dim: an #AdgDim
 *
 * Gets the pos2 coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Returns: the pos2 coordinates
 **/
const AdgPair *
adg_dim_get_pos2(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return &data->pos2;
}

/**
 * adg_dim_set_pos:
 * @dim: an #AdgDim
 * @pos1: the pos1 coordinates
 * @pos2: the pos2 coordinates
 *
 * Shortcut to set pos1 and pos2 points at once.
 **/
void
adg_dim_set_pos(AdgDim *dim, AdgPair *pos1, AdgPair *pos2)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (pos1 != NULL || pos2 != NULL) {
        AdgDimPrivate *data;
        GObject *object;

        data = dim->data;
        object = (GObject *) dim;

        g_object_freeze_notify(object);

        if (pos1 != NULL) {
            data->pos1 = *pos1;
            g_object_notify(object, "pos1");
        }
        if (pos2 != NULL) {
            data->pos2 = *pos2;
            g_object_notify(object, "pos2");
        }

        g_object_thaw_notify(object);
    }
}

/**
 * adg_dim_set_pos_explicit:
 * @dim: an #AdgDim
 * @pos1_x: x component of pos1
 * @pos1_y: y component of pos1
 * @pos2_x: x component of pos2
 * @pos2_y: y component of pos2
 *
 * Shortcut to set pos1 and pos2 points at once,
 * using explicit coordinates.
 **/
void
adg_dim_set_pos_explicit(AdgDim *dim, gdouble pos1_x, gdouble pos1_y,
                         gdouble pos2_x, gdouble pos2_y)
{
    AdgPair pos1;
    AdgPair pos2;

    pos1.x = pos1_x;
    pos1.y = pos1_y;
    pos2.x = pos2_x;
    pos2.y = pos2_y;

    adg_dim_set_pos(dim, &pos1, &pos2);
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
 * adg_dim_get_dim_style:
 * @dim: an #AdgDim entity
 *
 * Gets the #AdgDimStyle associated to @dim. The dress to style
 * resolution is done in the arrange() method so this value is
 * typically available in render() or in a derived arrange()
 * method, after the #AdgDim arrange() function has been chained up.
 *
 * Returns: the dim style of @entity
 **/
AdgDimStyle *
adg_dim_get_dim_style(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->dim_style;
}

/**
 * adg_dim_get_quote:
 * @dim: an #AdgDim
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Gets the quote container, if any. This function is valid only
 * after the #AdgDim implementation of the arrange() virtual method
 * has been called.
 *
 * Returns: the quote container
 **/
AdgContainer *
adg_dim_get_quote(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->quote.container;
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
    AdgEntity *value_entity;
    AdgEntity *min_entity;
    AdgEntity *max_entity;
    CpmlExtents extents;
    AdgMatrix map;
    const AdgPair *shift;

    dim = (AdgDim *) entity;
    data = dim->data;

    /* Resolve the dim style */
    if (data->dim_style == NULL)
        data->dim_style = (AdgDimStyle *) adg_entity_style(entity, data->dress);

    if (data->quote.container == NULL)
        data->quote.container = g_object_new(ADG_TYPE_CONTAINER,
                                             "parent", dim, NULL);

    if (data->quote.value == NULL) {
        AdgDress dress = adg_dim_style_get_value_dress(data->dim_style);

        data->quote.value = g_object_new(ADG_TYPE_TOY_TEXT,
                                         "dress", dress, NULL);

        adg_container_add(data->quote.container,
                          (AdgEntity *) data->quote.value);

        if (data->value == NULL) {
            /* Automatically generate the value text */
            gchar *text = ADG_DIM_GET_CLASS(dim)->default_value(dim);
            adg_toy_text_set_label(data->quote.value, text);
            g_free(text);
        } else {
            adg_toy_text_set_label(data->quote.value, data->value);
        }
    }

    if (data->quote.min == NULL && data->min != NULL) {
        AdgDress dress = adg_dim_style_get_min_dress(data->dim_style);

        data->quote.min = g_object_new(ADG_TYPE_TOY_TEXT, "dress", dress, NULL);

        adg_container_add(data->quote.container, (AdgEntity *) data->quote.min);
        adg_toy_text_set_label(data->quote.min, data->min);
    }

    if (data->quote.max == NULL && data->max != NULL) {
        AdgDress dress = adg_dim_style_get_max_dress(data->dim_style);

        data->quote.max = g_object_new(ADG_TYPE_TOY_TEXT, "dress", dress, NULL);

        adg_container_add(data->quote.container, (AdgEntity *) data->quote.max);
        adg_toy_text_set_label(data->quote.max, data->max);
    }

    value_entity = (AdgEntity *) data->quote.value;
    min_entity = (AdgEntity *) data->quote.min;
    max_entity = (AdgEntity *) data->quote.max;

    /* Basic value */
    adg_entity_get_extents(value_entity, &extents);

    /* Limit values (min and max) */
    if (min_entity != NULL || max_entity != NULL) {
        CpmlExtents min_extents = { 0, };
        CpmlExtents max_extents = { 0, };
        gdouble spacing = 0;

        /* Minimum limit */
        if (min_entity != NULL)
            adg_entity_get_extents(min_entity, &min_extents);

        /* Maximum limit */
        if (max_entity != NULL)
            adg_entity_get_extents(max_entity, &max_extents);

        shift = adg_dim_style_get_limits_shift(data->dim_style);
        if (min_entity != NULL && max_entity != NULL)
            spacing = adg_dim_style_get_limits_spacing(data->dim_style);

        cairo_matrix_init_translate(&map, extents.size.x + shift->x,
                                    (spacing + min_extents.size.y +
                                     max_extents.size.y) / 2 +
                                    shift->y - extents.size.y / 2);

        if (min_entity != NULL)
            adg_entity_set_global_map(min_entity, &map);

        if (max_entity != NULL) {
            cairo_matrix_translate(&map, 0, -min_extents.size.y - spacing);
            adg_entity_set_global_map(max_entity, &map);
        }

        extents.size.x += shift->x + MAX(min_extents.size.x, max_extents.size.x);
    }

    /* Center and apply the style displacements */
    shift = adg_dim_style_get_quote_shift(data->dim_style);
    cairo_matrix_init_translate(&map, shift->x - extents.size.x / 2, shift->y);
    adg_entity_set_global_map((AdgEntity *) data->quote.container, &map);
}


static void
global_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    PARENT_ENTITY_CLASS->global_changed(entity);

    if (data->quote.container != NULL)
        adg_entity_global_changed((AdgEntity *) data->quote.container);
}

static void
local_changed(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    PARENT_ENTITY_CLASS->local_changed(entity);

    if (data->quote.container != NULL)
        adg_entity_local_changed((AdgEntity *) data->quote.container);
}

static void
invalidate(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    if (data->quote.container != NULL)
        adg_entity_invalidate((AdgEntity *) data->quote.container);
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

    if (angle > G_PI_4 || angle <= -G_PI_4 * 3)
        angle = cpml_angle(angle + G_PI);

    return angle;
}

static gboolean
set_dress(AdgDim *dim, AdgDress dress)
{
    AdgDimPrivate *data = dim->data;

    if (adg_dress_set(&data->dress, dress)) {
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
