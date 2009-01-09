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


#ifndef __ADG_ENTITY_H__
#define __ADG_ENTITY_H__

#include <adg/adg-context.h>
#include <adg/adg-matrix.h>
#include <adg/adg-point.h>
#include <adg/adg-line-style.h>
#include <adg/adg-font-style.h>
#include <adg/adg-arrow-style.h>
#include <adg/adg-dim-style.h>


G_BEGIN_DECLS

/* Forward declarations */
typedef struct _AdgCanvas       AdgCanvas;
typedef struct _AdgContainer    AdgContainer;


#define ADG_TYPE_ENTITY             (adg_entity_get_type ())
#define ADG_ENTITY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_ENTITY, AdgEntity))
#define ADG_ENTITY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_ENTITY, AdgEntityClass))
#define ADG_IS_ENTITY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_ENTITY))
#define ADG_IS_ENTITY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_ENTITY))
#define ADG_ENTITY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_ENTITY, AdgEntityClass))


typedef struct _AdgEntity        AdgEntity;
typedef struct _AdgEntityClass   AdgEntityClass;
typedef struct _AdgEntityPrivate AdgEntityPrivate;


struct _AdgEntity {
    GInitiallyUnowned    initially_unowned;

    /*< private >*/
    AdgEntityPrivate    *priv;
};

struct _AdgEntityClass {
    GInitiallyUnownedClass parent_class;

    /* Signals */
    void                (*parent_set)           (AdgEntity      *entity,
                                                 AdgContainer   *old_parent);
    void                (*model_matrix_changed) (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
    void                (*paper_matrix_changed) (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
    void                (*invalidate)           (AdgEntity      *entity);
    void                (*render)               (AdgEntity      *entity,
                                                 cairo_t        *cr);
    /* Virtual Table */
    AdgContainer *      (*get_parent)           (AdgEntity      *entity);
    void                (*set_parent)           (AdgEntity      *entity,
                                                 AdgContainer   *parent);
    AdgContext *        (*get_context)          (AdgEntity      *entity);
    const AdgMatrix *   (*get_model_matrix)     (AdgEntity      *entity);
    const AdgMatrix *   (*get_paper_matrix)     (AdgEntity      *entity);
};


#define ADG_CALLBACK(f)             ((AdgCallback) (f))


typedef void (*AdgCallback) (AdgEntity *entity, gpointer user_data);


GType           adg_entity_get_type             (void) G_GNUC_CONST;
AdgCanvas *     adg_entity_get_canvas           (AdgEntity      *entity);

AdgContainer *  adg_entity_get_parent           (AdgEntity      *entity);
void            adg_entity_set_parent           (AdgEntity      *entity,
                                                 AdgContainer   *parent);
void		adg_entity_unparent		(AdgEntity      *entity);
void            adg_entity_reparent             (AdgEntity      *entity,
                                                 AdgContainer   *parent);
AdgContext *    adg_entity_get_context          (AdgEntity      *entity);
void            adg_entity_set_context          (AdgEntity      *entity,
                                                 AdgContext     *context);
const AdgMatrix*adg_entity_get_model_matrix     (AdgEntity      *entity);
const AdgMatrix*adg_entity_get_paper_matrix     (AdgEntity      *entity);
gboolean        adg_entity_build_paper2model    (AdgEntity      *entity,
                                                 AdgMatrix      *matrix);
gboolean        adg_entity_build_model2paper    (AdgEntity      *entity,
                                                 AdgMatrix      *matrix);
void            adg_entity_scale_to_model       (AdgEntity      *entity,
                                                 cairo_t        *cr);
void            adg_entity_scale_to_paper       (AdgEntity      *entity,
                                                 cairo_t        *cr);
void            adg_entity_model_matrix_changed (AdgEntity      *entity,
                                                 const AdgMatrix*parent_matrix);
void            adg_entity_paper_matrix_changed (AdgEntity      *entity,
                                                 const AdgMatrix*parent_matrix);
AdgStyle *      adg_entity_get_style            (AdgEntity      *entity,
                                                 AdgStyleSlot    style_slot);
void            adg_entity_apply                (AdgEntity      *entity,
                                                 AdgStyleSlot    style_slot,
                                                 cairo_t        *cr);
void            adg_entity_point_to_pair        (AdgEntity      *entity,
                                                 const AdgPoint *point,
                                                 AdgPair        *pair);
void            adg_entity_point_to_model_pair  (AdgEntity      *entity,
                                                 const AdgPoint *point,
                                                 AdgPair        *pair);
void            adg_entity_point_to_paper_pair  (AdgEntity      *entity,
                                                 const AdgPoint *point,
                                                 AdgPair        *pair);
gboolean        adg_entity_model_matrix_applied (AdgEntity      *entity);
gboolean        adg_entity_paper_matrix_applied (AdgEntity      *entity);
gboolean        adg_entity_model_applied        (AdgEntity      *entity);
void            adg_entity_invalidate           (AdgEntity      *entity);
void            adg_entity_render               (AdgEntity      *entity,
                                                 cairo_t        *cr);

G_END_DECLS


#endif /* __ADG_ENTITY_H__ */
