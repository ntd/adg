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
 * SECTION:adg-ldim
 * @short_description: Linear dimensions
 *
 * The #AdgLDim entity represents a linear dimension.
 **/

/**
 * AdgLDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-ldim.h"
#include "adg-ldim-private.h"
#include "adg-dim-private.h"
#include "adg-dim-style.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_ldim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_ldim_parent_class)


enum {
    PROP_0,
    PROP_DIRECTION,
    PROP_HAS_EXTENSION1,
    PROP_HAS_EXTENSION2
};


static void             dispose                 (GObject        *object);
static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             local_changed           (AdgEntity      *entity);
static void             invalidate              (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static gchar *          default_value           (AdgDim         *dim);
static gboolean         set_direction           (AdgLDim        *ldim,
                                                 gdouble         direction);
static void             update_geometry         (AdgLDim        *ldim);
static void             update_shift            (AdgLDim        *ldim);
static void             update_entities         (AdgLDim        *ldim);
static void             choose_flags            (AdgLDim        *ldim,
                                                 gboolean       *to_outside,
                                                 gboolean       *to_detach);
static void             unset_trail             (AdgLDim        *ldim);
static void             dispose_markers         (AdgLDim        *ldim);
static CpmlPath *       trail_callback          (AdgTrail       *trail,
                                                 gpointer        user_data);


G_DEFINE_TYPE(AdgLDim, adg_ldim, ADG_TYPE_DIM);


static void
adg_ldim_class_init(AdgLDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    AdgDimClass *dim_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;
    dim_class = (AdgDimClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgLDimPrivate));

    gobject_class->dispose = dispose;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->local_changed = local_changed;
    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    dim_class->default_value = default_value;

    param = g_param_spec_double("direction",
                                P_("Direction"),
                                P_("The inclination angle of the extension lines"),
                                -G_PI, G_PI, ADG_DIR_RIGHT,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_DIRECTION, param);

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
adg_ldim_init(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    cairo_path_data_t move_to, line_to;

    data = G_TYPE_INSTANCE_GET_PRIVATE(ldim, ADG_TYPE_LDIM, AdgLDimPrivate);
    move_to.header.type = CPML_MOVE;
    move_to.header.length = 2;
    line_to.header.type = CPML_LINE;
    line_to.header.length = 2;

    data->direction = ADG_DIR_RIGHT;
    data->has_extension1 = TRUE;
    data->has_extension2 = TRUE;

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.path.data = data->cpml.data;
    data->cpml.path.num_data = G_N_ELEMENTS(data->cpml.data);
    data->cpml.path.data[0] = move_to;
    data->cpml.path.data[2] = line_to;
    data->cpml.path.data[4] = move_to;
    data->cpml.path.data[6] = line_to;
    data->cpml.path.data[8] = move_to;
    data->cpml.path.data[10] = line_to;
    data->cpml.path.data[12] = move_to;
    data->cpml.path.data[14] = line_to;
    data->cpml.path.data[16] = move_to;
    data->cpml.path.data[18] = line_to;

    data->trail = NULL;
    data->marker1 = NULL;
    data->marker2 = NULL;

    data->geometry.is_arranged = FALSE;
    data->shift.is_arranged = FALSE;

    ldim->data = data;
}

static void
dispose(GObject *object)
{
    dispose_markers((AdgLDim *) object);

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgLDimPrivate *data = ((AdgLDim *) object)->data;

    switch (prop_id) {
    case PROP_DIRECTION:
        g_value_set_double(value, data->direction);
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
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgLDim *ldim;
    AdgLDimPrivate *data;

    ldim = (AdgLDim *) object;
    data = ldim->data;

    switch (prop_id) {
    case PROP_DIRECTION:
        set_direction(ldim, g_value_get_double(value));
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
 * adg_ldim_new:
 *
 * Creates a new - undefined - linear dimension. You must, at least,
 * define the start of the dimension in #AdgDim:ref1, the end in
 * #AdgDim:ref2 and the position of the quote in #AdgDim:pos using
 * any valid #AdgDim method. The director of the dimension (that is,
 * if it is horizontal, vertical or oblique at a specific angle)
 * should be specified with adg_ldim_set_direction().
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new(void)
{
    return g_object_new(ADG_TYPE_LDIM, NULL);
}

/**
 * adg_ldim_new_full:
 * @ref1: the first reference point
 * @ref2: the second reference point
 * @pos: the position reference
 * @direction: angle where to extend the dimension
 *
 * Creates a new linear dimension, specifing all the needed properties in
 * one shot.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new_full(const AdgPair *ref1, const AdgPair *ref2,
                  const AdgPair *pos, gdouble direction)
{
    AdgLDim *ldim;
    AdgDim *dim;

    ldim = g_object_new(ADG_TYPE_LDIM, "direction", direction, NULL);
    dim = (AdgDim *) ldim;

    adg_dim_set_ref1_from_pair(dim, ref1);
    adg_dim_set_ref2_from_pair(dim, ref2);
    adg_dim_set_pos_from_pair(dim, pos);

    return ldim;
}

/**
 * adg_ldim_new_full_explicit:
 * @ref1_x: the x coordinate of the first reference point
 * @ref1_y: the y coordinate of the first reference point
 * @ref2_x: the x coordinate of the second reference point
 * @ref2_y: the y coordinate of the second reference point
 * @pos_x: the x coordinate of the position reference
 * @pos_y: the y coordinate of the position reference
 * @direction: angle where to extend the dimension
 *
 * Wrappes adg_ldim_new_full() with explicit values.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new_full_explicit(gdouble ref1_x, gdouble ref1_y,
                           gdouble ref2_x, gdouble ref2_y,
                           gdouble pos_x,  gdouble pos_y, gdouble direction)
{
    AdgPair ref1;
    AdgPair ref2;
    AdgPair pos;

    ref1.x = ref1_x;
    ref1.y = ref1_y;
    ref2.x = ref2_x;
    ref2.y = ref2_y;
    pos.x = pos_x;
    pos.y = pos_y;

    return adg_ldim_new_full(&ref1, &ref2, &pos, direction);
}

/**
 * adg_ldim_new_full_from_model:
 * @model: the model from which the named pairs are taken
 * @ref1: the first reference point
 * @ref2: the second reference point
 * @pos: the position reference
 * @direction: angle where to extend the dimension
 *
 * Creates a new linear dimension, specifing all the needed properties in
 * one shot and using named pairs from @model.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new_full_from_model(AdgModel *model,
                             const gchar *ref1, const gchar *ref2,
                             const gchar *pos, gdouble direction)
{
    AdgLDim *ldim;
    AdgDim *dim;

    ldim = g_object_new(ADG_TYPE_LDIM, "direction", direction, NULL);
    dim = (AdgDim *) ldim;

    adg_dim_set_ref1_from_model(dim, model, ref1);
    adg_dim_set_ref2_from_model(dim, model, ref2);
    adg_dim_set_pos_from_model(dim, model, pos);

    return ldim;
}

/**
 * adg_ldim_set_direction:
 * @ldim: an #AdgLDim entity
 * @direction: an angle value, in radians
 *
 * Sets the direction angle where to extend @ldim.
 * @direction is normalized by cpml_angle() before being used.
 **/
