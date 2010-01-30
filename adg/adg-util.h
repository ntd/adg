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


#ifndef __ADG_UTIL_H__
#define __ADG_UTIL_H__

#include <glib.h>
#include <math.h>

#define ADG_DIR_RIGHT           0.
#define ADG_DIR_DOWN            M_PI_2
#define ADG_DIR_LEFT            M_PI
#define ADG_DIR_UP              (M_PI_2 * 3.)

#define ADG_FORWARD_DECL(id)    typedef struct _##id id
#define ADG_MESSAGE(message)    g_message("file `%s' at line %d: %s", \
                                          __FILE__, __LINE__, #message)


G_BEGIN_DECLS

gint            adg_strcmp              (const gchar    *s1,
                                         const gchar    *s2);
gboolean        adg_is_empty            (const gchar    *str);

G_END_DECLS


#endif /* __ADG_UTIL_H__ */
