/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


/**
 * SECTION:model
 * @title: AdgModel
 * @short_description: The data model
 *
 * Every drawing is a representation of a part: the #AdgModel contains all the
 * data needed by the drawing (overall, quotes).
 *
 * Because the #AdgModel instances are only a data collection, they are not
 * renderable, so the class is not derived from #AdgEntity.
 */

#include "adg-model.h"
#include "adg-model-private.h"
#include "adg-intl.h"

#define PARENT_CLASS ((GObjectClass *) adg_model_parent_class)


enum {
    PROP_0,
    PROP_NAME,
    PROP_MATERIAL,
    PROP_TREATMENT
};

enum {
    MODIFIED,
    LAST_SIGNAL
};


static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     set_name                (AdgModel       *model,
                                         const gchar    *name);
static void     set_material            (AdgModel       *model,
                                         const gchar    *material);
static void     set_treatment           (AdgModel       *model,
                                         const gchar    *treatment);

static guint model_signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE(AdgModel, adg_model, G_TYPE_OBJECT);


static void
adg_model_class_init(AdgModelClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgModelPrivate));

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    gobject_class->finalize = finalize;

    param = g_param_spec_string("name",
                                P_("Part Name"),
                                P_("Descriptive name of this part"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, param);

    param = g_param_spec_string("material",
                                P_("Material"),
                                P_("Material this part is done with"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MATERIAL, param);

    param = g_param_spec_string("treatment",
                                P_("Treatment"),
                                P_("Treatment this part must receive"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TREATMENT, param);
}

static void
adg_model_init(AdgModel *model)
{
    AdgModelPrivate *priv =
        G_TYPE_INSTANCE_GET_PRIVATE(model, ADG_TYPE_MODEL,
                                    AdgModelPrivate);
    priv->name = NULL;
    priv->material = NULL;
    priv->treatment = NULL;

    model->priv = priv;
}

static void
finalize(GObject *object)
{
    AdgModel *model = (AdgModel *) object;

    g_free(model->priv->name);
    g_free(model->priv->material);
    g_free(model->priv->treatment);

    PARENT_CLASS->finalize(object);
}


static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgModelPrivate *priv = ((AdgModel *) object)->priv;

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
    case PROP_MATERIAL:
        g_value_set_string(value, priv->material);
        break;
    case PROP_TREATMENT:
        g_value_set_string(value, priv->treatment);
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
    AdgModel *model = ADG_MODEL(object);

    switch (prop_id) {
    case PROP_NAME:
        set_name(model, g_value_get_string(value));
        break;
    case PROP_MATERIAL:
        set_material(model, g_value_get_string(value));
        break;
    case PROP_TREATMENT:
        set_treatment(model, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


gchar *
adg_model_get_name(AdgModel *model)
{
    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);

    return g_strdup(model->priv->name);
}

void
adg_model_set_name(AdgModel *model, const gchar *name)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    set_name(model, name);
    g_object_notify((GObject *) model, "name");
}


gchar *
adg_model_get_material(AdgModel *model)
{
    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);

    return g_strdup(model->priv->material);
}

void
adg_model_set_material(AdgModel *model, const gchar *material)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    set_material(model, material);
    g_object_notify((GObject *) model, "material");
}


gchar *
adg_model_get_treatment(AdgModel *model)
{
    g_return_val_if_fail(ADG_IS_MODEL(model), NULL);

    return g_strdup(model->priv->treatment);
}

void
adg_model_set_treatment(AdgModel *model, const gchar *treatment)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    set_treatment(model, treatment);
    g_object_notify((GObject *) model, "treatment");
}

static void
set_name(AdgModel *model, const gchar *name)
{
    g_free(model->priv->name);
    model->priv->name = g_strdup(name);
}

static void
set_material(AdgModel *model, const gchar *material)
{
    g_free(model->priv->material);
    model->priv->material = g_strdup(material);
}

static void
set_treatment(AdgModel *model, const gchar *treatment)
{
    g_free(model->priv->treatment);
    model->priv->treatment = g_strdup(treatment);
}
