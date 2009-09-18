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
 * The #AdgADim entity represents an angular dimension.
 */

/**
 * AdgADim:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-adim.h"
#include "adg-adim-private.h"
#include "adg-container.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_ANGLE1,
    PROP_ANGLE2
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
static void     render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gchar *  default_value           (AdgDim         *dim);
static void     set_angle1              (AdgADim        *adim,
                                         gdouble         angle);
static void     set_angle2              (AdgADim        *adim,
                                         gdouble         angle2);


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

    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->render = render;

    dim_class->default_value = default_value;

    param = g_param_spec_double("angle1",
                                P_("Angle 1"),
                                P_("Angle of the first reference line"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE1, param);

    param = g_param_spec_double("angle2",
                                P_("Angle 2"),
                                P_("Angle of the second reference line"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ANGLE2, param);
}

static void
adg_adim_init(AdgADim *adim)
{
    AdgADimPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(adim, ADG_TYPE_ADIM,
                                                       AdgADimPrivate);

    data->extension1.status = CAIRO_STATUS_SUCCESS;
    data->extension1.data = NULL;
    data->extension1.num_data = 4;

    data->extension2.status = CAIRO_STATUS_SUCCESS;
    data->extension2.data = NULL;
    data->extension2.num_data = 4;

    data->arrow_path.status = CAIRO_STATUS_SUCCESS;
    data->arrow_path.data = NULL;
    data->arrow_path.num_data = 4;

    data->baseline.status = CAIRO_STATUS_SUCCESS;
    data->baseline.data = NULL;
    data->baseline.num_data = 4;

    adim->data = data;
}

static void
finalize(GObject *object)
{
    AdgADimPrivate *data = ((AdgADim *) object)->data;

    g_free(data->extension1.data);
    g_free(data->extension2.data);
    g_free(data->arrow_path.data);
    g_free(data->baseline.data);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgADimPrivate *data = ((AdgADim *) object)->data;

    switch (prop_id) {
    case PROP_ANGLE1:
        g_value_set_double(value, data->angle1);
        break;
    case PROP_ANGLE2:
        g_value_set_double(value, data->angle2);
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
    case PROP_ANGLE1:
        set_angle1(adim, g_value_get_double(value));
        break;
    case PROP_ANGLE2:
        set_angle2(adim, g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    /* TODO */
}

static gchar *
default_value(AdgDim *dim)
{
    /* TODO */
    return g_strdup("TODO");
}



/**
 * adg_adim_new:
 *
 * Creates a new - unreferenced - angular dimension. You must, at least, define
 * the reference points with adg_dim_set_ref() and the position reference using
 * adg_dim_set_pos().
 *
 * Returns: the newly created angular dimension entity
 **/
AdgADim *
adg_adim_new(void)
{
    return g_object_new(ADG_TYPE_ADIM, NULL);
}

/**
 * adg_adim_get_angle1:
 * @adim: an #AdgADim
 *
 * Gets the angle of the first reference line of @adim.
 *
 * Returns: an angle in radians or %0 on errors
 **/
gdouble
adg_adim_get_angle1(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), 0);

    data = adim->data;

    return data->angle1;
}

/**
 * adg_adim_set_angle1:
 * @adim: an #AdgADim
 * @angle: the new angle (in radians)
 *
 * Sets the angle of the first reference line of @adim to @angle.
 **/
void
adg_adim_set_angle1(AdgADim *adim, gdouble angle)
{
    g_return_if_fail(ADG_IS_ADIM(adim));

    set_angle1(adim, angle);

    g_object_notify((GObject *) adim, "angle1");
}

/**
 * adg_adim_get_angle2:
 * @adim: an #AdgADim
 *
 * Gets the angle of the second reference line of @adim.
 *
 * Returns: an angle in radians or %0 on errors
 **/
gdouble
adg_adim_get_angle2(AdgADim *adim)
{
    AdgADimPrivate *data;

    g_return_val_if_fail(ADG_IS_ADIM(adim), 0);

    data = adim->data;

    return data->angle2;
}

/**
 * adg_adim_set_angle2:
 * @adim: an #AdgADim
 * @angle: the new angle (in radians)
 *
 * Sets the angle of the first reference line of @adim to @angle.
 **/
void
adg_adim_set_angle2(AdgADim *adim, gdouble angle)
{
    g_return_if_fail(ADG_IS_ADIM(adim));

    set_angle2(adim, angle);

    g_object_notify((GObject *) adim, "angle2");
}


static void
set_angle1(AdgADim *adim, gdouble angle)
{
    AdgADimPrivate *data = adim->data;

    data->angle1 = angle;
}

static void
set_angle2(AdgADim *adim, gdouble angle)
{
    AdgADimPrivate *data = adim->data;

    data->angle2 = angle;
}
