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


#ifndef __ADG_ENTITY_H__
#define __ADG_ENTITY_H__

#include <adg/adg-util.h>
#include <adg/adg-matrix.h>
#include <adg/adg-dress.h>


G_BEGIN_DECLS

#define ADG_TYPE_ENTITY             (adg_entity_get_type())
#define ADG_ENTITY(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ENTITY, AdgEntity))
#define ADG_ENTITY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_ENTITY, AdgEntityClass))
#define ADG_IS_ENTITY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ENTITY))
#define ADG_IS_ENTITY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_ENTITY))
#define ADG_ENTITY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_ENTITY, AdgEntityClass))


typedef struct _AdgEntity        AdgEntity;
typedef struct _AdgEntityClass   AdgEntityClass;

ADG_FORWARD_DECL(AdgCanvas);


struct _AdgEntity {
    /*< private >*/
    GInitiallyUnowned    parent;
    gpointer             data;
};

struct _AdgEntityClass {
    /*< private >*/
    GInitiallyUnownedClass parent_class;
    /*< public >*/
    /* Signals */
    void                (*parent_set)           (AdgEntity      *entity,
                                                 AdgEntity      *old_parent);
    /* Virtual Table */
    void                (*get_global_matrix)    (AdgEntity      *entity,
                                                 AdgMatrix      *matrix);
    void                (*get_local_matrix)     (AdgEntity      *entity,
                                                 AdgMatrix      *matrix);
    gboolean            (*invalidate)           (AdgEntity      *entity);
    gboolean            (*render)               (AdgEntity      *entity,
                                                 cairo_t        *cr);
};


GType           adg_entity_get_type             (void) G_GNUC_CONST;
AdgCanvas *     adg_entity_get_canvas           (AdgEntity       *entity);

AdgEntity *     adg_entity_get_parent           (AdgEntity       *entity);
void            adg_entity_set_parent           (AdgEntity       *entity,
                                                 AdgEntity       *parent);
gboolean        adg_entity_get_rendered         (AdgEntity       *entity);
void            adg_entity_set_rendered         (AdgEntity       *entity,
                                                 gboolean         rendered);
void            adg_entity_get_global_map       (AdgEntity       *entity,
                                                 AdgMatrix       *map);
void            adg_entity_set_global_map       (AdgEntity       *entity,
                                                 const AdgMatrix *map);
void            adg_entity_transform_global_map (AdgEntity       *entity,
                                                 const AdgMatrix *transformation);
void            adg_entity_get_global_matrix    (AdgEntity       *entity,
                                                 AdgMatrix       *matrix);
void            adg_entity_get_local_map        (AdgEntity       *entity,
                                                 AdgMatrix       *map);
void            adg_entity_set_local_map        (AdgEntity       *entity,
                                                 const AdgMatrix *map);
void            adg_entity_transform_local_map  (AdgEntity       *entity,
                                                 const AdgMatrix *transformation);
void            adg_entity_get_local_matrix     (AdgEntity       *entity,
                                                 AdgMatrix       *matrix);
void            adg_entity_apply_local_matrix   (AdgEntity       *entity,
                                                 cairo_t         *cr);
AdgStyle *      adg_entity_style                (AdgEntity       *entity,
                                                 AdgDress         dress);
AdgStyle *      adg_entity_get_style            (AdgEntity       *entity,
                                                 AdgDress         dress);
void            adg_entity_set_style            (AdgEntity       *entity,
                                                 AdgDress         dress,
                                                 AdgStyle        *style);
void            adg_entity_apply_dress          (AdgEntity       *entity,
                                                 AdgDress         dress,
                                                 cairo_t         *cr);
void            adg_entity_invalidate           (AdgEntity       *entity);
void            adg_entity_render               (AdgEntity       *entity,
                                                 cairo_t         *cr);

G_END_DECLS


#endif /* __ADG_ENTITY_H__ */
