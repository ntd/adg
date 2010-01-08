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
 * SECTION:adg-adim
 * @short_description: Angular dimensions
 *
 * The #AdgADim entity defines an angular dimension.
 */

/**
 * AdgADim:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-adim.h"
#include "adg-adim-private.h"
#include "adg-dim-private.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_adim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_adim_parent_class)


enum {
    PROP_0,
    PROP_ORG1,
    PROP_ORG2,
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
static void             update_geometry         (AdgADim        *adim);
static void             update_entities         (AdgADim        *adim);
static void             unset_trail             (AdgADim        *adim);
static void             dispose_markers         (AdgADim        *adim);
static gboolean         get_info                (AdgADim        *adim,
                                                 CpmlVector      vector[],
                                                 AdgPair        *center,
                                                 gdouble        *distance);
static CpmlPath *       trail_callback          (AdgTrail       *trail,
                                                 gpointer        user_data);


G_DEFINE_TYPE(AdgADim, adg_adim, ADG_TYPE_DIM);


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

    gobject_class->dispose = dispose;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->local_changed = local_changed;
    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    dim_class->default_value = default_value;

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
}

static void
adg_adim_init(AdgADim *adim)
{
    AdgADimPrivate *data;
    cairo_path_data_t move_to, line_to, arc_to;

    data = G_TYPE_INSTANCE_GET_PRIVATE(adim, ADG_TYPE_ADIM, AdgADimPrivate);
    move_to.header.type = CAIRO_PATH_MOVE_TO;
    move_to.header.length = 2;
    line_to.header.type = CAIRO_PATH_LINE_TO;
    line_to.header.length = 2;
    arc_to.header.type = CAIRO_PATH_ARC_TO;
    arc_to.header.length = 3;

    data->org1 = NULL;
    data->org2 = NULL;
    data->has_extension1 = TRUE;
    data->has_extension2 = TRUE;

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cpml.path.data = data->cpml.data;
    data->cpml.path.num_data = G_N_ELEMENTS(data->cpml.data);
    data->cpml.path.data[0] = move_to;
    data->cpml.path.data[2] = arc_to;
    data->cpml.path.data[5] = move_to;
    data->cpml.path.data[7] = line_to;
    data->cpml.path.data[9] = move_to;
    data->cpml.path.data[11] = line_to;

    data->trail = NULL;
    data->marker1 = NULL;
    data->marker2 = NULL;

    data->geometry_arranged = FALSE;

    adim->data = data;
}

static void
dispose(GObject *object)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) object;
    data = adim->data;

    dispose_markers((AdgADim *) object);

    if (data->org1 != NULL) {
        adg_point_destroy(data->org1);
        data->org1 = NULL;
    }
    if (data->org2 != NULL) {
        adg_point_destroy(data->org2);
        data->org2 = NULL;
    }

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgADimPrivate *data = ((AdgADim *) object)->data;

    switch (prop_id) {
    case PROP_ORG1:
        g_value_set_boxed(value, &data->org1);
        break;
    case PROP_ORG2:
        g_value_set_boxed(value, &data->org2);
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
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) object;
    data = adim->data;

    switch (prop_id) {
    case PROP_ORG1:
        if (data->org1 != NULL)
            adg_point_destroy(data->org1);
        data->org1 = g_value_dup_boxed(value);
        break;
    case PROP_ORG2:
        if (data->org2 != NULL)
            adg_point_destroy(data->org2);
        data->org2 = g_value_dup_boxed(value);
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
 * define the reference points with adg_dim_set_ref(), the origins of
 * the lines ending with the reference points with adg_adim_set_org()
 * and the reference for positioning the quote with adg_dim_set_pos().
 *
 * Returns: the newly created angular dimension entity
 **/
AdgADim *
adg_adim_new(void)
{
    return g_object_new(ADG_TYPE_ADIM, NULL);
}

