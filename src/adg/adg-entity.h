/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ENTITY_H__
#define __ADG_ENTITY_H__


G_BEGIN_DECLS

#define ADG_TYPE_ENTITY             (adg_entity_get_type())
#define ADG_ENTITY(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ENTITY, AdgEntity))
#define ADG_ENTITY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_ENTITY, AdgEntityClass))
#define ADG_IS_ENTITY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ENTITY))
#define ADG_IS_ENTITY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_ENTITY))
#define ADG_ENTITY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_ENTITY, AdgEntityClass))


typedef struct _AdgEntity       AdgEntity;
typedef struct _AdgEntityClass  AdgEntityClass;

struct _AdgEntity {
    /*< private >*/
    GInitiallyUnowned    parent;
    gpointer             data;
};

struct _AdgEntityClass {
    /*< private >*/
    GInitiallyUnownedClass      parent_class;
    /*< public >*/
    /* Signals */
    void                (*destroy)              (AdgEntity       *entity);
    void                (*parent_set)           (AdgEntity       *entity,
                                                 AdgEntity       *old_parent);
    void                (*global_changed)       (AdgEntity       *entity);
    void                (*local_changed)        (AdgEntity       *entity);
    void                (*invalidate)           (AdgEntity       *entity);
    void                (*arrange)              (AdgEntity       *entity);
    void                (*render)               (AdgEntity       *entity,
                                                 cairo_t         *cr);
};


void             adg_switch_extents             (gboolean         state);

GType            adg_entity_get_type            (void) G_GNUC_CONST;
void             adg_entity_destroy             (AdgEntity       *entity);
AdgCanvas *      adg_entity_get_canvas          (AdgEntity       *entity);
void             adg_entity_set_parent          (AdgEntity       *entity,
                                                 AdgEntity       *parent);
AdgEntity *      adg_entity_get_parent          (AdgEntity       *entity);
void             adg_entity_set_global_map      (AdgEntity       *entity,
                                                 const AdgMatrix *map);
void             adg_entity_transform_global_map(AdgEntity       *entity,
                                                 const AdgMatrix *transformation,
                                                 AdgTransformMode mode);
const AdgMatrix *adg_entity_get_global_map      (AdgEntity       *entity);
const AdgMatrix *adg_entity_get_global_matrix   (AdgEntity       *entity);
void             adg_entity_set_local_map       (AdgEntity       *entity,
                                                 const AdgMatrix *map);
void             adg_entity_transform_local_map (AdgEntity       *entity,
                                                 const AdgMatrix *transformation,
                                                 AdgTransformMode mode);
const AdgMatrix *adg_entity_get_local_map       (AdgEntity       *entity);
const AdgMatrix *adg_entity_get_local_matrix    (AdgEntity       *entity);
void             adg_entity_set_local_method    (AdgEntity       *entity,
                                                 AdgMixMethod     local_method);
AdgMixMethod     adg_entity_get_local_method    (AdgEntity       *entity);
void             adg_entity_set_extents         (AdgEntity       *entity,
                                                 const CpmlExtents *extents);
const CpmlExtents *
                 adg_entity_get_extents         (AdgEntity       *entity);
void             adg_entity_set_style           (AdgEntity       *entity,
                                                 AdgDress         dress,
                                                 AdgStyle        *style);
AdgStyle *       adg_entity_get_style           (AdgEntity       *entity,
                                                 AdgDress         dress);
AdgStyle *       adg_entity_style               (AdgEntity       *entity,
                                                 AdgDress         dress);
void             adg_entity_apply_dress         (AdgEntity       *entity,
                                                 AdgDress         dress,
                                                 cairo_t         *cr);
void             adg_entity_global_changed      (AdgEntity       *entity);
void             adg_entity_local_changed       (AdgEntity       *entity);
void             adg_entity_invalidate          (AdgEntity       *entity);
void             adg_entity_arrange             (AdgEntity       *entity);
void             adg_entity_render              (AdgEntity       *entity,
                                                 cairo_t         *cr);
AdgPoint *       adg_entity_point               (AdgEntity       *entity,
                                                 AdgPoint        *point,
                                                 AdgPoint        *new_point);

G_END_DECLS


#endif /* __ADG_ENTITY_H__ */
