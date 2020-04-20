/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_CONTAINER_H__
#define __ADG_CONTAINER_H__


G_BEGIN_DECLS

#define ADG_TYPE_CONTAINER             (adg_container_get_type())
#define ADG_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_CONTAINER, AdgContainer))
#define ADG_CONTAINER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_CONTAINER, AdgContainerClass))
#define ADG_IS_CONTAINER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_CONTAINER))
#define ADG_IS_CONTAINER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_CONTAINER))
#define ADG_CONTAINER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_CONTAINER, AdgContainerClass))


typedef struct _AdgContainer       AdgContainer;
typedef struct _AdgContainerClass  AdgContainerClass;

struct _AdgContainer {
    /*< private >*/
    AdgEntity           parent;
};

struct _AdgContainerClass {
    /*< private >*/
    AdgEntityClass      parent_class;

    /*< public >*/
    /* Virtual table */
    GSList *            (*children)             (AdgContainer   *container);

    /* Signals */
    void                (*add)                  (AdgContainer   *container,
                                                 AdgEntity      *entity);
    void                (*remove)               (AdgContainer   *container,
                                                 AdgEntity      *entity);
};


GType           adg_container_get_type          (void);

AdgContainer *  adg_container_new               (void);
GSList *        adg_container_children          (AdgContainer    *container);
void            adg_container_add               (AdgContainer    *container,
                                                 AdgEntity       *entity);
void            adg_container_remove            (AdgContainer    *container,
                                                 AdgEntity       *entity);

void            adg_container_foreach           (AdgContainer    *container,
                                                 GCallback        callback,
                                                 gpointer         user_data);
void            adg_container_propagate         (AdgContainer    *container,
                                                 guint            signal_id,
                                                 GQuark           detail,
                                                 ...);
void            adg_container_propagate_by_name (AdgContainer    *container,
                                                 const gchar     *detailed_signal,
                                                 ...);
void            adg_container_propagate_valist  (AdgContainer    *container,
                                                 guint            signal_id,
                                                 GQuark           detail,
                                                 va_list          var_args);

G_END_DECLS


#endif /* __ADG_CONTAINER_H__ */
