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
 */

/**
 * AdgDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-dim.h"
#include "adg-dim-private.h"
#include "adg-util.h"
#include "adg-intl.h"
#include <string.h>


enum {
    PROP_0,
    PROP_REF1,
    PROP_REF2,
    PROP_POS1,
    PROP_POS2,
    PROP_LEVEL,
    PROP_QUOTE,
    PROP_TOLERANCE_UP,
    PROP_TOLERANCE_DOWN,
    PROP_NOTE
};


static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     paper_matrix_changed    (AdgEntity      *entity,
                                         AdgMatrix      *parent_matrix);
static void     invalidate              (AdgEntity      *entity);
static void     clear                   (AdgDim         *entity);
static gchar *  default_quote           (AdgDim         *dim);
static void     quote_layout            (AdgDim         *dim,
                                         cairo_t        *cr);
static void     text_cache_init         (AdgTextCache   *text_cache);
static gboolean text_cache_update       (AdgTextCache   *text_cache,
                                         const gchar    *text,
                                         cairo_t        *cr,
                                         AdgStyle       *style);
static void     text_cache_clear        (AdgTextCache   *text_cache);
static void     text_cache_move_to      (AdgTextCache   *text_cache,
                                         const CpmlPair *to);
static void     text_cache_render       (AdgTextCache   *text_cache,
                                         cairo_t        *cr);


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

    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
 
    entity_class->paper_matrix_changed = paper_matrix_changed;
    entity_class->invalidate = invalidate;

    klass->default_quote = default_quote;
    klass->quote_layout = quote_layout;

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
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_LEVEL, param);

    param = g_param_spec_string("quote",
                                P_("Quote"),
                                P_("The quote to display: set to NULL to get the default quote"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_QUOTE, param);

    param = g_param_spec_string("tolerance-up",
                                P_("Up Tolerance"),
                                P_("The up tolerance of the quote: set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_UP, param);

    param = g_param_spec_string("tolerance-down",
                                P_("Down Tolerance"),
                                P_("The down tolerance of the quote: set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_DOWN, param);

    param = g_param_spec_string("note",
                                P_("Note"),
                                P_("A custom note appended to the dimension quote"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE, param);
}

static void
adg_dim_init(AdgDim *dim)
{
    AdgDimPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(dim, ADG_TYPE_DIM,
                                                      AdgDimPrivate);

    data->ref1.x = data->ref1.y = 0;
    data->ref2.x = data->ref2.y = 0;
    data->pos1.x = data->pos1.y = 0;
    data->pos2.x = data->pos2.y = 0;
    data->level = 1.;
    data->quote = NULL;
    data->tolerance_up = NULL;
    data->tolerance_down = NULL;
    data->note = NULL;

    data->org.x = data->org.y = 0;
    data->angle = 0;
    text_cache_init(&data->quote_cache);
    text_cache_init(&data->tolerance_up_cache);
    text_cache_init(&data->tolerance_down_cache);
    text_cache_init(&data->note_cache);

    dim->data = data;
}

static void
finalize(GObject *object)
{
    AdgDim *dim;
    AdgDimPrivate *data;
    GObjectClass *object_class;

    dim = (AdgDim *) object;
    data = dim->data;
    object_class = (GObjectClass *) adg_dim_parent_class;

    g_free(data->quote);
    g_free(data->tolerance_up);
    g_free(data->tolerance_down);

    clear(dim);

    if (object_class->finalize != NULL)
        object_class->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgDimPrivate *data = ((AdgDim *) object)->data;

    switch (prop_id) {
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
    case PROP_QUOTE:
        g_value_set_string(value, data->quote);
        break;
    case PROP_TOLERANCE_UP:
        g_value_set_string(value, data->tolerance_up);
        break;
    case PROP_TOLERANCE_DOWN:
        g_value_set_string(value, data->tolerance_down);
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
    case PROP_REF1:
        cpml_pair_copy(&data->ref1, (AdgPair *) g_value_get_boxed(value));
        clear(dim);
        break;
    case PROP_REF2:
        cpml_pair_copy(&data->ref2, (AdgPair *) g_value_get_boxed(value));
        clear(dim);
        break;
    case PROP_POS1:
        cpml_pair_copy(&data->pos1, (AdgPair *) g_value_get_boxed(value));
        clear(dim);
        break;
    case PROP_POS2:
        cpml_pair_copy(&data->pos2, (AdgPair *) g_value_get_boxed(value));
        clear(dim);
        break;
    case PROP_LEVEL:
        data->level = g_value_get_double(value);
        clear(dim);
        break;
    case PROP_QUOTE:
        g_free(data->quote);
        data->quote = g_value_dup_string(value);
        text_cache_clear(&data->quote_cache);
        break;
    case PROP_TOLERANCE_UP:
        g_free(data->tolerance_up);
        data->tolerance_up = g_value_dup_string(value);
        text_cache_clear(&data->tolerance_up_cache);
        break;
    case PROP_TOLERANCE_DOWN:
        g_free(data->tolerance_down);
        data->tolerance_down = g_value_dup_string(value);
        text_cache_clear(&data->tolerance_down_cache);
        break;
    case PROP_NOTE:
        g_free(data->note);
        data->note = g_value_dup_string(value);
        text_cache_clear(&data->note_cache);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_dim_get_org:
 * @dim: an #AdgDim
 *
 * Gets the origin (org) coordinates. The returned pair is internally
 * owned and must not be freed or modified. This function is only
 * useful in new dimension implementations.
 *
 * Return value: the org coordinates
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
 * Sets new org coordinates. This function is only useful
 * in new dimension implementations.
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
 * Explicitely sets new org coordinates. This function is only useful
 * in new dimension implementations.
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
 * adg_dim_get_angle:
 * @dim: an #AdgDim
 *
 * Gets the dimension angle. This function is only useful
 * in new dimension implementations.
 *
 * Return value: the angle (in radians)
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
 * Sets a new dimension angle. This function is only useful
 * in new dimension implementations.
 **/
