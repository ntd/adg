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
 */

/**
 * AdgLDim:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-ldim.h"
#include "adg-ldim-private.h"
#include "adg-container.h"
#include "adg-util.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_DIRECTION
};


static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     model_matrix_changed    (AdgEntity      *entity,
                                         AdgMatrix      *parent_matrix);
static void     paper_matrix_changed    (AdgEntity      *entity,
                                         AdgMatrix      *parent_matrix);
static void     invalidate              (AdgEntity      *entity);
static void     render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gchar *  default_quote           (AdgDim         *dim);
static void     update                  (AdgLDim        *ldim);
static void     clear                   (AdgLDim        *ldim);


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

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->model_matrix_changed = model_matrix_changed;
    entity_class->paper_matrix_changed = paper_matrix_changed;
    entity_class->invalidate = invalidate;
    entity_class->render = render;

    dim_class->default_quote = default_quote;

    param = g_param_spec_double("direction",
                                P_("Direction"),
                                P_("The inclination angle of the extension lines"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, ADG_DIR_RIGHT,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_DIRECTION, param);
}

static void
adg_ldim_init(AdgLDim *ldim)
{
    AdgLDimPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(ldim, ADG_TYPE_LDIM,
                                                       AdgLDimPrivate);

    data->direction = ADG_DIR_RIGHT;

    data->path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->path.data = data->path_data;
    data->path.num_data = 12;

    data->path_data[0].header.type = CAIRO_PATH_MOVE_TO;
    data->path_data[0].header.length = 2;
    data->path_data[2].header.type = CAIRO_PATH_LINE_TO;
    data->path_data[2].header.length = 2;
    data->path_data[4].header.type = CAIRO_PATH_MOVE_TO;
    data->path_data[4].header.length = 2;
    data->path_data[6].header.type = CAIRO_PATH_LINE_TO;
    data->path_data[6].header.length = 2;
    data->path_data[8].header.type = CAIRO_PATH_MOVE_TO;
    data->path_data[8].header.length = 2;
    data->path_data[10].header.type = CAIRO_PATH_LINE_TO;
    data->path_data[10].header.length = 2;

    data->director.data = data->director_data;
    data->director.num_data = 4;
    data->director.path = NULL;

    data->director_data[0].header.type = CAIRO_PATH_MOVE_TO;
    data->director_data[0].header.length = 2;
    data->director_data[2].header.type = CAIRO_PATH_LINE_TO;
    data->director_data[2].header.length = 2;

    ldim->data = data;
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_ldim_new:
 *
 * Creates a new - unreferenced - linear dimension. You must, at least, define
 * the reference points with adg_dim_set_ref(), the dimension direction with
 * adg_ldim_set_direction() and the position reference using adg_dim_set_pos()
 * or, better, adg_ldim_set_pos().
 *
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new(void)
{
    return (AdgEntity *) g_object_new(ADG_TYPE_LDIM, NULL);
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
 * Return value: the new entity
 */
AdgEntity *
adg_ldim_new_full(const AdgPair *ref1, const AdgPair *ref2,
                  gdouble direction, const AdgPair *pos)
{
    AdgEntity *entity = (AdgEntity *) g_object_new(ADG_TYPE_LDIM,
                                                   "ref1", ref1,
                                                   "ref2", ref2,
                                                   "direction", direction,
                                                   NULL);
    adg_ldim_set_pos((AdgLDim *) entity, pos);
    return entity;
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
 * Wrappes adg_ldim_new_full() with explicit quotes.
 *
 * Return value: the new entity
 */
AdgEntity *
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
 */
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

    adg_dim_set_pos((AdgDim *) ldim, &pos1, &pos2);
}

/**
 * adg_ldim_set_pos_explicit:
 * @ldim: an #AdgLDim entity
 * @pos_x: the new x coordinate position reference
 * @pos_y: the new y coordinate position reference
 *
 * Wrappers adg_ldim_set_pos() with explicit coordinates.
 */
