/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_DIM_H__
#define __ADG_DIM_H__


G_BEGIN_DECLS

#define ADG_TYPE_DIM             (adg_dim_get_type())
#define ADG_DIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_DIM, AdgDim))
#define ADG_DIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_DIM, AdgDimClass))
#define ADG_IS_DIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_DIM))
#define ADG_IS_DIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_DIM))
#define ADG_DIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_DIM, AdgDimClass))

typedef struct _AdgDim        AdgDim;
typedef struct _AdgDimClass   AdgDimClass;

struct _AdgDim {
    /*< private >*/
    AdgEntity            parent;
    gpointer             data;
};

struct _AdgDimClass {
    /*< private >*/
    AdgEntityClass       parent_class;

    /*< public >*/
    /* Virtual table */
    gdouble             (*quote_angle)          (gdouble         angle);
    gchar *             (*default_value)        (AdgDim         *dim);
};


GType           adg_dim_get_type                (void);
AdgDress        adg_dim_get_dim_dress           (AdgDim         *dim);
void            adg_dim_set_dim_dress           (AdgDim         *dim,
                                                 AdgDress        dress);
void            adg_dim_set_ref1                (AdgDim         *dim,
                                                 const AdgPoint *ref1);
void            adg_dim_set_ref1_explicit       (AdgDim         *dim,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_dim_set_ref1_from_pair      (AdgDim         *dim,
                                                 const CpmlPair *ref1);
void            adg_dim_set_ref1_from_model     (AdgDim         *dim,
                                                 AdgModel       *model,
                                                 const gchar    *ref1);
AdgPoint *      adg_dim_get_ref1                (AdgDim         *dim);
void            adg_dim_set_ref2                (AdgDim         *dim,
                                                 const AdgPoint *ref2);
void            adg_dim_set_ref2_explicit       (AdgDim         *dim,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_dim_set_ref2_from_pair      (AdgDim         *dim,
                                                 const CpmlPair *ref2);
void            adg_dim_set_ref2_from_model     (AdgDim         *dim,
                                                 AdgModel       *model,
                                                 const gchar    *ref2);
AdgPoint *      adg_dim_get_ref2                (AdgDim         *dim);
void            adg_dim_set_pos                 (AdgDim         *dim,
                                                 const AdgPoint *pos);
void            adg_dim_set_pos_explicit        (AdgDim         *dim,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_dim_set_pos_from_pair       (AdgDim         *dim,
                                                 const CpmlPair *pos);
void            adg_dim_set_pos_from_model      (AdgDim         *dim,
                                                 AdgModel       *model,
                                                 const gchar    *pos);
AdgPoint *      adg_dim_get_pos                 (AdgDim         *dim);
void            adg_dim_set_level               (AdgDim         *dim,
                                                 gdouble         level);
gdouble         adg_dim_get_level               (AdgDim         *dim);
void            adg_dim_set_outside             (AdgDim         *dim,
                                                 AdgThreeState   outside);
AdgThreeState   adg_dim_get_outside             (AdgDim         *dim);
void            adg_dim_set_detached            (AdgDim         *dim,
                                                 AdgThreeState   detached);
AdgThreeState   adg_dim_get_detached            (AdgDim         *dim);
void            adg_dim_set_value               (AdgDim         *dim,
                                                 const gchar    *value);
const gchar *   adg_dim_get_value               (AdgDim         *dim);
void            adg_dim_set_limits              (AdgDim         *dim,
                                                 const gchar    *min,
                                                 const gchar    *max);
void            adg_dim_set_min                 (AdgDim         *dim,
                                                 const gchar    *min);
const gchar *   adg_dim_get_min                 (AdgDim         *dim);
void            adg_dim_set_max                 (AdgDim         *dim,
                                                 const gchar    *max);
const gchar *   adg_dim_get_max                 (AdgDim         *dim);
gdouble         adg_dim_quote_angle             (AdgDim         *dim,
                                                 gdouble         angle);
AdgAlignment *  adg_dim_get_quote               (AdgDim         *dim);

G_END_DECLS


#endif /* __ADG_DIM_H__ */
