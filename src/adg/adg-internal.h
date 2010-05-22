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

/*
 * This header is included by every .c files of the library to
 * enable the inclusion of the internal headers and initialize
 * some common stuff.
 */

#ifndef __ADG_INTERNAL_H__
#define __ADG_INTERNAL_H__

#define G_LOG_DOMAIN  "adg"

/* The following define enables the inclusion of internal headers */
#define __ADG_H__

#include <config.h>
#include "adg-utils.h"


#ifdef ENABLE_NLS

#include <glib/gi18n-lib.h>
#define P_(String) dgettext(GETTEXT_PACKAGE "-properties",String)

#else /* !ENABLE_NLS */

#define _(String)               (String)
#define P_(String)              (String)
#define Q_(String)              (String)
#define N_(String)              (String)
#define C_(Context,String)      (String)
#define NC_(Context, String)    (String)

#endif

#endif /* __ADG_INTERNAL_H__ */
