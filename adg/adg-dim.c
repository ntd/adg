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


enum {
    PROP_0,
    PROP_DRESS,
    PROP_REF1,
    PROP_REF2,
    PROP_POS1,
    PROP_POS2,
    PROP_ANGLE,
    PROP_LEVEL,
    PROP_OUTSIDE,
    PROP_VALUE,
    PROP_VALUE_MIN,
    PROP_VALUE_MAX,
    PROP_NOTE
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
static gboolean invalidate              (AdgEntity      *entity);
static gchar *  default_value           (AdgDim         *dim);
static void     quote_layout            (AdgDim         *dim,
                                         cairo_t        *cr);
static gboolean set_dress               (AdgDim         *dim,
                                         AdgDress        dress);
static gboolean set_angle               (AdgDim         *dim,
                                         gdouble         angle);
static gboolean set_value               (AdgDim         *dim,
                                         const gchar    *value);
static gboolean set_value_min           (AdgDim         *dim,
                                         const gchar    *value_min);
static gboolean set_value_max           (AdgDim         *dim,
                                         const gchar    *value_max);
static gboolean set_note                (AdgDim         *dim,
                                         const gchar    *note);
static void     detach_entity           (AdgEntity     **p_entity);


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

    entity_class->invalidate = invalidate;

    klass->default_value = default_value;
    klass->quote_layout = quote_layout;

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

    param = g_param_spec_double("angle",
                                P_("Angle"),
                                P_("The dimension direction, if relevant"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE, param);

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
    g_object_class_install_property(gobject_class, PROP_VALUE_MIN, param);

    param = g_param_spec_string("value-max",
                                P_("Maximum Value or High Tolerance"),
                                P_("The maximum value allowed or the highest tolerance from value (depending of the dimension style): set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VALUE_MAX, param);

    param = g_param_spec_string("note",
                                P_("Note"),
                                P_("A custom note appended to the end of the quote"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE, param);
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
    data->angle = 0;
    data->level = 1;
    data->value = NULL;
    data->value_min = NULL;
    data->value_max = NULL;
    data->note = NULL;

    data->value_entity = g_object_new(ADG_TYPE_TOY_TEXT,
                                      "parent", dim,
                                      "dress", ADG_DRESS_TEXT_VALUE,
                                      NULL);
    data->value_min_entity = g_object_new(ADG_TYPE_TOY_TEXT,
                                          "parent", dim,
                                          "dress", ADG_DRESS_TEXT_LIMIT,
                                          NULL);
    data->value_max_entity = g_object_new(ADG_TYPE_TOY_TEXT,
                                          "parent", dim,
                                          "dress", ADG_DRESS_TEXT_LIMIT,
                                          NULL);
    data->note_entity = g_object_new(ADG_TYPE_TOY_TEXT,
                                     "parent", dim,
                                     "dress", ADG_DRESS_TEXT_VALUE,
                                     NULL);

    dim->data = data;
}

static void
dispose(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    detach_entity(&data->value_entity);
    detach_entity(&data->value_min_entity);
    detach_entity(&data->value_max_entity);
    detach_entity(&data->note_entity);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
finalize(GObject *object)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    g_free(data->value);
    g_free(data->value_min);
    g_free(data->value_max);
    g_free(data->note);

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
    case PROP_ANGLE:
        g_value_set_double(value, data->angle);
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
    case PROP_VALUE_MIN:
        g_value_set_string(value, data->value_min);
        break;
    case PROP_VALUE_MAX:
        g_value_set_string(value, data->value_max);
        break;
    case PROP_NOTE:
        g_value_set_string(value, data->note);
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
    case PROP_ANGLE:
        set_angle(dim, g_value_get_double(value));
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
    case PROP_VALUE_MIN:
        set_value_min(dim, g_value_get_string(value));
        break;
    case PROP_VALUE_MAX:
        set_value_max(dim, g_value_get_string(value));
        break;
    case PROP_NOTE:
        set_note(dim, g_value_get_string(value));
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
 * adg_dim_get_org:
 * @dim: an #AdgDim
 *
 * Gets the origin (org) coordinates. The returned pair is internally
 * owned and must not be freed or modified. This function is only
 * useful in new dimension implementations.
 *
 * Returns: the org coordinates
 **/
const AdgPair *
adg_dim_get_org(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return &data->org;
}

/**
 * adg_dim_set_org:
 * @dim: an #AdgDim
 * @org: the org coordinates
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Sets new org coordinates.
 **/
void
adg_dim_set_org(AdgDim *dim, const AdgPair *org)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));
    g_return_if_fail(org != NULL);

    data = dim->data;
    data->org = *org;
}

/**
 * adg_dim_set_org_explicit:
 * @dim: an #AdgDim
 * @org_x: x component of org
 * @org_y: y component of org
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Explicitely sets new org coordinates.
 **/
void
adg_dim_set_org_explicit(AdgDim *dim, gdouble org_x, gdouble org_y)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    data->org.x = org_x;
    data->org.y = org_y;
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
 * adg_dim_get_angle:
 * @dim: an #AdgDim
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Gets the dimension angle.
 *
 * Returns: the angle (in radians)
 **/
gdouble
adg_dim_get_angle(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), 0);

