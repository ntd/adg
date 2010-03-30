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
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_UTIL_H__
#define __ADG_UTIL_H__

#include <glib-object.h>
#include <math.h>

#define ADG_DIR_RIGHT           0.
#define ADG_DIR_DOWN            G_PI_2
#define ADG_DIR_LEFT            G_PI
#define ADG_DIR_UP              -G_PI_2

#define ADG_FORWARD_DECL(id)    typedef struct _##id id


G_BEGIN_DECLS

#if GLIB_CHECK_VERSION(2, 16, 0)
#define         adg_strcmp              g_strcmp0
#else
gint            adg_strcmp              (const gchar    *s1,
                                         const gchar    *s2);
#endif

gboolean        adg_is_string_empty     (const gchar    *str);
gboolean        adg_is_enum_value       (int             value,
                                         GType           enum_type);
gboolean        adg_is_boolean_value    (gboolean        value);

G_END_DECLS


#endif /* __ADG_UTIL_H__ */
