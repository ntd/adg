/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_PARAM_DRESS_H__
#define __ADG_PARAM_DRESS_H__


G_BEGIN_DECLS

#define ADG_TYPE_PARAM_DRESS            (adg_param_dress_get_type())
#define ADG_PARAM_SPEC_DRESS(pspec)     (G_TYPE_CHECK_INSTANCE_CAST((pspec), ADG_TYPE_PARAM_DRESS, AdgParamSpecDress))
#define ADG_IS_PARAM_DRESS(pspec)       (G_TYPE_CHECK_INSTANCE_TYPE((pspec), ADG_TYPE_PARAM_DRESS))


GType           adg_param_dress_get_type        (void);
GParamSpec *    adg_param_spec_dress            (const gchar    *name,
                                                 const gchar    *nick,
                                                 const gchar    *blurb,
                                                 AdgDress        dress,
                                                 GParamFlags     flags);

G_END_DECLS


#endif /* __ADG_PARAM_DRESS_H__ */