    data = dim->data;

    return data->angle;
}

/**
 * adg_dim_set_angle:
 * @dim: an #AdgDim
 * @angle: the new angle (in radians)
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Sets a new dimension angle. The @angle could be modified by this
 * function to fit in the current dimension style mode, so do not
 * expect to get the same value with adg_dim_get_angle().
 **/
void
adg_dim_set_angle(AdgDim *dim, gdouble angle)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_angle(dim, angle))
        g_object_notify((GObject *) dim, "angle");
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
 * adg_dim_get_value_min:
 * @dim: an #AdgDim
 *
 * Gets the minimum value text or %NULL on minimum value disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: the mimimum value text
 **/
const gchar *
adg_dim_get_value_min(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->value_min;
}

/**
 * adg_dim_set_value_min:
 * @dim: an #AdgDim
 * @value_min: the new minimum value
 *
 * Sets the minimum value. Use %NULL as @value_min to disable it.
 **/
void
adg_dim_set_value_min(AdgDim *dim, const gchar *value_min)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_value_min(dim, value_min))
        g_object_notify((GObject *) dim, "value-min");
}

/**
 * adg_dim_get_value_max:
 * @dim: an #AdgDim
 *
 * Gets the maximum value text or %NULL on maximum value disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Returns: the maximum value text
 **/
const gchar *
adg_dim_get_value_max(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->value_max;
}

/**
 * adg_dim_set_value_max:
 * @dim: an #AdgDim
 * @value_max: the new maximum value
 *
 * Sets the maximum value. Use %NULL as @value_max to disable it.
 **/
void
adg_dim_set_value_max(AdgDim *dim, const gchar *value_max)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_value_max(dim, value_max))
        g_object_notify((GObject *) dim, "value-max");
}

/**
 * adg_dim_set_tolerances:
 * @dim: an #AdgDim
 * @value_min: the new minumum value
 * @value_max: the new maximum value
 *
 * Shortcut to set both the tolerances at once.
 **/
void
adg_dim_set_tolerances(AdgDim *dim,
                       const gchar *value_min, const gchar *value_max)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_object_freeze_notify((GObject *) dim);
    adg_dim_set_value_min(dim, value_min);
    adg_dim_set_value_max(dim, value_max);
    g_object_thaw_notify((GObject *) dim);
}

/**
 * adg_dim_get_note:
 * @dim: and #AdgDim
 *
 * Gets the note text or %NULL if the note is not used. The string is
 * internally owned and must not be freed or modified.
 *
 * Returns: the note text
 **/
