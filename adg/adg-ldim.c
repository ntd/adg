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


#include "adg-ldim.h"
#include "adg-ldim-private.h"
#include "adg-dim-style.h"
#include "adg-intl.h"

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
static void             invalidate              (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static void             render                  (AdgEntity      *entity,
                                                 cairo_t        *cr);
static gchar *          default_value           (AdgDim         *dim);
static gdouble          get_distance            (AdgLDim        *ldim);
static void             update_shift            (AdgLDim        *ldim);
static void             update_entities         (AdgLDim        *ldim);
static gboolean         choose_outside          (AdgLDim        *ldim);
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

    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    dim_class->default_value = default_value;

    param = g_param_spec_double("direction",
                                P_("Direction"),
                                P_("The inclination angle of the extension lines"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, ADG_DIR_RIGHT,
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
    move_to.header.type = CAIRO_PATH_MOVE_TO;
    move_to.header.length = 2;
    line_to.header.type = CAIRO_PATH_LINE_TO;
    line_to.header.length = 2;

    data->distance = -1;
    data->direction = 0;
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

    data->shift.is_arranged = FALSE;

    ldim->data = data;
}

static void
dispose(GObject *object)
{
    dispose_markers((AdgLDim *) object);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
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
    AdgLDimPrivate *data = ((AdgLDim *) object)->data;

    switch (prop_id) {
    case PROP_DIRECTION:
        data->direction = g_value_get_double(value);
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
 * define the reference points with adg_dim_set_ref(), the dimension
 * direction with adg_ldim_set_direction() and the position reference
 * using adg_dim_set_pos() or, better, adg_ldim_set_pos().
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
 * @direction: angle where to extend the dimension
 * @pos: the position reference
 *
 * Creates a new linear dimension, specifing all the needed properties in
 * one shot.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new_full(const AdgPair *ref1, const AdgPair *ref2,
                  gdouble direction, const AdgPair *pos)
{
    AdgLDim *ldim = g_object_new(ADG_TYPE_LDIM, "ref1", ref1, "ref2", ref2,
                                 "direction", direction, NULL);
    adg_ldim_set_pos(ldim, pos);
    return ldim;
}

/**
 * adg_ldim_new_full_explicit:
 * @ref1_x: the x coordinate of the first reference point
 * @ref1_y: the y coordinate of the first reference point
 * @ref2_x: the x coordinate of the second reference point
 * @ref2_y: the y coordinate of the second reference point
 * @direction: angle where to extend the dimension
 * @pos_x: the x coordinate of the position reference
 * @pos_y: the y coordinate of the position reference
 *
 * Wrappes adg_ldim_new_full() with explicit values.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgLDim *
adg_ldim_new_full_explicit(gdouble ref1_x, gdouble ref1_y,
                           gdouble ref2_x, gdouble ref2_y,
                           gdouble direction, gdouble pos_x, gdouble pos_y)
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

    return adg_ldim_new_full(&ref1, &ref2, direction, &pos);
}

/**
 * adg_ldim_set_pos:
 * @ldim: an #AdgLDim entity
 * @pos: an #AdgPair structure
 *
 * Sets the position references (pos1 and pos2 properties) of @ldim using a
 * single @pos point. Before this call, @ldim MUST HAVE defined the reference
 * points and the direction. If these conditions are not met, an error message
 * is logged and the position references will not be set.
 **/
void
adg_ldim_set_pos(AdgLDim *ldim, const AdgPair *pos)
{
    AdgLDimPrivate *data;
    const AdgPair *ref1, *ref2;
    AdgPair pos1, pos2;
    CpmlPair baseline_vector, extension_vector;
    gdouble d, k;

    g_return_if_fail(ADG_IS_LDIM(ldim));

    data = ldim->data;
    ref1 = adg_dim_get_ref1((AdgDim *) ldim);
    ref2 = adg_dim_get_ref2((AdgDim *) ldim);

    cpml_vector_from_angle(&extension_vector, data->direction, 1);

    baseline_vector.x = -extension_vector.y;
    baseline_vector.y = extension_vector.x;

    d = extension_vector.y * baseline_vector.x -
        extension_vector.x * baseline_vector.y;
    g_return_if_fail(d != 0);

    k = ((pos->y - ref1->y) * baseline_vector.x -
         (pos->x - ref1->x) * baseline_vector.y) / d;
    pos1.x = ref1->x + k * extension_vector.x;
    pos1.y = ref1->y + k * extension_vector.y;

    k = ((pos->y - ref2->y) * baseline_vector.x -
         (pos->x - ref2->x) * baseline_vector.y) / d;
    pos2.x = ref2->x + k * extension_vector.x;
    pos2.y = ref2->y + k * extension_vector.y;

    data->distance = -1;

    adg_dim_set_pos((AdgDim *) ldim, &pos1, &pos2);
}

/**
 * adg_ldim_set_pos_explicit:
 * @ldim: an #AdgLDim entity
 * @x: the new x coordinate position reference
 * @y: the new y coordinate position reference
 *
 * Wrappers adg_ldim_set_pos() with explicit coordinates.
 **/
void
adg_ldim_set_pos_explicit(AdgLDim *ldim, gdouble x, gdouble y)
{
    AdgPair pos;

    pos.x = x;
    pos.y = y;

    adg_ldim_set_pos(ldim, &pos);
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
 * adg_ldim_set_direction:
 * @ldim: an #AdgLDim entity
 * @direction: an angle value, in radians
 *
 * Sets the direction angle where to extend @ldim.
 **/
void
adg_ldim_set_direction(AdgLDim *ldim, gdouble direction)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));

    data = ldim->data;
    data->direction = direction;

    g_object_notify((GObject *) ldim, "direction");
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
 * adg_ldim_switch_extension1:
 * @ldim: an #AdgLDim entity
 * @state: the new state
 *
 * Shows (if @state is %TRUE) or hide (if @state is %FALSE) the first
 * extension line of @ldim.
 **/
void
adg_ldim_switch_extension1(AdgLDim *ldim, gboolean state)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));

    data = ldim->data;

    data->has_extension1 = state;
    g_object_notify((GObject *) ldim, "has-extension1");
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

/**
 * adg_ldim_switch_extension2:
 * @ldim: an #AdgLDim entity
 * @state: the new state
 *
 * Shows (if @state is %TRUE) or hide (if @state is %FALSE) the second
 * extension line of @ldim.
 **/
void
adg_ldim_switch_extension2(AdgLDim *ldim, gboolean state)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));

    data = ldim->data;

    data->has_extension2 = state;
    g_object_notify((GObject *) ldim, "has-extension2");
}