void
adg_ldim_set_direction(AdgLDim *ldim, gdouble direction)
{
    g_return_if_fail(ADG_IS_LDIM(ldim));

    if (set_direction(ldim, direction))
        g_object_notify((GObject *) ldim, "direction");
}

/**
 * adg_ldim_get_direction:
 * @ldim: an #AdgLDim entity
 *
 * Gets the direction where @ldim will extend.
 *
 * Returns: the direction angle in radians
 **/
gdouble
adg_ldim_get_direction(AdgLDim *ldim)
{
    AdgLDimPrivate *data;

    g_return_val_if_fail(ADG_IS_LDIM(ldim), 0);

    data = ldim->data;

    return data->direction;
}

/**
 * adg_ldim_switch_extension1:
 * @ldim: an #AdgLDim entity
 * @new_state: the new state
 *
 * Shows (if @new_state is %TRUE) or hide (if @new_state is %FALSE)
 * the first extension line of @ldim.
 **/
void
adg_ldim_switch_extension1(AdgLDim *ldim, gboolean new_state)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));
    g_return_if_fail(adg_is_boolean_value(new_state));

    data = ldim->data;

    if (data->has_extension1 != new_state) {
        data->has_extension1 = new_state;
        g_object_notify((GObject *) ldim, "has-extension1");
    }
}

/**
 * adg_ldim_has_extension1:
 * @ldim: an #AdgLDim entity
 *
 * Checks if @ldim should render also the first extension line.
 *
 * Returns: %TRUE on first extension line presents, %FALSE otherwise
 **/
