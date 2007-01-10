/* ADG - Automatic Drawing Generation.
 * Copyright (C) 2007 - Fontana Nicola <ntd@users.sourceforge.net>
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


#ifndef __ADGMODEL_H__
#define __ADGMODEL_H__

#include <glib-object.h>


G_BEGIN_DECLS

#define ADG_TYPE_MODEL             (adg_model_get_type ())
#define ADG_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_MODEL, AdgModel))
#define ADG_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_MODEL, AdgModel))
#define ADG_IS_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_MODEL))
#define ADG_IS_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_MODEL))
#define ADG_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_MODEL, AdgModel))

typedef struct _AdgModel       AdgModel;
typedef struct _AdgModelClass  AdgModelClass;

struct _AdgModel
{
  GObject		 object;

  /*< private >*/

  gchar                 *name;
  gchar                 *material;
  gchar                 *treatment;
};

struct _AdgModelClass
{
  GObjectClass	         parent_class;
};


GType			adg_model_get_type	(void) G_GNUC_CONST;

gchar *                 adg_model_get_name      (AdgModel        *model);
void                    adg_model_set_name      (AdgModel        *model,
                                                 const gchar    *name);
gchar *                 adg_model_get_material  (AdgModel        *model);
void                    adg_model_set_material  (AdgModel        *model,
                                                 const gchar    *material);
gchar *                 adg_model_get_treatment (AdgModel        *model);
void                    adg_model_set_treatment (AdgModel        *model,
                                                 const gchar    *treatment);

G_END_DECLS


#endif /* __ADGMODEL_H__ */