const gchar *
adg_dim_get_note(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->note;
}

/**
 * adg_dim_set_note:
 * @dim: an #AdgDim
 * @note: the new note
 *
 * Sets a new note text, usually appended at the end of the dimension text.
 **/
void
adg_dim_set_note(AdgDim *dim, const gchar *note)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (set_note(dim, note))
        g_object_notify((GObject *) dim, "note");
}

/**
 * adg_dim_render_quote:
 * @dim: an #AdgDim object
 * @cr: a #cairo_t drawing context
 *
 * <note><para>
 * This function is only useful in new dimension implementations.
 * </para></note>
 *
 * Renders the quote of @dim at the @org position.
 **/
void
adg_dim_render_quote(AdgDim *dim, cairo_t *cr)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;

    /* Check if the basic value text needs to be automatically generated */
    if (data->value == NULL) {
        gchar *text = ADG_DIM_GET_CLASS(dim)->default_value(dim);
        adg_toy_text_set_label((AdgToyText *) data->value_entity, text);
        g_free(text);
    }

    ADG_DIM_GET_CLASS(dim)->quote_layout(dim, cr);

    adg_entity_render(data->value_entity, cr);
    adg_entity_render(data->value_min_entity, cr);
    adg_entity_render(data->value_max_entity, cr);
    adg_entity_render(data->note_entity, cr);
}

static gboolean
invalidate(AdgEntity *entity)
{
    AdgDimPrivate *data = ((AdgDim *) entity)->data;

    adg_entity_invalidate(data->value_entity);
    adg_entity_invalidate(data->value_min_entity);
    adg_entity_invalidate(data->value_max_entity);
    adg_entity_invalidate(data->note_entity);

    return TRUE;
}

