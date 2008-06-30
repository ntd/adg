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
#define adg_pair_unset(pair)    adg_pair_set_explicit ((pair), ADG_NAN, ADG_NAN)

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
AdgPair *               adg_pair_set                    (AdgPair        *pair,
                                                         const AdgPair  *src);
AdgPair *               adg_pair_set_explicit           (AdgPair        *pair,
                                                         double          x,
                                                         double          y);

double                  adg_pair_get_squared_length     (const AdgPair  *pair);
double                  adg_pair_get_length             (const AdgPair  *pair);

double                  adg_pair_get_angle              (const AdgPair  *pair);

AdgPair *               adg_pair_add                    (AdgPair        *pair,
                                                         const AdgPair  *pair2);
AdgPair *               adg_pair_sub                    (AdgPair        *pair,
                                                         const AdgPair  *pair2);
AdgPair *               adg_pair_mul                    (AdgPair        *pair,
                                                         const AdgPair  *pair2);
AdgPair *               adg_pair_scale                  (AdgPair        *pair,
                                                         double          scale);
AdgPair *               adg_pair_mid                    (AdgPair        *pair,
                                                         const AdgPair  *pair2);
AdgPair *               adg_pair_transform              (AdgPair        *pair,
                                                         const AdgMatrix*matrix);
AdgPair *               adg_pair_scale_and_transform    (AdgPair        *pair,
                                                         double          scale,
                                                         const AdgMatrix*matrix);

AdgPair *               adg_pair_intersection           (AdgPair        *pair,
                                                         const AdgVector*vector,
                                                         const AdgPair  *pair2,
                                                         const AdgVector*vector2);
AdgPair *               adg_pair_projection             (AdgPair        *pair,
                                                         const AdgPair  *pair2,
                                                         const AdgVector*vector2);

AdgVector *             adg_vector_set_with_pair        (AdgVector      *vector,
                                                         const AdgPair  *pair);
AdgVector *             adg_vector_set_with_angle       (AdgVector      *vector,
                                                         double          angle);

AdgVector *             adg_vector_normal               (AdgVector      *vector);

G_END_DECLS


#endif /* __ADG_PAIR_H__ */