gboolean
adg_ldim_has_extension1(AdgLDim *ldim)
{
    AdgLDimPrivate *data;

    g_return_val_if_fail(ADG_IS_LDIM(ldim), FALSE);

    data = ldim->data;

    return data->has_extension1;
}

/**
 * adg_ldim_switch_extension2:
 * @ldim: an #AdgLDim entity
 * @new_state: the new new_state
 *
 * Shows (if @new_state is %TRUE) or hide (if @new_state is %FALSE)
 * the second extension line of @ldim.
 **/
void
adg_ldim_switch_extension2(AdgLDim *ldim, gboolean new_state)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));
    g_return_if_fail(adg_is_boolean_value(new_state));

    data = ldim->data;

    if (data->has_extension2 != new_state) {
        data->has_extension2 = new_state;
        g_object_notify((GObject *) ldim, "has-extension2");
    }
}

/**
 * adg_ldim_has_extension2:
 * @ldim: an #AdgLDim entity
 *
 * Checks if @ldim should render also the second extension line.
 *
 * Returns: %TRUE on first extension line presents, %FALSE otherwise
 **/
gboolean
adg_ldim_has_extension2(AdgLDim *ldim)
{
    AdgLDimPrivate *data;

    g_return_val_if_fail(ADG_IS_LDIM(ldim), FALSE);

    data = ldim->data;

    return data->has_extension2;
}


static void
local_changed(AdgEntity *entity)
{
    unset_trail((AdgLDim *) entity);

    if (PARENT_ENTITY_CLASS->local_changed)
        PARENT_ENTITY_CLASS->local_changed(entity);
}

static void
invalidate(AdgEntity *entity)
{
    AdgLDim *ldim;
    AdgLDimPrivate *data;

    ldim = (AdgLDim *) entity;
    data = ldim->data;

    dispose_markers(ldim);
    data->geometry.is_arranged = FALSE;
    data->shift.is_arranged = FALSE;
    unset_trail(ldim);

    if (PARENT_ENTITY_CLASS->invalidate)
        PARENT_ENTITY_CLASS->invalidate(entity);
}

