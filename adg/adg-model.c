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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


/**
 * SECTION:adgmodel
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
#include "adg-intl.h"


enum
{
  PROP_0,
  PROP_NAME,
  PROP_MATERIAL,
  PROP_TREATMENT
};

enum
{
  MODIFIED,
  LAST_SIGNAL
};

static void		adg_model_finalize	(GObject	*object);
static void		adg_model_get_property	(GObject	*object,
						 guint		 prop_id,
						 GValue		*value,
						 GParamSpec	*pspec);
static void		adg_model_set_property	(GObject	*object,
						 guint		 prop_id,
						 const GValue	*value,
						 GParamSpec	*pspec);

static guint		model_signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE (AdgModel, adg_model, G_TYPE_OBJECT);

#define PARENT_CLASS ((GObjectClass *) adg_model_parent_class)


static void
adg_model_class_init (AdgModelClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = adg_model_set_property;
  gobject_class->get_property = adg_model_get_property;
  gobject_class->finalize = adg_model_finalize;

  param = g_param_spec_string ("name",
			      P_("Part Name"),
			      P_("Descriptive name of this part"),
			      NULL,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NAME, param);

  param = g_param_spec_string ("material",
			      P_("Material"),
			      P_("Material this part is done with"),
			      NULL,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_MATERIAL, param);

  param = g_param_spec_string ("treatment",
			      P_("Treatment"),
			      P_("Treatment this part must receive"),
			      NULL,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TREATMENT, param);
}

static void
adg_model_init (AdgModel *model)
{
  model->name = NULL;
  model->material = NULL;
  model->treatment = NULL;
}

static void
adg_model_finalize (GObject *object)
{
  AdgModel *model = ADG_MODEL (object);

  g_free (model->name);

  PARENT_CLASS->finalize (object);
}


static void
adg_model_get_property (GObject    *object,
	 		 guint       prop_id,
			 GValue     *value,
			 GParamSpec *pspec)
{
  AdgModel *model = ADG_MODEL (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, model->name);
      break;
    case PROP_MATERIAL:
      g_value_set_string (value, model->material);
      break;
    case PROP_TREATMENT:
      g_value_set_string (value, model->treatment);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
adg_model_set_property (GObject      *object,
                       guint         prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  AdgModel *model = ADG_MODEL (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (model->name);
      model->name = g_value_dup_string (value);
      break;
    case PROP_MATERIAL:
      g_free (model->material);
      model->material = g_value_dup_string (value);
      break;
    case PROP_TREATMENT:
      g_free (model->treatment);
      model->treatment = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


gchar *
adg_model_get_name (AdgModel *model)
{
  g_return_val_if_fail (ADG_IS_MODEL (model), NULL);

  if (model->name == NULL)
    return NULL;

  return g_strdup (model->name);
}

void
adg_model_set_name (AdgModel   *model,
                   const gchar *name)
{
  g_return_if_fail (ADG_IS_MODEL (model));

  g_free (model->name);

  if (name == NULL)
    model->name = NULL;
  else
    model->name = g_strdup (name);

  g_object_notify (G_OBJECT (model), "name");
}


gchar *
adg_model_get_material (AdgModel *model)
{
  g_return_val_if_fail (ADG_IS_MODEL (model), NULL);

  if (model->material == NULL)
    return NULL;

  return g_strdup (model->material);
}

void
adg_model_set_material (AdgModel   *model,
                       const gchar *material)
{
  g_return_if_fail (ADG_IS_MODEL (model));

  g_free (model->material);

  if (material == NULL)
    model->material = NULL;
  else
    model->material = g_strdup (material);

  g_object_notify (G_OBJECT (model), "material");
}


gchar *
adg_model_get_treatment (AdgModel *model)
{
  g_return_val_if_fail (ADG_IS_MODEL (model), NULL);

  if (model->treatment == NULL)
    return NULL;

  return g_strdup (model->treatment);
}

void
adg_model_set_treatment (AdgModel   *model,
                        const gchar *treatment)
{
  g_return_if_fail (ADG_IS_MODEL (model));

  g_free (model->treatment);

  if (treatment == NULL)
    model->treatment = NULL;
  else
    model->treatment = g_strdup (treatment);

  g_object_notify (G_OBJECT (model), "treatment");
}