static void
invalidate(AdgEntity *entity)
{
    AdgLDim *ldim;
    AdgLDimPrivate *data;

    ldim = (AdgLDim *) entity;
    data = ldim->data;

    dispose_markers(ldim);
    data->shift.is_arranged = FALSE;

    if (PARENT_ENTITY_CLASS->invalidate != NULL)
        PARENT_ENTITY_CLASS->invalidate(entity);
}

static void
arrange(AdgEntity *entity)
{
    AdgLDim *ldim;
    AdgDim *dim;
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgContainer *quote;
    gboolean outside;
    const AdgMatrix *local;
    AdgPair ref1, ref2, pos1, pos2;
    AdgPair pair;
    gint n;

    PARENT_ENTITY_CLASS->arrange(entity);

    ldim = (AdgLDim *) entity;
    dim = (AdgDim *) ldim;
    data = ldim->data;
    dim_style = adg_dim_get_dim_style(dim);
    quote = adg_dim_get_quote(dim);

    update_shift(ldim);
    update_entities(ldim);

    switch (adg_dim_get_outside(dim)) {
    case ADG_THREE_STATE_OFF:
        outside = FALSE;
        break;
    case ADG_THREE_STATE_ON:
        outside = TRUE;
        break;
    case ADG_THREE_STATE_UNKNOWN:
    default:
        outside = choose_outside(ldim);
        break;
    }

    local = adg_entity_local_matrix(entity);
    cpml_pair_copy(&ref1, adg_dim_get_ref1(dim));
    cpml_pair_copy(&ref2, adg_dim_get_ref2(dim));
    cpml_pair_copy(&pos1, adg_dim_get_pos1(dim));
    cpml_pair_copy(&pos2, adg_dim_get_pos2(dim));

    cpml_pair_transform(&ref1, local);
    cpml_pair_transform(&ref2, local);
    cpml_pair_transform(&pos1, local);
    cpml_pair_transform(&pos2, local);

    cpml_pair_add(cpml_pair_copy(&pair, &ref1), &data->shift.from);
    cpml_pair_to_cairo(&pair, &data->cpml.data[13]);

    cpml_pair_add(cpml_pair_copy(&pair, &pos1), &data->shift.marker);
    cpml_pair_to_cairo(&pair, &data->cpml.data[1]);

    cpml_pair_add(&pair, &data->shift.to);
    cpml_pair_to_cairo(&pair, &data->cpml.data[15]);

    cpml_pair_add(cpml_pair_copy(&pair, &ref2), &data->shift.from);
    cpml_pair_to_cairo(&pair, &data->cpml.data[17]);

    cpml_pair_add(cpml_pair_copy(&pair, &pos2), &data->shift.marker);
    cpml_pair_to_cairo(&pair, &data->cpml.data[3]);

    cpml_pair_add(&pair, &data->shift.to);
    cpml_pair_to_cairo(&pair, &data->cpml.data[19]);

    /* Calculate the outside segments */
    if (outside) {
        gdouble beyond;
        CpmlVector vector;

        beyond = adg_dim_style_get_beyond(dim_style);
        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);

        cpml_pair_from_cairo(&vector, &data->cpml.data[3]);
        cpml_pair_sub(&vector, &pair);
        cpml_vector_set_length(&vector, beyond);

        cpml_pair_from_cairo(&pair, &data->cpml.data[1]);
        cpml_pair_to_cairo(&pair, &data->cpml.data[5]);

        cpml_pair_sub(&pair, &vector);
        cpml_pair_to_cairo(&pair, &data->cpml.data[7]);

        cpml_pair_from_cairo(&pair, &data->cpml.data[3]);
        cpml_pair_to_cairo(&pair, &data->cpml.data[11]);

        cpml_pair_add(&pair, &vector);
        cpml_pair_to_cairo(&pair, &data->cpml.data[9]);

        data->cpml.data[2].header.length = 2;
        data->cpml.data[10].header.length = 2;
        n = 10;
    } else {
        data->cpml.data[2].header.length = 10;
        n = 2;
    }

    /* Play with header lengths to show or hide the extension lines */
    if (data->has_extension1) {
        data->cpml.data[14].header.length = data->has_extension2 ? 2 : 6;
    } else {
        data->cpml.data[n].header.length += 4;
        if (!data->has_extension2)
            data->cpml.data[n].header.length += 4;
    }

    data->cpml.path.status = CAIRO_STATUS_SUCCESS;

    if (quote != NULL) {
        /* Update global and local map of the quote container */
        AdgEntity *quote_entity;
        gdouble angle;
        AdgMatrix matrix;

        quote_entity = (AdgEntity *) quote;
        angle = adg_dim_quote_angle(dim, data->direction + G_PI_2);
        adg_matrix_copy(&matrix, local);
        cairo_matrix_invert(&matrix);

        pair.x = (data->cpml.data[1].point.x + data->cpml.data[3].point.x) / 2;
        pair.y = (data->cpml.data[1].point.y + data->cpml.data[3].point.y) / 2;
        cairo_matrix_transform_point(&matrix, &pair.x, &pair.y);
        cairo_matrix_init_translate(&matrix, pair.x, pair.y);
        adg_entity_set_local_map(quote_entity, &matrix);

        cairo_matrix_init_rotate(&matrix, angle);
        adg_entity_transform_global_map(quote_entity, &matrix, ADG_TRANSFORM_BEFORE);
    }

    if (data->marker1 != NULL) {
        adg_marker_set_segment(data->marker1, data->trail, outside ? 2 : 1);
        adg_entity_local_changed((AdgEntity *) data->marker1);
    }

    if (data->marker2 != NULL) {
        adg_marker_set_segment(data->marker2, data->trail, outside ? 3 : 1);
        adg_entity_local_changed((AdgEntity *) data->marker2);
    }

    /* TODO: compute the extents */
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgLDim *ldim;
    AdgDim *dim;
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;

    ldim = (AdgLDim *) entity;
    dim = (AdgDim *) entity;
    data = ldim->data;
    dim_style = adg_dim_get_dim_style(dim);

    dress = adg_dim_style_get_color_dress(dim_style);
    adg_entity_apply_dress(entity, dress, cr);

    if (data->marker1 != NULL)
        adg_entity_render((AdgEntity *) data->marker1, cr);

    if (data->marker2 != NULL)
        adg_entity_render((AdgEntity *) data->marker2, cr);

    adg_entity_render((AdgEntity *) adg_dim_get_quote(dim), cr);

    /* This CpmlPath has no arcs, so it can be fed directly into cairo */
    dress = adg_dim_style_get_line_dress(dim_style);
    adg_entity_apply_dress(entity, dress, cr);
    cairo_append_path(cr, &data->cpml.path);
    cairo_stroke(cr);
}

