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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


#ifndef __ADG_PAIR_H__
#define __ADG_PAIR_H__

#include <adg/adg-matrix.h>
#include <cpml/cpml.h>


G_BEGIN_DECLS


#define ADG_TYPE_PAIR  (adg_pair_get_type ())


/**
 * adg_pair_unset:
 * @pair: an #AdgPair structure
 *
 * Unsets @pair, so its content is no more valid. Use adg_pair_is_set() to
 * check if the pair is valid.
 */
#define adg_pair_unset(pair)    (pair)->x = (pair)->y = ADG_NAN

/**
 * adg_pair_is_set:
 * @pair: an #AdgPair
 *
 * Checks if @pair is a valid pair. A pair is invalid when one or both its xy
 * components are equal to #ADG_NAN.
 *
 * For performance reasons, this function is implemented as a macro.
 */
#define adg_pair_is_set(pair)                 \
  ((pair) != NULL && !adg_isnan ((pair)->x) && !adg_isnan ((pair)->y))


typedef CpmlPair AdgPair;
typedef CpmlPair AdgVector;


GType                   adg_pair_get_type               (void) G_GNUC_CONST;
AdgPair *               adg_pair_dup                    (const AdgPair  *pair);
AdgPair *               adg_pair_intersection           (AdgPair        *pair,
                                                         const AdgVector*vector,
                                                         const AdgPair  *pair2,
                                                         const AdgVector*vector2);
AdgPair *               adg_pair_projection             (AdgPair        *pair,
                                                         const AdgPair  *pair2,
                                                         const AdgVector*vector2);

G_END_DECLS


#endif /* __ADG_PAIR_H__ */