/* TODO: split the following crap in more manageable functions */
static void
arrange(AdgEntity *entity)
{
    AdgLDim *ldim;
    AdgDim *dim;
    AdgLDimPrivate *data;
    AdgAlignment *quote;
    AdgDimStyle *dim_style;
    gboolean to_outside, to_detach;
    const AdgMatrix *local;
    AdgPair ref1, ref2, base1, base2;
    AdgPair pair;
    gint n;
    CpmlExtents extents;

    if (PARENT_ENTITY_CLASS->arrange)
        PARENT_ENTITY_CLASS->arrange(entity);

    ldim = (AdgLDim *) entity;
    dim = (AdgDim *) ldim;
    data = ldim->data;
    quote = adg_dim_get_quote(dim);

    update_geometry(ldim);
    update_shift(ldim);
    update_entities(ldim);

    /* Check for cached result */
    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS) {
        AdgEntity *quote_entity = (AdgEntity *) quote;
        adg_entity_set_global_map(quote_entity, &data->quote.global_map);
        return;
    }

    choose_flags(ldim, &to_outside, &to_detach);

    dim_style = GET_DIM_STYLE(dim);
    local = adg_entity_get_local_matrix(entity);
    cpml_pair_copy(&ref1, adg_point_get_pair(adg_dim_get_ref1(dim)));
    cpml_pair_copy(&ref2, adg_point_get_pair(adg_dim_get_ref2(dim)));
    cpml_pair_copy(&base1, &data->geometry.base1);
    cpml_pair_copy(&base2, &data->geometry.base2);

    cpml_pair_transform(&ref1, local);
    cpml_pair_transform(&ref2, local);
    cpml_pair_transform(&base1, local);
    base1.x += data->shift.base.x;
    base1.y += data->shift.base.y;
    cpml_pair_transform(&base2, local);
    base2.x += data->shift.base.x;
    base2.y += data->shift.base.y;

    pair.x = ref1.x + data->shift.from.x;
    pair.y = ref1.y + data->shift.from.y;
    cpml_pair_to_cairo(&pair, &data->cpml.data[13]);

    cpml_pair_to_cairo(&base1, &data->cpml.data[1]);

    pair.x = base1.x + data->shift.to.x;
    pair.y = base1.y + data->shift.to.y;
    cpml_pair_to_cairo(&pair, &data->cpml.data[15]);

    pair.x = ref2.x + data->shift.from.x;
    pair.y = ref2.y + data->shift.from.y;
    cpml_pair_to_cairo(&pair, &data->cpml.data[17]);

    cpml_pair_to_cairo(&base2, &data->cpml.data[3]);

    pair.x = base2.x + data->shift.to.x;
    pair.y = base2.y + data->shift.to.y;
    cpml_pair_to_cairo(&pair, &data->cpml.data[19]);

    /* Calculate the outside segments */
    if (to_outside) {
        gdouble beyond;
        CpmlVector vector;

        beyond = adg_dim_style_get_beyond(dim_style);
        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);

        cpml_pair_from_cairo(&vector, &data->cpml.data[3]);
        vector.x -= pair.x;
        vector.y -= pair.y;
        cpml_vector_set_length(&vector, beyond);

        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);
        cpml_pair_to_cairo(&pair, &data->cpml.data[5]);

        pair.x -= vector.x;
        pair.y -= vector.y;
        cpml_pair_to_cairo(&pair, &data->cpml.data[7]);

        cpml_pair_from_cairo(&pair, &data->cpml.data[3]);
        cpml_pair_to_cairo(&pair, &data->cpml.data[11]);

        pair.x += vector.x;
        pair.y += vector.y;
        cpml_pair_to_cairo(&pair, &data->cpml.data[9]);

        data->cpml.data[2].header.length = 2;
        n = 10;
    } else {
        data->cpml.data[2].header.length = 10;
        n = 2;
    }

    data->cpml.data[10].header.length = 2;
    extents.is_defined = FALSE;

    /* Add the quote */
    if (quote != NULL) {
        AdgEntity *quote_entity;
        gdouble angle;
        AdgPair middle, factor;
        AdgMatrix map;

        quote_entity = (AdgEntity *) quote;
        angle = adg_dim_quote_angle(dim, data->direction + G_PI_2);
        middle.x = (base1.x + base2.x) / 2;
        middle.y = (base1.y + base2.y) / 2;
        factor.x = factor.y = 0;

        if (to_detach) {
            /* Detached quote: position the quote at "pos" */
            AdgPair p_line, p_quote;
            gdouble same_sd, opposite_sd, quote_size;
            gint n_side;
            cairo_path_data_t *to_extend;

            /* Set "pair" to the properly converted "pos" coordinates */
            cpml_pair_copy(&pair, adg_point_get_pair(adg_dim_get_pos(dim)));
            cpml_pair_transform(&pair, local);
            pair.x += data->shift.base.x;
            pair.y += data->shift.base.y;

            /* Taking "middle" as the reference, check if the quote
             * is on the _left_ or on the _right_ side. This is got
             * by checking if (pos-middle) is closer to the quote
             * vector or to the negated quote vector by using an
             * algorithm based on the squared distances. */
            p_line.x = pair.x - middle.x;
            p_line.y = pair.y - middle.y;
            cpml_vector_from_angle(&p_quote, angle);

            same_sd = cpml_pair_squared_distance(&p_quote, &p_line);
            p_quote.x = -p_quote.x;
            p_quote.y = -p_quote.y;
            opposite_sd = cpml_pair_squared_distance(&p_quote, &p_line);
            quote_size = adg_entity_get_extents(quote_entity)->size.x;

            /* Properly align the quote, depending on its side */
            if (same_sd > opposite_sd) {
                factor.x = 1;
            } else {
                factor.x = 0;
                p_quote.x = -p_quote.x;
                p_quote.y = -p_quote.y;
            }

            cpml_vector_set_length(&p_quote, quote_size);
            p_quote.x += pair.x;
            p_quote.y += pair.y;

            /* Extends the base line to include the "p_quote" pair,
             * that is underline a detached quote */
            if (cpml_pair_squared_distance(&p_quote, &base1) >
                cpml_pair_squared_distance(&p_quote, &base2))
                n_side = 2;
            else
                n_side = 1;

            if (to_outside) {
                to_extend = &data->cpml.data[n_side == 1 ? 7 : 9];
            } else {
                to_extend = &data->cpml.data[9];
                cpml_pair_to_cairo(&middle, &data->cpml.data[9]);
                cpml_pair_to_cairo(n_side == 1 ? &base1 : &base2,
                                   &data->cpml.data[11]);
                data->cpml.data[2].header.length = 6;
                n = 10;
            }

            cpml_pair_from_cairo(&p_line, to_extend);

            /* Extend the base line only if needed */
            if (cpml_pair_squared_distance(&p_quote, &middle) >
                cpml_pair_squared_distance(&p_line, &middle))
                cpml_pair_to_cairo(&p_quote, to_extend);
        } else {
            /* Center the quote in the middle of the base line */
            factor.x = 0.5;
            cpml_pair_copy(&pair, &middle);
        }

        adg_alignment_set_factor(quote, &factor);
        cairo_matrix_init_translate(&map, pair.x, pair.y);
        cairo_matrix_rotate(&map, angle);
        adg_entity_set_global_map(quote_entity, &map);
        adg_entity_arrange(quote_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(quote_entity));

        adg_matrix_copy(&data->quote.global_map,
                        adg_entity_get_global_map(quote_entity));
    }

    /* Play with header lengths to show or hide the extension lines */
    if (data->has_extension1) {
        data->cpml.data[14].header.length = data->has_extension2 ? 2 : 6;
    } else {
        data->cpml.data[14].header.length = 2;
        data->cpml.data[n].header.length += 4;
        if (!data->has_extension2)
            data->cpml.data[n].header.length += 4;
    }

    data->cpml.path.status = CAIRO_STATUS_SUCCESS;
    cpml_extents_add(&extents, adg_trail_get_extents(data->trail));

    if (data->marker1 != NULL) {
        AdgEntity *marker1_entity = (AdgEntity *) data->marker1;
        adg_marker_set_segment(data->marker1, data->trail, to_outside ? 2 : 1);
        adg_entity_local_changed(marker1_entity);
        adg_entity_arrange(marker1_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(marker1_entity));
    }

    if (data->marker2 != NULL) {
        AdgEntity *marker2_entity = (AdgEntity *) data->marker2;
        adg_marker_set_segment(data->marker2, data->trail, to_outside ? 3 : 1);
        adg_entity_local_changed(marker2_entity);
        adg_entity_arrange(marker2_entity);
        cpml_extents_add(&extents, adg_entity_get_extents(marker2_entity));
    }

    adg_entity_set_extents(entity, &extents);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgLDim *ldim;
    AdgDim *dim;
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;
    const cairo_path_t *cairo_path;

    ldim = (AdgLDim *) entity;
    dim = (AdgDim *) entity;
    data = ldim->data;
    dim_style = GET_DIM_STYLE(dim);

    adg_style_apply((AdgStyle *) dim_style, entity, cr);

    if (data->marker1 != NULL)
        adg_entity_render((AdgEntity *) data->marker1, cr);

    if (data->marker2 != NULL)
        adg_entity_render((AdgEntity *) data->marker2, cr);

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
    AdgLDim *ldim;
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    const gchar *format;

    ldim = (AdgLDim *) dim;
    data = ldim->data;
    dim_style = GET_DIM_STYLE(dim);
    format = adg_dim_style_get_number_format(dim_style);

    update_geometry(ldim);

    return g_strdup_printf(format, data->geometry.distance);
}