/**
 * adg_adim_new_full:
 * @ref1: first reference point
 * @ref2: second reference point
 * @org1: first origin point
 * @org2: second origin point
 *
 * Creates a new angular dimension, specifing all the needed
 * properties in one shot.
 *
 * Returns: the newly created angular dimension entity
 **/
AdgADim *
adg_adim_new_full(const AdgPair *ref1, const AdgPair *ref2,
                  const AdgPair *org1, const AdgPair *org2,
                  const AdgPair *pos)
{
    AdgADim *adim;
    AdgDim *dim;

    adim = g_object_new(ADG_TYPE_ADIM, NULL);
    dim = (AdgDim *) adim;

    adg_dim_set_ref(dim, ref1, ref2);
    adg_adim_set_org(adim, org1, org2);
    adg_dim_set_pos(dim, pos);

    return adim;
}

/**
 * adg_adim_new_full_explicit:
 * @ref1_x: the x coordinate of the first reference point
 * @ref1_y: the y coordinate of the first reference point
 * @ref2_x: the x coordinate of the second reference point
 * @ref2_y: the y coordinate of the second reference point
 * @direction: angle where to extend the dimension
 * @pos_x: the x coordinate of the position reference
 * @pos_y: the y coordinate of the position reference
 *
 * Wrappes adg_adim_new_full() with explicit values.
 *
 * Returns: the newly created linear dimension entity
 **/
AdgADim *
adg_adim_new_full_explicit(gdouble ref1_x, gdouble ref1_y,
                           gdouble ref2_x, gdouble ref2_y,
                           gdouble org1_x, gdouble org1_y,
                           gdouble org2_x, gdouble org2_y,
                           gdouble pos_x,  gdouble pos_y)
{
    AdgPair ref1, ref2, org1, org2, pos;

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
 * @model: the model from which the named pairs are taken
 * @ref1: the end point of the first line
 * @ref2: the end point of the second line
 * @org1: the origin of the first line
 * @org2: the origin of the second line
 * @pos: the position reference
 *
 * Creates a new angular dimension, specifing all the needed properties
 * in one shot and using named pairs from @model.
 *
 * Returns: the newly created angular dimension entity
 **/
AdgADim *
adg_adim_new_full_from_model(AdgModel *model,
                             const gchar *ref1, const gchar *ref2,
                             const gchar *org1, const gchar *org2,
                             const gchar *pos)
{
    AdgADim *adim;
    AdgDim *dim;

    adim = g_object_new(ADG_TYPE_ADIM, NULL);
    dim = (AdgDim *) adim;

    adg_dim_set_ref_from_model(dim, model, ref1, ref2);
    adg_dim_set_pos_from_model(dim, model, pos);
    adg_adim_set_org_from_model(adim, model, org1, org2);

    return adim;
}

/**
 * adg_adim_get_org1:
 * @adim: an #AdgADim
 *
 * Gets the first origin of @adim. The returned pair is owned by
 * @adim and should not be modified or freed.
 *
 * Returns: a pointer to the internal #AdgPair or %NULL on errors
 **/
const AdgPair *
adg_adim_get_org1(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), NULL);

    data = adim->data;

    return adg_point_pair(data->org1);
}

/**
 * adg_adim_get_org2:
 * @adim: an #AdgADim
 *
 * Gets the second origin of @adim. The returned pair is owned by
 * @adim and should not be modified or freed.
 *
 * Returns: a pointer to the internal #AdgPair or %NULL on errors
 **/
const AdgPair *
adg_adim_get_org2(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), NULL);

    data = adim->data;

    return adg_point_pair(data->org2);
}

/**
 * adg_adim_set_org:
 * @adim: an #AdgADim
 * @org1: the first origin
 * @org2: the second origin
 *
 * Sets at once the two origins on @adim. One of @org1 or @org2
 * (but not both) could be %NULL, in which case only the non-null
 * origin is set.
 **/
