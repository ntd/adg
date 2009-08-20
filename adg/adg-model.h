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


#ifndef __ADG_MODEL_H__
#define __ADG_MODEL_H__

#include <glib-object.h>


G_BEGIN_DECLS

#define ADG_TYPE_MODEL             (adg_model_get_type())
#define ADG_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_MODEL, AdgModel))
#define ADG_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_MODEL, AdgModel))
#define ADG_IS_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_MODEL))
#define ADG_IS_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_MODEL))
#define ADG_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_MODEL, AdgModelClass))

typedef struct _AdgModel        AdgModel;
typedef struct _AdgModelClass   AdgModelClass;

struct _AdgModel {
    /*< private >*/
    GObject              parent;
    gpointer             data;
};

struct _AdgModelClass {
    /*< private >*/
    GObjectClass         parent_class;
    /*< public >*/
    /* Virtual Table */
    void                (*changed)              (AdgModel       *model);
};


GType                   adg_model_get_type      (void) G_GNUC_CONST;
void                    adg_model_changed       (AdgModel       *model);

G_END_DECLS


#endif /* __ADG_MODEL_H__ */