static gboolean
set_direction(AdgLDim *ldim, gdouble direction)
{
    AdgLDimPrivate *data = ldim->data;

    direction = cpml_angle(direction);

    if (data->direction == direction)
        return FALSE;

    data->direction = direction;

    return TRUE;
}

static void
update_geometry(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgDim *dim;
    const AdgPair *ref1, *ref2;
    const AdgPair *pos;
    CpmlVector baseline, extension;
    gdouble d, k;

    data = ldim->data;

    if (data->geometry.is_arranged)
        return;

    dim = (AdgDim *) ldim;
    ref1 = adg_point_get_pair(adg_dim_get_ref1(dim));
    ref2 = adg_point_get_pair(adg_dim_get_ref2(dim));
    pos = adg_point_get_pair(adg_dim_get_pos(dim));
    cpml_vector_from_angle(&extension, data->direction);
    cpml_pair_copy(&baseline, &extension);
    cpml_vector_normal(&baseline);

    d = extension.y * baseline.x -
        extension.x * baseline.y;
    g_return_if_fail(d != 0);

    k = ((pos->y - ref1->y) * baseline.x -
         (pos->x - ref1->x) * baseline.y) / d;
    data->geometry.base1.x = ref1->x + k * extension.x;
    data->geometry.base1.y = ref1->y + k * extension.y;

    k = ((pos->y - ref2->y) * baseline.x -
         (pos->x - ref2->x) * baseline.y) / d;
    data->geometry.base2.x = ref2->x + k * extension.x;
    data->geometry.base2.y = ref2->y + k * extension.y;

    data->geometry.distance = cpml_pair_distance(&data->geometry.base1,
                                                 &data->geometry.base2);

    data->geometry.is_arranged = TRUE;
}