static gchar *
default_value(AdgDim *dim)
{
    g_warning("AdgDim::default_value not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(dim)));
    return g_strdup("undef");
}

static void
quote_layout(AdgDim *dim, cairo_t *cr)
{
    AdgEntity *entity;
    AdgDimPrivate *data;
    AdgDimStyle *dim_style;
    cairo_text_extents_t extents;
    AdgMatrix map;
    const AdgPair *shift;

    entity = (AdgEntity *) dim;
    data = dim->data;
    dim_style = (AdgDimStyle *) adg_entity_style(entity, data->dress);

    /* Initialize local maps to the origin */
    cairo_matrix_init_translate(&map, data->org.x, data->org.y);

    adg_entity_set_local_map(data->value_entity, &map);
    adg_entity_set_local_map(data->value_min_entity, &map);
    adg_entity_set_local_map(data->value_max_entity, &map);
    adg_entity_set_local_map(data->note_entity, &map);

    /* Initialize global maps to the quote rotation angle */
    cairo_matrix_init_rotate(&map, data->angle);

    adg_entity_set_global_map(data->value_entity, &map);
    adg_entity_set_global_map(data->value_min_entity, &map);
    adg_entity_set_global_map(data->value_max_entity, &map);
    adg_entity_set_global_map(data->note_entity, &map);

    /* Basic value */
    adg_toy_text_get_extents((AdgToyText *) data->value_entity, cr, &extents);

    /* Limit values (value_min and value_max) */
    if (data->value_min != NULL || data->value_max != NULL) {
        cairo_text_extents_t min_extents = { 0 };
        cairo_text_extents_t max_extents = { 0 };
        gdouble spacing = 0;

        /* Low tolerance */
        if (data->value_min != NULL)
            adg_toy_text_get_extents((AdgToyText *) data->value_min_entity,
                                     cr, &min_extents);

        /* High tolerance */
        if (data->value_max != NULL)
            adg_toy_text_get_extents((AdgToyText *) data->value_max_entity,
                                     cr, &max_extents);

        shift = adg_dim_style_get_tolerance_shift(dim_style);
        if (data->value_min != NULL && data->value_max != NULL)
            spacing = adg_dim_style_get_tolerance_spacing(dim_style);

        cairo_matrix_init_translate(&map, extents.width + shift->x,
                                    (spacing + min_extents.height +
                                     max_extents.height) / 2 +
                                    shift->y - extents.height / 2);
        adg_entity_transform_global_map(data->value_min_entity, &map);
        cairo_matrix_translate(&map, 0, -min_extents.height - spacing);
        adg_entity_transform_global_map(data->value_max_entity, &map);

        extents.width += shift->x + MAX(min_extents.width, max_extents.width);
    }

    /* Note */
    if (data->note != NULL) {
        cairo_text_extents_t note_extents;

        adg_toy_text_get_extents((AdgToyText *) data->note_entity,
                                 cr, &note_extents);
        shift = adg_dim_style_get_note_shift(dim_style);

        cairo_matrix_init_translate(&map, extents.width + shift->x, shift->y);
        adg_entity_transform_global_map(data->note_entity, &map);

        extents.width += shift->x + note_extents.width;
    }

    /* Center and apply the style displacements */
    shift = adg_dim_style_get_quote_shift(dim_style);
    cairo_matrix_init_translate(&map, shift->x - extents.width / 2, shift->y);

    adg_entity_transform_global_map(data->value_entity, &map);
    adg_entity_transform_global_map(data->value_min_entity, &map);
    adg_entity_transform_global_map(data->value_max_entity, &map);
    adg_entity_transform_global_map(data->note_entity, &map);
}

static gboolean
set_dress(AdgDim *dim, AdgDress dress)
{
    AdgDimPrivate *data = dim->data;

    if (dress == data->dress)
        return FALSE;

    if (!adg_dress_are_related(dress, data->dress)) {
        g_warning("%s: `%s' and `%s' families are not coherents",
                  G_STRLOC, adg_dress_name(dress),
                  adg_dress_name(data->dress));
        return FALSE;
    }

    data->dress = dress;

    return TRUE;
}

static gboolean
set_angle(AdgDim *dim, gdouble angle)
{
    AdgDimPrivate *data = dim->data;

    angle = cpml_angle(angle);
    if (angle > G_PI_4 || angle <= -G_PI_4 * 3)
        angle = cpml_angle(angle + G_PI);

    if (angle == data->angle)
        return FALSE;

    data->angle = angle;

    return TRUE;
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
    adg_toy_text_set_label((AdgToyText *) data->value_entity, value);

    return TRUE;
}

static gboolean
set_value_min(AdgDim *dim, const gchar *value_min)
{
    AdgDimPrivate *data = dim->data;

    if (adg_strcmp(value_min, data->value_min) == 0)
        return FALSE;

    g_free(data->value_min);
    data->value_min = g_strdup(value_min);
    adg_toy_text_set_label((AdgToyText *) data->value_min_entity, value_min);

    return TRUE;
}

static gboolean
set_value_max(AdgDim *dim, const gchar *value_max)
{
    AdgDimPrivate *data = dim->data;

    if (adg_strcmp(value_max, data->value_max) == 0)
        return FALSE;

    g_free(data->value_max);
    data->value_max = g_strdup(value_max);
    adg_toy_text_set_label((AdgToyText *) data->value_max_entity, value_max);

    return TRUE;
}

static gboolean
set_note(AdgDim *dim, const gchar *note)
{
    AdgDimPrivate *data = dim->data;

    if (adg_strcmp(note, data->note) == 0)
        return FALSE;

    g_free(data->note);
    data->note = g_strdup(note);
    adg_toy_text_set_label((AdgToyText *) data->note_entity, note);

    return TRUE;
}

static void
detach_entity(AdgEntity **p_entity)
{
    if (*p_entity != NULL) {
        adg_entity_set_parent(*p_entity, NULL);
        g_object_unref(*p_entity);
        *p_entity = NULL;
    }
}
