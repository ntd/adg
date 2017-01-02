/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-projection
 * @short_description: The standard symbol for specifying the projection scheme
 *
 * The #AdgProjection is an entity representing the standard symbol
 * of the projection scheme.
 *
 * Since: 1.0
 **/

/**
 * AdgProjection:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-model.h"
#include "adg-trail.h"
#include "adg-path.h"
#include "adg-dress.h"
#include "adg-param-dress.h"

#include "adg-projection.h"
#include "adg-projection-private.h"


G_DEFINE_TYPE(AdgProjection, adg_projection, ADG_TYPE_ENTITY)

enum {
    PROP_0,
    PROP_SYMBOL_DRESS,
    PROP_AXIS_DRESS,
    PROP_SCHEME
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_arrange_class      (AdgProjectionClass *projection_class,
                                                 AdgProjectionScheme scheme);


static void
adg_projection_class_init(AdgProjectionClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;
    AdgProjectionClassPrivate *data_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgProjectionPrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    param = adg_param_spec_dress("symbol-dress",
                                 P_("Symbol Dress"),
                                 P_("The line dress to use for rendering the views of the projection"),
                                 ADG_DRESS_LINE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SYMBOL_DRESS, param);

    param = adg_param_spec_dress("axis-dress",
                                 P_("Axis Dress"),
                                 P_("The line dress to use for rendering the axis of the projection scheme"),
                                 ADG_DRESS_LINE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_AXIS_DRESS, param);

    param = g_param_spec_enum("scheme",
                              P_("Projection Scheme"),
                              P_("The projection scheme to be represented"),
                              ADG_TYPE_PROJECTION_SCHEME,
                              ADG_PROJECTION_SCHEME_UNDEFINED,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SCHEME, param);

    /* Initialize the private class data: the allocated struct is
     * never freed as this type is registered statically, hence
     * never destroyed. A better approach would be to use the old
     * type initialization (no G_TYPE_DEFINE and friends) that
     * allows to specify a custom class finalization method */
    data_class = g_new(AdgProjectionClassPrivate, 1);

    data_class->scheme = ADG_PROJECTION_SCHEME_UNDEFINED;
    data_class->symbol = NULL;
    data_class->axis = NULL;
    data_class->extents.is_defined = FALSE;

    klass->data_class = data_class;
}