static void
update_shift(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    gdouble from_offset, to_offset;
    gdouble baseline_spacing, level;
    CpmlVector vector;

    data = ldim->data;

    if (data->shift.is_arranged)
        return;

    dim_style = GET_DIM_STYLE(ldim);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level((AdgDim *) ldim);

    cpml_vector_from_angle(&vector, data->direction);

    cpml_vector_set_length(&vector, from_offset);
    cpml_pair_copy(&data->shift.from, &vector);

    cpml_vector_set_length(&vector, to_offset);
    cpml_pair_copy(&data->shift.to, &vector);

    cpml_vector_set_length(&vector, level * baseline_spacing);
    cpml_pair_copy(&data->shift.base, &vector);

    data->shift.is_arranged = TRUE;
}

static void
update_entities(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;

    data = ldim->data;
    dim_style = GET_DIM_STYLE(ldim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(trail_callback, ldim);

    if (data->marker1 == NULL)
        data->marker1 = adg_dim_style_marker1_new(dim_style);

    if (data->marker2 == NULL)
        data->marker2 = adg_dim_style_marker2_new(dim_style);
}

static void
choose_flags(AdgLDim *ldim, gboolean *to_outside, gboolean *to_detach)
{
    AdgDim *dim;
    AdgThreeState outside, detached;
    AdgLDimPrivate *data;
    AdgEntity *quote;
    const AdgMatrix *local;
    gdouble available_space, markers_space, quote_space;

    dim = (AdgDim *) ldim;
    outside = adg_dim_get_outside(dim);
    detached = adg_dim_get_detached(dim);

    *to_outside = outside == ADG_THREE_STATE_ON;
    *to_detach = detached == ADG_THREE_STATE_ON;

    /* On explicit flags, no further investigation is required */
    if (outside != ADG_THREE_STATE_UNKNOWN &&
        detached != ADG_THREE_STATE_UNKNOWN)
        return;

    data = ldim->data;
    quote = (AdgEntity *) adg_dim_get_quote((AdgDim *) ldim);
    local = adg_entity_get_local_matrix((AdgEntity *) ldim);
    available_space = data->geometry.distance * local->xx;

    if (outside == ADG_THREE_STATE_ON)
        markers_space = 0;
    else
        markers_space =
            (data->marker1 == NULL ? 0 : adg_marker_get_size(data->marker1)) +
            (data->marker2 == NULL ? 0 : adg_marker_get_size(data->marker2));

    /* Leave at least 0.25 marker_space between the markers */
    if (detached == ADG_THREE_STATE_ON)
        quote_space = markers_space * 0.25;
    else
        quote_space = adg_entity_get_extents(quote)->size.x;

    if (outside == ADG_THREE_STATE_UNKNOWN &&
        detached == ADG_THREE_STATE_UNKNOWN) {
        /* Both flags need to be choosed */
        if (quote_space + markers_space < available_space) {
            *to_detach = FALSE;
            *to_outside = FALSE;
        } else if (quote_space < available_space) {
            *to_detach = FALSE;
            *to_outside = TRUE;
        } else {
            *to_detach = TRUE;
            *to_outside = markers_space * 1.25 > available_space;
        }
    } else if (outside == ADG_THREE_STATE_UNKNOWN) {
        /* Only the outside flag may be guessed */
        *to_outside = quote_space + markers_space > available_space;
    } else {
        /* Only the detached flag may be guessed */
        *to_detach = quote_space + markers_space > available_space;
    }
}

static void
unset_trail(AdgLDim *ldim)
{
    AdgLDimPrivate *data = ldim->data;

    if (data->trail != NULL)
        adg_model_clear((AdgModel *) data->trail);

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}

static void
dispose_markers(AdgLDim *ldim)
{
    AdgLDimPrivate *data = ldim->data;

    if (data->trail != NULL) {
        g_object_unref(data->trail);
        data->trail = NULL;
    }

    if (data->marker1 != NULL) {
        g_object_unref(data->marker1);
        data->marker1 = NULL;
    }

    if (data->marker2 != NULL) {
        g_object_unref(data->marker2);
        data->marker2 = NULL;
    }
}

static CpmlPath *
trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgLDim *ldim;
    AdgLDimPrivate *data;

    ldim = (AdgLDim *) user_data;
    data = ldim->data;

    return &data->cpml.path;
}
