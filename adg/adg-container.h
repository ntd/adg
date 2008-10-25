/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_CONTAINER_H__
#define __ADG_CONTAINER_H__

#include <adg/adg-entity.h>


G_BEGIN_DECLS

#define ADG_TYPE_CONTAINER             (adg_container_get_type ())
#define ADG_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_CONTAINER, AdgContainer))
#define ADG_CONTAINER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_CONTAINER, AdgContainerClass))
#define ADG_IS_CONTAINER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_CONTAINER))
#define ADG_IS_CONTAINER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_CONTAINER))
#define ADG_CONTAINER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_CONTAINER, AdgContainerClass))


/* AdgContainer declared in adg-entity.h */
typedef struct _AdgContainerClass   AdgContainerClass;
typedef struct _AdgContainerPrivate AdgContainerPrivate;

struct _AdgContainer {
    AdgEntity            entity;

    /*< private >*/
    AdgContainerPrivate *priv;
};

struct _AdgContainerClass {
    AdgEntityClass       parent_class;

    /* Virtual Table */
    GSList *    (*get_children)                 (AdgContainer   *container);
    gboolean    (*add)                          (AdgContainer   *container,
                                                 AdgEntity      *entity);
    gboolean    (*remove)                       (AdgContainer   *container,
                                                 AdgEntity      *entity);
};


GType            adg_container_get_type         (void) G_GNUC_CONST;
GSList *         adg_container_get_children	(AdgContainer   *container);
void             adg_container_add		(AdgContainer   *container,
						 AdgEntity      *entity);
void             adg_container_remove		(AdgContainer   *container,
						 AdgEntity      *entity);

void             adg_container_foreach		(AdgContainer   *container,
						 GCallback	 callback,
						 gpointer	 user_data);
void             adg_container_propagate	(AdgContainer   *container,
						 guint           signal_id,
						 GQuark          detail,
						 ...);
void             adg_container_propagate_by_name(AdgContainer   *container,
						 const gchar    *detailed_signal,
						 ...);
void             adg_container_propagate_valist(AdgContainer    *container,
						 guint           signal_id,
						 GQuark          detail,
						 va_list         var_args);
const AdgMatrix *adg_container_get_model_transformation
                                                (AdgContainer   *container);
void             adg_container_set_model_transformation
                                                (AdgContainer   *container,
                                                 AdgMatrix      *transformation);
const AdgMatrix *adg_container_get_paper_transformation
                                                (AdgContainer   *container);
void             adg_container_set_paper_transformation
                                                (AdgContainer   *container,
                                                 AdgMatrix      *transformation);

G_END_DECLS


#endif /* __ADG_CONTAINER_H__ */