static gchar *
default_value(AdgDim *dim)
{
    AdgDimStyle *dim_style;
    const gchar *format;
    gdouble distance;

    dim_style = adg_dim_get_dim_style(dim);
    format = adg_dim_style_get_number_format(dim_style);
    distance = get_distance((AdgLDim *) dim);

    return g_strdup_printf(format, distance);
}

static gdouble
get_distance(AdgLDim *ldim)
{
    AdgLDimPrivate *data = ldim->data;

    if (data->distance < 0) {
        AdgDim *dim;
        const AdgPair *pos1;
        const AdgPair *pos2;

        dim = (AdgDim *) ldim;
        pos1 = adg_dim_get_pos1(dim);
        pos2 = adg_dim_get_pos2(dim);

        data->distance = cpml_pair_distance(pos1, pos2);
    }

    return data->distance;
}

static void
update_shift(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;
    AdgMatrix matrix;
    gdouble from_offset;
    gdouble to_offset;
    gdouble baseline_spacing;
    gdouble level;

    data = ldim->data;

    if (data->shift.is_arranged)
        return;

    dim_style = adg_dim_get_dim_style((AdgDim *) ldim);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level((AdgDim *) ldim);

    cairo_matrix_init_rotate(&matrix, data->direction);

    data->shift.from.x = data->shift.from.y = 0;
    data->shift.marker.x = data->shift.marker.y = 0;
    data->shift.to.x = data->shift.to.y = 0;

    cairo_matrix_translate(&matrix, from_offset, 0);
    cpml_pair_transform(&data->shift.from, &matrix);
    cairo_matrix_translate(&matrix, to_offset-from_offset, 0);
    cpml_pair_transform(&data->shift.to, &matrix);
    cairo_matrix_translate(&matrix, level*baseline_spacing-to_offset, 0);
    cpml_pair_transform(&data->shift.marker, &matrix);

    data->shift.is_arranged = TRUE;
}

static void
update_entities(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgDimStyle *dim_style;

    data = ldim->data;
    dim_style = adg_dim_get_dim_style((AdgDim *) ldim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(trail_callback, ldim);

    if (data->marker1 == NULL)
        data->marker1 = adg_dim_style_marker1_new(dim_style);

    if (data->marker2 == NULL)
        data->marker2 = adg_dim_style_marker2_new(dim_style);
}

static gboolean
choose_outside(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgContainer *quote;
    const AdgMatrix *local;
    CpmlExtents extents;
    gdouble marker1, marker2;
    gdouble needed, available;

    data = ldim->data;
    quote = adg_dim_get_quote((AdgDim *) ldim);
    local = adg_entity_local_matrix((AdgEntity *) ldim);
    adg_entity_get_extents((AdgEntity *) quote, &extents);
    marker1 = data->marker1 == NULL ? 0 : adg_marker_get_size(data->marker1);
    marker2 = data->marker2 == NULL ? 0 : adg_marker_get_size(data->marker2);

    needed = extents.size.x + marker1 + marker2;
    available = get_distance(ldim) * local->xx;

    return needed > available;
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

    adg_trail_clear_cairo_path(trail);

    return &data->cpml.path;
}