void
adg_dim_set_angle(AdgDim *dim, gdouble angle)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    data->angle = angle;
}

/**
 * adg_dim_get_ref1:
 * @dim: an #AdgDim
 *
 * Gets the ref1 coordinates. The returned pair is internally owned
 * and must not be freed or modified.
 *
 * Return value: the ref1 coordinates
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
 * Return value: the ref2 coordinates
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
        clear(dim);
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
 * Return value: the pos1 coordinates
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
 * Return value: the pos2 coordinates
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
        clear(dim);
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
 * Return value: the level value
 **/
gdouble
adg_dim_get_level(AdgDim  *dim)
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
 * (specified in paper space).
 **/
void
adg_dim_set_level(AdgDim *dim, gdouble level)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    data->level = level;

    g_object_notify((GObject *) dim, "level");
    clear(dim);
}

/**
 * adg_dim_get_quote:
 * @dim: an #AdgDim
 *
 * Gets the quote text. The string is internally owned and
 * must not be freed or modified.
 *
 * Return value: the quote text
 **/
const gchar *
adg_dim_get_quote(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->quote;
}

/**
 * adg_dim_set_quote:
 * @dim: an #AdgDim
 * @quote: the quote text
 *
 * Explicitely sets the text to use as quote. If @quote is %NULL or
 * was never set, an automatic text is calculated using the format as
 * specified by the dim_style associated to this entity and getting
 * the number from the construction points (ref1, ref2, pos1 and pos2).
 **/
void
adg_dim_set_quote(AdgDim *dim, const gchar *quote)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;

    g_free(data->quote);
    data->quote = g_strdup(quote);
    g_object_notify((GObject *) dim, "quote");

    text_cache_clear(&data->quote_cache);
}

/**
 * adg_dim_get_tolerance_up:
 * @dim: an #AdgDim
 *
 * Gets the upper tolerance text or %NULL on upper tolerance disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Return value: the tolerance text
 **/
const gchar *
adg_dim_get_tolerance_up(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->tolerance_up;
}

