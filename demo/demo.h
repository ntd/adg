/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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

/* Needed for localization support */
#include <adg/adg-internal.h>

/* Force the reinclusion of adg.h */
#undef __ADG_H__


#ifndef __DEMO_H__
#define __DEMO_H__


G_BEGIN_DECLS

extern gboolean is_installed;
extern gchar *  basedir;


void        _demo_init          (gint            argc,
                                 gchar          *argv[]);
gchar *     _demo_file          (const gchar    *file_name);

G_END_DECLS


#endif /* __DEMO_H__ */
