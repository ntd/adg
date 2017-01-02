/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_DASH_H__
#define __ADG_DASH_H__


G_BEGIN_DECLS

#define ADG_TYPE_DASH                           (adg_dash_get_type())


typedef struct _AdgDash AdgDash;


GType           adg_dash_get_type               (void);
AdgDash *       adg_dash_dup                    (const AdgDash  *src);

AdgDash *       adg_dash_new                    (void);
AdgDash *       adg_dash_new_with_dashes        (gint            num_dashes,
                                                 ...);
void            adg_dash_append_dash            (AdgDash        *dash,
                                                 gdouble         length);
void            adg_dash_append_dashes          (AdgDash        *dash,
                                                 gint            num_dashes,
                                                 ...);
void            adg_dash_append_dashes_valist   (AdgDash        *dash,
                                                 gint            num_dashes,
                                                 va_list         var_args);
void            adg_dash_append_dashes_array    (AdgDash        *dash,
                                                 gint            num_dashes,
                                                 const gdouble  *dashes);
gint            adg_dash_get_num_dashes         (const AdgDash  *dash);
const gdouble * adg_dash_get_dashes             (const AdgDash  *dash);
void            adg_dash_clear_dashes           (AdgDash        *dash);
void            adg_dash_set_offset             (AdgDash        *dash,
                                                 gdouble         offset);
gdouble         adg_dash_get_offset             (const AdgDash  *dash);
void            adg_dash_destroy                (AdgDash        *dash);

G_END_DECLS


#endif /* __ADG_DASH_H__ */