/**
 * adg_dim_set_tolerance_up:
 * @dim: an #AdgDim
 * @tolerance_up: the upper tolerance
 *
 * Sets the upper tolerance. Use %NULL as @tolerance_up to disable it.
 **/
void
adg_dim_set_tolerance_up(AdgDim *dim, const gchar *tolerance_up)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;

    g_free(data->tolerance_up);
    data->tolerance_up = g_strdup(tolerance_up);
    g_object_notify((GObject *) dim, "tolerance-up");

    text_cache_clear(&data->tolerance_up_cache);
}

/**
 * adg_dim_get_tolerance_down:
 * @dim: an #AdgDim
 *
 * Gets the lower tolerance text or %NULL on lower tolerance disabled.
 * The string is internally owned and must not be freed or modified.
 *
 * Return value: the tolerance text
 **/
const gchar *
adg_dim_get_tolerance_down(AdgDim *dim)
{
    AdgDimPrivate *data;

    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    data = dim->data;

    return data->tolerance_down;
}

/**
 * adg_dim_set_tolerance_down:
 * @dim: an #AdgDim
 * @tolerance_down: the lower tolerance
 *
 * Sets the lower tolerance. Use %NULL as @tolerance_down to disable it.
 **/
void
adg_dim_set_tolerance_down(AdgDim *dim, const gchar *tolerance_down)
{
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;

    g_free(data->tolerance_down);
    data->tolerance_down = g_strdup(tolerance_down);
    g_object_notify((GObject *) dim, "tolerance-down");

    text_cache_clear(&data->tolerance_down_cache);
}

/**
 * adg_dim_set_tolerances:
 * @dim: an #AdgDim
 * @tolerance_up: the upper tolerance text
 * @tolerance_down: the lower tolerance text
 *
 * Shortcut to set both the tolerance at once.
 **/
void
adg_dim_set_tolerances(AdgDim *dim, const gchar *tolerance_up,
                       const gchar *tolerance_down)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_object_freeze_notify((GObject *) dim);
    adg_dim_set_tolerance_up(dim, tolerance_up);
    adg_dim_set_tolerance_down(dim, tolerance_down);
    g_object_thaw_notify((GObject *) dim);
}

/**
 * adg_dim_get_note:
 * @dim: and #AdgDim
 *
 * Gets the note text or %NULL if the note is not used. The string is
 * internally owned and must not be freed or modified.
 *
 * Return value: the note text
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
    AdgDimPrivate *data;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;

    g_free(data->note);
    data->note = g_strdup(note);
    g_object_notify((GObject *) dim, "note");

    text_cache_clear(&data->note_cache);
}

/**
 * adg_dim_render_quote:
 * @dim: an #AdgDim object
 * @cr: a #cairo_t drawing context
 *
 * Renders the quote of @dim at the @org position. This function
 * is only useful in new dimension implementations.
 */
void
adg_dim_render_quote(AdgDim *dim, cairo_t *cr)
{
    AdgDimPrivate *data;
    AdgEntity *entity;
    AdgDimStyle *dim_style;
    const AdgMatrix *paper;
    cairo_matrix_t matrix;

    g_return_if_fail(ADG_IS_DIM(dim));

    data = dim->data;
    entity = (AdgEntity *) dim;
    dim_style = (AdgDimStyle *) adg_entity_get_style(entity, ADG_SLOT_DIM_STYLE);
    paper = adg_entity_get_paper_matrix(entity);

    if (data->quote == NULL)
        adg_dim_set_quote(dim, ADG_DIM_GET_CLASS(dim)->default_quote(dim));

    cairo_save(cr);

    cairo_set_matrix(cr, paper);
    ADG_DIM_GET_CLASS(dim)->quote_layout(dim, cr);
    cairo_set_matrix(cr, adg_entity_get_model_matrix(entity));

    cairo_translate(cr, data->org.x, data->org.y);
    cairo_get_matrix(cr, &matrix);
    matrix.xx = paper->xx;
    matrix.yy = paper->yy;
    cairo_set_matrix(cr, &matrix);
    cairo_rotate(cr, -data->angle);

    /* Rendering quote */
    adg_style_apply(adg_dim_style_get_quote_style(dim_style), cr);
    text_cache_render(&data->quote_cache, cr);

    /* Rendering tolerances */
    if (data->tolerance_up != NULL || data->tolerance_down != NULL) {
        adg_style_apply(adg_dim_style_get_tolerance_style(dim_style), cr);

        if (data->tolerance_up != NULL)
            text_cache_render(&data->tolerance_up_cache, cr);

        if (data->tolerance_down != NULL)
            text_cache_render(&data->tolerance_down_cache, cr);
    }

    /* Rendering the note */
    if (data->note != NULL) {
        adg_style_apply(adg_dim_style_get_note_style(dim_style), cr);
        text_cache_render(&data->note_cache, cr);
    }

    cairo_restore(cr);
}


