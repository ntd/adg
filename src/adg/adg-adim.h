/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ADIM_H__
#define __ADG_ADIM_H__

#include "adg-dim.h"


G_BEGIN_DECLS

#define ADG_TYPE_ADIM             (adg_adim_get_type())
#define ADG_ADIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ADIM, AdgADim))
#define ADG_ADIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_ADIM, AdgADimClass))
#define ADG_IS_ADIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ADIM))
#define ADG_IS_ADIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_ADIM))
#define ADG_ADIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_ADIM, AdgADimClass))


typedef struct _AdgADim        AdgADim;
typedef struct _AdgADimClass   AdgADimClass;

struct _AdgADim {
    /*< private >*/
    AdgDim               parent;
    gpointer             data;
};

struct _AdgADimClass {
    /*< private >*/
    AdgDimClass          parent_class;
};


GType           adg_adim_get_type               (void) G_GNUC_CONST;
AdgADim *       adg_adim_new                    (void);
AdgADim *       adg_adim_new_full               (const AdgPair  *ref1,
                                                 const AdgPair  *ref2,
                                                 const AdgPair  *org1,
                                                 const AdgPair  *org2,
                                                 const AdgPair  *pos);
AdgADim *       adg_adim_new_full_explicit      (gdouble         ref1_x,
                                                 gdouble         ref1_y,
                                                 gdouble         ref2_x,
                                                 gdouble         ref2_y,
                                                 gdouble         org1_x,
                                                 gdouble         org1_y,
                                                 gdouble         org2_x,
                                                 gdouble         org2_y,
                                                 gdouble         pos_x,
                                                 gdouble         pos_y);
AdgADim *       adg_adim_new_full_from_model    (AdgModel       *model,
                                                 const gchar    *ref1,
                                                 const gchar    *ref2,
                                                 const gchar    *org1,
                                                 const gchar    *org2,
                                                 const gchar    *pos);
void            adg_adim_set_org1               (AdgADim        *adim,
                                                 const AdgPoint *org1);
void            adg_adim_set_org1_explicit      (AdgADim        *adim,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_adim_set_org1_from_pair     (AdgADim        *adim,
                                                 const AdgPair  *org1);
void            adg_adim_set_org1_from_model    (AdgADim        *adim,
                                                 AdgModel       *model,
                                                 const gchar    *org1);
AdgPoint *      adg_adim_get_org1               (AdgADim        *adim);
void            adg_adim_set_org2               (AdgADim        *adim,
                                                 const AdgPoint *org2);
void            adg_adim_set_org2_explicit      (AdgADim        *adim,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_adim_set_org2_from_pair     (AdgADim        *adim,
                                                 const AdgPair  *org2);
void            adg_adim_set_org2_from_model    (AdgADim        *adim,
                                                 AdgModel       *model,
                                                 const gchar    *org2);
AdgPoint *      adg_adim_get_org2               (AdgADim        *adim);
void            adg_adim_switch_extension1      (AdgADim        *adim,
                                                 gboolean        new_state);
gboolean        adg_adim_has_extension1         (AdgADim        *adim);
void            adg_adim_switch_extension2      (AdgADim        *adim,
                                                 gboolean        new_state);
gboolean        adg_adim_has_extension2         (AdgADim        *adim);

G_END_DECLS


#endif /* __ADG_ADIM_H__ */
