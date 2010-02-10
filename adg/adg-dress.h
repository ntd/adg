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


#if !defined(__ADG_H__)
#error "Only <adg/adg.h> can be included directly."
#endif


#ifndef __ADG_DRESS_H__
#define __ADG_DRESS_H__

#include <adg/adg-style.h>


G_BEGIN_DECLS

#define ADG_TYPE_DRESS                  (adg_dress_get_type())
#define ADG_VALUE_HOLDS_DRESS(value)    (G_TYPE_CHECK_VALUE_TYPE((value), ADG_TYPE_DRESS))

#define ADG_TYPE_PARAM_SPEC_DRESS       (_adg_param_spec_dress_get_type())
#define ADG_IS_PARAM_SPEC_DRESS(spec)   (G_TYPE_CHECK_INSTANCE_TYPE((spec), ADG_TYPE_PARAM_SPEC_DRESS))


/* AdgDress defined in adg-entity.h */


GType           adg_dress_get_type              (void) G_GNUC_CONST;
AdgDress        adg_dress_new                   (const gchar    *name,
                                                 AdgStyle       *fallback);
AdgDress        adg_dress_new_full              (const gchar    *name,
                                                 AdgStyle       *fallback,
                                                 GType           ancestor_type);
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

GType           _adg_param_spec_dress_get_type  (void) G_GNUC_CONST;
GParamSpec *    adg_param_spec_dress            (const gchar    *name,
                                                 const gchar    *nick,
                                                 const gchar    *blurb,
                                                 AdgDress        dress,
                                                 GParamFlags     flags);

G_END_DECLS


#endif /* __ADG_DRESS_H__ */
