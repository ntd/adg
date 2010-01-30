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


#if !defined (__ADG_H__)
#error "Only <adg/adg.h> can be included directly."
#endif


#ifndef __ADG_ALIGNMENT_H__
#define __ADG_ALIGNMENT_H__

#include <adg/adg-container.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_ALIGNMENT             (adg_alignment_get_type())
#define ADG_ALIGNMENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_ALIGNMENT, AdgAlignment))
#define ADG_ALIGNMENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_ALIGNMENT, AdgAlignmentClass))
#define ADG_IS_ALIGNMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_ALIGNMENT))
#define ADG_IS_ALIGNMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_ALIGNMENT))
#define ADG_ALIGNMENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_ALIGNMENT, AdgAlignmentClass))


typedef struct _AdgAlignment       AdgAlignment;
typedef struct _AdgAlignmentClass  AdgAlignmentClass;

struct _AdgAlignment {
    /*< private >*/
    AdgContainer         parent;
    gpointer             data;
};

struct _AdgAlignmentClass {
    /*< private >*/
    AdgContainerClass    parent_class;
};


GType           adg_alignment_get_type          (void) G_GNUC_CONST;

AdgAlignment *  adg_alignment_new               (const AdgPair  *factor);
AdgAlignment *  adg_alignment_new_explicit      (gdouble         x_factor,
                                                 gdouble         y_factor);

void            adg_alignment_set_factor        (AdgAlignment   *alignment,
                                                 const AdgPair  *factor);
void            adg_alignment_set_factor_explicit
                                                (AdgAlignment   *alignment,
                                                 gdouble         x_factor,
                                                 gdouble         y_factor);
const AdgPair * adg_alignment_get_factor        (AdgAlignment   *alignment);

G_END_DECLS


#endif /* __ADG_ALIGNMENT_H__ */
