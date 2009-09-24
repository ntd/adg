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
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_adim_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_adim_parent_class)


enum {
    PROP_0,
    PROP_ORG1,
    PROP_ORG2
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
static void             update_shift            (AdgADim        *adim);
static void             update_entities         (AdgADim        *adim);
static gboolean         set_org1                (AdgADim        *adim,
                                                 const AdgPair  *org1);
static gboolean         set_org2                (AdgADim        *adim,
                                                 const AdgPair  *org2);
static void             dispose_markers         (AdgADim        *adim);
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

    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    dim_class->default_value = default_value;

    param = g_param_spec_boxed("org1",
                               P_("First Origin"),
                               P_("Where the first line comes from: this point is used toghether with \"ref1\" to align the first extension line"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ORG1, param);

    param = g_param_spec_boxed("org2",
                               P_("Second Origin"),
                               P_("Where the second line comes from: this point is used toghether with \"ref2\" to align the second extension line"),
                               ADG_TYPE_PAIR,
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

    data->org1.x = data->org1.y = 0;
    data->org2.x = data->org2.y = 0;
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

    data->angle1 = -1;
    data->angle2 = -1;
    data->trail = NULL;
    data->marker1 = NULL;
    data->marker2 = NULL;

    data->shift.is_arranged = FALSE;

    adim->data = data;
}

static void
dispose(GObject *object)
{
    dispose_markers((AdgADim *) object);

    if (PARENT_OBJECT_CLASS->dispose != NULL)
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgADim *adim = (AdgADim *) object;

    switch (prop_id) {
    case PROP_ORG1:
        set_org1(adim, g_value_get_boxed(value));
        break;
    case PROP_ORG2:
        set_org2(adim, g_value_get_boxed(value));
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
 * the lines to quote with adg_adim_set_org() and the position reference
 * using adg_dim_set_pos().
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
 * @pos: the position reference
 *
 * Creates a new angular dimension, specifing all the needed
 * properties in one shot.
 *
 *
 * Returns: the newly created angular dimension entity
 **/
AdgADim *
adg_adim_new_full(const AdgPair *ref1, const AdgPair *ref2,
                  const AdgPair *org1, const AdgPair *org2,
                  const AdgPair *pos)
{
    AdgADim *adim = g_object_new(ADG_TYPE_ADIM, "ref1", ref1, "ref2", ref2,
                                 "org1", org1, "org2", org2, NULL);
    adg_adim_set_pos(adim, pos);
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

    return &data->org1;
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

    return &data->org2;
}

/**
 * adg_adim_set_org:
 * @adim: an #AdgADim
 * @org1: the first origin
 * @org2: the second origin
 *
 * Sets at once the two origins on @adim.
 **/
void
adg_adim_set_org(AdgADim *adim, const AdgPair *org1, const AdgPair *org2)
{
    GObject *object;

    g_return_if_fail(ADG_IS_ADIM(adim));

    object = (GObject *) adim;

    g_object_freeze_notify(object);

    if (set_org1(adim, org1))
        g_object_notify(object, "org1");

    if (set_org2(adim, org1))
        g_object_notify(object, "org2");

    g_object_thaw_notify(object);
}

/**
 * adg_adim_set_pos:
 * @adim: an #AdgADim entity
 * @pos: an #AdgPair structure
 *
 * Sets the position references (pos1 and pos2 properties) of @adim using a
 * single @pos point. Before this call, @adim MUST HAVE defined the reference
 * points and the direction. If these conditions are not met, an error message
 * is logged and the position references will not be set.
 **/
void
adg_adim_set_pos(AdgADim *adim, const AdgPair *pos)
{
    g_return_if_fail(ADG_IS_ADIM(adim));

    ADG_MESSAGE("TODO");
}

/**
 * adg_adim_set_pos_explicit:
 * @adim: an #AdgADim entity
 * @x: the new x coordinate position reference
 * @y: the new y coordinate position reference
 *
 * Wrappers adg_adim_set_pos() with explicit coordinates.
 **/
void
adg_adim_set_pos_explicit(AdgADim *adim, gdouble x, gdouble y)
{
    AdgPair pos;

    pos.x = x;
    pos.y = y;

    adg_adim_set_pos(adim, &pos);
}


static void
invalidate(AdgEntity *entity)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) entity;
    data = adim->data;

    dispose_markers(adim);
    data->shift.is_arranged = FALSE;

    if (PARENT_ENTITY_CLASS->invalidate != NULL)
        PARENT_ENTITY_CLASS->invalidate(entity);
}

static void
arrange(AdgEntity *entity)
{
    AdgADim *adim;

    adim = (AdgADim *) entity;

    update_shift(adim);
    update_entities(adim);

    ADG_MESSAGE("TODO");
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    ADG_MESSAGE("TODO");
}

static gchar *
default_value(AdgDim *dim)
{
    ADG_MESSAGE("TODO");

    return g_strdup("TODO");
}


static void
update_shift(AdgADim *adim)
{
    AdgADimPrivate *data;
    AdgDim *dim;
    AdgDimStyle *dim_style;
    CpmlVector vector;
    gdouble from_offset;
    gdouble to_offset;
    gdouble baseline_spacing;
    gdouble level;

    data = adim->data;

    if (data->shift.is_arranged)
        return;

    dim = (AdgDim *) adim;
    dim_style = adg_dim_get_dim_style((AdgDim *) adim);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    level = adg_dim_get_level((AdgDim *) adim);

    data->shift.from1.x = data->shift.from1.y = 0;
    data->shift.from2.x = data->shift.from2.y = 0;
    data->shift.marker1.x = data->shift.marker1.y = 0;
    data->shift.marker2.x = data->shift.marker2.y = 0;
    data->shift.to1.x = data->shift.to1.y = 0;
    data->shift.to2.x = data->shift.to2.y = 0;

    cpml_pair_copy(&vector, adg_dim_get_ref1(dim));
    cpml_pair_sub(&vector, &data->org1);

    cpml_vector_set_length(&vector, from_offset);
    cpml_pair_copy(&data->shift.from1, &vector);

    cpml_vector_set_length(&vector, level * baseline_spacing);
    cpml_pair_copy(&data->shift.marker1, &vector);

    cpml_vector_set_length(&vector, to_offset);
    cpml_pair_copy(&data->shift.to1, &vector);

    cpml_pair_copy(&vector, adg_dim_get_ref2(dim));
    cpml_pair_sub(&vector, &data->org2);

    cpml_vector_set_length(&vector, from_offset);
    cpml_pair_copy(&data->shift.from2, &vector);

    cpml_vector_set_length(&vector, level * baseline_spacing);
    cpml_pair_copy(&data->shift.marker2, &vector);

    cpml_vector_set_length(&vector, to_offset);
    cpml_pair_copy(&data->shift.to2, &vector);

    data->shift.is_arranged = TRUE;
}

static void
update_entities(AdgADim *adim)
{
    AdgADimPrivate *data;
    AdgDimStyle *dim_style;

    data = adim->data;
    dim_style = adg_dim_get_dim_style((AdgDim *) adim);

    if (data->trail == NULL)
        data->trail = adg_trail_new(trail_callback, adim);

    if (data->marker1 == NULL)
        data->marker1 = adg_dim_style_marker1_new(dim_style);

    if (data->marker2 == NULL)
        data->marker2 = adg_dim_style_marker2_new(dim_style);
}

static gboolean
set_org1(AdgADim *adim, const AdgPair *org1)
{
    AdgADimPrivate *data = adim->data;

    if (adg_pair_equal(&data->org1, org1))
        return FALSE;

    data->org1 = *org1;

    return TRUE;
}

static gboolean
set_org2(AdgADim *adim, const AdgPair *org2)
{
    AdgADimPrivate *data = adim->data;

    if (adg_pair_equal(&data->org2, org2))
        return FALSE;

    data->org2 = *org2;

    return TRUE;
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

static CpmlPath *
trail_callback(AdgTrail *trail, gpointer user_data)
{
    AdgADim *adim;
    AdgADimPrivate *data;

    adim = (AdgADim *) user_data;
    data = adim->data;

    adg_trail_clear_cairo_path(trail);

    return &data->cpml.path;
}
