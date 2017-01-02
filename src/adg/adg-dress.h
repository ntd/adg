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


#ifndef __ADG_DRESS_H__
#define __ADG_DRESS_H__


G_BEGIN_DECLS

#define ADG_VALUE_HOLDS_DRESS(value)    (G_TYPE_CHECK_VALUE_TYPE((value), ADG_TYPE_DRESS))


AdgDress        adg_dress_from_name             (const gchar    *name);
gboolean        adg_dress_set                   (AdgDress       *dress,
                                                 AdgDress        src);
gboolean        adg_dress_are_related           (AdgDress        dress1,
                                                 AdgDress        dress2);
const gchar *   adg_dress_get_name              (AdgDress        dress);
GType           adg_dress_get_ancestor_type     (AdgDress        dress);
void            adg_dress_set_fallback          (AdgDress        dress,
                                                 AdgStyle       *fallback);
AdgStyle *      adg_dress_get_fallback          (AdgDress        dress);
gboolean        adg_dress_style_is_compatible   (AdgDress        dress,
                                                 AdgStyle       *style);

G_END_DECLS


#endif /* __ADG_DRESS_H__ */