void
adg_ldim_set_pos_explicit(AdgLDim *ldim, gdouble pos_x, gdouble pos_y)
{
    AdgPair pos;

    pos.x = pos_x;
    pos.y = pos_y;

    adg_ldim_set_pos(ldim, &pos);
}

/**
 * adg_ldim_get_direction:
 * @ldim: an #AdgLDim entity
 *
 * Gets the direction where @ldim will extend.
 *
 * Return value: the direction angle in radians
 */
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
 */
void
adg_ldim_set_direction(AdgLDim *ldim, gdouble direction)
{
    AdgLDimPrivate *data;

    g_return_if_fail(ADG_IS_LDIM(ldim));

    data = ldim->data;
    data->direction = direction;

    g_object_notify((GObject *) ldim, "direction");
}


static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_ldim_parent_class;

    clear((AdgLDim *) entity);

    if (entity_class->model_matrix_changed != NULL)
        entity_class->model_matrix_changed(entity, parent_matrix);
}

static void
paper_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_ldim_parent_class;

    clear((AdgLDim *) entity);

    if (entity_class->paper_matrix_changed != NULL)
        entity_class->paper_matrix_changed(entity, parent_matrix);
}

static void
invalidate(AdgEntity *entity)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_ldim_parent_class;

    clear((AdgLDim *) entity);

    if (entity_class->invalidate != NULL)
        entity_class->invalidate(entity);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgLDim *ldim;
    AdgLDimPrivate *data;
    AdgEntityClass *entity_class;
    AdgStyle *dim_style;
    AdgStyle *line_style;
    AdgStyle *arrow_style;

    ldim = (AdgLDim *) entity;
    data = ldim->data;
    entity_class = (AdgEntityClass *) adg_ldim_parent_class;
    dim_style = adg_entity_get_style(entity, ADG_SLOT_DIM_STYLE);
    line_style = adg_dim_style_get_line_style((AdgDimStyle *) dim_style);
    arrow_style = adg_dim_style_get_arrow_style((AdgDimStyle *) dim_style);

    update(ldim);

    cairo_save(cr);
    adg_entity_apply(entity, ADG_SLOT_DIM_STYLE, cr);

    adg_arrow_style_render((AdgArrowStyle *) arrow_style, cr, &data->director);
    cpml_segment_reverse(&data->director);
    adg_arrow_style_render((AdgArrowStyle *) arrow_style, cr, &data->director);
    cpml_segment_reverse(&data->director);

    adg_style_apply(line_style, cr);

    cairo_append_path(cr, &data->path);
    cairo_stroke(cr);

    adg_dim_render_quote((AdgDim *) ldim, cr);

    cairo_restore(cr);

    if (entity_class->render != NULL)
        entity_class->render(entity, cr);
}

static gchar *
default_quote(AdgDim *dim)
{
    const AdgPair *pos1, *pos2;
    AdgStyle *dim_style;
    gdouble distance;
    const gchar *format;

    pos1 = adg_dim_get_pos1(dim);
    pos2 = adg_dim_get_pos2(dim);
    distance = cpml_pair_distance(pos1, pos2);
    dim_style = adg_entity_get_style((AdgEntity *) dim, ADG_SLOT_DIM_STYLE);
    format = adg_dim_style_get_number_format((AdgDimStyle *) dim_style);

    return g_strdup_printf(format, distance);
}

