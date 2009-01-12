/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
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
 * This file is based on galloca.h distribuited by the GLib project
 * (glib-2.18.3). It is a private header, internally used by cpml,
 * so it should not be installed.
 */

#ifndef __CPML_ALLOCA_H__
#define __CPML_ALLOCA_H__

#include <config.h>
#include <stddef.h>

#ifdef  __GNUC__
/* GCC does the right thing */
# undef alloca
# define alloca(size)   __builtin_alloca(size)
#elif defined (HAVE_ALLOCA_H)
/* a native and working alloca.h is there */ 
# include <alloca.h>
#else /* !__GNUC__ && !HAVE_ALLOCA_H */
# if defined(_MSC_VER) || defined(__DMC__)
#  include <malloc.h>
#  define alloca _alloca
# else /* !_MSC_VER && !__DMC__ */
#  ifdef _AIX
#   pragma alloca
#  else /* !_AIX */
#   ifndef alloca /* predefined by HP cc +Olibcalls */
G_BEGIN_DECLS
char *alloca ();
G_END_DECLS
#   endif /* !alloca */
#  endif /* !_AIX */
# endif /* !_MSC_VER && !__DMC__ */
#endif /* !__GNUC__ && !HAVE_ALLOCA_H */

#define cpml_alloca(size)       alloca(size)
#define cpml_newa(type,n)       ((type *) alloca(sizeof(type) * (size_t) (n)))

#endif /* __CPML_ALLOCA_H__ */
