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


#ifndef __ADG_UTIL_H__
#define __ADG_UTIL_H__

#include <glib.h>
#include <cairo.h>
#include <math.h>


G_BEGIN_DECLS


#define ADG_ISSET(flags,mask)	(((flags) & (mask)) != 0 ? TRUE : FALSE)
#define ADG_SET(flags,mask)	G_STMT_START{ (flags) |= (mask); }G_STMT_END
#define ADG_UNSET(flags,mask)	G_STMT_START{ (flags) &= ~(mask); }G_STMT_END


#define ADG_CHECKPOINT_WITH_MESSAGE(message) \
                                g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, \
                                       "file `%s' at line %d: %s", \
                                       __FILE__, __LINE__, #message)
#define ADG_CHECKPOINT()        ADG_CHECKPOINT_WITH_MESSAGE ("check point")

#define ADG_STUB()              ADG_CHECKPOINT_WITH_MESSAGE("stub")


gboolean                adg_cairo_path_data_start_point (cairo_path_data_t
                                                                        *path_data,
                                                         double         *p_x,
                                                         double         *p_y);
gboolean                adg_cairo_path_data_end_point   (cairo_path_data_t
                                                                        *path_data,
                                                         double         *p_x,
                                                         double         *p_y);
cairo_path_data_t *     adg_cairo_path_tail             (cairo_path_t   *path);


G_END_DECLS


#endif /* __ADG_UTIL_H__ */
