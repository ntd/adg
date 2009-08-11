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


#ifndef __ADG_LDIM_H__
#define __ADG_LDIM_H__

#include <adg/adg-dim.h>


G_BEGIN_DECLS

#define ADG_TYPE_LDIM             (adg_ldim_get_type ())
#define ADG_LDIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_LDIM, AdgLDim))
#define ADG_LDIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_LDIM, AdgLDimClass))
#define ADG_IS_LDIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_LDIM))
#define ADG_IS_LDIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_LDIM))
#define ADG_LDIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_LDIM, AdgLDimClass))


typedef struct _AdgLDim        AdgLDim;
typedef struct _AdgLDimClass   AdgLDimClass;

struct _AdgLDim {
    /*< private >*/
    AdgDim               parent;
    gpointer             data;
};

struct _AdgLDimClass {
    /*< private >*/
    AdgDimClass          parent_class;
};


GType           adg_ldim_get_type               (void) G_GNUC_CONST;

AdgEntity *     adg_ldim_new                    (void);
AdgEntity *     adg_ldim_new_full               (const AdgPair  *ref1,
                                                 const AdgPair  *ref2,
                                                 gdouble         direction,
                                                 const AdgPair  *pos);
AdgEntity *     adg_ldim_new_full_explicit      (gdouble         ref1_x,
                                                 gdouble         ref1_y,
                                                 gdouble         ref2_x,
                                                 gdouble         ref2_y,
                                                 gdouble         direction,
                                                 gdouble         pos_x,
                                                 gdouble         pos_y);

void            adg_ldim_set_pos                (AdgLDim        *ldim,
                                                 const AdgPair  *pos);
void            adg_ldim_set_pos_explicit       (AdgLDim        *ldim,
                                                 gdouble         pos_x,
                                                 gdouble         pos_y);
gdouble         adg_ldim_get_direction          (AdgLDim        *ldim);
void            adg_ldim_set_direction          (AdgLDim        *ldim,
                                                 gdouble         direction);

G_END_DECLS


#endif /* __ADG_LDIM_H__ */