void
adg_adim_set_org(AdgADim *adim, const AdgPair *org1, const AdgPair *org2)
{
    GObject *object;
    AdgADimPrivate *data;

    g_return_if_fail(ADG_IS_ADIM(adim));
    g_return_if_fail(org1 != NULL || org2 != NULL);

    object = (GObject *) adim;
    data = adim->data;

    g_object_freeze_notify(object);

    if (org1 != NULL) {
        if (data->org1 == NULL)
            data->org1 = adg_point_new();

        adg_point_set(data->org1, org1);

        g_object_notify(object, "org1");
    }

    if (org2 != NULL) {
        if (data->org2 == NULL)
            data->org2 = adg_point_new();

        adg_point_set(data->org2, org2);

        g_object_notify(object, "org2");
    }

    g_object_thaw_notify(object);
}

/**
 * adg_adim_set_org_explicit:
 * @adim: an #AdgADim
 * @org1_x: x coordinate of org1
 * @org1_y: y coordinate of org1
 * @org2_x: x coordinate of org2
 * @org2_y: y coordinate of org2
 *
 * Works in the same way as adg_adim_set_org() but using
 * explicit coordinates instead of #AdgPair args. The
 * notable difference is that, by using gdouble values,
 * you can't set only a single origin point.
 **/
void
adg_adim_set_org_explicit(AdgADim *adim,
                          gdouble org1_x, gdouble org1_y,
                          gdouble org2_x, gdouble org2_y)
{
    AdgPair org1, org2;

    org1.x = org1_x;
    org1.y = org1_y;
    org2.x = org2_x;
    org2.y = org2_y;

    adg_adim_set_org(adim, &org1, &org2);
}

/**
 * adg_adim_set_org_from_model:
 * @adim: an #AdgADim
 * @model: the source #AdgModel
 * @org1: name of the pair in @model to use as org1
 * @org2: name of the pair in @model to use as org2
 *
 * Sets #AdgADim:org1 and #AdgADim:org2 properties by linking
 * them to the @org1 and @org2 named pairs in @model. @org1
 * or @org2 could be %NULL (but not both), in which case
 * only the non-null origin point is changed.
 *
 * Using this function twice you can also link the origin
 * points to named pairs taken from different models:
 *
 * |[
 * adg_adim_set_org_from_model(adim, model1, org1, NULL);
 * adg_adim_set_org_from_model(adim, model2, NULL, org2);
 * ]|
 **/
void
adg_adim_set_org_from_model(AdgADim *adim, AdgModel *model,
                            const gchar *org1, const gchar *org2)
{
    GObject *object;
    AdgADimPrivate *data;

    g_return_if_fail(ADG_IS_ADIM(adim));
    g_return_if_fail(ADG_IS_MODEL(model));
    g_return_if_fail(org1 != NULL || org2 != NULL);

    object = (GObject *) adim;
    data = adim->data;

    g_object_freeze_notify(object);

    if (org1 != NULL) {
        if (data->org1 == NULL)
            data->org1 = adg_point_new();

        adg_point_set_from_model(data->org1, model, org1);

        g_object_notify(object, "org1");
    }

    if (org2 != NULL) {
        if (data->org2 == NULL)
            data->org2 = adg_point_new();

        adg_point_set_from_model(data->org2, model, org2);

        g_object_notify(object, "org2");
    }

    g_object_thaw_notify(object);
}


static void
local_changed(AdgEntity *entity)
{
    unset_trail((AdgADim *) entity);

    if (PARENT_ENTITY_CLASS->local_changed)
        PARENT_ENTITY_CLASS->local_changed(entity);
}

static void
invalidate(AdgEntity *entity)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) entity;
    data = adim->data;

    dispose_markers(adim);
    data->geometry_arranged = FALSE;
    unset_trail(adim);

    adg_point_invalidate(data->org1);
    adg_point_invalidate(data->org2);

    if (PARENT_ENTITY_CLASS->invalidate)
        PARENT_ENTITY_CLASS->invalidate(entity);
}

