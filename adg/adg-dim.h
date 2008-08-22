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


#ifndef __ADG_DIM_H__
#define __ADG_DIM_H__

#include <adg/adg-entity.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_DIM             (adg_dim_get_type ())
#define ADG_DIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_DIM, AdgDim))
#define ADG_DIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_DIM, AdgDimClass))
#define ADG_IS_DIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_DIM))
#define ADG_IS_DIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_DIM))
#define ADG_DIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_DIM, AdgDimClass))

typedef struct _AdgDim        AdgDim;
typedef struct _AdgDimClass   AdgDimClass;
typedef struct _AdgDimPrivate AdgDimPrivate;

struct _AdgDim {
    AdgEntity            entity;
    /*< private >*/
    AdgDimPrivate       *priv;
};

struct _AdgDimClass {
    AdgEntityClass       parent_class;
    /* Virtual Table */
    gchar *             (*default_quote)        (AdgDim         *dim);
    void                (*quote_layout)         (AdgDim         *dim,
                                                 cairo_t        *cr);
};


GType           adg_dim_get_type                (void) G_GNUC_CONST;
const AdgPair * adg_dim_get_org                 (AdgDim         *dim);
void            adg_dim_set_org                 (AdgDim         *dim,
                                                 const AdgPair  *org);
void            adg_dim_set_org_explicit        (AdgDim         *dim,
                                                 gdouble         org_x,
                                                 gdouble         org_y);
gdouble         adg_dim_get_angle               (AdgDim         *dim);
void            adg_dim_set_angle               (AdgDim         *dim,
                                                 gdouble         angle);
const AdgPair * adg_dim_get_ref1                (AdgDim         *dim);
const AdgPair * adg_dim_get_ref2                (AdgDim         *dim);
void            adg_dim_set_ref                 (AdgDim         *dim,
                                                 const AdgPair  *ref1,
                                                 const AdgPair  *ref2);
void            adg_dim_set_ref_explicit        (AdgDim         *dim,
                                                 gdouble         ref1_x,
                                                 gdouble         ref1_y,
                                                 gdouble         ref2_x,
                                                 gdouble         ref2_y);
const AdgPair * adg_dim_get_pos1                (AdgDim         *dim);
const AdgPair * adg_dim_get_pos2                (AdgDim         *dim);
void            adg_dim_set_pos                 (AdgDim         *dim,
                                                 AdgPair        *pos1,
                                                 AdgPair        *pos2);
void            adg_dim_set_pos_explicit        (AdgDim         *dim,
                                                 gdouble         pos1_x,
                                                 gdouble         pos1_y,
                                                 gdouble         pos2_x,
                                                 gdouble         pos2_y);
gdouble         adg_dim_get_level               (AdgDim         *dim);
void            adg_dim_set_level               (AdgDim         *dim,
                                                 gdouble         level);
const gchar *   adg_dim_get_quote               (AdgDim         *dim);
void            adg_dim_set_quote               (AdgDim         *dim,
                                                 const gchar    *quote);
const gchar *   adg_dim_get_tolerance_up        (AdgDim         *dim);
void            adg_dim_set_tolerance_up        (AdgDim         *dim,
                                                 const gchar    *tolerance_up);
const gchar *   adg_dim_get_tolerance_down      (AdgDim         *dim);
void            adg_dim_set_tolerance_down      (AdgDim         *dim,
                                                 const gchar    *tolerance_down);
void            adg_dim_set_tolerances          (AdgDim         *dim,
                                                 const gchar    *tolerance_up,
                                                 const gchar    *tolerance_down);
const gchar *   adg_dim_get_note                (AdgDim         *dim);
void            adg_dim_set_note                (AdgDim         *dim,
                                                 const gchar    *note);
void            adg_dim_render_quote            (AdgDim         *dim,
                                                 cairo_t        *cr);

G_END_DECLS


#endif /* __ADG_DIM_H__ */
