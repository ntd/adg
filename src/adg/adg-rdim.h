/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_RDIM_H__
#define __ADG_RDIM_H__


G_BEGIN_DECLS

#define ADG_TYPE_RDIM             (adg_rdim_get_type())
#define ADG_RDIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_RDIM, AdgRDim))
#define ADG_RDIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_RDIM, AdgRDimClass))
#define ADG_IS_RDIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_RDIM))
#define ADG_IS_RDIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_RDIM))
#define ADG_RDIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_RDIM, AdgRDimClass))


typedef struct _AdgRDim        AdgRDim;
typedef struct _AdgRDimClass   AdgRDimClass;

struct _AdgRDim {
    /*< private >*/
    AdgDim              parent;
};

struct _AdgRDimClass {
    /*< private >*/
    AdgDimClass         parent_class;
};


GType           adg_rdim_get_type               (void);

AdgRDim *       adg_rdim_new                    (void);
AdgRDim *       adg_rdim_new_full               (const CpmlPair *center,
                                                 const CpmlPair *radius,
                                                 const CpmlPair *pos);
AdgRDim *       adg_rdim_new_full_explicit      (gdouble         center_x,
                                                 gdouble         center_y,
                                                 gdouble         radius_x,
                                                 gdouble         radius_y,
                                                 gdouble         pos_x,
                                                 gdouble         pos_y);
AdgRDim *       adg_rdim_new_full_from_model    (AdgModel       *model,
                                                 const gchar    *center,
                                                 const gchar    *radius,
                                                 const gchar    *pos);

G_END_DECLS


#endif /* __ADG_RDIM_H__ */