static void
arrange(AdgEntity *entity)
{
    AdgADim *adim;
    AdgDim *dim;
    AdgADimPrivate *data;
    AdgContainer *quote;
    const AdgMatrix *local;
    AdgPair ref1, ref2, base1, base12, base2;
    AdgPair pair;

    if (PARENT_ENTITY_CLASS->arrange)
        PARENT_ENTITY_CLASS->arrange(entity);

    adim = (AdgADim *) entity;
    dim = (AdgDim *) adim;
    data = adim->data;
    quote = adg_dim_get_quote(dim);

    update_geometry(adim);
    update_entities(adim);

    if (data->cpml.path.status == CAIRO_STATUS_SUCCESS) {
        AdgEntity *quote_entity = (AdgEntity *) quote;
        adg_entity_set_global_map(quote_entity, &data->quote.global_map);
        adg_entity_set_local_map(quote_entity, &data->quote.local_map);
        return;
    }

    local = adg_entity_get_local_matrix(entity);

    /* Apply the local matrix to the relevant points */
    cpml_pair_transform(cpml_pair_copy(&ref1, adg_dim_get_ref1(dim)), local);
    cpml_pair_transform(cpml_pair_copy(&ref2, adg_dim_get_ref2(dim)), local);
    cpml_pair_transform(cpml_pair_copy(&base1, &data->point.base1), local);
    cpml_pair_transform(cpml_pair_copy(&base12, &data->point.base12), local);
    cpml_pair_transform(cpml_pair_copy(&base2, &data->point.base2), local);

    /* Combine points and global shifts to build the path */
    cpml_pair_add(cpml_pair_copy(&pair, &ref1), &data->shift.from1);
    cpml_pair_to_cairo(&pair, &data->cpml.data[6]);

    cpml_pair_add(cpml_pair_copy(&pair, &base1), &data->shift.base1);
    cpml_pair_to_cairo(&pair, &data->cpml.data[1]);

    cpml_pair_add(&pair, &data->shift.to1);
    cpml_pair_to_cairo(&pair, &data->cpml.data[8]);

    cpml_pair_add(cpml_pair_copy(&pair, &base12), &data->shift.base12);
    cpml_pair_to_cairo(&pair, &data->cpml.data[3]);

    cpml_pair_add(cpml_pair_copy(&pair, &ref2), &data->shift.from2);
    cpml_pair_to_cairo(&pair, &data->cpml.data[10]);

    cpml_pair_add(cpml_pair_copy(&pair, &base2), &data->shift.base2);
    cpml_pair_to_cairo(&pair, &data->cpml.data[4]);

    cpml_pair_add(&pair, &data->shift.to2);
    cpml_pair_to_cairo(&pair, &data->cpml.data[12]);

    data->cpml.path.status = CAIRO_STATUS_SUCCESS;

    if (quote != NULL) {
        /* Update global and local map of the quote container */
        AdgEntity *quote_entity;
        gdouble angle;
        AdgMatrix map;

        quote_entity = (AdgEntity *) quote;
        angle = adg_dim_quote_angle(dim, (data->angle1 + data->angle2) / 2 + G_PI_2);
        cpml_pair_from_cairo(&pair, &data->cpml.data[3]);

        cairo_matrix_init_translate(&map, pair.x, pair.y);
        cairo_matrix_rotate(&map, angle);
        adg_entity_set_global_map(quote_entity, &map);

        adg_matrix_copy(&data->quote.global_map,
                        adg_entity_get_global_map(quote_entity));
        adg_matrix_copy(&data->quote.local_map,
                        adg_entity_get_local_map(quote_entity));
    }

    /* Signal to the markers (if any) that the path has changed */
    if (data->marker1 != NULL) {
        adg_marker_set_segment(data->marker1, data->trail, 1);
        adg_entity_local_changed((AdgEntity *) data->marker1);
    }

    if (data->marker2 != NULL) {
        adg_marker_set_segment(data->marker2, data->trail, 1);
        adg_entity_local_changed((AdgEntity *) data->marker2);
    }

    /* TODO: compute the extents */
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgADim *adim;
    AdgDim *dim;
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;
    AdgDress dress;
    const cairo_path_t *cairo_path;

    adim = (AdgADim *) entity;
    dim = (AdgDim *) entity;
    data = adim->data;
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
    AdgADim *adim;
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;
    gdouble angle;
    const gchar *format;

    adim = (AdgADim *) dim;
    data = adim->data;
    dim_style = GET_DIM_STYLE(dim);
    format = adg_dim_style_get_number_format(dim_style);

    update_geometry(adim);
    angle = (data->angle2 - data->angle1) * 180 / M_PI;

    return g_strdup_printf(format, angle);
}