static void
paper_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_dim_parent_class;

    clear((AdgDim *) entity);

    if (entity_class->paper_matrix_changed != NULL)
        entity_class->paper_matrix_changed(entity, parent_matrix);
}

static void
invalidate(AdgEntity *entity)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_dim_parent_class;

    clear((AdgDim *) entity);

    if (entity_class->invalidate != NULL)
        entity_class->invalidate(entity);
}

static void
clear(AdgDim *dim)
{
    AdgDimPrivate *data = dim->data;

    text_cache_clear(&data->quote_cache);
    text_cache_clear(&data->tolerance_up_cache);
    text_cache_clear(&data->tolerance_down_cache);
    text_cache_clear(&data->note_cache);
}

static gchar *
default_quote(AdgDim *dim)
{
    g_warning("AdgDim::default_quote not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(dim)));
    return g_strdup("undef");
}

static void
quote_layout(AdgDim *dim, cairo_t *cr)
{
    AdgDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgPair shift;
    CpmlPair cp;
    CpmlPair tolerance_up_org, tolerance_down_org, note_org;

    data = dim->data;
    dim_style = (AdgDimStyle *) adg_entity_get_style((AdgEntity *) dim,
                                                     ADG_SLOT_DIM_STYLE);
    tolerance_up_org.x = tolerance_up_org.y = 0;
    tolerance_down_org.x = tolerance_down_org.y = 0;
    note_org.x = note_org.y = 0;

    /* Compute the quote */
    if (text_cache_update(&data->quote_cache, data->quote, cr,
                          adg_dim_style_get_quote_style(dim_style))) {
        cp.x = data->quote_cache.extents.width;
        cp.y = data->quote_cache.extents.height / -2.;
    } else {
        cp.x = 0;
        cp.y = 0;
    }

    /* Compute the tolerances */
    if (data->tolerance_up != NULL || data->tolerance_down != NULL) {
        gdouble width;
        gdouble midspacing;

        adg_style_apply(adg_dim_style_get_tolerance_style(dim_style), cr);

        width = 0;
        midspacing = adg_dim_style_get_tolerance_spacing(dim_style) / 2.;
        cpml_pair_copy(&shift, adg_dim_style_get_tolerance_shift(dim_style));
        cp.x += shift.x;

        if (text_cache_update(&data->tolerance_up_cache,
                              data->tolerance_up, cr, NULL)) {
            tolerance_up_org.x = cp.x;
            tolerance_up_org.y = cp.y + shift.y - midspacing;

            width = data->tolerance_up_cache.extents.width;
        }

        if (text_cache_update(&data->tolerance_down_cache,
                              data->tolerance_down, cr, NULL)) {
            tolerance_down_org.x = cp.x;
            tolerance_down_org.y = cp.y + shift.y + midspacing +
                data->tolerance_down_cache.extents.height;

            if (data->tolerance_down_cache.extents.width > width)
                width = data->tolerance_down_cache.extents.width;
        }

        cp.x += width;
    }

    /* Compute the note */
    if (text_cache_update(&data->note_cache, data->note, cr,
                          adg_dim_style_get_note_style(dim_style))) {
        cpml_pair_copy(&shift, adg_dim_style_get_note_shift(dim_style));
        cp.x += shift.x;

        note_org.x = cp.x;
        note_org.y = cp.y + shift.y + data->note_cache.extents.height / 2.;

        cp.x += data->note_cache.extents.width;
    }

    /* Centering and shifting the whole group */
    cpml_pair_copy(&shift, adg_dim_style_get_quote_shift(dim_style));
    shift.x -= cp.x / 2.;

    if (data->quote_cache.glyphs) {
        text_cache_move_to(&data->quote_cache, &shift);
    }

    if (data->tolerance_up_cache.glyphs) {
        tolerance_up_org.x += shift.x;
        tolerance_up_org.y += shift.y;
        text_cache_move_to(&data->tolerance_up_cache, &tolerance_up_org);
    }

    if (data->tolerance_down_cache.glyphs) {
        tolerance_down_org.x += shift.x;
        tolerance_down_org.y += shift.y;
        text_cache_move_to(&data->tolerance_down_cache, &tolerance_down_org);
    }

    if (data->note_cache.glyphs) {
        note_org.x += shift.x;
        note_org.y += shift.y;
        text_cache_move_to(&data->note_cache, &note_org);
    }
}

static void
text_cache_init(AdgTextCache *text_cache)
{
    text_cache->utf8 = NULL;
    text_cache->utf8_len = -1;
    text_cache->glyphs = NULL;
    text_cache->num_glyphs = 0;
    text_cache->clusters = NULL;
    text_cache->num_clusters = 0;
    text_cache->cluster_flags = 0;
    memset(&text_cache->extents, 0, sizeof(text_cache->extents));
}

static gboolean
text_cache_update(AdgTextCache *text_cache, const gchar *text,
                  cairo_t *cr, AdgStyle *style)
{
    if (!text)
        return FALSE;

    text_cache->utf8 = text;
    text_cache->utf8_len = g_utf8_strlen(text, -1);

    if (style)
        adg_style_apply(style, cr);

    if (!text_cache->glyphs) {
        cairo_status_t status;

        status = cairo_scaled_font_text_to_glyphs(cairo_get_scaled_font(cr),
                                                  0, 0,
                                                  text_cache->utf8,
                                                  text_cache->utf8_len,
                                                  &text_cache->glyphs,
                                                  &text_cache->num_glyphs,
                                                  &text_cache->clusters,
                                                  &text_cache->num_clusters,
                                                  &text_cache->cluster_flags);

        if (status != CAIRO_STATUS_SUCCESS) {
            g_error("Unable to build glyphs (cairo message: %s)",
                    cairo_status_to_string(status));
            return FALSE;
        }

        cairo_glyph_extents(cr, text_cache->glyphs, text_cache->num_glyphs,
                            &text_cache->extents);
    }

    return TRUE;
}

static void
text_cache_clear(AdgTextCache *text_cache)
{
    text_cache->utf8 = NULL;
    text_cache->utf8_len = -1;

    if (text_cache->glyphs) {
        cairo_glyph_free(text_cache->glyphs);
        text_cache->glyphs = NULL;
        text_cache->num_glyphs = 0;
    }
    if (text_cache->clusters) {
        cairo_text_cluster_free(text_cache->clusters);
        text_cache->clusters = NULL;
        text_cache->num_clusters = 0;
        text_cache->cluster_flags = 0;
    }
    memset(&text_cache->extents, 0, sizeof(text_cache->extents));
}

static void
text_cache_move_to(AdgTextCache *text_cache, const CpmlPair *to)
{
    cairo_glyph_t *glyph;
    int cnt;
    gdouble x, y;

    glyph = text_cache->glyphs;
    cnt = text_cache->num_glyphs;
    x = to->x - glyph->x;
    y = to->y - glyph->y;

    while (cnt --) {
        glyph->x += x;
        glyph->y += y;
        ++ glyph;
    }
}

static void
text_cache_render(AdgTextCache *text_cache, cairo_t *cr)
{
    cairo_show_text_glyphs(cr, text_cache->utf8, text_cache->utf8_len,
                           text_cache->glyphs, text_cache->num_glyphs,
                           text_cache->clusters, text_cache->num_clusters,
                           text_cache->cluster_flags);
}
