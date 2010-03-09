/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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
#error "Only <adg/adg.h> can be included directly."
#endif


#ifndef __ADG_POINT_H__
#define __ADG_POINT_H__

#include <adg/adg-pair.h>
#include <adg/adg-model.h>


G_BEGIN_DECLS

#define ADG_TYPE_POINT                  (adg_point_get_type())


typedef struct _AdgPoint  AdgPoint;


GType           adg_point_get_type              (void) G_GNUC_CONST;

AdgPoint *      adg_point_new                   (void);
AdgPoint *      adg_point_dup                   (const AdgPoint *src);
void            adg_point_destroy               (AdgPoint       *point);
void            adg_point_copy                  (AdgPoint       *point,
                                                 const AdgPoint *src);
void            adg_point_set_pair              (AdgPoint       *point,
                                                 const AdgPair  *pair);
void            adg_point_set_pair_explicit     (AdgPoint       *point,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_point_set_pair_from_model   (AdgPoint       *point,
                                                 AdgModel       *model,
                                                 const gchar    *name);
const AdgPair * adg_point_get_pair              (AdgPoint       *point);
void            adg_point_invalidate            (AdgPoint       *point);
gboolean        adg_point_equal                 (const AdgPoint *point1,
                                                 const AdgPoint *point2);

G_END_DECLS


#endif /* __ADG_POINT_H__ */