/* With "geometry" is considered any data (point, vector or angle)
 * that can be cached: this is strictly related on how the arrange()
 * method works */
static void
update_geometry(AdgADim *adim)
{
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;
    gdouble from_offset, to_offset;
    gdouble spacing, level;
    CpmlVector vector[3];
    CpmlPair center;
    gdouble distance;

    data = adim->data;

    if (data->geometry_arranged)
        return;

    if (!get_info(adim, vector, &center, &distance)) {
        /* Parallel lines: hang with an error message */
        g_warning("%s: trying to set an angular dimension on parallel lines",
                  G_STRLOC);
        return;
    }

    dim_style = GET_DIM_STYLE(adim);
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
    cpml_pair_add(cpml_pair_copy(&data->point.base1, &vector[0]), &center);

    /* point.base2 */
    cpml_vector_set_length(&vector[2], distance);
    cpml_pair_add(cpml_pair_copy(&data->point.base2, &vector[2]), &center);

    /* point.base12 */
    cpml_vector_set_length(&vector[1], distance);
    cpml_pair_add(cpml_pair_copy(&data->point.base12, &vector[1]), &center);

    data->geometry_arranged = TRUE;
}

static void
update_entities(AdgADim *adim)
{
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;

    data = adim->data;
    dim_style = GET_DIM_STYLE(adim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(trail_callback, adim);

    if (data->marker1 == NULL)
        data->marker1 = adg_dim_style_marker1_new(dim_style);

    if (data->marker2 == NULL)
        data->marker2 = adg_dim_style_marker2_new(dim_style);
}

static void
unset_trail(AdgADim *adim)
{
    AdgADimPrivate *data = adim->data;

    if (data->trail != NULL)
        adg_model_clear((AdgModel *) data->trail);

    data->cpml.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}

static void
dispose_markers(AdgADim *adim)
{
    AdgADimPrivate *data = adim->data;

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

static gboolean
get_info(AdgADim *adim, CpmlVector vector[],
         AdgPair *center, gdouble *distance)
{
    AdgDim *dim;
    AdgADimPrivate *data;
    const AdgPair *ref1, *ref2;
    gdouble factor;

    dim = (AdgDim *) adim;
    data = adim->data;
    ref1 = adg_dim_get_ref1(dim);
    ref2 = adg_dim_get_ref2(dim);

    cpml_pair_sub(cpml_pair_copy(&vector[0], ref1), adg_point_pair(data->org1));
    cpml_pair_sub(cpml_pair_copy(&vector[2], ref2), adg_point_pair(data->org2));

    factor = vector[0].x * vector[2].y - vector[0].y * vector[2].x;
    if (factor == 0)
        return FALSE;

    factor = ((ref1->y - ref2->y) * vector[2].x -
              (ref1->x - ref2->x) * vector[2].y) / factor;

    center->x = ref1->x + vector[0].x * factor;
    center->y = ref1->y + vector[0].y * factor;
    *distance = cpml_pair_distance(center, adg_dim_get_pos(dim));
    data->angle1 = cpml_vector_angle(&vector[0]);
    data->angle2 = cpml_vector_angle(&vector[2]);
    while (data->angle2 < data->angle1)
        data->angle2 += M_PI * 2;

    cpml_vector_from_angle(&vector[1], (data->angle1 + data->angle2) / 2);

    return TRUE;
}

static CpmlPath *
trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) user_data;
    data = adim->data;

    return &data->cpml.path;
}
