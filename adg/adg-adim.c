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
#include "adg-util.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_DIRECTION
};


static void     finalize                (GObject        *object);
static void     model_matrix_changed    (AdgEntity      *entity,
                                         AdgMatrix      *parent_matrix);
static void     render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gchar *  default_quote           (AdgDim         *dim);


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

    entity_class->model_matrix_changed = model_matrix_changed;
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
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    AdgEntityClass *entity_class = (AdgEntityClass *) adg_adim_parent_class;

    /* TODO */

    if (entity_class->model_matrix_changed != NULL)
        entity_class->model_matrix_changed(entity, parent_matrix);
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    /* TODO */
}

static gchar *
default_quote(AdgDim *dim)
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
 * Return value: the new entity
 */
AdgEntity *
adg_adim_new(void)
{
    return (AdgEntity *) g_object_new(ADG_TYPE_ADIM, NULL);
}