static void
adg_projection_init(AdgProjection *projection)
{
    AdgProjectionPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(projection, ADG_TYPE_PROJECTION,
                                                       AdgProjectionPrivate);

    data->symbol_dress = ADG_DRESS_LINE;
    data->axis_dress = ADG_DRESS_LINE;
    data->scheme = ADG_PROJECTION_SCHEME_UNDEFINED;

    projection->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgProjectionPrivate *data = ((AdgProjection *) object)->data;

    switch (prop_id) {
    case PROP_SYMBOL_DRESS:
        g_value_set_enum(value, data->symbol_dress);
        break;
    case PROP_AXIS_DRESS:
        g_value_set_enum(value, data->axis_dress);
        break;
    case PROP_SCHEME:
        g_value_set_enum(value, data->scheme);
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
    AdgProjection *projection;
    AdgProjectionPrivate *data;

    projection = (AdgProjection *) object;
    data = projection->data;

    switch (prop_id) {
    case PROP_SYMBOL_DRESS:
        data->symbol_dress = g_value_get_enum(value);
        break;
    case PROP_AXIS_DRESS:
        data->axis_dress = g_value_get_enum(value);
        break;
    case PROP_SCHEME:
        data->scheme = g_value_get_enum(value);
        adg_entity_invalidate((AdgEntity *) object);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_projection_new:
 * @scheme: the scheme represented by this projection
 *
 * Creates a new projection entity representing the selected @scheme.
 * If @scheme is invalid, a projection symbol without a scheme is
 * returned, that is #AdgProjection:scheme is set to
 * #ADG_PROJECTION_SCHEME_UNDEFINED, and a warning is raised.
 *
 * Returns: (transfer full): the newly created projection entity.
 *
 * Since: 1.0
 **/
AdgProjection *
adg_projection_new(AdgProjectionScheme scheme)
{
    return g_object_new(ADG_TYPE_PROJECTION, "scheme", scheme, NULL);
}

/**
 * adg_projection_set_symbol_dress:
 * @projection: an #AdgProjection
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering the symbol of @projection. The
 * new dress must be a line dress: the check is done by calling
 * adg_dress_are_related() with @dress and the old dress as
 * arguments. Check out its documentation for further details.
 *
 * The default dress is a transparent line dress: the rendering
 * callback will stroke the symbol using the default color with
 * a predefined thickness.
 *
 * Since: 1.0
 **/
void
adg_projection_set_symbol_dress(AdgProjection *projection, AdgDress dress)
{
    g_return_if_fail(ADG_IS_PROJECTION(projection));
    g_object_set(projection, "symbol-dress", dress, NULL);
}

/**
 * adg_projection_get_symbol_dress:
 * @projection: an #AdgProjection
 *
 * Gets the line dress to be used in stroking the symbol of @projection.
 *
 * Returns: (transfer none): the requested line dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_projection_get_symbol_dress(AdgProjection *projection)
{
    AdgProjectionPrivate *data;

    g_return_val_if_fail(ADG_IS_PROJECTION(projection), ADG_DRESS_UNDEFINED);

    data = projection->data;

    return data->symbol_dress;
}

/**
 * adg_projection_set_axis_dress:
 * @projection: an #AdgProjection
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering the axis of @projection.
 * The new dress must be a line dress: the check is done by
 * calling adg_dress_are_related() with @dress and the old
 * dress as arguments. Check out its documentation for
 * further details.
 *
 * The default dress is a transparent line dress: the rendering
 * callback will stroke the axis using the default line style.
 *
 * Since: 1.0
 **/
void
adg_projection_set_axis_dress(AdgProjection *projection, AdgDress dress)
{
    g_return_if_fail(ADG_IS_PROJECTION(projection));
    g_object_set(projection, "axis-dress", dress, NULL);
}

/**
 * adg_projection_get_axis_dress:
 * @projection: an #AdgProjection
 *
 * Gets the line dress to be used in stroking the axis of @projection.
 *
 * Returns: (transfer none): the requested line dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_projection_get_axis_dress(AdgProjection *projection)
{
    AdgProjectionPrivate *data;

    g_return_val_if_fail(ADG_IS_PROJECTION(projection), ADG_DRESS_UNDEFINED);

    data = projection->data;

    return data->axis_dress;
}

/**
 * adg_projection_set_scheme:
 * @projection: an #AdgProjection
 * @scheme: the new projection scheme
 *
 * Sets a new scheme on @projection. If @scheme is different
 * from the old one, @projection is invalidated.
 *
 * Since: 1.0
 **/
void
adg_projection_set_scheme(AdgProjection *projection,
                          AdgProjectionScheme scheme)
{
    g_return_if_fail(ADG_IS_PROJECTION(projection));
    g_object_set(projection, "scheme", scheme, NULL);
}

/**
 * adg_projection_get_scheme:
 * @projection: an #AdgProjection
 *
 * Gets the scheme represented by @projection.
 *
 * Returns: (transfer none): the scheme of @projection
 *
 * Since: 1.0
 **/
AdgProjectionScheme
adg_projection_get_scheme(AdgProjection *projection)
{
    AdgProjectionPrivate *data;

    g_return_val_if_fail(ADG_IS_PROJECTION(projection),
                         ADG_PROJECTION_SCHEME_UNDEFINED);

    data = projection->data;

    return data->scheme;
}


static void
_adg_arrange(AdgEntity *entity)
{
    AdgProjectionPrivate *data;
    AdgProjectionClass *projection_class;
    AdgProjectionClassPrivate *data_class;
    CpmlExtents extents;

    data = ((AdgProjection *) entity)->data;
    projection_class = ADG_PROJECTION_GET_CLASS(entity);
    data_class = projection_class->data_class;

    _adg_arrange_class(projection_class, data->scheme);
    cpml_extents_copy(&extents, &data_class->extents);

    cpml_extents_transform(&extents, adg_entity_get_local_matrix(entity));
    cpml_extents_transform(&extents, adg_entity_get_global_matrix(entity));
    adg_entity_set_extents(entity, &extents);
}

static void
_adg_arrange_class(AdgProjectionClass *projection_class,
                   AdgProjectionScheme scheme)
{
    AdgProjectionClassPrivate *data_class;
    AdgPath *symbol, *axis;

    data_class = projection_class->data_class;

    if (data_class->scheme == scheme)
        return;

    if (data_class->symbol != NULL)
        g_object_unref(data_class->symbol);

    if (data_class->axis != NULL)
        g_object_unref(data_class->axis);

    data_class->scheme = scheme;

    switch(scheme) {
    case ADG_PROJECTION_SCHEME_UNDEFINED:
        symbol = NULL;
        axis = NULL;
        break;
    case ADG_PROJECTION_SCHEME_FIRST_ANGLE:
        symbol = adg_path_new();
        adg_path_move_to_explicit(symbol, 4, 19);
        adg_path_line_to_explicit(symbol, 24, 24);
        adg_path_line_to_explicit(symbol, 24, 4);
        adg_path_line_to_explicit(symbol, 4, 9);
        adg_path_close(symbol);
        adg_path_move_to_explicit(symbol, 49, 14);
        adg_path_arc_to_explicit(symbol, 29, 14, 49, 14);
        adg_path_move_to_explicit(symbol, 44, 14);
        adg_path_arc_to_explicit(symbol, 34, 14, 44, 14);

        axis = adg_path_new();
        adg_path_move_to_explicit(axis, 0, 14);
        adg_path_line_to_explicit(axis, 53, 14);
        adg_path_move_to_explicit(axis, 39, 0);
        adg_path_line_to_explicit(axis, 39, 28);
        break;
    case ADG_PROJECTION_SCHEME_THIRD_ANGLE:
        symbol = adg_path_new();
        adg_path_move_to_explicit(symbol, 29, 19);
        adg_path_line_to_explicit(symbol, 49, 24);
        adg_path_line_to_explicit(symbol, 49, 4);
        adg_path_line_to_explicit(symbol, 29, 9);
        adg_path_close(symbol);
        adg_path_move_to_explicit(symbol, 24, 14);
        adg_path_arc_to_explicit(symbol, 4, 14, 24, 14);
        adg_path_move_to_explicit(symbol, 19, 14);
        adg_path_arc_to_explicit(symbol, 9, 14, 19, 14);

        axis = adg_path_new();
        adg_path_move_to_explicit(axis, 0, 14);
        adg_path_line_to_explicit(axis, 53, 14);
        adg_path_move_to_explicit(axis, 14, 0);
        adg_path_line_to_explicit(axis, 14, 28);
        break;
    default:
        g_return_if_reached();
        break;
    }

    data_class->symbol = symbol;
    data_class->axis = axis;
    data_class->extents.is_defined = FALSE;

    if (axis != NULL)
        cpml_extents_add(&data_class->extents,
                         adg_trail_get_extents((AdgTrail *) axis));

    if (symbol != NULL)
        cpml_extents_add(&data_class->extents,
                         adg_trail_get_extents((AdgTrail *) symbol));
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgProjectionClassPrivate *data_class;
    AdgProjectionPrivate *data;
    const cairo_path_t *cairo_path;

    data_class = ADG_PROJECTION_GET_CLASS(entity)->data_class;
    data = ((AdgProjection *) entity)->data;

    cairo_transform(cr, adg_entity_get_global_matrix(entity));

    if (data_class->symbol != NULL) {
        cairo_path = adg_trail_get_cairo_path((AdgTrail *) data_class->symbol);

        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_set_line_width(cr, 2);
        adg_entity_apply_dress(entity, data->symbol_dress, cr);

        cairo_stroke(cr);
    }

    if (data_class->axis != NULL) {
        const gdouble dashes[] = { 5, 2, 1, 2 };

        cairo_path = adg_trail_get_cairo_path((AdgTrail *) data_class->axis);

        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_set_line_width(cr, 1);
        cairo_set_dash(cr, dashes, G_N_ELEMENTS(dashes), -1.5);
        adg_entity_apply_dress(entity, data->axis_dress, cr);

        cairo_stroke(cr);
    }
}