static void
update(AdgLDim *ldim)
{
    AdgLDimPrivate *data;
    AdgStyle *dim_style;
    AdgStyle *arrow_style;
    const AdgPair *ref1, *ref2, *pos1, *pos2;
    gdouble level;
    gdouble baseline_spacing, from_offset;
    cairo_path_data_t *baseline1, *baseline2;
    cairo_path_data_t *from1, *to1, *from2, *to2;
    cairo_path_data_t *arrow1, *arrow2;
    AdgMatrix paper2model;
    CpmlPair vector;
    AdgPair offset;
    gdouble angle;

    data = ldim->data;
    if (data->path.status == CAIRO_STATUS_SUCCESS)
        return;

    dim_style = adg_entity_get_style((AdgEntity *) ldim, ADG_SLOT_DIM_STYLE);
    arrow_style = adg_dim_style_get_arrow_style((AdgDimStyle *) dim_style);

    ref1 = adg_dim_get_ref1((AdgDim *) ldim);
    ref2 = adg_dim_get_ref2((AdgDim *) ldim);
    pos1 = adg_dim_get_pos1((AdgDim *) ldim);
    pos2 = adg_dim_get_pos2((AdgDim *) ldim);
    level = adg_dim_get_level((AdgDim *) ldim);

    baseline_spacing = adg_dim_style_get_baseline_spacing((AdgDimStyle *) dim_style);
    from_offset = adg_dim_style_get_from_offset((AdgDimStyle *) dim_style);

    baseline1 = data->path_data + 1;
    baseline2 = data->path_data + 3;
    from1 = data->path_data + 5;
    to1 = data->path_data + 7;
    from2 = data->path_data + 9;
    to2 = data->path_data + 11;
    arrow1 = data->director_data + 1;
    arrow2 = data->director_data + 3;

    /* Get the inverted transformation matrix */
    if (!adg_entity_build_paper2model((AdgEntity *) ldim, &paper2model))
        return;

    /* Set vector to the director of the extension lines */
    cpml_vector_from_angle(&vector, data->direction, 1);

    /* Calculate from1 and from2 */
    offset.x = vector.x * from_offset;
    offset.y = vector.y * from_offset;
    cairo_matrix_transform_distance(&paper2model, &offset.x, &offset.y);

    from1->point.x = ref1->x + offset.x;
    from1->point.y = ref1->y + offset.y;
    from2->point.x = ref2->x + offset.x;
    from2->point.y = ref2->y + offset.y;

    /* Calculate arrow1 and arrow2 */
    offset.x = vector.x * baseline_spacing * level;
    offset.y = vector.y * baseline_spacing * level;
    cairo_matrix_transform_distance(&paper2model, &offset.x, &offset.y);

    arrow1->point.x = pos1->x + offset.x;
    arrow1->point.y = pos1->y + offset.y;
    arrow2->point.x = pos2->x + offset.x;
    arrow2->point.y = pos2->y + offset.y;

    /* Calculate to1 and to2 */
    offset.x = vector.x * adg_dim_style_get_to_offset((AdgDimStyle *) dim_style);
    offset.y = vector.y * adg_dim_style_get_to_offset((AdgDimStyle *) dim_style);
    cairo_matrix_transform_distance(&paper2model, &offset.x, &offset.y);

    to1->point.x = arrow1->point.x + offset.x;
    to1->point.y = arrow1->point.y + offset.y;
    to2->point.x = arrow2->point.x + offset.x;
    to2->point.y = arrow2->point.y + offset.y;

    /* Set vector to the director of the baseline */
    offset.x = arrow2->point.x - arrow1->point.x;
    offset.y = arrow2->point.y - arrow1->point.y;
    cpml_vector_set_length(cpml_pair_copy(&vector, &offset), 1);

    /* Update the AdgDim cache contents */
    adg_dim_set_org_explicit((AdgDim *) ldim,
                             (arrow1->point.x + arrow2->point.x) / 2.,
                             (arrow1->point.y + arrow2->point.y) / 2.);
    angle = cpml_vector_angle(&vector);
    adg_dim_set_angle((AdgDim *) ldim, angle);

    /* Calculate baseline1 and baseline2 */
    offset.y = adg_arrow_style_get_margin((AdgArrowStyle *) arrow_style);
    offset.x = vector.x * offset.y;
    offset.y *= vector.y;
    cairo_matrix_transform_distance(&paper2model, &offset.x, &offset.y);

    baseline1->point.x = arrow1->point.x + offset.x;
    baseline1->point.y = arrow1->point.y + offset.y;
    baseline2->point.x = arrow2->point.x - offset.x;
    baseline2->point.y = arrow2->point.y - offset.y;

    data->path.status = CAIRO_STATUS_SUCCESS;
}

static void
clear(AdgLDim *ldim)
{
    AdgLDimPrivate *data = ldim->data;

    data->path.status = CAIRO_STATUS_INVALID_PATH_DATA;
}
